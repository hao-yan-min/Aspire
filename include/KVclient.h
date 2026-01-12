#ifndef __KVCLIENT__H
#define __KVCLIENT__H
#include "RecSplit.hpp"
#include "seal/seal.h"
#include "utils.h"
#include <iostream>
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/osrng.h>

using namespace seal;
using namespace CryptoPP;

class KVClient{

public:
    
    Database* original_KV_db;
    vector<vector<string>> Encrypted_DB;

    //the communication stream from client to server.
    stringstream parms_stream; //FHE params
    stringstream data_stream; //relin_keys and galois_keys
    stringstream query_stream; //batch query

    //parameters of seal;
    EncryptionParameters* parms;
    uint64_t poly_mod_degree;
    uint64_t batch_prime_bitlength;
    uint64_t coefficient_hex_length;
    uint64_t plain_module_bit_length;
    SEALContext* context;
    KeyGenerator* keygen;
    SecretKey sk;
    Evaluator* evaluator;

    //the comminocation cost;
    uint64_t relin_keys_pos=0,galois_keys_pos=0,encrypted_query_pos=0,encrypted_db_pos;
    
    //params of the original db
    uint64_t number_of_kv_pair,keyword_bit_length,keyword_hex_length,value_bit_length,value_hex_length;
    uint64_t resp_c_number,slots_number_per_value,ct_num_per_value;


    //parameters of batch code and query
    hash<string> hash_func; //the hash function mapping keywords to buckets
    uint64_t max_bucket_size_old,max_bucket_size_new;//max_bucket_size: the bucket size in setup,max_bucket_size_temp: the bucket size in updates
    uint64_t bucket_number,hash_func_number,query_number,max_attempts=200;
    bool reach_max_attempts = false;
    uint64_t hamming_weight,cw_coding_length;
    uint64_t s;//parameters of PIRANA
    uint64_t old_t,new_t;//for changing db size

    //the minimal perfect hash for each bucket.
    vector<sux::function::RecSplit<8>> *MPH; //map keywords to its pos in bucket, the input keywords is hex with upper case
   
    
    //params for AES
    CryptoPP::AutoSeededRandomPool prng;
    CryptoPP::SecByteBlock old_key,new_key;
    CryptoPP::byte iv[AES::BLOCKSIZE];
    uint64_t cipher_text_hex_length_pair; //the ciphertext length of value||key_verify_id||hash_func_id

    //local storage for updates and reconstruct
    uint64_t reconstruct_time=0;//the reconstruct time, reconstruct_time%bucket_number is the current to reconstruct bucket
    vector<map<string, pair<Update_Type,uint64_t>>> S1; //for each bucket, recodes the keyword and its corresponding update type and its hash_func
    map<string,pair<string,uint64_t>> S2; //map the update keyword to value and un-uploaded number, for a DEL, map the keyword to an empty string
    vector<uint64_t> Bucket_count; //recode the number of pairs for each bucket, this will be changed in update

    //query and results
    vector<string> query_keywords;
    map<uint64_t,pair<uint64_t,uint64_t>> bucket2index_count;
    map<string,uint64_t> keywords_count;//sheduled keywords with its hash func number
    map<string,string> query_results;
    uint64_t noise_budget=99999;

    KVClient(Database* db,SealParams seal_parms,BatchCodeParams batch_code_params);
    void init();//generate he params and save them.
    uint64_t encode_and_encrypt_db();
    vector<string> get_random_keywords(uint64_t number);
    void generate_query(const vector<string> keywords);
    void decrypt_response(stringstream &response_stream);

    void add(string key,string value);
    void edit(string key,string value);
    void del(string key);
    void update(Update_Type type,string key,string value);//add/delete/edit a key-value pair, this information is stored in client locally.
    vector<string> reconstruct(vector<string> bucket_content);//upload part of the updated information to server.
};








#endif