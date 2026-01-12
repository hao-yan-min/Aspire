#include "KVclient.h"

KVClient::KVClient(Database* db,SealParams seal_parms,BatchCodeParams batch_code_params){
    this->original_KV_db = db;
    this->poly_mod_degree=seal_parms.poly_mod_degree;
    this->batch_prime_bitlength=seal_parms.batch_prime_bitlength;
    this->bucket_number = batch_code_params.bucket_number;
    this->hash_func_number = batch_code_params.hash_func_number;
    this->query_number = batch_code_params.query_number;

    this->hamming_weight = batch_code_params.hamming_weight;

    this->number_of_kv_pair = db->number_of_keywords;

    vector<map<string, pair<Update_Type,uint64_t>>> S1(this->bucket_number);
    this->S1 = S1;
    this->Bucket_count = vector<uint64_t>(this->bucket_number,0);


}

void KVClient::init(){
    //generate he params and save them.

    this->parms = new EncryptionParameters(scheme_type::bfv);
    this->parms->set_poly_modulus_degree(this->poly_mod_degree);
    this->parms->set_coeff_modulus(CoeffModulus::BFVDefault(this->poly_mod_degree));
    this->parms->set_plain_modulus(PlainModulus::Batching(this->poly_mod_degree, this->batch_prime_bitlength));
    this->parms->save(this->parms_stream);
    
    this->context = new SEALContext(*(this->parms));
    //print_parameters(*this->context);
    this->keygen = new KeyGenerator(*(this->context));
    this->sk = keygen->secret_key();
    this->evaluator = new Evaluator(*(this->context));

    Serializable<RelinKeys> relin_keys = this->keygen->create_relin_keys();
    GaloisKeys galois_keys;
    this->keygen->create_galois_keys(galois_keys);

    this->relin_keys_pos += relin_keys.save(this->data_stream);
    this->galois_keys_pos =+ galois_keys.save(this->data_stream);

    auto &context_data = *(this->context)->key_context_data();
    this->plain_module_bit_length = context_data.parms().plain_modulus().bit_count();
}

