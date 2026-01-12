#ifndef __SERVER_H
#define __SERVER_H

#include <seal/context.h>
#include <seal/util/ntt.h>
#include "PIRANA_pir_parms.h"
#include <bitset>
using namespace std;
class PIRANA_Server {
 private:
  /* data */

  std::unique_ptr<seal::SEALContext> _context;
  std::unique_ptr<seal::BatchEncoder> _batch_encoder;
  std::unique_ptr<seal::Evaluator> _evaluator;
  seal::GaloisKeys _galois_keys;
  seal::RelinKeys _relin_keys;

  uint64_t _p_modulus;
  uint64_t _N;
  uint64_t _pre_rotate;
  uint64_t _pre_rot_steps;
  bool _set_db;

  // Database in encrypted form from client, treated as PIR plaintexts
  vector<vector<string>> Encrypted_db_from_client;
  uint64_t max_bucket_size = 0;
  // Using only for debug
  std::unique_ptr<seal::Decryptor> _decryptor;
  std::vector<std::vector<uint64_t>> _raw_db;
  std::vector<seal::Plaintext> _encoded_db;
  
  PirParms &_pir_parms;


  uint64_t binary_str_size = 0; // size of the binary string before encoding
  bool keep_encrypted_DB = true;
 public:
  uint64_t get_storage_size();
  void set_max_bucket_size();

  vector<vector<vector<uint32_t>>> Encrypted_db_from_client_slots;
  // Preprocessing
  PIRANA_Server(PirParms &pir_parms, bool is_batch, bool random_db, vector<vector<string>> Encrypted_db);

  void encode_to_ntt_db();
  // void batch_encode_to_ntt_db_without_compress();
  // void batch_encode_to_ntt_db_with_compress();
  void batch_encode_db_from_client_to_ntt_db_without_compress();
  vector<string> get_bucket_content(uint64_t bucket_id, bool keep_encrypted_DB= false);
  void set_binary_str_size(uint64_t size) {
    this->binary_str_size = size;
  }
  uint64_t get_binary_str_size() {
    return this->binary_str_size;
  }
  void reconstruct(vector<string> bucket_content, uint64_t bucket_id);

  std::vector<seal::Ciphertext> load_query(std::stringstream &query_stream,
                                           uint32_t query_ct_size);
  std::vector<seal::Ciphertext> gen_selection_vector(
      std::vector<seal::Ciphertext> &query);

  std::vector<seal::Ciphertext> gen_selection_vector_batch(
      std::vector<seal::Ciphertext> &query);

  std::vector<seal::Ciphertext> rotate_selection_vector(
      const std::vector<seal::Ciphertext> &selection_vectors);

  std::vector<seal::Ciphertext> mul_database_with_compress(
      const std::vector<seal::Ciphertext> &rotated_selection_vectors);

  std::stringstream inner_product(
      const std::vector<seal::Ciphertext> &selection_vector);

  std::stringstream gen_response(std::stringstream &query);

  std::stringstream gen_batch_response(std::stringstream &query_stream);

  void set_keys(std::stringstream &keys) {
    _galois_keys.load(*_context, keys);
    _relin_keys.load(*_context, keys);
  }



  // only for debug
  void selection_vector_debug(std::vector<seal::Ciphertext> &selection_vectors);
  void set_decryptor(std::stringstream &sk_stream) {
    seal::SecretKey sk;
    sk.load(*_context, sk_stream);
    _decryptor = std::make_unique<seal::Decryptor>(*_context, sk);
  }
  inline std::vector<uint64_t> get_plain_response(uint32_t index) {
    return _raw_db.at(index);
  };

  inline void set_bucket_size(uint64_t bucket_size) {
    this->_pir_parms.set_bucket_size(bucket_size);
  };
  inline void set_encode_size() {
    this->_pir_parms.set_encode_size();
  };
};

#endif