#include "KVserver.h"

class KVClient;

KVServer::KVServer(KVClient *client){
    this->client = client;
}
void KVServer::init(stringstream &parms_stream,stringstream &data_stream){
    
    
    //load the Encrypted_db and set basic params from client
    this->Encrypted_db = (this->client->Encrypted_DB);
    this->bucket_number = (this->Encrypted_db).size();
    this->hamming_weight = this->client->hamming_weight;
    this->cw_coding_length = this->client->cw_coding_length;

    //set params for seal from client
    this->parms=new EncryptionParameters();
    this->parms->load(parms_stream);
    parms_stream.seekg(0, parms_stream.beg);
    this->context = new SEALContext(*(this->parms));
    this->evaluator = new Evaluator(*context);

    this->rlk_server.load(*context, data_stream);
    this->gal_keys_server.load(*context, data_stream);

    this->plain_module_bit_length = this->client->plain_module_bit_length;
    this->cipher_text_hex_length_pair = this->client->cipher_text_hex_length_pair;
}
uint64_t KVServer::encode_db(){
    this->s = uint64_t((this->parms->poly_modulus_degree()/this->bucket_number));
    this->old_t = uint64_t(ceil(float(this->Encrypted_db[0].size())/this->s));
    this->new_t = this->old_t;
    this->bucket_size_old = this->Encrypted_db[0].size();
    this->bucket_size_new = this->bucket_size_old;

    this->value_bit_length = (this->Encrypted_db)[0][0].size()*8; //value_bit_length is bit length of value||key_verify_id||hash_func_id
    this->value_hex_length = this->value_bit_length/4;

    auto &context_data = *(this->context)->key_context_data();
    uint64_t plain_module_bit_length = context_data.parms().plain_modulus().bit_count();
    this->slots_number_per_value = uint64_t(ceil(float(this->value_hex_length)/(plain_module_bit_length/4)));

    this->encoded_db = new vector<Plaintext>[this->slots_number_per_value];
    
    BatchEncoder batch_encoder(*(this->context));

    for(uint64_t i=0;i<this->old_t;i++){
        vector<vector<string>> slice(this->slots_number_per_value);
        for(uint64_t j=0;j<this->s;j++){
            for(uint64_t k=0;k<this->bucket_number;k++){
                vector<string> value_slots;
                if(j*this->old_t+i < this->Encrypted_db[k].size()){
                    string temp_hex_string = stringToHex(this->Encrypted_db[k][j*this->old_t+i]);
                    value_slots = divide_hex_string(temp_hex_string,(plain_module_bit_length/4));
                }
                else{
                    value_slots = zero_string_vector(this->slots_number_per_value);
                }
                for(uint64_t l=0;l<this->slots_number_per_value;l++){
                    slice[l].push_back(value_slots[l]);
                }
            }
        }
        //fill empty with zeros
        uint64_t zero_numbers = this->parms->poly_modulus_degree()-slice[0].size();
        for(uint64_t j=0;j<zero_numbers;j++){
            vector<string> value_slots = zero_string_vector(this->slots_number_per_value);
            for(uint64_t l=0;l<this->slots_number_per_value;l++){
                slice[l].push_back(value_slots[l]);
            }
        }
        //encode for fhe computation
        //#pragma omp parallel for
        for(uint64_t k=0;k<this->slots_number_per_value;k++){
            Plaintext _pt;
            batch_encoder.encode(hex_vector_to_int_vec(slice[k]),_pt);
            //this->evaluator->transform_to_ntt_inplace(_pt, this->context->first_parms_id());
            
            this->encoded_db[k].push_back(_pt);
        }
    }
    this->resp_c_number = uint64_t(ceil(float(this->slots_number_per_value)/this->s));
    cout<<"Size of Encoded DB: "<<this->old_t * this->slots_number_per_value * this->encoded_db[0][0].save_size()<<endl;
    return this->old_t * this->slots_number_per_value * this->encoded_db[0][0].save_size();
}