uint64_t KVClient::encode_and_encrypt_db(){

    //get parameters of the db
    this->keyword_bit_length = this->original_KV_db->keyword_bit_length;
    this->keyword_hex_length = this->original_KV_db->keyword_hex_length;
    this->value_bit_length = this->original_KV_db->value_bit_length;
    this->value_hex_length = this->original_KV_db->value_hex_length;
    this->cipher_text_hex_length_pair = this->keyword_hex_length + this->value_hex_length + 1;
    

    //map each keyword to buckets
    cout<<"Map keyword to buckets"<<endl;
    vector<set<pair<string,uint64_t>>> Bucket_id(this->bucket_number); //recode each id of the bucket
    //#pragma omp parallel for
    for(uint64_t i=0;i<this->number_of_kv_pair;i++){
        for(uint64_t j=0;j<this->hash_func_number;j++){
            uint64_t b = hash_to_bucket_keyword(this->original_KV_db->keywords[i],j,bucket_number);
            bool uninserted_flag = true;
            for(uint64_t k=0;k<j;k++){
                if(Bucket_id[b].find(make_pair(this->original_KV_db->keywords[i],k))!=Bucket_id[b].end()){
                    uninserted_flag=false;
                }
            }
            if(uninserted_flag==true){
                Bucket_id[b].insert(make_pair(this->original_KV_db->keywords[i],j));
            }
        }
    }

    uint64_t max_bucket_size=0;
    for(uint64_t i=0;i<this->bucket_number;i++){
        if(max_bucket_size<Bucket_id[i].size()){
            max_bucket_size=Bucket_id[i].size();
        }
    }
    this->max_bucket_size_old = max_bucket_size;
    
    //recode the number of pairs of each bucket in the Bucket_count
    for(uint64_t i=0;i<this->bucket_number;i++){
        this->Bucket_count[i] = Bucket_id[i].size();
    }

    cout<<"Generating MPH."<<endl;
    //generate a minimal perfect hash func for each bucket
    this->MPH = new vector<sux::function::RecSplit<8>>(this->bucket_number);
    //#pragma omp parallel for
    for(uint64_t i=0;i<this->bucket_number;i++){
        set<pair<string,uint64_t>> temp_set = Bucket_id[i];
        vector<string> Id;
        for(auto it=temp_set.begin();it!=temp_set.end();++it){
            Id.push_back(toUpper(it->first));
        }
        sux::function::RecSplit<8> mph(Id, 100);
        stringstream temp_stream;
        temp_stream<<mph;
        temp_stream>>(*this->MPH)[i];
    }
    cout<<"Generate MPH finished."<<endl;
    cout<<"Rerange k-v pair."<<endl;
    vector<vector<string>> Bucket_id_reranged(this->bucket_number); //recode each id after mph map of the bucket
    vector<vector<uint16_t>> Bucket_id_count_reranged(this->bucket_number); //recode the hash func of each id after mph map of the bucket
    //#pragma omp parallel for
    for(uint64_t i=0;i<this->bucket_number;i++){
        vector<string> bucket_id_reranged(Bucket_id[i].size());
        vector<uint16_t> bucket_id_count_reranged(Bucket_id[i].size());
        //#pragma omp parallel
        for(auto j=Bucket_id[i].begin();j!=Bucket_id[i].end();j++){
            uint64_t pos_ = (*this->MPH)[i](toUpper(j->first));
            bucket_id_reranged[pos_] = j->first;
            bucket_id_count_reranged[pos_] = j->second;
        }
        Bucket_id_reranged[i] = bucket_id_reranged;
        Bucket_id_count_reranged[i] = bucket_id_count_reranged;
    }
    cout<<"Encrypt begin"<<endl;
    //encrypt each bucket and send it to server
    SecByteBlock key_old(AES::DEFAULT_KEYLENGTH),key_new(AES::DEFAULT_KEYLENGTH);
    prng.GenerateBlock(key_old, key_old.size());
    this->old_key = key_old;
    prng.GenerateBlock(key_new, key_new.size());
    this->new_key = key_new;
    prng.GenerateBlock(this->iv, sizeof(this->iv));

    vector<vector<string>> Encrypted_DB(this->bucket_number,vector<string>(this->max_bucket_size_old));//Encrypted_DB will be sent to server
    //#pragma omp parallel for
    for(uint64_t i=0;i<this->bucket_number;i++){
        //#pragma omp parallel
        for(uint64_t j=0;j<this->max_bucket_size_old;j++){
            string to_encrypt;
            if(j<Bucket_id_reranged[i].size()){//encrypt one k-v pair in this bucket, the content is value||key_verify_id||hash_func_id
                // string key_verify_id = intStringToHexString(to_string(this->key_verify_hash(Bucket_id_reranged[i][j]) % (1<<this->key_verify_bits)));
                // /////////////////////////////////////////////////////////
                // key_verify_id = Bucket_id_reranged[i][j];
                // /////////////////////////////////////////////////////////
                // while(key_verify_id.size()<ceil(float(this->key_verify_bits)/4)){
                //     key_verify_id = "0"+key_verify_id;
                // }

                to_encrypt = this->original_KV_db->get_value_from_keyword(Bucket_id_reranged[i][j]) + \
                Bucket_id_reranged[i][j] +\
                to_string(Bucket_id_count_reranged[i][j]);
            }
            else{//encrypt random
                string zero_string(this->keyword_hex_length,'0');
                to_encrypt = generate_random_hex_string(this->value_hex_length) + zero_string + to_string(this->hash_func_number);
            }

            string to_encrypt_string = hexToString(to_encrypt);
            string temp_ciphertext;
            CFB_Mode<AES>::Encryption cfbEncryption(this->old_key, (this->old_key).size(), this->iv);
            StringSource(to_encrypt_string, true, new StreamTransformationFilter(cfbEncryption, new StringSink(temp_ciphertext)));
            Encrypted_DB[i][j]=temp_ciphertext;
        }
    }
   
    this->Encrypted_DB = Encrypted_DB;
    cout<<"Size of Encrypted DB: "<<(this->Encrypted_DB).size() * (this->Encrypted_DB)[0].size()*(this->Encrypted_DB)[0][0].size()<<"bytes"<<endl;
    this->s = uint64_t((this->parms->poly_modulus_degree()/this->bucket_number));
    this->old_t = uint64_t(ceil(float((this->Encrypted_DB)[0].size())/this->s));
    this->new_t = this->old_t;

    return (this->Encrypted_DB).size() * (this->Encrypted_DB)[0].size()*(this->Encrypted_DB)[0][0].size();
}

