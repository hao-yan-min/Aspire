#include "PIRANA_server.h"

#include <assert.h>

#include <random>

PIRANA_Server::PIRANA_Server(PirParms &pir_parms, bool is_batch, bool random_db,vector<vector<string>> Encrypted_db)
    : _pir_parms(pir_parms), Encrypted_db_from_client(Encrypted_db) {
  _context = std::make_unique<seal::SEALContext>(pir_parms.get_seal_parms());
  _evaluator = std::make_unique<seal::Evaluator>(*_context);
  _batch_encoder = std::make_unique<seal::BatchEncoder>(*_context);
  _N = pir_parms.get_seal_parms().poly_modulus_degree();
  _p_modulus = pir_parms.get_seal_parms().plain_modulus().bit_count();
  _pre_rot_steps = pir_parms.get_rotate_step();
  _pre_rotate = pir_parms.get_pre_rotate();
  this->set_max_bucket_size();
  if (random_db == true) {
    cout<<"Generate random database is not needed!"<<endl;
  } else {
    // read encrypted db from client
    _set_db = true;
    batch_encode_db_from_client_to_ntt_db_without_compress();
  }
  
}

void PIRANA_Server::batch_encode_db_from_client_to_ntt_db_without_compress() {
// read this->Encrypted_db_from_client and encode to _encoded_db
  assert(_pir_parms.get_is_compress() == false &&
         "Wrong call! only used when compress is false!");
  assert(_set_db && "Database has not been loaded correctly!");
  std::cout << "Encode database from client now!" << std::endl;
  auto db_pt_size = this->max_bucket_size * _pir_parms.get_bundle_size() *
                    _pir_parms.get_num_payload_slot();

  uint64_t plain_prime_len = this->_pir_parms.get_plain_prime_len()-1;
  vector<vector<vector<uint32_t>>> Encrypted_db_from_client_slots;

  chrono::high_resolution_clock::time_point time_start_local = chrono::high_resolution_clock::now();

  for (uint64_t i = 0; i < this->Encrypted_db_from_client.size(); i++) {
    vector<vector<uint32_t>> slots;
    for (uint64_t j = 0; j < this->Encrypted_db_from_client[i].size(); j++) {
      std::string ciphertext = Encrypted_db_from_client[i][j];

      



      

      std::vector<uint32_t> int_segments;
      uint32_t acc = 0;
      uint32_t bits_collected = 0;
      for(unsigned char c : ciphertext) {
        for (int bit = 7; bit >= 0; --bit) {
          acc = (acc << 1) | ((c >> bit) & 1);
          bits_collected++;
          if (bits_collected == plain_prime_len) {
              int_segments.push_back(acc);
              acc = 0;
              bits_collected = 0;
          }
        }
      }
      if (bits_collected > 0) {
        acc <<= (plain_prime_len - bits_collected);
        int_segments.push_back(acc);
      }
      slots.push_back(int_segments);



      
    }
    
    Encrypted_db_from_client_slots.push_back(slots);
  }
  this->Encrypted_db_from_client_slots = Encrypted_db_from_client_slots;
  
  chrono::high_resolution_clock::time_point time_end_local = chrono::high_resolution_clock::now();
  cout<< "Time:" << chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local).count()<<endl;

  // (col_size, num_slot)
  _encoded_db.resize(db_pt_size);
  std::vector<uint64_t> plain_vector(_N, 0);
  for (uint64_t pl_slot_index = 0;
       pl_slot_index < _pir_parms.get_num_payload_slot(); pl_slot_index++) {
    for (uint64_t col_index = 0; col_index < this->max_bucket_size;
         col_index++) {
      for (uint32_t bundle_index = 0;
           bundle_index < _pir_parms.get_bundle_size(); bundle_index++) {
        seal::Plaintext encoded_plain;
        for (uint64_t i = 0; i < _N; i++) {
          if ((col_index < Encrypted_db_from_client_slots[bundle_index * _N + i].size())&&(bundle_index * _N + i < Encrypted_db_from_client_slots.size())) {
            plain_vector.at(i) = Encrypted_db_from_client_slots[bundle_index * _N + i][col_index][pl_slot_index];
          } else {
            // Dummy slot can't be 0
            // If bundle size > 2, then cause ALL zeros plaintext;
            plain_vector.at(i) = 1;
          }
        }
        _batch_encoder->encode(plain_vector, encoded_plain);
        _evaluator->transform_to_ntt_inplace(encoded_plain, _context->first_parms_id());
        _encoded_db.at(pl_slot_index * this->max_bucket_size *
                           _pir_parms.get_bundle_size() +
                       col_index * _pir_parms.get_bundle_size() +
                       bundle_index) = encoded_plain;
      }
    }
  }
  this->Encrypted_db_from_client_slots.clear();
}

