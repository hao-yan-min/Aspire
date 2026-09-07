#include "DBclient.h"

#include <unistd.h>  
#include <getopt.h> 

DBClient::DBClient(Database* DB,BatchCodeParams batch_code_params,string PIRType){
    this->PIRType = PIRType;
    this->DB = DB;
    this->number_of_keywords = DB->number_of_keywords;
    this->bucket_number = batch_code_params.bucket_number;
    this->hash_func_number = batch_code_params.hash_func_number;
    this->query_number = batch_code_params.query_number;
    this->PIRs=nullptr;
    this->S1 = vector<map<string, pair<Update_Type,uint64_t>>>(this->bucket_number,map<string, pair<Update_Type,uint64_t>>());
}

uint64_t DBClient::setup(){
    
    //map each keyword to buckets, {K0,K1,...,Kb−1} = PBC.Encode(K);
    if (this->debug){
        cout<<"Setup begin"<<endl;
        cout<<"Map keyword to buckets"<<endl;
    }
   


    vector<set<pair<string,uint64_t>>> Keywords_and_hash_func_in_bucket(this->bucket_number); //recode each keyword of the bucket
    for(uint64_t i=0;i<this->DB->number_of_keywords;i++){
        for(uint64_t j=0;j<this->hash_func_number;j++){
            uint64_t b = hash_to_bucket_keyword(this->DB->keywords[i],j,bucket_number);
            //check if the keyword is already in the bucket
            bool uninserted_flag = true;
            for(uint64_t k=0;k<j;k++){
                if(Keywords_and_hash_func_in_bucket[b].find(make_pair(this->DB->keywords[i],k))!=Keywords_and_hash_func_in_bucket[b].end()){
                    uninserted_flag=false;
                }
            }
            if(uninserted_flag==true){
                Keywords_and_hash_func_in_bucket[b].insert(make_pair(this->DB->keywords[i],j));
            }
        }
    }
    
    //recode the number of pairs of each bucket in the Bucket_count
    //get the max bucket size
    this->Bucket_count = vector<uint64_t>(this->bucket_number,0);
    
    
    double bucket_num_n=static_cast<double>(this->bucket_number);
    double term1 = 3.0 * (this->number_of_keywords) / bucket_num_n;
    double term2 = std::sqrt((6.0 * this->number_of_keywords * std::log(bucket_num_n)) / bucket_num_n);

    
    this->max_bucket_size = static_cast<uint64_t>(std::ceil(term1 + term2));

    this->max_bucket_size_old = this->max_bucket_size;
    if (this->debug){
        cout<<"Max bucket size: "<<this->max_bucket_size<<endl;
    }
    
    for(uint64_t i=0;i<this->bucket_number;i++){
        
        while(Keywords_and_hash_func_in_bucket[i].size()>this->max_bucket_size){
            string keyword;
            string value;
            uint64_t hash_func_id;
            auto it = prev(Keywords_and_hash_func_in_bucket[i].end());
            keyword = it->first;
            value = this->DB->get_value_from_keyword(keyword);
            hash_func_id = it->second;
            Keywords_and_hash_func_in_bucket[i].erase(it);
            this->S1[i][keyword] = make_pair(ADD,hash_func_id);
            if (this->S2.find(keyword) != this->S2.end()){
                this->S2[keyword].second += 1;
            }
            else{
                this->S2[keyword] = make_pair(value,1);
            }
        }
    }


    

    //generate a minimal perfect hash function for each bucket
    this->MPH = new vector<sux::function::RecSplit<8>>(this->bucket_number);
    for(uint64_t i=0;i<this->bucket_number;i++){
        set<pair<string,uint64_t>> temp_set = Keywords_and_hash_func_in_bucket[i];
        vector<string> Id;
        for(auto it=temp_set.begin();it!=temp_set.end();++it){
            Id.push_back(toUpper(it->first));
        }
        sux::function::RecSplit<8> mph(Id, this->mph_parameter);
        stringstream temp_stream;
        temp_stream<<mph;
        temp_stream>>(*this->MPH)[i];
    }
    if (this->debug){
        cout<<"MPH generated."<<endl;
    }
    
    
    if (this->debug){
        cout<<"Rerange k-v pair."<<endl;
    }
    vector<vector<string>> Keywords_in_bucket_reranged(this->bucket_number); //recode each id after mph map of the bucket
    vector<vector<uint16_t>> hash_func_reranged(this->bucket_number); //recode the hash func of each id after mph map of the bucket
    for(uint64_t i=0;i<this->bucket_number;i++){
        vector<string> bucket_id_reranged(Keywords_and_hash_func_in_bucket[i].size());
        vector<uint16_t> bucket_id_count_reranged(Keywords_and_hash_func_in_bucket[i].size());
        for(auto j=Keywords_and_hash_func_in_bucket[i].begin();j!=Keywords_and_hash_func_in_bucket[i].end();j++){
            uint64_t pos_ = (*this->MPH)[i](toUpper(j->first));
            bucket_id_reranged[pos_] = j->first;
            bucket_id_count_reranged[pos_] = j->second;
        }
        Keywords_in_bucket_reranged[i] = bucket_id_reranged;
        hash_func_reranged[i] = bucket_id_count_reranged;
    }
    if (this->debug){
        cout<<"Rerange k-v pair done."<<endl;
    }


    //encrypt each bucket and send it to server
    if (this->debug){
        cout<<"Encrypt begin"<<endl;
    }
    SecByteBlock aes_key(AES::DEFAULT_KEYLENGTH);
    prng.GenerateBlock(aes_key, aes_key.size());
    this->aes_key = aes_key;
    
    

    vector<vector<string>> Encrypted_KV(this->bucket_number,vector<string>(this->max_bucket_size));//Encrypted_KV will be sent to server
    for(uint64_t i=0;i<this->bucket_number;i++){
        for(uint64_t j=0;j<this->max_bucket_size;j++){
            string to_encrypt;
            if(j<Keywords_in_bucket_reranged[i].size()){//encrypt one k-v pair in this bucket, the content is value||key_verify_id||hash_func_id||random
                to_encrypt = this->DB->get_value_from_keyword(Keywords_in_bucket_reranged[i][j]) + \
                Keywords_in_bucket_reranged[i][j] +\
                to_string(hash_func_reranged[i][j]);
            }
            else{//encrypt random
                string zero_string(this->DB->keyword_hex_length,'0');
                to_encrypt = generate_random_hex_string(this->DB->value_hex_length) + zero_string + to_string(this->hash_func_number);
            }
            //to_encrypt = to_encrypt + generate_random_hex_string(this->security_parameter/4); //add random string to the end of the ciphertext
            string to_encrypt_string = hexToString(to_encrypt);
            string temp_ciphertext;
            CryptoPP::byte iv[AES::BLOCKSIZE];
            this->prng.GenerateBlock(iv, sizeof(iv));
            cfbEncryption.SetKeyWithIV(this->aes_key, this->aes_key.size(), iv);
            StringSource(to_encrypt_string, true, new StreamTransformationFilter(this->cfbEncryption, new StringSink(temp_ciphertext)));
            Encrypted_KV[i][j]=temp_ciphertext+ string(reinterpret_cast<const char*>(iv), AES::BLOCKSIZE); //append iv to the end of the ciphertext
        }
    }
   
    this->Encrypted_KV = Encrypted_KV;
    this->encrypted_kv_bits_number = (this->Encrypted_KV)[0][0].size()*8;
    if (this->debug){
        cout<<"Encrypt done."<<endl;
        cout<<"Size of Encrypted DB: "<<(this->Encrypted_KV).size() * (this->Encrypted_KV)[0].size()*(this->Encrypted_KV)[0][0].size()<<"bytes"<<endl;
    }

    if (this->debug){
        cout<<"Setup the PIR."<<endl;
    }
    
    if (this->PIRType == "FakePIR"){
        this->PIRs = new vector<FakePIRClient>[this->bucket_number];
        for(uint64_t i = 0; i < this->bucket_number; i++){
            FakePIRClient fakepirclient(0);
            this->PIRs->emplace_back(fakepirclient);
        }
    }
    else if(this->PIRType == "SpiralPIR"){
       
        for(uint64_t i = 0; i < this->bucket_number; i++){
            auto ptr = std::make_unique<Spiral_client>(0);
            this->PIRs_uptr.emplace_back(std::move(ptr));
        }
    }
    else if(this->PIRType == "ORAM"){
        for(uint64_t i = 0; i < this->bucket_number; i++){
            auto ptr = std::make_unique<oram_client_wrapper>(0);
            this->orams_uptr.emplace_back(std::move(ptr));
        }
    }
    else if (this->PIRType == "PIRANA"){
        uint64_t payload_size = (this->Encrypted_KV)[0][0].size();
        this->PIRANA_params = new PirParms(this->DB->number_of_keywords, payload_size, this->query_number, this->bucket_number, this->max_bucket_size, true, false, true);
        
        this->PIRANA_client = new PIRANA_Client(*this->PIRANA_params);
        this->keys = PIRANA_client->save_keys();
    }
    
    else{
        cout<<"Error: The PIR type is not supported."<<endl;
        exit(1);
    }
   
    if (this->debug){
        cout<<"Setup finished."<<endl;
    }
    
    return (this->Encrypted_KV).size() * (this->Encrypted_KV)[0].size()*(this->Encrypted_KV)[0][0].size(); //
}
DBClient::~DBClient(){
    if (this->PIRs!=nullptr){
        delete[] this->PIRs;
        this->PIRs = nullptr;
    }
    else if (this->PIRType == "PIRANA"){
        delete this->PIRANA_client;
        delete this->PIRANA_params;
        this->PIRANA_client = nullptr;
        this->PIRANA_params = nullptr;
    }
    delete this->MPH;
    this->MPH = nullptr;
}
vector<string> DBClient::query_generate_query_FakePIR(vector<string> keywords){
    //generate the query for each keyword
    //the query is a vector of string, each string is a keyword   
    if (this->debug){
        cout<<"Query generate begin"<<endl;
    }
    if (keywords.size() != this->query_number){
        cout<<"The number of keywords to be queried is wrong"<<endl;
        return vector<string>();
    }

    if (this->debug){
        cout<<"PBC GenSchedule begin."<<endl;
    }
    this->reach_max_attempts = false;//record if this batch query reaches max evict attempts.
    this->query_keywords = keywords;
    //get the bucket id for each keyword (batch code eviction)
    map<uint64_t,pair<uint64_t,uint64_t>> bucket2index_count; //recode the hashed keywords and its hash func to bucket
    map<string,uint64_t> keywords_count;
    for(uint64_t i=0;i<keywords.size();i++){
        bool find_empty = false;
        for(uint64_t j=0;j<this->hash_func_number;j++){
            uint64_t bucket_hash_to = hash_to_bucket_keyword(keywords[i],j,this->bucket_number);
            if(bucket2index_count.find(bucket_hash_to) == bucket2index_count.end()){
                bucket2index_count[bucket_hash_to] = make_pair(i,j);
                keywords_count[keywords[i]] = j;
                find_empty = true;
                break;
            }
        }
        if(find_empty==false){
            //evict and re-insert
            uint64_t evict_times = 0;
            uint64_t current_id = i;
            uint64_t current_count = rand()%this->hash_func_number;
            while (find_empty==false and evict_times<this->max_attempts){
                uint64_t bucket_hash_to = hash_to_bucket_keyword(keywords[current_id],current_count,this->bucket_number);
                if(bucket2index_count.find(bucket_hash_to) == bucket2index_count.end()){
                    bucket2index_count[bucket_hash_to] = make_pair(current_id,current_count);
                    keywords_count[keywords[current_id]] = current_count;
                    find_empty = true;
                }
                else{
                    auto [old_id,old_count] =  bucket2index_count[bucket_hash_to];
                    bucket2index_count[bucket_hash_to] = make_pair(current_id,current_count);
                    current_id = old_id;
                    current_count = rand() % this->hash_func_number;
                    evict_times += 1;
                }
            }
            if(evict_times==this->max_attempts){
                cout<<"Reach max evict attempts"<<endl;
                cout<<"The keyword "<<keywords[current_id]<<" is not in the query"<<endl;
                this->reach_max_attempts = true;
            }
        }
    }
    this->bucket2index_count = bucket2index_count;
    this->keywords_count = keywords_count;
    if (this->debug){
        cout<<"PBC generate schedule finished."<<endl;
    }

    if (this->debug){
        cout<<"PIR Query generate begin."<<endl;
    }
    this->query_index_for_pir = vector<uint64_t>(this->bucket_number,0);
    this->generated_query_for_pir = vector<string>(this->bucket_number,"");
    for (uint64_t i=0;i<this->bucket_number;i++){
        if(this->bucket2index_count.find(i) == this->bucket2index_count.end()){
            if (this->reconstruct_time%this->bucket_number > i){
                this->query_index_for_pir[i] =rand()%this->max_bucket_size;
            }
            else{
                this->query_index_for_pir[i] =rand()%this->max_bucket_size_old;
            }
            continue;
        }
        auto [keyword_id,hash_func_id] = this->bucket2index_count[i];

        uint64_t id_pos = (*this->MPH)[i](toUpper(keywords[keyword_id]));
        

        this->query_index_for_pir[i] = id_pos;
    }
    //generate encrypted query
    if(this->PIRType=="FakePIR"){
        for (uint64_t i=0;i<this->bucket_number;i++){
            this->generated_query_for_pir[i] = (*this->PIRs)[i].generate_query(this->query_index_for_pir[i]);
        }
    }
    else if(this->PIRType=="SpiralPIR"){
        for (uint64_t i=0;i<this->bucket_number;i++){
            this->generated_query_for_pir[i] = this->PIRs_uptr[i]->generate_query(this->query_index_for_pir[i]);
        }
    }
    else if(this->PIRType=="ORAM"){
        for (uint64_t i=0;i<this->bucket_number;i++){
            this->generated_query_for_pir[i] = this->orams_uptr[i]->generate_query(this->query_index_for_pir[i]);
        }
    }
    
    if (this->debug){
        cout<<"PIR Query generate finished."<<endl;
        for (uint64_t i=0;i<this->bucket_number;i++){
            if(stoi(this->generated_query_for_pir[i]) >= this->max_bucket_size){
                cout<<"Error: The query for bucket "<<i<<" is not correct."<<endl;
            }
        }
    }
    return this->generated_query_for_pir;
}