void KVClient::generate_query(const vector<string> keywords){
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
                this->reach_max_attempts = true;
            }
        }
    }
    this->bucket2index_count = bucket2index_count;
    this->keywords_count = keywords_count;
    cout<<"PBC generate schedule finished."<<endl;

    //generate encrypted query
    uint64_t larger_t = max_(this->old_t,this->new_t);
    this->cw_coding_length = get_cw_length(larger_t,this->hamming_weight);
    vector<vector<bool>> Query(this->cw_coding_length,vector(this->parms->poly_modulus_degree(),false));

    for (auto it = bucket2index_count.begin(); it != bucket2index_count.end(); it++){
        auto [id,count] = it->second;
        uint64_t bucket = it->first;
        uint64_t id_pos = (*this->MPH)[bucket](toUpper(keywords[id]));

        //decide the position in bucket, considering the padding in the server 
        uint64_t bucket_to_reconstruct = this->reconstruct_time % this->bucket_number;
        if(bucket >= bucket_to_reconstruct){//this bucket is not reconstructed yet in the new round
            if(this->new_t > this->old_t){ 
                //server padding this bucket to the new size, by adding zero to the end in each row (new_t-old_t elements per old_t elements)
                uint64_t row = id_pos/this->old_t;
                uint64_t padded_zero_number_before_pos = row*(this->new_t-this->old_t);
                id_pos = id_pos + padded_zero_number_before_pos;
            }
        }
        else{
            if(this->new_t < this->old_t){
                //server padding this bucket to the old size, by adding zero to the end in each row (old_t-new_t elements per new_t elements)
                uint64_t row = id_pos/this->new_t;
                uint64_t padded_zero_number_before_pos = row*(this->old_t-this->new_t);
                id_pos = id_pos + padded_zero_number_before_pos;
            }
        }
        
        //get the cw code for each row
        vector<bool> cw_coding = get_perfect_constant_weight_codeword(id_pos%larger_t,this->cw_coding_length,this->hamming_weight);
        for(uint64_t j=0;j<cw_coding.size();j++){
            Query[j][(id_pos/larger_t)*this->bucket_number+bucket] = cw_coding[j];
        }
    }

    //encrypt the query
    BatchEncoder batch_encoder(*(this->context));
    Encryptor sym_encryptor(*(this->context), this->sk);
    for(uint64_t i=0;i<this->cw_coding_length;i++){
        vector<uint64_t> temp(this->parms->poly_modulus_degree());
        transform(Query[i].begin(), Query[i].end(), temp.begin(),[](bool b) { return b ? 1 : 0; });
        Plaintext temp_plaintext;
        batch_encoder.encode(temp,temp_plaintext);
        Serializable<Ciphertext> temp_cipherttext=sym_encryptor.encrypt_symmetric(temp_plaintext);
        this->encrypted_query_pos += temp_cipherttext.save(this->query_stream);
    }
}