void KVServer::sv_mul_db(stringstream &query_stream,bool store_temp){

    uint64_t larger_t = max_(this->old_t,this->new_t);
    this->cw_coding_length = get_cw_length(larger_t,this->hamming_weight);
    vector<Ciphertext> Q(this->cw_coding_length);
    for (uint64_t i=0;i<cw_coding_length;i++){
        Q[i].load(*context, query_stream);
    }

    vector<Ciphertext> u(this->slots_number_per_value);
    if(store_temp==true){//use more runtime space to support parallel
        vector<vector<Ciphertext>> encrypted_res_temp(this->slots_number_per_value,vector<Ciphertext>(larger_t));
        //#pragma omp parallel for
        for (uint64_t i=0;i<larger_t;i++){
            vector<uint64_t> y = get_perfect_cw_ones_pos(i,this->cw_coding_length,this->hamming_weight);
            vector<Ciphertext> q;
            for(uint64_t j=0;j<y.size();j++){
                q.push_back(Q[y[j]]);
            }
            Ciphertext w;
            this->evaluator->multiply_many(q,this->rlk_server,w);
         
            //#pragma omp parallel for
            for(uint64_t j=0;j<this->slots_number_per_value;j++){
                this->evaluator->multiply_plain(w,this->encoded_db[j][i],encrypted_res_temp[j][i]);
            }
        }
        //#pragma omp parallel for
        for(uint64_t i=0;i<this->slots_number_per_value;i++){
            this->evaluator->add_many(encrypted_res_temp[i],u[i]);
        }
    }
    else{
      
        //initiate u
        vector<uint64_t> y = get_perfect_cw_ones_pos(0,this->cw_coding_length,this->hamming_weight);
        vector<Ciphertext> q;
        for(uint64_t j=0;j<y.size();j++){
            q.push_back(Q[y[j]]);
        }
        Ciphertext w;
        this->evaluator->multiply_many(q,this->rlk_server,w);
        this->evaluator->mod_switch_to_next_inplace(w);
        this->evaluator->relinearize_inplace(w,this->rlk_server);
        


        Ciphertext temp;
        for(uint64_t j=0;j<this->slots_number_per_value;j++){
            this->evaluator->multiply_plain(w,this->encoded_db[j][0],temp);
            {
                u[j]=temp;
            }
        }
        //#pragma omp parallel for
        for (uint64_t i=1;i<larger_t;i++){
            vector<uint64_t> y = get_perfect_cw_ones_pos(i,this->cw_coding_length,this->hamming_weight);
            vector<Ciphertext> q;
            for(uint64_t j=0;j<y.size();j++){
                q.push_back(Q[y[j]]);
            }
            Ciphertext w;
            this->evaluator->multiply_many(q,this->rlk_server,w);
            this->evaluator->mod_switch_to_next_inplace(w);
            this->evaluator->relinearize_inplace(w,this->rlk_server);
            
            //#pragma omp parallel 
            {
            Ciphertext temp;
            for(uint64_t j=0;j<this->slots_number_per_value;j++){
                this->evaluator->multiply_plain(w,this->encoded_db[j][i],temp);
                //#pragma omp critical
                {
                    this->evaluator->add_inplace(u[j],temp);
                }
            }
            }
        }
    }

    //#pragma omp parallel for
    for(uint64_t j=0;j<this->slots_number_per_value;j++){
        this->evaluator->mod_switch_to_next_inplace(u[j]);
    }
    
    //rotation and sum
    vector<Ciphertext> v(this->resp_c_number);
    //#pragma omp parallel for
    for(uint64_t i=0;i<this->resp_c_number;i++){
        v[i]=u[i*this->s];
        for(uint64_t j=1;j<this->s;j++){
            
            if(i*(this->s)+j < u.size()){
                if(j*this->bucket_number >= (this->parms->poly_modulus_degree()/2)){
                    this->evaluator->rotate_columns_inplace(u[i*(this->s)+j],this->gal_keys_server);
                    this->evaluator->rotate_rows_inplace(u[i*(this->s)+j],\
                    -((j*this->bucket_number)-(this->parms->poly_modulus_degree()/2)),this->gal_keys_server);
                }
                else{
                    this->evaluator->rotate_rows_inplace(u[i*(this->s) +j],\
                    -(j*this->bucket_number)%(this->parms->poly_modulus_degree()/2),this->gal_keys_server);
                }
                this->evaluator->add_inplace(v[i],u[i*(this->s)+j]);
            }
        }
    }

    this->enc_res_size=0;
    for (uint64_t i = 0; i < this->resp_c_number; i++){
        this->evaluator->mod_switch_to_next_inplace(v[i]);
        this->enc_res_size+=v[i].save(this->response_stream);
    }

    cout<<"Server selection vector mul, rotate, and add database finished"<<endl;

}

