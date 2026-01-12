#ifndef __DBCLIENT__H
#define __DBCLIENT__H

#include "RecSplit.hpp"
#include "seal/seal.h"
#include "utils.h"
#include "FakePIR.h"
#include "PIRANA_client.h"
#include "PIRANA_pir_parms.h"
#include "Spiral_Instance.h"
#include <iostream>
#include <set>
#include "oram_wrapper.h"
#include <vector>
#include "Spiral_Instance.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/osrng.h>
#include <bitset>


using namespace CryptoPP;

class DBClient{
    public:
    bool debug = false;
    int number_of_keywords ;
    Database* DB;
    vector<vector<string>> Encrypted_KV;
    ~DBClient();
    string PIRType = "FakePIR";

    //For Fake PIR
    vector<FakePIRClient> *PIRs;
    vector<std::unique_ptr<Spiral_client>> PIRs_uptr;
    vector<std::unique_ptr<oram_client_wrapper>> orams_uptr;
    //For PIRANA
    PIRANA_Client *PIRANA_client;
    PirParms *PIRANA_params;
    stringstream keys; //the fhe public keys for PIRANA
    
    //the communication stream from client to server.
    // stringstream parms_stream; //FHE params
    // stringstream data_stream; //relin_keys and galois_keys
    // stringstream query_stream; //batch query

    //parameters of seal;
    // EncryptionParameters* parms;
    // uint64_t poly_mod_degree;
    // uint64_t batch_prime_bitlength;
    // uint64_t coefficient_hex_length;
    // uint64_t plain_module_bit_length;
    // SEALContext* context;
    // KeyGenerator* keygen;
    // SecretKey sk;
    // Evaluator* evaluator;

    //the comminocation cost;
    //uint64_t relin_keys_pos=0,galois_keys_pos=0,encrypted_query_pos=0,encrypted_db_pos;
    
    //params of the original db
    // uint64_t number_of_kv_pair,keyword_bit_length,keyword_hex_length,value_bit_length,value_hex_length;
    // uint64_t resp_c_number,slots_number_per_value,ct_num_per_value;


    //parameters of batch code and query
    // hash<string> hash_func; //the hash function mapping keywords to buckets
    // uint64_t max_bucket_size_old,max_bucket_size_new;//max_bucket_size: the bucket size in setup,max_bucket_size_temp: the bucket size in updates
    uint64_t bucket_number,hash_func_number=3;
    
    // uint64_t hamming_weight,cw_coding_length;
    // uint64_t s;//parameters of PIRANA
    // uint64_t old_t,new_t;//for changing db size

    //the minimal perfect hash for each bucket.
    vector<sux::function::RecSplit<8>> *MPH; //map keywords to its position in each bucket, the input keywords is hex with upper case
    uint64_t mph_parameter = 100;
    
    //params for AES
    CryptoPP::AutoSeededRandomPool prng;
    CryptoPP::SecByteBlock aes_key;
    //CryptoPP::byte iv[AES::BLOCKSIZE];
    CFB_Mode<AES>::Encryption cfbEncryption;
    uint64_t security_parameter = 16; //the bit length of random string
    uint64_t encrypted_kv_bits_number = 0; //the bit length of each ciphertext
    //uint64_t cipher_text_hex_length_pair; //the ciphertext length of value||key_verify_id||hash_func_id

    //local storage for updates and reconstruct
    uint64_t reconstruct_time=0;//the reconstruct time, reconstruct_time%bucket_number is the current to reconstruct bucket
    vector<map<string, pair<Update_Type,uint64_t>>> S1; //for each bucket, recodes the keyword and its corresponding update type and its hash_func
    map<string,pair<string,uint64_t>> S2; //map the update keyword to value and un-uploaded number, for a DEL, map the keyword to an empty string
    vector<uint64_t> Bucket_count; //recode the number of pairs for each bucket, this will be changed in update
    uint64_t max_bucket_size=0;//the max bucket size in the current round
    uint64_t max_bucket_size_old=0;//the max bucket size in the last round


    //for query
    uint64_t query_number; //the number of keywords in each batch query
    uint64_t max_attempts = 200; //max evict times in PBC.GenSchedule
    bool reach_max_attempts = false; //record if max evict attempts has been reached.
    vector<string> query_keywords;
    map<uint64_t,pair<uint64_t,uint64_t>> bucket2index_count; //recode the hashed keywords id and its hash function id of each bucket in PBC.GenSchedule
    map<string,uint64_t> keywords_count; //record sheduled keywords with its hash function id in PBC.GenSchedule
    vector<uint64_t> query_index_for_pir; //record the index of the keywords of each bucket, random if the bucket is not been assigned
    vector<string> generated_query_for_pir; //record the generated PIR query for each bucket
    
    uint64_t max_query_index = 0;

    DBClient(Database* DB,BatchCodeParams batch_code_params,string PIRType);

    // void init();//generate he params and save them.
    uint64_t setup();
    vector<string> query_generate_query_FakePIR(vector<string> keywords);
    vector<string> query_recovery_FakePIR(vector<string> query_results);
    stringstream query_generate_query_PIRANA(vector<string> keywords);
    vector<string> query_recovery_PIRANA(stringstream& query_results);
    // vector<string> get_random_keywords(uint64_t number);
    // void generate_query(const vector<string> keywords);
    // void decrypt_response(stringstream &response_stream);
    void add(string keyword,string value);
    void del(string keyword);
    // void add(string key,string value);
    // void edit(string key,string value);
    // void del(string key);
    // void update(Update_Type type,string key,string value);//add/delete/edit a key-value pair, this information is stored in client locally.
    vector<string> reconstruct(vector<string> bucket_content);//upload part of the updated information to server.

    uint64_t get_storage_size();
};

#endif