stringstream DBClient::query_generate_query_PIRANA(vector<string> keywords){
    //generate the query for each keyword
    //the query is a vector of string, each string is a keyword   
    if (this->debug){
        cout<<"Query generate begin"<<endl;
    }
    if (keywords.size() != this->query_number){
        cout<<"The number of keywords to be queried is wrong"<<endl;
    }

    if (this->debug){
        cout<<"PBC GenSchedule begin."<<endl;
    }

    chrono::high_resolution_clock::time_point time_start_local = chrono::high_resolution_clock::now();
    chrono::high_resolution_clock::time_point time_end_local = chrono::high_resolution_clock::now();
    chrono::microseconds time_1,time_2;

    time_start_local = chrono::high_resolution_clock::now();


    this->reach_max_attempts = false;//record if this batch query reaches max evict attempts.
    this->query_keywords = keywords;
    //get the bucket id for each keyword (batch code eviction)
    map<uint64_t,pair<uint64_t,uint64_t>> bucket2index_count; //recode the hashed keywords and its hash func to bucket
    map<string,uint64_t> keywords_count;
    for(uint64_t i=0;i<keywords.size();i++){
        bool find_empty = false;
        for(uint64_t j=0;j<this->hash_func_number;j++){
            uint64_t bucket_hash_to = hash_to_bucket_keyword(keywords[i],j,this->bucket_number);
            if(bucket2index_count.find(bucket_hash_to) == bucket2index_count.end()){
                bucket2index_count[bucket_hash_to] = make_pair(i,j);
                keywords_count[keywords[i]] = j;
                find_empty = true;
                break;
            }
        }
        if(find_empty==false){
            //evict and re-insert
            uint64_t evict_times = 0;
            uint64_t current_id = i;
            uint64_t current_count = rand()%this->hash_func_number;
            while (find_empty==false and evict_times<this->max_attempts){
                uint64_t bucket_hash_to = hash_to_bucket_keyword(keywords[current_id],current_count,this->bucket_number);
                if(bucket2index_count.find(bucket_hash_to) == bucket2index_count.end()){
                    bucket2index_count[bucket_hash_to] = make_pair(current_id,current_count);
                    keywords_count[keywords[current_id]] = current_count;
                    find_empty = true;
                }
                else{
                    auto [old_id,old_count] =  bucket2index_count[bucket_hash_to];
                    bucket2index_count[bucket_hash_to] = make_pair(current_id,current_count);
                    current_id = old_id;
                    current_count = rand() % this->hash_func_number;
                    evict_times += 1;
                }
            }
            if(evict_times==this->max_attempts){
                cout<<"Reach max evict attempts"<<endl;
                cout<<"The keyword "<<keywords[current_id]<<" is not in the query"<<endl;
                this->reach_max_attempts = true;
            }
        }
    }
    this->bucket2index_count = bucket2index_count;
    this->keywords_count = keywords_count;
    if (this->debug){
        cout<<"PBC generate schedule finished."<<endl;
    }

    if (this->debug){
        cout<<"PIR Query generate begin."<<endl;
    }
    this->query_index_for_pir = vector<uint64_t>(this->bucket_number,0);
    this->generated_query_for_pir = vector<string>(this->bucket_number,"");
    for (uint64_t i=0;i<this->bucket_number;i++){
        if(this->bucket2index_count.find(i) == this->bucket2index_count.end()){
            if (this->reconstruct_time%this->bucket_number > i){
                this->query_index_for_pir[i] =rand()%this->max_bucket_size;
            }
            else{
                this->query_index_for_pir[i] =rand()%this->max_bucket_size_old;
            }
            continue;
        }
        auto [keyword_id,hash_func_id] = this->bucket2index_count[i];

        uint64_t id_pos = (*this->MPH)[i](toUpper(keywords[keyword_id]));
        // if (this->reconstruct_time%this->bucket_number > i){
        //     id_pos = id_pos%this->max_bucket_size;
        // }
        // else{
        //     id_pos = id_pos%this->max_bucket_size_old;
        // }

        this->query_index_for_pir[i] = id_pos;
    }
    //generate encrypted query
    vector<uint32_t> query_index_for_pir_temp;
    this->max_query_index = 0;
    for (uint64_t i=0;i<this->query_index_for_pir.size();i++){
        query_index_for_pir_temp.push_back(static_cast<uint32_t>(this->query_index_for_pir[i]));
        if (this->query_index_for_pir[i] > this->max_query_index){
            this->max_query_index = this->query_index_for_pir[i];
        }
    }

    time_end_local = chrono::high_resolution_clock::now();
    time_1 = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);

    time_start_local = chrono::high_resolution_clock::now();

    stringstream query_stream = this->PIRANA_client->gen_batch_query(query_index_for_pir_temp);

    time_end_local = chrono::high_resolution_clock::now();
    time_2 = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
    cout<<"Time for generating query: "<<time_1.count()<<"us, time for PIRANA query generation: "<<time_2.count()<<"us"<<endl;

    if (this->debug){
        cout<<"PIR Query generate finished."<<endl;
    }
    return query_stream;
}