void KVClient::decrypt_response(stringstream &response_stream){
    
    //decrypt and decode the response
    Decryptor decryptor(*(this->context), this->sk);
    BatchEncoder batch_encoder(*(this->context));
    this->slots_number_per_value = ceil(float(this->cipher_text_hex_length_pair)/(this->plain_module_bit_length/4));
    this->resp_c_number = uint64_t(ceil(float(this->slots_number_per_value)/this->s));
    vector<Plaintext> resp(this->resp_c_number);
    vector<vector<uint64_t>> pod_result(this->resp_c_number,vector<uint64_t>(this->poly_mod_degree));
    Ciphertext temp;
    for(uint64_t i=0;i<this->resp_c_number;i++){
        temp.load(*(this->context), response_stream);
        cout<<"invariant_noise_budget"<<decryptor.invariant_noise_budget(temp)<<endl;
        if(decryptor.invariant_noise_budget(temp) < this->noise_budget){
            this->noise_budget = decryptor.invariant_noise_budget(temp);
        }
        decryptor.decrypt(temp, resp[i]);
        batch_encoder.decode(resp[i], pod_result[i]);
    }

    //get the result by inverse the rotation
    map<uint64_t, vector<uint64_t>> result;
    map<uint64_t, uint64_t> id_bucket_map;
    uint64_t larger_t = max_(this->old_t,this->new_t);

    for (auto it = this->bucket2index_count.begin(); it != this->bucket2index_count.end(); it++){
        auto [id,count] = it->second;
        result[id] = vector<uint64_t>(this->slots_number_per_value);
        uint64_t bucket = it->first;
        id_bucket_map[id] = bucket;

        //decide the position in bucket, considering the padding in the server 
        uint64_t id_pos = (*this->MPH)[bucket](toUpper(query_keywords[id]));
        uint64_t bucket_to_reconstruct = this->reconstruct_time % this->bucket_number;
        if(bucket >= bucket_to_reconstruct){//this bucket is not reconstructed yet in the new round
            if(this->new_t > this->old_t){ 
                //server padding this bucket to the new size, by adding zero to the end in each row (new_t-old_t elements per old_t elements)
                uint64_t row = id_pos/this->old_t;
                uint64_t padded_zero_number_before_pos = row*(this->new_t-this->old_t);
                id_pos = id_pos + padded_zero_number_before_pos;
            }
        }
        else{
            if(this->new_t < this->old_t){
                //server padding this bucket to the old size, by adding zero to the end in each row (old_t-new_t elements per new_t elements)
                uint64_t row = id_pos/this->new_t;
                uint64_t padded_zero_number_before_pos = row*(this->old_t-this->new_t);
                id_pos = id_pos + padded_zero_number_before_pos;
            }
        }

        //get the results
        for(uint64_t i=0;i<this->resp_c_number;i++){
            for(uint64_t j=0;j<this->s;j++){
                if(i*this->s+j < this->slots_number_per_value){
                    // here (id_pos/larger_t)*this->bucket_number +bucket is the index, (j*this->bucket_number) is the rotation offset
                    // following complex if-elses are for reversing the FHE rotation, as the fhe encoding in seal encodes and rotates two vectors together
                    uint64_t start_point = (id_pos/larger_t)*this->bucket_number + bucket;
                    uint64_t offset = (j*this->bucket_number);
                    if((start_point%pod_result[i].size())<pod_result[i].size()/2){
                        if(offset<pod_result[i].size()/2){
                            result[id][i*this->s+j] = pod_result[i][(offset+start_point)%(pod_result[i].size()/2)];
                        }
                        else{
                            result[id][i*this->s+j] = pod_result[i][(offset+start_point)%(pod_result[i].size()/2)+(pod_result[i].size()/2)];
                        }
                    }
                    else{
                        if(offset<pod_result[i].size()/2){
                            result[id][i*this->s+j] = pod_result[i][(offset+start_point)%(pod_result[i].size()/2)+(pod_result[i].size()/2)];
                        }
                        else{
                            result[id][i*this->s+j] = pod_result[i][(offset+start_point)%(pod_result[i].size()/2)];
                        }
                    }
                }
            }
        }
    }

    for(uint64_t i=0;i<this->query_keywords.size();i++){

        //check if the keyword is updated
        if(this->S2.find(toLower(this->query_keywords[i]))!=S2.end()){
            this->query_results[this->query_keywords[i]] = S2[toLower(this->query_keywords[i])].first;

            /////////////////////////////////////////////////////
            //debug: check if the received value is correct
            if(this->query_results[this->query_keywords[i]] != toLower(this->original_KV_db->get_value_from_keyword(this->query_keywords[i]))){
                cout<<"Error in receive the keyword: "<<this->query_keywords[i]<<endl;
            }
            /////////////////////////////////////////////////////

            continue;
        }
        //check if the keyword is deleted and store in the stash
        bool flag_deleted = false;
        for(uint64_t p=0;p<this->hash_func_number;p++){
            uint64_t bucket_id = hash_to_bucket_keyword(this->query_keywords[i],p,this->bucket_number);
            if(this->S1[bucket_id].find(toLower(this->query_keywords[i]))!=this->S1[bucket_id].end()){
                if(this->S1[bucket_id][toLower(this->query_keywords[i])].first == DEL){
                    flag_deleted = true;
                }
            }
        }
        if(flag_deleted==true){
            cout<<"This keyword is deleted: "<< this->query_keywords[i]<<"  ID: "<<i<<endl;
            continue;
        }
        
        //decrypte the results and check
        string enc_result = "";
        for(uint64_t j=0;j<result[i].size();j++){
            std::ostringstream ss;
            ss << std::hex <<result[i][j];
            string temp_result = ss.str();
            uint64_t pad_size = 0;
            if(j==temp_result.size()-1){
                if ((this->cipher_text_hex_length_pair % (this->plain_module_bit_length/4)) == 0){
                    pad_size=0;
                }
                else{
                    pad_size = (this->cipher_text_hex_length_pair % (this->plain_module_bit_length/4)) - temp_result.size();
                }
                //pad_size = (this->cipher_text_hex_length_pair % (this->plain_module_bit_length/4)) - temp_result.size();
            }
            else{
                pad_size = this->plain_module_bit_length/4 - temp_result.size();
            }
            while(pad_size>0){
                temp_result = "0"+temp_result;
                pad_size -= 1;
            }
            if(temp_result.size()>this->plain_module_bit_length/4){
                cout<<"Error Here!!" << "Size: "<<temp_result.size() << "Value: "<< temp_result<<endl;
            }
            enc_result = enc_result + temp_result;
        }
        string ciphertext = hexToString(enc_result);
        CryptoPP::SecByteBlock temp_key;
        if(id_bucket_map[i]<this->reconstruct_time%this->bucket_number){
            temp_key = this->new_key;
        }
        else{
            temp_key = this->old_key;
        }
        CFB_Mode<AES>::Decryption cfbDecryption(temp_key, (temp_key).size(), this->iv);
        std::string decryptedtext;
        StringSource(ciphertext, true, new StreamTransformationFilter(cfbDecryption, new StringSink(decryptedtext)));
        string hex_decryptedtext = stringToHex(decryptedtext);
        string rec_value = hex_decryptedtext.substr(0,this->value_hex_length);
        string rec_key = hex_decryptedtext.substr(this->value_hex_length,this->keyword_hex_length);
        if(toLower(this->query_keywords[i]) == rec_key){
            this->query_results[this->query_keywords[i]] = rec_value;

            /////////////////////////////////////////////////////
            // //debug: check if the received value is correct
            // if(this->query_results[this->query_keywords[i]] != toLower(this->original_KV_db->get_value_from_keyword(this->query_keywords[i]))){
            //     cout<<"Error in receive the keyword: "<<this->query_keywords[i]<<endl;
            // }
            /////////////////////////////////////////////////////
            
        }
        else{    
            cout<<"This keyword not reterived: "<< this->query_keywords[i]<<"  ID: "<<i<<endl;  
        }
    }
    cout<< "Decrypt Response Finished" <<endl;
}