vector<string> PIRANA_Server::get_bucket_content(uint64_t bucket_id,bool keep_encrypted_DB) {
  assert(bucket_id < this->_pir_parms.get_bucket_number() &&
         "Bucket ID out of range!");
  vector<string> bucket_content;
  if (keep_encrypted_DB == true) { 
    bucket_content = this->Encrypted_db_from_client[bucket_id];
  }
  else {
    
  }
  return bucket_content;
}

void PIRANA_Server::reconstruct(vector<string> bucket_content, uint64_t bucket_id) {
  //reconstruct the bucket content
  assert(bucket_id < this->_pir_parms.get_bucket_number() &&
         "Bucket ID out of range!");
  
  
  if(this->keep_encrypted_DB == true) { //PIRANA has 8192 buckets. To ensure the reconstructed data volume corresponds to bucket=512, 
                                        //we update 16 buckets at once. In normal usage scenarios, updating just one bucket is sufficient.
    #ifdef TEST_FOR_DEBUG
    int bucket_num=std::stoi(bucket_content[0]);
    int start_index=1;
    for(int i=0;i<bucket_num;i++)
    {
      int size_bucket=std::stoi( bucket_content[start_index]);
      vector<string> single_bucket_content(bucket_content.begin()+start_index+1,bucket_content.begin()+start_index+1+size_bucket);
      start_index=start_index+1+size_bucket;
      this->Encrypted_db_from_client[(bucket_id+i)%(this->_pir_parms.get_bucket_number())] = single_bucket_content;
      
    }
    #else
    this->Encrypted_db_from_client[bucket_id] = bucket_content;
    #endif
    this->set_max_bucket_size();
    this->batch_encode_db_from_client_to_ntt_db_without_compress();
  }
  else{

  }
  
}

void PIRANA_Server::set_max_bucket_size() {
  // set the max bucket size
  this->max_bucket_size = 0;
  for (uint64_t i = 0; i < this->Encrypted_db_from_client.size(); i++) {
    if(this->Encrypted_db_from_client[i].size() > this->max_bucket_size) {
      this->max_bucket_size = this->Encrypted_db_from_client[i].size();
    }
  }
}




std::vector<seal::Ciphertext> PIRANA_Server::load_query(
    std::stringstream &query_stream, uint32_t query_ct_size) {
  std::vector<seal::Ciphertext> query(query_ct_size);
  for (auto &x : query) {
    x.load(*_context, query_stream);
  }
  return query;
};

void PIRANA_Server::selection_vector_debug(
    std::vector<seal::Ciphertext> &selection_vectors) {
  uint64_t N = _pir_parms.get_seal_parms().poly_modulus_degree();
  uint64_t s = selection_vectors.size();
  for (uint64_t i = 0; i < s; i++) {
    seal::Plaintext encoded_result;
    if (selection_vectors.at(i).is_ntt_form())
      _evaluator->transform_from_ntt_inplace(selection_vectors.at(i));
    _decryptor->decrypt(selection_vectors.at(i), encoded_result);
    std::vector<uint64_t> result(N, 0);
    _batch_encoder->decode(encoded_result, result);
    for (uint64_t slot = 0; slot < N; slot++) {
      if (result.at(slot) != 0) {
        std::cout << "*Debug* No. selection vectors: " << i << std::endl;
        std::cout << "*Debug* result index: " << slot << " response "
                  << result.at(slot) << std::endl;
      }
    }
  }
}