vector<string> DBClient::query_recovery_FakePIR(vector<string> query_results){
    //recovery the query results
    if (this->debug){
        cout<<"Query recovery begin"<<endl;
    }
    vector<string> aes_encrypted_results(this->query_number);
    for (uint64_t i=0;i<this->bucket_number;i++){
        if(this->bucket2index_count.find(i) == this->bucket2index_count.end()){
            continue;
        }
        auto [keyword_id,hash_func_id] = this->bucket2index_count[i];
        if(this->PIRType=="FakePIR"){
            string response = (*this->PIRs)[i].recovery(query_results[i]);
            aes_encrypted_results[keyword_id] = response;
        }
        else if(this->PIRType=="SpiralPIR"){
            string response = this->PIRs_uptr[i]->recovery(query_results[i]);
            aes_encrypted_results[keyword_id] = response;
        }
        // string response = (*this->PIRs)[i].recovery(query_results[i]);
        else if(this->PIRType=="ORAM"){
            string response = this->orams_uptr[i]->recovery(query_results[i]);
            aes_encrypted_results[keyword_id] = response;
        }
        
    }


    vector<string> results(this->query_number,"");
    for(uint64_t i=0;i<this->query_number;i++){
        string encrypted_result = aes_encrypted_results[i];
        string temp_ciphertext = encrypted_result.substr(0, encrypted_result.size() - AES::BLOCKSIZE);
        string iv = encrypted_result.substr(encrypted_result.size() - AES::BLOCKSIZE, AES::BLOCKSIZE);
        CryptoPP::byte iv_bytes[AES::BLOCKSIZE];
        for (size_t j = 0; j < AES::BLOCKSIZE; ++j) {
            iv_bytes[j] = static_cast<CryptoPP::byte>(iv[j]);
        }
        string plaintext,hex_plaintext;
        CFB_Mode<AES>::Decryption cfbDecryption(this->aes_key, (this->aes_key).size(), iv_bytes);
        StringSource(temp_ciphertext, true, new StreamTransformationFilter(cfbDecryption, new StringSink(plaintext)));
        hex_plaintext = toUpper(stringToHex(plaintext));
        string value = hex_plaintext.substr(0,this->DB->value_hex_length);
        string keyword = hex_plaintext.substr(this->DB->value_hex_length,this->DB->keyword_hex_length);
        if(toUpper(keyword) == this->query_keywords[i]){
            results[i] = toUpper(value);
        }
        else{
            if (this->debug){
                cout<<"Waring: The keyword received is not correct, i.e., the keyword is not in the database on the server."<<endl;
                cout<<"The received keyword is "<<keyword<<" and the original keyword is "<<this->query_keywords[i]<<endl;
            }
        }
    }
    //////////////check S2//////////////
    for (uint64_t i=0;i<this->query_number;i++){
        if(this->S2.find(this->query_keywords[i]) != this->S2.end()){
            string value = this->S2[this->query_keywords[i]].first;
            if (value != ""){
                results[i] = value;
            }
            else {
                if (this->debug){
                    cout<<"The keyword "<<this->query_keywords[i]<<" is deleted."<<endl;
                }
                results[i] = "";
            }
        }
    }

    ///////////////////////////////////
    if (this->debug){
        cout<<"Query recovery finished."<<endl;
    }
    if (this->debug){
        cout<<"Query recovery results: "<<endl;
        for (uint64_t i=0;i<this->query_number;i++){
            cout<<this->query_keywords[i]<<" : "<<results[i]<<endl;
            if(this->DB->get_value_from_keyword(this->query_keywords[i])!=results[i]){
                if (this->DB->get_value_from_keyword(this->query_keywords[i]) == ""){
                    cout<<"Error: The query result is not correct, the keyword is not in the database."<<endl;
                }
                else{
                    cout<<"Error: The query result is not correct."<<endl;
                    cout<<"The query result is "<<results[i]<<" and the original value is "<<this->DB->get_value_from_keyword(this->query_keywords[i])<<endl;
                }
            }
        }
    }
    return results;
}