vector<string> KVClient::reconstruct(vector<string> bucket_content){
    //re-encrypte the bucket reconstruct_time%bucket_number with the new key and apply the corresponding updates in stash

    //when reconstruct the first bucket
    //decide the bucket size of this round of reconstruction
    uint64_t bucket_to_reconstruct = this->reconstruct_time % this->bucket_number;
    if((bucket_to_reconstruct)==0) {
        this->max_bucket_size_new = max_vector(this->Bucket_count);
        //cout<<"Max Bucket size:"<<this->max_bucket_size_new<<endl;
        this->new_t = uint64_t(ceil(float(this->max_bucket_size_new)/this->s));
    }

    //decrypt the received bucket, store the valid content in keyword_value_count_map
    map<string,pair<string,uint64_t>> keyword_value_count_map;
    for(uint64_t i=0;i<bucket_content.size();i++){

        /////////////////////////////////////////////////////////
        // string zero_string2(bucket_content[0].size(),'0');
        // if(zero_string2 == bucket_content[i]){
        //     cout<<"Error Here in reconstruction, client receive zeros."<<endl;
        // }
        /////////////////////////////////////////////////////////

        string decryptedtext;
        CFB_Mode<AES>::Decryption cfbDecryption(this->old_key, (this->old_key).size(), this->iv);
        StringSource(hexToString(bucket_content[i]), true, new StreamTransformationFilter(cfbDecryption, new StringSink(decryptedtext)));
        string hex_decryptedtext = stringToHex(decryptedtext);
        
        string rec_key = hex_decryptedtext.substr(this->value_hex_length,this->keyword_hex_length);
        string zero_string(this->keyword_hex_length,'0');
        if(rec_key == zero_string){//if the keyword is all zero (not valide), means the content is padded by the client
            break;
        }
        string count_string = hex_decryptedtext.substr(this->value_hex_length + this->keyword_hex_length, 1);
        uint64_t count = stoi(count_string);
        string value = hex_decryptedtext.substr(0,this->value_hex_length);
        ////////////////////////////debug///////////////////////////////////
        // string real_value = this->original_KV_db->get_value_from_keyword(rec_key);
        // if(toLower(real_value)!=value){
        //     if(this->S1[bucket_to_reconstruct].find(rec_key)!=this->S1[bucket_to_reconstruct].end()){
        //         if(this->S1[bucket_to_reconstruct][rec_key].first!=DEL){
        //             cout<<"Error here in receiving the to-reconstruct content,value is not correct or keyword is not exist: "<<bucket_to_reconstruct<<endl;
        //         }
        //     }
        // }
        ///////////////////////////////////////////////////////////////
        keyword_value_count_map[rec_key] = make_pair(value,count);
    }

    //apply the stashed updates to keyword_value_count_map
    vector<string> stashed_keys;
    for(auto it=this->S1[bucket_to_reconstruct].begin();it!=this->S1[bucket_to_reconstruct].end();++it){
        stashed_keys.push_back(it->first);
    }
    for(uint64_t i=0;i<stashed_keys.size();i++){
        if(this->S1[bucket_to_reconstruct][stashed_keys[i]].first==DEL){
            if(keyword_value_count_map.find(stashed_keys[i])!=keyword_value_count_map.end()){
                keyword_value_count_map.erase(stashed_keys[i]);
            }
            else{//has deleted a not exist keyword, false delete, edit
                Bucket_count[bucket_to_reconstruct] += 1;
            }
        }
        else if(this->S1[bucket_to_reconstruct][stashed_keys[i]].first==ADD){
            if(keyword_value_count_map.find(stashed_keys[i])!=keyword_value_count_map.end()){//false add, edit
                Bucket_count[bucket_to_reconstruct] -= 1;
            }
            keyword_value_count_map[stashed_keys[i]] = make_pair(this->S2[stashed_keys[i]].first,this->S1[bucket_to_reconstruct][stashed_keys[i]].second);
            this->S2[stashed_keys[i]].second -= 1;
        }
        else if(this->S1[bucket_to_reconstruct][stashed_keys[i]].first==EDIT){
            if(keyword_value_count_map.find(stashed_keys[i])==keyword_value_count_map.end()){//false edit, add
                Bucket_count[bucket_to_reconstruct] += 1;
            }
            keyword_value_count_map[stashed_keys[i]] = make_pair(this->S2[stashed_keys[i]].first,this->S1[bucket_to_reconstruct][stashed_keys[i]].second);
            this->S2[stashed_keys[i]].second -= 1;
        }
        if(this->S2[stashed_keys[i]].second == 0){//All updated about the keyword
            this->S2.erase(stashed_keys[i]);
        }
    }
    this->S1[bucket_to_reconstruct].clear();

    //if the size exceed max bucket size in this round, some items should write back to stash
    ///////////////////////////////////////////////////////////
    // bool cout_flag_updated_keys_size = false;
    // if(keyword_value_count_map.size()>this->max_bucket_size_new){
    //     cout<<"Bucket size exceed the max bucket size in bucket: "<<bucket_to_reconstruct<<endl;
    //     cout_flag_updated_keys_size = true;
    // }
    ///////////////////////////////////////////////////////////
    while(keyword_value_count_map.size()>this->max_bucket_size_new){
        auto it = keyword_value_count_map.begin();
        string keyword = it->first;
        auto [value,count] = it->second;
        ////////////////////////////debug///////////////////////////////////
        // string real_value = this->original_KV_db->get_value_from_keyword(keyword);
        // if(toLower(real_value)!=value){
        //     cout<<"Error here in overflow: the value is not correct or the keyword is not exist"<<bucket_to_reconstruct<<endl;
        // }
        ///////////////////////////////////////////////////////////////
        this->S1[bucket_to_reconstruct][keyword] = make_pair(ADD,count);
        if(this->S2.find(keyword)!=this->S2.end()){
            S2[keyword].second += 1;
        }
        else{
            S2[keyword] = make_pair(value,1);
        }
        keyword_value_count_map.erase(keyword);
    }

    //re-encrypt the bucket
    //get new mph
    vector<string> updated_keys;
    for(auto it = keyword_value_count_map.begin();it!=keyword_value_count_map.end();++it){
        updated_keys.push_back(toUpper(it->first));
    }
    sux::function::RecSplit<8> mph(updated_keys, 100);
    stringstream temp_stream;
    temp_stream<<mph;
    temp_stream>>(*this->MPH)[bucket_to_reconstruct];
    vector<string> updated_keys_reranged(updated_keys.size());
    for(uint64_t i=0;i<updated_keys.size();i++){
        uint64_t mapped_pos = (*this->MPH)[bucket_to_reconstruct](toUpper(updated_keys[i]));
        updated_keys_reranged[mapped_pos] = toLower(updated_keys[i]);
    }
    //re-encrypt the bucket with new-key
    vector<string> re_encrypted_bucket(this->max_bucket_size_new);
    for(uint64_t i=0;i<this->max_bucket_size_new;i++){
        string to_encrypt;
        if(i<updated_keys_reranged.size()){
            to_encrypt = keyword_value_count_map[updated_keys_reranged[i]].first +\
            updated_keys_reranged[i] +\
            to_string(keyword_value_count_map[updated_keys_reranged[i]].second); 
            ////////////////////////////////////////////debug
            // if(this->original_KV_db->get_value_from_keyword(updated_keys_reranged[i])!=toUpper(keyword_value_count_map[updated_keys_reranged[i]].first)){
            //     cout<<"Error Here"<<endl;
            // }
            // if(keyword_value_count_map[updated_keys_reranged[i]].second>2){
            //     cout<<"error here 2"<<endl;
            // }
            ////////////////////////////////////////////
        }
        else{
            string zero_string(this->keyword_hex_length,'0');
            to_encrypt = generate_random_hex_string(this->value_hex_length) + zero_string + to_string(this->hash_func_number);
        }

        string to_encrypt_string = hexToString(to_encrypt);
        string temp_ciphertext;
        CFB_Mode<AES>::Encryption cfbEncryption(this->new_key, (this->new_key).size(), this->iv);
        StringSource(to_encrypt_string, true, new StreamTransformationFilter(cfbEncryption, new StringSink(temp_ciphertext)));
        re_encrypted_bucket[i]=temp_ciphertext;
    }

    //the last reconstruction in this round, update the key and t
    if(bucket_to_reconstruct == this->bucket_number-1){
        SecByteBlock temp_key(AES::DEFAULT_KEYLENGTH);
        prng.GenerateBlock(temp_key, temp_key.size());
        this->old_key = this->new_key;
        this->new_key = temp_key;
        this->old_t = this->new_t;
    }

    this->reconstruct_time += 1;
    return re_encrypted_bucket;
}