std::vector<seal::Ciphertext> PIRANA_Server::gen_selection_vector(
    std::vector<seal::Ciphertext> &query) {
  std::vector<seal::Ciphertext> selection_vector;
  uint64_t col_size = _pir_parms.get_col_size();
  selection_vector.resize(col_size);
  uint64_t encoding_size = _pir_parms.get_encoding_size();
  uint64_t col_index = 0;
  if (encoding_size == 1) {
    selection_vector[0] = query[0];
  } else {
    for (uint64_t i_1 = 1; i_1 < encoding_size && col_index < col_size; i_1++) {
      for (uint64_t i_2 = 0; i_2 < i_1 && col_index < col_size; i_2++) {
        multiply(*_context, query.at(i_1), query.at(i_2),
                 selection_vector.at(col_index));
        _evaluator->relinearize_inplace(selection_vector.at(col_index),
                                        _relin_keys);
        col_index++;
      }
    }
  }
  return selection_vector;
}

std::vector<seal::Ciphertext> PIRANA_Server::gen_selection_vector_batch(
    std::vector<seal::Ciphertext> &query) {
  std::vector<seal::Ciphertext> selection_vector;
  auto col_size = _pir_parms.get_col_size();
  auto bundle_size = _pir_parms.get_bundle_size();
  selection_vector.resize(col_size * bundle_size);
  auto encoding_size = _pir_parms.get_encoding_size();
  uint64_t col_index = 0;
  // Todo: now only support hamming weight k = 2
  // Try to support more k

 //for parallel, need to make sure different col_index do not affect each other 
 std::vector<std::pair<uint64_t, uint64_t>> pairs;
  for (uint64_t i_1 = 1; i_1 < encoding_size && col_index < col_size; i_1++) {
    for (uint64_t i_2 = 0; i_2 < i_1 && col_index < col_size; i_2++)
    {
      pairs.push_back(std::make_pair(i_1, i_2));
      col_index++;
    }
  }
  #pragma omp parallel for collapse(2) schedule(dynamic)
  for(uint64_t idx=0; idx<col_index; idx++){
      for (uint32_t bundle_index = 0; bundle_index < bundle_size;
           bundle_index++) {
        multiply(*_context, query.at(pairs[idx].first * bundle_size + bundle_index),
                 query.at(pairs[idx].second * bundle_size + bundle_index),
                 selection_vector.at(idx * bundle_size + bundle_index));
        _evaluator->relinearize_inplace(
            selection_vector.at(idx * bundle_size + bundle_index),
            _relin_keys);
        _evaluator->transform_to_ntt_inplace(
            selection_vector.at(idx * bundle_size + bundle_index));
      }
  }

  
  return selection_vector;
}