vector<string> DBClient::query_recovery_PIRANA(stringstream& query_results){
    if (this->debug){
        cout<<"Query recovery begin"<<endl;
    }
    vector<vector<uint64_t>> temp_results = this->PIRANA_client->extract_batch_answer(query_results);

    vector<string> aes_encrypted_results(this->query_number);
    uint64_t plain_prime_len = this->PIRANA_params->get_plain_prime_len()-1;


    map<uint64_t, uint64_t> keyword2bucket_id;
    for (uint64_t i=0;i<this->bucket_number;i++){
        if(this->bucket2index_count.find(i) == this->bucket2index_count.end()){
            continue;
        }
        auto [keyword_id,hash_func_id] = this->bucket2index_count[i];
        keyword2bucket_id[keyword_id] = i;
    }


    for (uint64_t i=0;i<this->query_number;i++){
        string temp_aes_encrypted_result;
        uint64_t bucket_id = keyword2bucket_id[i];
        vector<uint32_t> int_segments;
        for (uint64_t j=0;j<temp_results.size();j++){
            int_segments.push_back(static_cast<uint32_t>(temp_results[j][bucket_id]));
        }
        
        std::string binary_str;
        for (uint32_t seg : int_segments) {
            std::bitset<32> bits(seg);
            std::string seg_bin = bits.to_string().substr(32 - plain_prime_len, plain_prime_len);
            binary_str += seg_bin;
        }
       
        binary_str = binary_str.substr(0, this->encrypted_kv_bits_number);

       
        for (size_t i = 0; i + 8 <= binary_str.size(); i += 8) {
            std::string byte_str = binary_str.substr(i, 8);
            char byte = static_cast<char>(std::stoi(byte_str, nullptr, 2));
            temp_aes_encrypted_result += byte;
        }
   
        // string binary_results = bitset<30>(temp_results[bucket_id][-1]).to_string();
        // binary_results = binary_results.substr(0,this->encrypted_kv_bits_number-temp_aes_encrypted_result.size());
        // temp_aes_encrypted_result += binary_results;
        aes_encrypted_results[i] = temp_aes_encrypted_result;
    }
    vector<string> results(this->query_number,"");
    for(uint64_t i=0;i<this->query_number;i++){
        string encrypted_result = aes_encrypted_results[i];
        string temp_ciphertext = encrypted_result.substr(0, encrypted_result.size() - AES::BLOCKSIZE);
        string iv = encrypted_result.substr(encrypted_result.size() - AES::BLOCKSIZE, AES::BLOCKSIZE);
        CryptoPP::byte iv_bytes[AES::BLOCKSIZE];
        for (size_t j = 0; j < AES::BLOCKSIZE; ++j) {
            iv_bytes[j] = static_cast<CryptoPP::byte>(iv[j]);
        }
        string plaintext,hex_plaintext;
        CFB_Mode<AES>::Decryption cfbDecryption(this->aes_key, (this->aes_key).size(), iv_bytes);
        StringSource(temp_ciphertext, true, new StreamTransformationFilter(cfbDecryption, new StringSink(plaintext)));
        hex_plaintext = toUpper(stringToHex(plaintext));
        string value = hex_plaintext.substr(0,this->DB->value_hex_length);
        string keyword = hex_plaintext.substr(this->DB->value_hex_length,this->DB->keyword_hex_length);
        if(toUpper(keyword) == this->query_keywords[i]){
            results[i] = toUpper(value);
        }
        else{
            if (this->debug){
                cout<<"Waring: The keyword received is not correct, i.e., the keyword is not in the database on the server."<<endl;
                cout<<"The received keyword is "<<keyword<<" and the original keyword is "<<this->query_keywords[i]<<endl;
            }
        }
    }

    //////////////check S2//////////////
    for (uint64_t i=0;i<this->query_number;i++){
        if(this->S2.find(this->query_keywords[i]) != this->S2.end()){
            string value = this->S2[this->query_keywords[i]].first;
            if (value != ""){
                results[i] = value;
            }
            else {
                if (this->debug){
                    cout<<"The keyword "<<this->query_keywords[i]<<" is deleted."<<endl;
                }
                results[i] = "";
            }
        }
    }

    ///////////////////////////////////
    if (this->debug){
        cout<<"Query recovery finished."<<endl;
    }
    if (this->debug){
        cout<<"Query recovery results: "<<endl;
        for (uint64_t i=0;i<this->query_number;i++){
            cout<<this->query_keywords[i]<<" : "<<results[i]<<endl;
            if(this->DB->get_value_from_keyword(this->query_keywords[i])!=results[i]){
                if (this->DB->get_value_from_keyword(this->query_keywords[i]) == ""){
                    cout<<"Error: The query result is not correct, the keyword is not in the database."<<endl;
                }
                else{
                    cout<<"Error: The query result is not correct."<<endl;
                    cout<<"The query result is "<<results[i]<<" and the original value is "<<this->DB->get_value_from_keyword(this->query_keywords[i])<<endl;
                }
            }
        }
    }
    return results;
}