void KVClient::add(string key,string value){
    //for debug (the add and edit is the same in original_KV_db)
    this->original_KV_db->add(key,value);

    vector<uint64_t> hash_pos;
    for(uint64_t i=0;i<this->hash_func_number;i++){
        uint64_t bucket_id = hash_to_bucket_keyword(key,i,this->bucket_number);
        if(find(hash_pos.begin(),hash_pos.end(),bucket_id) == hash_pos.end()){
            hash_pos.push_back(bucket_id);
        }
    }

    //check if the key is already added or deleted
    bool in_stash = false;
    for(uint64_t i=0;i<hash_pos.size();i++){
        if(this->S1[hash_pos[i]].find(key)!=this->S1[hash_pos[i]].end()){
            in_stash = true;
        }
    }

    if(in_stash==true){
        this->edit(key,value);
    }
    else{
        for(uint64_t i=0;i<hash_pos.size();i++){
            this->S1[hash_pos[i]][key] = make_pair(ADD,i);
            this->Bucket_count[hash_pos[i]] += 1;
        }
        this->S2[key] = make_pair(value,hash_pos.size());
    }
}
void KVClient::edit(string key,string value){
    //for debug (the add and edit is the same in original_KV_db)
    this->original_KV_db->add(key,value);

    vector<uint64_t> hash_pos;
    for(uint64_t i=0;i<this->hash_func_number;i++){
        uint64_t bucket_id = hash_to_bucket_keyword(key,i,this->bucket_number);
        if(find(hash_pos.begin(),hash_pos.end(),bucket_id) == hash_pos.end()){
            hash_pos.push_back(bucket_id);
        }
        
    }

    //update the Bucket_count
    for(uint64_t i=0;i<hash_pos.size();i++){
        if(this->S1[hash_pos[i]][key].first==DEL){
            this->Bucket_count[hash_pos[i]] += 1;
        }
        this->S1[hash_pos[i]][key] =make_pair(EDIT,i);
    }
    this->S2[key] = make_pair(value,hash_pos.size());
}
void KVClient::del(string key){
    
    //for debug
    this->original_KV_db->del(key);

    vector<uint64_t> hash_pos;
    for(uint64_t i=0;i<this->hash_func_number;i++){
        uint64_t bucket_id = hash_to_bucket_keyword(key,i,this->bucket_number);
        if(find(hash_pos.begin(),hash_pos.end(),bucket_id) == hash_pos.end()){
            hash_pos.push_back(bucket_id);
        }
        
    }

    //update the Bucket_count
    for(uint64_t i=0;i<hash_pos.size();i++){
        this->Bucket_count[hash_pos[i]] -= 1;
        this->S1[hash_pos[i]][key] = make_pair(DEL,i);
    }

    if(this->S2.find(key)!=this->S2.end()){
        this->S2.erase(key);
    }
}