vector<string> KVServer::reconstruct_send(uint64_t bucket_id){
    //send the to reconstruct bucket to the client
    
    /////////////////////////debug
    //this->bucket_size_old = this->old_t * this->s;
    /////////////////////////

    vector<vector<string>> bucket_cipher_text_slots(this->slots_number_per_value,vector<string>(this->bucket_size_old));
    BatchEncoder batch_encoder(*(this->context));
    //#pragma omp parallel for
    for(uint64_t i=0;i<this->old_t;i++){
        //#pragma omp parallel for
        for(uint64_t k=0;k<this->slots_number_per_value;k++){
            vector<uint64_t> pod_result;
            batch_encoder.decode(this->encoded_db[k][i],pod_result);
            //#pragma omp parallel for
            for(uint64_t j=0;j<this->s;j++){
                if(this->old_t*j+i<this->bucket_size_old){
                    string slot_ = intStringToHexString(to_string(pod_result[bucket_id+j*this->bucket_number])); 
                    if(k==this->slots_number_per_value-1){
                        while(slot_.size()<this->cipher_text_hex_length_pair % uint32_t(this->plain_module_bit_length/4)){
                            slot_ = "0"+slot_;
                        }
                    }
                    else{
                        while(slot_.size()<this->plain_module_bit_length/4){
                            slot_ = "0"+slot_;
                        }
                    }
                    
                    bucket_cipher_text_slots[k][this->old_t*j+i] = slot_;
                }
            }
        }
    }
    vector<string> bucket_cipher_text(this->bucket_size_old);
    //#pragma omp parallel for
    for(uint64_t i=0;i<(this->bucket_size_old);i++){
        bucket_cipher_text[i] = bucket_cipher_text_slots[0][i];
        for(uint64_t j=1;j<this->slots_number_per_value;j++){
            bucket_cipher_text[i] = bucket_cipher_text[i] + bucket_cipher_text_slots[j][i];
        }
    }
    return bucket_cipher_text;
}

void KVServer::reconstruct_receive(vector<string> encrypted_bucket, uint64_t bucket_id){

    uint64_t bucket_to_reconstruc = (this->client->reconstruct_time - 1)%this->bucket_number;
    if(bucket_to_reconstruc == 0){
        this->bucket_size_new = encrypted_bucket.size();
        this->new_t = uint64_t(ceil(float(bucket_size_new)/this->s));
        if(this->new_t > this->old_t){//pad zeros in each row
            BatchEncoder batch_encoder(*(this->context));
            vector<string> value_slots = zero_string_vector(this->slots_number_per_value);
            for(uint64_t i=0;i<(this->new_t-this->old_t);i++){
                for(uint64_t k=0;k<this->slots_number_per_value;k++){
                    Plaintext _pt;
                    batch_encoder.encode(hex_vector_to_int_vec(value_slots),_pt);
                    //this->evaluator->transform_to_ntt_inplace(_pt, this->context->first_parms_id());
                    this->encoded_db[k].push_back(_pt);
                }
            }
        }
    }
    if(this->new_t < this->old_t){
        //database size decreased, add empty slots to each row of the reconstruct bucket
        //these empty slots will be removed at the end of this round of reconstruct
        vector<string> encrypted_bucket_with_pad;
        string zero_string(this->value_hex_length,'0');
        for(uint64_t i=0;i<this->s;i++){
            for(uint64_t j=0;j<this->new_t;j++){
                if(i*this->new_t +j < encrypted_bucket.size()){
                    encrypted_bucket_with_pad.push_back(encrypted_bucket[i*this->new_t +j]);
                }
                else{
                    encrypted_bucket_with_pad.push_back(zero_string);
                }
            }
            for(uint64_t j=0;j<(this->old_t-this->new_t);j++){
                encrypted_bucket_with_pad.push_back(zero_string);
            }
        }
        encrypted_bucket = encrypted_bucket_with_pad;
    }

    //re-encode the to reconstruct bucket
    uint64_t larger_t = max_(this->new_t,this->old_t);
    vector<vector<uint64_t>> slots_encrypted_bucket;
    for(uint64_t i=0;i<encrypted_bucket.size();i++){
        slots_encrypted_bucket.push_back(hex_vector_to_int_vec(divide_hex_string(stringToHex(encrypted_bucket[i]),uint64_t(plain_module_bit_length/4))));
    }
    BatchEncoder batch_encoder(*(this->context));
    //#pragma omp parallel for
    for(uint64_t k=0;k<this->slots_number_per_value;k++){
        //#pragma omp parallel for
        for(uint64_t i=0;i<larger_t;i++){
            vector<uint64_t> pod_result;
            batch_encoder.decode(this->encoded_db[k][i],pod_result);
            
            for(uint64_t j=0;j<this->s;j++){
                if(j*larger_t + i < slots_encrypted_bucket.size()){
                    pod_result[j*this->bucket_number + bucket_id] = slots_encrypted_bucket[j*larger_t + i][k];
                }
            }
            batch_encoder.encode(pod_result,this->encoded_db[k][i]);
        }
    }

    if(bucket_to_reconstruc == this->bucket_number-1){
        if(this->bucket_size_new < this->bucket_size_old){
            //remove the padded zero in this round of reconstruction
            for(uint64_t i=0;i<(this->old_t-this->new_t);i++){
                for(uint64_t k=0;k<this->slots_number_per_value;k++){
                    this->encoded_db[k].pop_back();
                }
            }
        }
        this->bucket_size_old = this->bucket_size_new;
        this->old_t = this->new_t;
    }
}