// rotate the selection vector and transform them to ntt form
std::vector<seal::Ciphertext> PIRANA_Server::rotate_selection_vector(
    const std::vector<seal::Ciphertext> &selection_vectors) {
  // Todo: make sure have_done computed correctly
  // Add some explanation comments
  uint64_t have_done = std::max(1, int(_N / _pir_parms.get_num_payloads()));

  uint64_t rot_factor = _pre_rotate / have_done;

  // std::cout << "Repead times in one ciphertext: " << have_done << std::endl;
  // std::cout << "One to n: " << _pre_rotate << std::endl;
  std::vector<seal::Ciphertext> rotated_selection_vectors(
      selection_vectors.size() * rot_factor);

  if (_pre_rotate <= 1) {
    // Don't need to rotate the selection vector
    for (size_t i = 0; i < selection_vectors.size(); i++) {
      _evaluator->transform_to_ntt(selection_vectors[i],
                                   rotated_selection_vectors[i]);
    }
    return rotated_selection_vectors;
  }
  if (have_done == 1) {
    for (uint64_t sv_i = 0; sv_i < selection_vectors.size(); sv_i++) {
      rotated_selection_vectors[sv_i * _pre_rotate] = selection_vectors[sv_i];
      _evaluator->rotate_columns(
          selection_vectors[sv_i], _galois_keys,
          rotated_selection_vectors[sv_i * _pre_rotate + _pre_rotate / 2]);

      // Continuous rotating same steps can use the same Galois key.
      // [0,0,1,0,0,0,0,0]
      // [0,0,0,0,0,0,1,0]
      for (uint64_t j = 0; j < _pre_rotate / 2 - 1; j++) {
        _evaluator->rotate_rows(
            rotated_selection_vectors[sv_i * _pre_rotate + j], -_pre_rot_steps,
            _galois_keys,
            rotated_selection_vectors[sv_i * _pre_rotate + j + 1]);
      }
      for (uint64_t j = 0; j < _pre_rotate / 2 - 1; j++) {
        _evaluator->rotate_rows(
            rotated_selection_vectors[sv_i * _pre_rotate + _pre_rotate / 2 + j],
            -_pre_rot_steps, _galois_keys,
            rotated_selection_vectors[sv_i * _pre_rotate + _pre_rotate / 2 + j +
                                      1]);
      }
    }
  } else {
    // The 'have_done > 1' means poly_degree > n, and floor(poly_degree/n) > 2;
    // The first row and the second row in the BFV ciphertext is same.
    // Don't need to swap rows.
    uint64_t todo = _pre_rotate / have_done;
    assert(selection_vectors.size() == 1);
    rotated_selection_vectors[0] = selection_vectors[0];
    for (uint64_t j = 0; j < todo - 1; j++) {
      _evaluator->rotate_rows(rotated_selection_vectors[j], -_pre_rot_steps,
                              _galois_keys, rotated_selection_vectors[j + 1]);
    }
  }

  // selection_vector_debug(rotated_selection_vectors);
  for (auto &c : rotated_selection_vectors)
    _evaluator->transform_to_ntt_inplace(c);
  return rotated_selection_vectors;
}

std::vector<seal::Ciphertext> PIRANA_Server::mul_database_with_compress(
    const std::vector<seal::Ciphertext> &rotated_selection_vectors) {
  // preprocess the n to be a power of 2
  uint64_t sel_vector_size = _pir_parms.get_col_size();
  uint64_t num_output_ciphers =
      std::ceil((double)_pir_parms.get_num_payload_slot() / _N);

  uint64_t repeat_time = std::max(1, (int)(_N / _pir_parms.get_num_payloads()));

  uint64_t real_rotate = _pre_rotate / repeat_time;
  assert(real_rotate * _pir_parms.get_col_size() ==
         rotated_selection_vectors.size());
  uint64_t total_mul = 0;
  if (repeat_time == 1) {
    total_mul = _pir_parms.get_num_payload_slot();
  } else {
    total_mul =
        std::ceil((double)_pir_parms.get_num_payload_slot() / _pre_rotate) *
        _pre_rotate;
  }

  uint64_t rot = _N / _pre_rotate;

  uint64_t total_rot =
      std::ceil((double)_pir_parms.get_num_payload_slot() / _pre_rotate);

  std::vector<seal::Ciphertext> response(num_output_ciphers);

  // rotate_mul_res_time
  uint64_t mul_count = 0;
  for (uint64_t out_i = 0; out_i < num_output_ciphers; out_i++) {
    for (uint64_t slot_i = 0, slot_len = std::min(rot, total_rot - out_i * rot);
         slot_i < slot_len; slot_i++) {
      seal::Ciphertext mul_result;
      seal::Ciphertext sum_result;

      for (uint64_t mul_i = 0; mul_i < real_rotate && mul_count < total_mul;
           mul_i++, mul_count++) {
        for (uint64_t col_i = 0; col_i < _pir_parms.get_col_size(); col_i++) {
          _evaluator->multiply_plain(
              rotated_selection_vectors.at(col_i * real_rotate + mul_i),
              _encoded_db.at(mul_count * _pir_parms.get_col_size() + col_i),
              mul_result);
          if (col_i == 0 && mul_i == 0) {
            sum_result = mul_result;
          } else {
            _evaluator->add_inplace(sum_result, mul_result);
          }
        }
      }
      _evaluator->transform_from_ntt_inplace(sum_result);
      if (slot_i == 0) {
        response.at(out_i) = sum_result;
      } else {
        _evaluator->add_inplace(response.at(out_i), sum_result);
      }
      _evaluator->rotate_rows_inplace(response.at(out_i), 1, _galois_keys);
    }
    _evaluator->mod_switch_to_inplace(response.at(out_i),
                                      _context->last_parms_id());
    try_clear_irrelevant_bits(_context->last_context_data()->parms(),
                              response.at(out_i));
  }
  return response;
};

