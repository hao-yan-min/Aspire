#ifndef __UTILS__H
#define __UTILS__H

#include <stdio.h>  
#include <unistd.h>  
#include <string>
#include <vector>

#include <cstdlib>
#include <ctime> 
#include <cmath>
#include <iostream>
#include <set>
#include <sstream>
#include "seal/seal.h"
#include <openssl/sha.h>
#include <iomanip>

using namespace std;

enum Update_Type {
    NO_OPERATION,
    ADD,
    DEL,
    EDIT
};

// class RealDatabase{
//     public:

//     bool debug=false;
//     uint64_t number_of_keywords;
//     uint64_t keyword_bit_length;
//     uint64_t value_bit_length;

//     uint64_t keyword_hex_length;
//     uint64_t value_hex_length;

//     //the keywords and values are stored as hex string
//     vector<string> keywords;
//     vector<string> values;
//     map<string,string> keywords_values_map;

    
//     RealDatabase(uint64_t number_of_keywords,uint64_t keywords_bit_length,uint64_t value_bit_length,bool debug);
//     string get_value_from_index(uint64_t index);
//     string get_value_from_keyword(string keyword);
//     string get_random_keyword();

//     void add(string keyword,string value);
//     void del(string keyword);

//     pair<string,string> generate_new_kv_pair();
// };

class Database{

    public:
    uint64_t number_of_keywords;
    uint64_t keyword_bit_length;
    uint64_t value_bit_length;

    uint64_t keyword_hex_length;
    uint64_t value_hex_length;

    //the keywords and values are stored as hex string
    vector<string> keywords;
    vector<string> values;
    map<string,string> keywords_values_map;

    //buck up keywords and values for update testing
    // vector<string> keywords_backup;
    // vector<string> values_backup;
    map<string,string> keywords_values_map_backup;


    Database(uint64_t number_of_keywords,uint64_t keywords_bit_length,uint64_t value_bit_length);
    string get_value_from_index(uint64_t index);
    string get_value_from_keyword(string keyword);
    string get_random_keyword();
    vector<string> get_multi_random_keyword(uint64_t number);
    void add(string keyword,string value);
    void del(string keyword);

    pair<string,string> generate_new_kv_pair();
    void generate_back_up_dataset(uint64_t number);
    pair<string,string> get_back_up_k_v_pair();


    void clear();
};

class IndexDatabase{

    public:
    uint64_t number_of_values,value_bit_length,value_hex_length;
    vector<string> values;
    IndexDatabase(uint64_t number_of_values,uint64_t value_bit_length);
    string get_value_from_index(uint64_t index);
    uint64_t get_random_index();
    vector<uint64_t> get_multi_random_index(uint64_t number);
};
class SealParams{
    public:
    uint64_t poly_mod_degree,batch_prime_bitlength;
};
class BatchCodeParams{
    public:
    uint64_t bucket_number,hash_func_number,query_number;
    uint64_t hamming_weight,cw_coding_length;
};


string generate_random_hex_string(uint64_t length);
string generate_random_string(uint64_t length);
vector<string> divide_hex_string(string hex_string,uint64_t size);
string hex_vector_to_poly_string(vector<string> coeffs);
vector<uint64_t> hex_vector_to_int_vec(vector<string> coeffs,uint64_t size);
std::string intStringToHexString(const std::string& intString);
vector<uint64_t> hex_vector_to_int_vec(vector<string> coeffs);
std::string hexStringToIntString(const std::string& hexString);
std::string hexToString(const std::string& input);
std::string stringToHex(const std::string& input);
vector<string> poly_string_to_hex_vector(std::string poly_string, uint64_t max_size);
vector<string> uint_vector_to_hex_vector(vector<uint64_t> int_vec);

uint64_t hash_to_bucket(uint64_t id, uint64_t count, uint64_t total_buckets,hash<uint64_t> hash_fuc);
uint64_t hash_to_bucket_keyword(string id, uint64_t count, uint64_t total_buckets);


void print_parameters(const seal::SEALContext &context);

inline vector<string> zero_string_vector(uint64_t size){
    vector<string> zero_string_vector(size,"0");
    return zero_string_vector;
}


uint64_t choose(uint64_t n, uint64_t k);
uint64_t get_cw_length(uint64_t number,uint64_t k);
vector<bool> get_perfect_constant_weight_codeword(uint64_t __number, uint64_t encoding_size, uint64_t hamming_weight);
vector<uint64_t> get_perfect_cw_ones_pos(uint64_t __number, uint64_t encoding_size, uint64_t hamming_weight);
vector<uint64_t> get_ones_pos(vector<bool> cw_code);
std::string toLower(const std::string& str);
std::string toUpper(const std::string& str);
uint64_t max_(uint64_t a,uint64_t b);
uint64_t max_vector(vector<uint64_t> A);
#endif