void DBClient::add(string keyword,string value){
    if (this->debug){
        //cout<<"Add begin"<<endl;
    }
    if (true){
        this->DB->add(keyword,value);
        this->number_of_keywords+=1;
    }
    set<uint64_t> Pos;
    for (uint64_t i=0;i<this->hash_func_number;i++){
        uint64_t pos = hash_to_bucket_keyword(keyword,i,this->bucket_number);
        Pos.insert(pos);
        if(this->S1[pos].find(keyword) == this->S1[pos].end()){
            this->Bucket_count[pos] += 1;
        }
        this->S1[pos][keyword] = make_pair(ADD,i);
    }
    this->S2[keyword] = make_pair(value,Pos.size());
    if (this->debug){
        //cout<<"Add finished"<<endl;
    }
}
void DBClient::del(string keyword){
    if (this->debug){
        //cout<<"Del begin"<<endl;
    }
    if (true){
        this->DB->del(keyword);
        this->number_of_keywords-=1;
    }
    set<uint64_t> Pos;
    for (uint64_t i=0;i<this->hash_func_number;i++){
        uint64_t pos = hash_to_bucket_keyword(keyword,i,this->bucket_number);
        Pos.insert(pos);
        if(this->S1[pos].find(keyword) == this->S1[pos].end()){
            this->Bucket_count[pos] -= 1;
        }
        this->S1[pos][keyword] = make_pair(DEL,i);
    }
    this->S2[keyword] = make_pair(generate_random_hex_string(this->DB->value_hex_length),Pos.size());
    if (this->debug){
        //cout<<"Del finished"<<endl;
    }
}