void KVClient::update(Update_Type type,string key,string value){
    key = toLower(key);
    value = toLower(value);
    if(type==DEL){
        if((key.size()!=this->keyword_hex_length)){
            cout<<"Error in del a keyword at client side: Keyword length is not match."<<endl;
            return;
        }
        this->del(key);
    }
    else if(type==ADD){
        if((key.size()!=this->keyword_hex_length) or (value.size()!=this->value_hex_length)){
            cout<<"Error in adding a new keyword-value pair at client side: Keyword or value length is not match."<<endl;
            return;
        }
        this->add(key,value);
    }
    else if(type==EDIT){
        if((key.size()!=this->keyword_hex_length) or (value.size()!=this->value_hex_length)){
            cout<<"Error in editting a keyword-value pair at client side: Keyword or value length is not match."<<endl;
            return;
        }
        this->edit(key,value);
    }
    else{
        cout<<"Update type not correct."<<endl;
    }
}

vector<string> KVClient::get_random_keywords(uint64_t number){
    
    set<string> keywords_set;
    for(uint64_t i=0;i<number;i++){
        bool get_flag = false;
        while(get_flag == false){
            string keyword = this->original_KV_db->get_random_keyword();
            if(keywords_set.find(keyword)==keywords_set.end()){
                keywords_set.insert(keyword);
                get_flag =true;
            }
        }
    }
    vector<string> keywords(keywords_set.begin(),keywords_set.end());
    return keywords;
}