std::stringstream PIRANA_Server::inner_product(
    const std::vector<seal::Ciphertext> &selection_vector) {
  std::stringstream result;
  auto col_size = _pir_parms.get_col_size();
  auto bundle_size = _pir_parms.get_bundle_size();
  auto num_ct = _pir_parms.get_batch_pir_num_compress_slot();

  std::vector<seal::Ciphertext> multi_add_res(num_ct * bundle_size);
  std::vector<seal::Ciphertext> result_cipher(num_ct * bundle_size);


  #pragma omp parallel  for collapse(2) schedule(static)
  for (uint32_t ct = 0; ct < num_ct; ct++) {
    for (uint32_t bundle = 0; bundle < bundle_size; bundle++) {
      std::vector<seal::Ciphertext> tmp_multi_res(col_size);
      for (uint32_t col = 0; col < col_size; col++) {
        // encode_db (num_plot, col_size, bundle_size)
        uint32_t plain_index =
            ct * bundle_size * col_size + bundle + col * bundle_size;
        uint32_t sv_index = bundle + bundle_size * col;
        _evaluator->multiply_plain(selection_vector.at(sv_index),
                                   _encoded_db.at(plain_index),
                                   tmp_multi_res.at(col));
      }
      _evaluator->add_many(tmp_multi_res,
                           multi_add_res.at(ct * bundle_size + bundle));
      _evaluator->transform_from_ntt_inplace(
          multi_add_res.at(ct * bundle_size + bundle));
      _evaluator->mod_switch_to_inplace(
          multi_add_res.at(ct * bundle_size + bundle),
          _context->last_parms_id());
      try_clear_irrelevant_bits(_context->last_context_data()->parms(),
                                multi_add_res.at(ct * bundle_size + bundle));
    }
  }


  
  for (auto &i : multi_add_res) {
    i.save(result);
  }
  return result;
}

std::stringstream PIRANA_Server::gen_response(std::stringstream &query_stream) {
  std::stringstream response;
  std::vector<seal::Ciphertext> query =
      load_query(query_stream, _pir_parms.get_encoding_size());

  std::vector<seal::Ciphertext> selection_vector = gen_selection_vector(query);
  std::vector<seal::Ciphertext> rotated_selection_vectors =
      rotate_selection_vector(selection_vector);

  std::vector<seal::Ciphertext> response_cipher =
      mul_database_with_compress(rotated_selection_vectors);

  for (auto &r : response_cipher) {
    r.save(response);
  }
  return response;
};

std::stringstream PIRANA_Server::gen_batch_response(std::stringstream &query_stream) {
  std::vector<seal::Ciphertext> query =
      load_query(query_stream,
                 _pir_parms.get_encoding_size() * _pir_parms.get_bundle_size());

  std::vector<seal::Ciphertext> selection_vector =
      gen_selection_vector_batch(query);
  std::stringstream response;
  response = inner_product(selection_vector);
  return response;
};

uint64_t PIRANA_Server::get_storage_size() {
  // return the storage size of the database
  uint64_t storage_size = 0;
  //get the Encrypted_db_from_client size
  for (const auto &bucket : this->Encrypted_db_from_client) {
    for (const auto &ciphertext : bucket) {
      storage_size += ciphertext.size();//in bytes
    }
  }
  cout << "Storage size of the Encrypted_db_from_client: " << storage_size << " Bytes" << endl;
  // Add the size of the encoded database
  for (const auto &encoded_plain : this->_encoded_db) {
    stringstream encoded_plain_stream;
    encoded_plain.save(encoded_plain_stream);
    // Get the size of the encoded plaintext in bytes
    storage_size += encoded_plain_stream.str().size(); // in bytes
    encoded_plain_stream.clear(); // Clear the stream for reuse
  }
  cout << "Storage size of the encoded database + Encrypted_db_from_client: " << storage_size << " Bytes" << endl;
  return storage_size;
}