vector<string> DBClient::reconstruct(vector<string> bucket_content){
    if (this->debug){
        cout<<"Reconstruct begin"<<endl;
    }
    
    uint64_t bucket_id = this->reconstruct_time%this->bucket_number;

    if (this->debug){
        cout<<"decrypt the bucket content."<<endl;
    }
    map<string,pair<string,uint64_t>> keyword_to_value_and_hash_func;
    for (uint64_t i=0;i<bucket_content.size();i++){
        string temp_ciphertext = bucket_content[i].substr(0, bucket_content[i].size() - AES::BLOCKSIZE);
        string iv = bucket_content[i].substr(bucket_content[i].size() - AES::BLOCKSIZE, AES::BLOCKSIZE);
        CryptoPP::byte iv_bytes[AES::BLOCKSIZE];
        for (size_t j = 0; j < AES::BLOCKSIZE; ++j) {
            iv_bytes[j] = static_cast<CryptoPP::byte>(iv[j]);
        }
        string plaintext,hex_plaintext;

        CFB_Mode<AES>::Decryption cfbDecryption(this->aes_key, (this->aes_key).size(), iv_bytes);
        StringSource(temp_ciphertext, true, new StreamTransformationFilter(cfbDecryption, new StringSink(plaintext)));
        hex_plaintext = toUpper(stringToHex(plaintext));
        string value = hex_plaintext.substr(0,this->DB->value_hex_length);
        string keyword = hex_plaintext.substr(this->DB->value_hex_length,this->DB->keyword_hex_length);
        uint64_t hash_func_id = stoi(hexStringToIntString(hex_plaintext.substr(this->DB->value_hex_length+this->DB->keyword_hex_length,1)));
        keyword_to_value_and_hash_func[keyword] = make_pair(value,hash_func_id);
    }

    if (this->debug){
        cout<<"decrypt the bucket content finished."<<endl;
    }
    if (this->debug){
        cout<<"update the bucket content."<<endl;
    }
    //update the bucket content from the S1 and S2
    vector<string> stashed_keys;
    for (auto it=this->S1[bucket_id].begin();it!=this->S1[bucket_id].end();++it){
        stashed_keys.push_back(it->first);
    }
    for (uint64_t i=0;i<stashed_keys.size();i++){
        string keyword = stashed_keys[i];
        if(this->S1[bucket_id][keyword].first == ADD){
            string value = this->S2[keyword].first;
            uint64_t un_uploaded = this->S2[keyword].second;
            if(keyword_to_value_and_hash_func.find(keyword) != keyword_to_value_and_hash_func.end()){
                if(un_uploaded == 1){
                    this->S2.erase(keyword);
                }
                this->Bucket_count[bucket_id] -= 1; //repeated add
            }
            keyword_to_value_and_hash_func[keyword] = make_pair(value,this->S1[bucket_id][keyword].second);
        }
        else{
            uint64_t un_uploaded = this->S2[keyword].second;
            if(keyword_to_value_and_hash_func.find(keyword) != keyword_to_value_and_hash_func.end()){
                if(un_uploaded == 1){
                    this->S2.erase(keyword);
                }
                this->Bucket_count[bucket_id] += 1; //deleted nothing
            }
            keyword_to_value_and_hash_func.erase(keyword);
        }
    }
    this->S1[bucket_id].clear();

    if (bucket_id == 0){
        
       
        double Count = static_cast<double>(this->number_of_keywords);
        double b_bucket=static_cast<double>(this->bucket_number);
        double term1 = 3.0*(Count) / b_bucket;
        double term2 = std::sqrt((6.0 * Count * std::log(b_bucket)) / b_bucket);

       
        uint64_t B_size = static_cast<uint64_t>(std::ceil(term1 + term2));
        this->max_bucket_size_old = this->max_bucket_size;
        this->max_bucket_size = B_size;
        uint64_t temp_max_bucket_size = (this->max_bucket_size > this->max_bucket_size_old) ? this->max_bucket_size : this->max_bucket_size_old;
        if (this->PIRType == "PIRANA"){
            this->PIRANA_params->set_bucket_size(temp_max_bucket_size);
            this->PIRANA_params->set_encode_size();
            this->PIRANA_client->set_pir_parms(*this->PIRANA_params);
        }
    }

    
    if (keyword_to_value_and_hash_func.size() > this->max_bucket_size){ //if the bucket is full, we need to evict some keys
        while (keyword_to_value_and_hash_func.size() > this->max_bucket_size){
            string keyword;
            string value;
            uint64_t hash_func_id;
            auto it = prev(keyword_to_value_and_hash_func.end());
            keyword = it->first;
            value = it->second.first;
            hash_func_id = it->second.second;
            keyword_to_value_and_hash_func.erase(it);
            this->S1[bucket_id][keyword] = make_pair(ADD,hash_func_id);
            if (this->S2.find(keyword) != this->S2.end()){
                this->S2[keyword].second += 1;
            }
            else{
                this->S2[keyword] = make_pair(value,1);
            }
        }
    }
    vector<string> updated_keywords;
    for (auto it=keyword_to_value_and_hash_func.begin();it!=keyword_to_value_and_hash_func.end();++it){
        updated_keywords.push_back(it->first);
    }
    if (this->debug){
        cout<<"update the bucket content finished."<<endl;
    }

    //generate the MPH for the bucket
    if (this->debug){
        cout<<"Re-generate the MPH."<<endl;
    }
    sux::function::RecSplit<8> mph(updated_keywords, this->mph_parameter);
    stringstream temp_stream;
    temp_stream<<mph;
    temp_stream>>(*this->MPH)[bucket_id];

    vector<string> updated_keywords_reranged(updated_keywords.size());
    for(uint64_t i=0;i<updated_keywords.size();i++){
        uint64_t mapped_pos = (*this->MPH)[bucket_id](toUpper(updated_keywords[i]));
        updated_keywords_reranged[mapped_pos] = updated_keywords[i];
    }

    //re-encrypt the bucket and padding
    if (this->debug){
        cout<<"Re-encrypt the bucket."<<endl;
    }
    vector<string> re_encrypted_bucket(this->max_bucket_size);
    for(uint64_t i=0;i<this->max_bucket_size;i++){
        string to_encrypt;
        if(i<updated_keywords_reranged.size()){
            to_encrypt = keyword_to_value_and_hash_func[updated_keywords_reranged[i]].first +\
            updated_keywords_reranged[i] +\
            to_string(keyword_to_value_and_hash_func[updated_keywords_reranged[i]].second); 
        }
        else{
            string zero_string(this->DB->keyword_hex_length,'0');
            to_encrypt = generate_random_hex_string(this->DB->value_hex_length) + zero_string + to_string(this->hash_func_number);
        }
        //to_encrypt = to_encrypt + generate_random_hex_string(this->security_parameter/4); //add random string to the end of the ciphertext
        string to_encrypt_string = hexToString(to_encrypt);
        string temp_ciphertext;
        CryptoPP::byte iv[AES::BLOCKSIZE];
        this->prng.GenerateBlock(iv, sizeof(iv));
        cfbEncryption.SetKeyWithIV(this->aes_key, this->aes_key.size(), iv);
        StringSource(to_encrypt_string, true, new StreamTransformationFilter(this->cfbEncryption, new StringSink(temp_ciphertext)));
        re_encrypted_bucket[i]=temp_ciphertext+ string(reinterpret_cast<const char*>(iv), AES::BLOCKSIZE); //append iv to the end of the ciphertext
    }
    this->reconstruct_time += 1;
    if (this->debug){
        cout<<"Reconstruct finished"<<endl;
    }
    return re_encrypted_bucket;
}

uint64_t DBClient::get_storage_size(){
    //get S1 size
    uint64_t S1_size = 0;
    for (uint64_t i=0;i<this->bucket_number;i++){
        S1_size += this->S1[i].size()*(this->DB->keyword_bit_length + 1 + 2); // keyword + hash func id + status (ADD/DEL)
    }
    //get S2 size
    uint64_t S2_size = this->S2.size() * (this->DB->value_bit_length + this->DB->keyword_bit_length + 2); // value + keyword + flag
    
    cout<<"S1 size: "<<S1_size/8<<" bytes, S2 size: "<<S2_size/8<<" bytes"<<endl;
    //get MPH size
    uint64_t MPH_size = 0;
    stringstream MPH_stream;
    for (uint64_t i=0;i<this->bucket_number;i++){
        MPH_stream<<(*this->MPH)[i];
        MPH_size += MPH_stream.str().size();
        MPH_stream.str(""); // clear the stream for the next bucket
    }
    return (S1_size + S2_size)/8 + MPH_size;
}