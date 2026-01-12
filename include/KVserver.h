#include "KVclient.h"
#include "utils.h"
#include <sstream>
#include "RecSplit.hpp"
#include "seal/seal.h"
#include <thread>
#include <map>
#include <sys/stat.h> 
class KVClient;
using namespace seal;

class KVServer{
    public:
    KVClient * client;
    vector<vector<string>> Encrypted_db;

    uint64_t value_bit_length;//value_bit_length is bit length of value||key_verify_id||hash_func_id
    uint64_t value_hex_length,slots_number_per_value,ct_num_per_value;

    uint64_t bucket_number,s;
    uint64_t old_t,new_t;//for changing DB size
    uint64_t resp_c_number;

    vector<Plaintext>* encoded_db;

    uint64_t hamming_weight,cw_coding_length;

   
    stringstream response_stream;

    EncryptionParameters* parms;
    SEALContext* context;
    Evaluator* evaluator;
    RelinKeys rlk_server;
    GaloisKeys gal_keys_server;
    uint64_t plain_module_bit_length;

    uint64_t cipher_text_hex_length_pair;

    uint64_t enc_res_size=0;

    uint64_t bucket_size_new,bucket_size_old;
    


    KVServer(KVClient *client);
    void init(stringstream &parms_stream,stringstream &data_stream);
    uint64_t encode_db();
    void sv_mul_db(stringstream &query_stream,bool store_temp = true);
    vector<string> reconstruct_send(uint64_t bucket_id);
    void reconstruct_receive(vector<string> encrypted_bucket, uint64_t bucket_id);
    void test();
};

