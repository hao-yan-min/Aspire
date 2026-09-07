#include "PIRANA_pir_parms.h"

#include <map>
#include <unordered_set>

#include "assert.h"
#include "PIRANA_utils.h"


void PirParms::set_seal_parms(uint64_t poly_degree,
                              std::vector<int> coeff_modulus,
                              uint64_t prime_len) {
  _seal_parms = seal::EncryptionParameters(seal::scheme_type::bfv);
  _seal_parms.set_poly_modulus_degree(poly_degree);
  _seal_parms.set_coeff_modulus(
      seal::CoeffModulus::Create(poly_degree, coeff_modulus));
  _seal_parms.set_plain_modulus(
      seal::PlainModulus::Batching(poly_degree, prime_len));
}

void PirParms::print_seal_parms() {
  std::string scheme_name;
  if (_seal_parms.scheme() == seal::scheme_type::bfv)
    scheme_name = "BFV";
  else {
    throw std::invalid_argument("unsupported scheme");
  }

  std::cout << "/" << std::endl;
  std::cout << "|   Encryption parameters: " << std::endl;
  std::cout << "|   scheme: " << scheme_name << std::endl;
  std::cout << "|   poly_modulus_degree: " << _seal_parms.poly_modulus_degree()
            << std::endl;
  /*
    For the BFV scheme print the plain_modulus parameter.
    */
  if (_seal_parms.scheme() == seal::scheme_type::bfv) {
    std::cout << "|   plain_modulus: " << _seal_parms.plain_modulus().value()
              << std::endl;
  }
  /*
  Print the size of the true (product) coefficient modulus.
  */
  std::cout << "|   coeff_modulus size: ";
  std::cout << " (";
  uint64_t total_coeff_modulus = 0;
  auto coeff_modulus = _seal_parms.coeff_modulus();
  std::size_t coeff_modulus_size = coeff_modulus.size();
  for (std::size_t i = 0; i < coeff_modulus_size - 1; i++) {
    std::cout << coeff_modulus[i].bit_count() << " + ";
    total_coeff_modulus += coeff_modulus[i].bit_count();
  }
  std::cout << coeff_modulus.back().bit_count();
  total_coeff_modulus += coeff_modulus.back().bit_count();
  std::cout << ") " << total_coeff_modulus << " bits" << std::endl;

  std::cout << "\\" << std::endl;
};



PirParms::PirParms(const uint64_t num_payloads, const uint64_t payload_size,
                   const uint64_t num_query, const uint64_t bucket_number, const uint64_t col_size,const bool is_batch,
                   const bool is_compress, const bool enable_rotate)
    : _num_payloads(num_payloads),
      _payload_size(payload_size),
      _num_query(num_query),
      _is_compress(is_compress),
      _enable_rotate(enable_rotate),
      _bucket_number(bucket_number),
      _col_size(col_size) {
  // Todo: add rotate version

  assert(is_batch == true && num_query > 1);

  uint64_t poly_degree = 8192; 
  std::vector<int> coeff_modulus = {56, 56, 24, 24};
  uint64_t plain_prime_len = 31;

  // uint64_t poly_degree = 4096; 
  // std::vector<int> coeff_modulus = {48, 32, 24};
  // uint64_t plain_prime_len = 18;


  this->plain_prime_len = plain_prime_len;

  set_seal_parms(poly_degree, coeff_modulus, plain_prime_len);

  _num_payload_slot = std::ceil(payload_size * 8.0 / (plain_prime_len - 1));

  uint32_t N = _seal_parms.poly_modulus_degree();
  if (_is_compress == false) {
    _bundle_size = 0;
    while (_bundle_size * N < _bucket_number) {
      _bundle_size++;
    }
    _table_size = _bundle_size * N;
    _num_slot = 1;
  } else {
    _table_size = _bucket_number;
    _num_slot = std::floor(N / _bucket_number);
    _bundle_size = 1;
  }
  // _rotate_step = poly_degree / _pre_rotate;
  set_encode_size();
  print_seal_parms();
  print_pir_parms();
}

void PirParms::print_pir_parms() {
  std::cout << "--------------------------------------" << std::endl;
  std::cout << "/" << std::endl;
  std::cout << "|   PIR parameters: " << std::endl;
  std::cout << "|   Number of payloads (n): " << _num_payloads << std::endl;
  std::cout << "|   Payload size (|pl|): " << _payload_size << " Bytes"
            << std::endl;
  std::cout << "|   Number of payload slot: " << _num_payload_slot << std::endl;
  std::cout << "|   Number of query (L): " << _num_query << std::endl;
  std::cout << "|   Col Size: " << _col_size << std::endl;

  std::cout << "|   Hamming weight (k): " << _hamming_weight << std::endl;
  std::cout << "|   Encoding size (m): " << _encoding_size << std::endl;
  std::cout << "\\" << std::endl;
}
