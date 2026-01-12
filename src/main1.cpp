// #include <stdio.h>  
// #include <unistd.h>  
// #include <getopt.h> 
// #include <string>
// #include <iostream>



// //#include "server.h"
// //#include "client.h"
// #include "KVserver.h"
// #include "KVclient.h"
// #include "utils.h"

// using namespace std;

// int main(int argc, char *argv[]){ 

//     uint64_t test_case = 3;

//     bool debug=true;

//     uint64_t poly_module_degree = 1<<13;
//     uint64_t batch_prime_bitlength = 23;
//     uint64_t number_of_keywords=10000;
//     uint64_t number_of_values = 10000;
//     uint64_t keywords_bit_length=64;
//     uint64_t value_bit_length=32;

//     //bucket number needs to divide the poly_module_degree
//     uint64_t bucket_number=1<<4;
    

//     uint64_t query_number = bucket_number/(1.5);
    
    
//     uint64_t hash_func_number=3;
//     uint64_t hamming_weight = 2;

//     const char * type = argv[1];

//     int opt;
//     while(1){

//         int option_index = 0;
//         static struct option long_options[] = 
//         {
//             {"test_case",optional_argument, NULL,'t' },
//             {"debug",optional_argument, NULL,'d' },
//             {"keyword_bit_length",optional_argument, NULL,'k' },
//             {"bucket_number",optional_argument, NULL,'c'},
//             {"value_bit_length",optional_argument, NULL,'v' },
//             {"number_of_keywords",optional_argument, NULL,'n' },
//             {"number_of_values",optional_argument, NULL,'a' },
//             {"poly_module_degree",optional_argument, NULL,'p' },
//             {"batch_prime_bitlength",optional_argument, NULL,'l' }
//         };

//         opt = getopt_long(argc, argv, "c:t", long_options, &option_index);
//         if (opt == -1)
//             break;
//         switch(opt){  
//             case 't':
//                 test_case = stoi(optarg);
//                 break;
//             case 'd':
//                 debug = stoi(optarg);
//                 break;
//             case 'k':
//                 keywords_bit_length = stoi(optarg);
//                 break;
//             case 'v':
//                 value_bit_length = stoi(optarg);
//                 break;
//             case 'n':
//                 number_of_keywords = stoi(optarg);
//                 break;
//             case 'a':
//                 number_of_values = stoi(optarg);
//                 break;
//             case 'p':
//                 poly_module_degree = stoi(optarg);
//                 break;
//             case 'l':
//                 batch_prime_bitlength = stoi(optarg);
//                 break;
//             case 'c':
//                 bucket_number = stoi(optarg);
//                 query_number = bucket_number/1.5;
//             case '?':
//                 printf("unknown option: %c\n", optopt); 
//                 break; 
            
//         }
//     }
//     cout << "Test: " << test_case << endl;


//     chrono::high_resolution_clock::time_point time_start_local, time_end_local;
//     chrono::microseconds time_diff_local_e_d;
//     chrono::microseconds time_diff_local_q_g;
//     chrono::microseconds time_diff_local_mul_db;
//     chrono::microseconds time_diff_local_c_dec;
//     bool reach_max_attempts;
//     uint64_t query_size;
//     uint64_t res_size;
//     uint64_t hint_size,noise_budget;

//     // if (test_case==1){
//     //     cout<<"Test simple pir"<<endl;
//     //     cout<< "Generate Database" << endl;
//     //     Database db(number_of_keywords,keywords_bit_length,value_bit_length);
        
//     //     cout<< "Initiate Server and Client"<<endl;
//     //     Server server(&db);
        

//     //     cout<< "Generate hint" <<endl;
//     //     string s2c_hint_ad="./s2c_channel/hint_mph.mph";
//     //     //string c2s_params_ad="./c2s_channel/params";

        
//     //     hint_size = server.rerange_and_generate_hint(s2c_hint_ad);

//     //     Client client(&server,poly_module_degree,batch_prime_bitlength);
//     //     client.init();

//     //     if (debug==true){
//     //         server.sk = client.sk;
//     //     }
//     //     server.encode_db();
//     //     string random_keyword = db.get_random_keyword();
//     //     cout<< "random keyword:"<<random_keyword<<endl;

//     //     time_start_local = chrono::high_resolution_clock::now();
//     //     cout<<"corresponding value:"<<db.get_value_from_keyword(random_keyword)<<endl;
//     //     time_end_local = chrono::high_resolution_clock::now();
//     //     time_diff_local_q_g = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
//     //     cout<<"Time naive access:"<<float(time_diff_local_q_g.count())<<endl;


//     //     time_start_local = chrono::high_resolution_clock::now();
//     //     uint64_t index = client.generate_query(random_keyword);
//     //     time_end_local = chrono::high_resolution_clock::now();
//     //     time_diff_local_q_g = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
//     //     cout<<"Time sel mul db:"<<float(time_diff_local_q_g.count())/1000000<<endl;
        
//     //     query_size = client.encrypted_query_pos;
        
        
//     //     //cout<<server.values[index]<<endl;


        
//     //     time_start_local = chrono::high_resolution_clock::now();
//     //     server.sv_mul_db(client.query_stream);
//     //     time_end_local = chrono::high_resolution_clock::now();
//     //     time_diff_local_mul_db = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
//     //     cout<<"Time sel mul db:"<<float(time_diff_local_mul_db.count())/1000000<<endl;
//     //     res_size = server.enc_res_size;


//     //     time_start_local = chrono::high_resolution_clock::now();
//     //     client.decrypt_response(server.response_stream);
//     //     time_end_local = chrono::high_resolution_clock::now();
//     //     time_diff_local_c_dec = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);


        
//     // }
//     // else if (test_case==2){
//     //     cout<<"Test keyword pir with rotation"<<endl;
//     //     cout<< "Generate Database" << endl;
//     //     Database db(number_of_keywords,keywords_bit_length,value_bit_length);
        
//     //     cout<< "Initiate Server and Client"<<endl;
//     //     Server server(&db);
//     //     cout<< "Generate hint" <<endl;
//     //     string s2c_hint_ad="./s2c_channel/hint_mph.mph";
//     //     string c2s_params_ad="./c2s_channel/params";
//     //     hint_size = server.rerange_and_generate_hint(s2c_hint_ad);
//     //     Client client(&server,poly_module_degree,batch_prime_bitlength);
//     //     client.init();
//     //     if (debug==true){
//     //         server.sk = client.sk;
//     //     }
//     //     cout<<"Server encodes db"<<endl;
//     //     server.encode_db_r();

//     //     string random_keyword = db.get_random_keyword();
//     //     cout<< "random keyword:"<<random_keyword<<endl;
//     //     cout<<"corresponding value:"<<db.get_value_from_keyword(random_keyword)<<endl;
        
        
        
//     //     time_start_local = chrono::high_resolution_clock::now();

//     //     uint64_t index = client.generate_query_r(random_keyword);
//     //     time_end_local = chrono::high_resolution_clock::now();
//     //     time_diff_local_q_g = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
//     //     cout<<"Time sel mul db:"<<float(time_diff_local_q_g.count())/1000000<<endl;
//     //     query_size = client.encrypted_query_pos;

//     //     // cout<<"Value in this index in Server:"<<endl;
//     //     // cout<<server.values[index]<<endl;
        
        
//     //     time_start_local = chrono::high_resolution_clock::now();
//     //     server.sv_mul_db_r(client.query_stream);
//     //     time_end_local = chrono::high_resolution_clock::now();
//     //     time_diff_local_mul_db = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
//     //     cout<<"Time sel mul db:"<<float(time_diff_local_mul_db.count())/1000000<<endl;
//     //     res_size = server.enc_res_size;

//     //     time_start_local = chrono::high_resolution_clock::now();
//     //     client.decrypt_response_r(server.response_stream);
//     //     time_end_local = chrono::high_resolution_clock::now();
//     //     time_diff_local_c_dec = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);

//     // } else
//     if(test_case==3){
//         IndexDatabase db(number_of_values,value_bit_length);
//         PIRANAServer server(&db);
//         PIRANAClient client(&server,poly_module_degree,batch_prime_bitlength);
//         client.init();
//         if (debug==true){
//             server.sk = client.sk;
//         }
//         server.set_batch_code_params(bucket_number,query_number);
//         server.hamming_weight=hamming_weight;
//         server.hash_func_number=hash_func_number;

//         cout<<"server encodes db"<<endl;
//         time_start_local = chrono::high_resolution_clock::now();
//         server.encode_db();
//         time_end_local = chrono::high_resolution_clock::now();
//         time_diff_local_e_d = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);


//         client.id_bucket_pos_map = server.id_bucket_pos_map;
//         client.hash_func = server.hash_func;
//         client.bucket_number = server.bucket_number;
//         client.hash_func_number = server.hash_func_number;
//         client.query_number = server.query_number;
//         client.hamming_weight = hamming_weight;
//         client.cw_coding_length = server.cw_coding_length;
//         client.t = server.t;
//         client.hash_func = server.hash_func;
//         client.slots_number_per_value = server.slots_number_per_value;
//         client.value_hex_length = db.value_hex_length;


//         vector<uint64_t> index_vector = db.get_multi_random_index(client.query_number);
//         sort(index_vector.begin(), index_vector.end());
//         time_end_local = chrono::high_resolution_clock::now();
//         time_diff_local_mul_db = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);


//         cout<<"To Queried Index and its Value:"<<endl;
//         for(uint64_t i=0;i<index_vector.size();i++){
//             cout<< "Index: " << index_vector[i] << ". Value: " << db.get_value_from_index(index_vector[i]) <<endl;
//         }
//         cout<<"Query number: "<<index_vector.size()<<endl;

//         time_start_local = chrono::high_resolution_clock::now();
//         client.generate_query(index_vector);
//         time_end_local = chrono::high_resolution_clock::now();
//         time_diff_local_q_g = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);


//         time_start_local = chrono::high_resolution_clock::now();
//         server.sv_mul_db(client.query_stream,false);
//         time_end_local = chrono::high_resolution_clock::now();
//         time_diff_local_mul_db = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);


//         time_start_local = chrono::high_resolution_clock::now();
//         client.decrypt_response(server.response_stream);
//         time_end_local = chrono::high_resolution_clock::now();
//         time_diff_local_c_dec = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
        
//         query_size = client.encrypted_query_pos;
//         res_size = server.enc_res_size;
//         reach_max_attempts = client.reach_max_attempts;
//         noise_budget = client.noise_budget;
//     }
//     else if (test_case ==4)
//     {
//         Database db(number_of_keywords,keywords_bit_length,value_bit_length);
        
//         SealParams seal_params;
//         seal_params.poly_mod_degree = poly_module_degree;
//         seal_params.batch_prime_bitlength = batch_prime_bitlength;
//         BatchCodeParams batch_code_params;
//         batch_code_params.bucket_number = bucket_number;
//         batch_code_params.query_number = query_number;
//         batch_code_params.hash_func_number = hash_func_number;
//         batch_code_params.hamming_weight = hamming_weight;

//         KVClient client(&db,seal_params,batch_code_params);
//         client.init();
//         cout<<"Client encodes and encrypt database: "<<endl;
//         client.encode_and_encrypt_db();
//         KVServer server(&client);
//         server.init(client.parms_stream,client.data_stream);

//         cout<<"Server encodes database: "<<endl;
//         server.encode_db();

       
//         cout<<"Test updating and reconstructing."<<endl;

//         cout<<"generate back up dataset"<<endl;
//         client.original_KV_db->generate_back_up_dataset(number_of_keywords);
//         cout<<"generate back up dataset finished"<<endl;
//         uint64_t number_of_updates_per_reconstruction = 20;
//         uint64_t reconstruct_times = 1;
        
//         for(uint64_t i=0;i<reconstruct_times;i++){
//             for(uint64_t j=0;j<number_of_updates_per_reconstruction;j++){
                
//                 auto [keyword,value] = client.original_KV_db->get_back_up_k_v_pair();
//                 //client update locally
//                 client.update(ADD,keyword,value);
//             }
           
//             //reconstruct
//             vector<string> bucket_content;
//             bucket_content = server.reconstruct_send(client.reconstruct_time%server.bucket_number);
//             vector<string> encrypted_bucket;
//             encrypted_bucket = client.reconstruct(bucket_content);
//             server.reconstruct_receive(encrypted_bucket,(client.reconstruct_time-1)%server.bucket_number);
//         }

        

//         cout<<client.S2.size()<<endl;
//         cout<<number_of_keywords<<endl;
        


//         vector<string> keywords = client.get_random_keywords(query_number);

        

//         cout<<"To Queried Keywords and its Value:"<<endl;
//         for(uint64_t i=0;i<keywords.size();i++){
//             cout<< "Keywords: " << keywords[i] << ". Value: " << db.get_value_from_keyword(keywords[i]) <<endl;
//         }
//         cout<<"Query number: "<<keywords.size()<<endl;

       

//         client.generate_query(keywords);
//         server.sv_mul_db(client.query_stream,false);
//         client.decrypt_response(server.response_stream);
//     }
    
//     if(test_case==5){
//         //run experiments 
//         chrono::high_resolution_clock::time_point time_start_local, time_end_local;
//         chrono::microseconds time_diff_local;
//         // test setup time and size
//         vector <chrono::microseconds> Setup_time_client,Setup_time_server;
//         vector <uint64_t> MPH_size,DB_size;
//         keywords_bit_length = 261;
//         value_bit_length = 256;
//         vector<uint64_t> Number_of_keywords = {1<<10,1<<14};//DB size is 65KB,2M,67MB,2.1GB
//         for(uint64_t i=0;i<Number_of_keywords.size();i++){
//             Database db(Number_of_keywords[i],keywords_bit_length,value_bit_length);
//             db.generate_back_up_dataset(500);
//             SealParams seal_params;
//             seal_params.poly_mod_degree = poly_module_degree;
//             seal_params.batch_prime_bitlength = batch_prime_bitlength;
//             BatchCodeParams batch_code_params;
//             batch_code_params.bucket_number = bucket_number;
//             batch_code_params.query_number = query_number;
//             batch_code_params.hash_func_number = hash_func_number;
//             batch_code_params.hamming_weight = hamming_weight;

//             time_start_local = chrono::high_resolution_clock::now();
//             KVClient client(&db,seal_params,batch_code_params);
//             client.init();
//             client.encode_and_encrypt_db();
//             time_end_local = chrono::high_resolution_clock::now();
//             time_diff_local = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
//             Setup_time_client.push_back(time_diff_local);

//             stringstream MPH_stream;
//             for(uint64_t j=0;j<(*client.MPH).size();j++){
//                 MPH_stream<<(*client.MPH)[j];
//             }
//             MPH_size.push_back(MPH_stream.str().size());

//             time_start_local = chrono::high_resolution_clock::now();
//             KVServer server(&client);
//             server.init(client.parms_stream,client.data_stream);
//             uint64_t db_size;
//             db_size = server.encode_db();
//             DB_size.push_back(db_size);
//             time_end_local = chrono::high_resolution_clock::now();
//             time_diff_local = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
//             Setup_time_server.push_back(time_diff_local);

//             uint64_t update_time_total_add = 0,update_time_total_del = 0,reconstruction_time_server_send_total = 0;
//             uint64_t reconstruction_time_server_receive_total = 0,reconstruction_time_client_total = 0;
//             uint64_t reconstruct_com_size_total = 0;
            
//             for(uint64_t j=0;j<200;j++){
//                 auto [keyword,value] = client.original_KV_db->get_back_up_k_v_pair();
//                 //client update locally
//                 time_start_local = chrono::high_resolution_clock::now();
//                 client.update(ADD,keyword,value);
//                 time_end_local = chrono::high_resolution_clock::now();
//                 time_diff_local = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
//                 update_time_total_add += time_diff_local.count();
//                 //reconstruct

//                 vector<string> bucket_content;
//                 time_start_local = chrono::high_resolution_clock::now();
//                 bucket_content = server.reconstruct_send(client.reconstruct_time%server.bucket_number);
//                 time_end_local = chrono::high_resolution_clock::now();
//                 time_diff_local = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
//                 reconstruction_time_server_send_total += time_diff_local.count();
//                 reconstruct_com_size_total += bucket_content.size() * bucket_content[0].size();

//                 vector<string> encrypted_bucket;
//                 time_start_local = chrono::high_resolution_clock::now();
//                 encrypted_bucket = client.reconstruct(bucket_content);
//                 time_end_local = chrono::high_resolution_clock::now();
//                 time_diff_local = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
//                 reconstruction_time_client_total += time_diff_local.count();
//                 reconstruct_com_size_total += encrypted_bucket.size() * encrypted_bucket[0].size();

//                 time_start_local = chrono::high_resolution_clock::now();
//                 server.reconstruct_receive(encrypted_bucket,(client.reconstruct_time-1)%server.bucket_number);
//                 time_end_local = chrono::high_resolution_clock::now();
//                 time_diff_local = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
//                 reconstruction_time_server_receive_total += time_diff_local.count();
                
//             }
//             for(uint64_t j=0;j<200;j++){
//                 string keyword = client.original_KV_db->get_random_keyword();
//                 //client update locally
//                 time_start_local = chrono::high_resolution_clock::now();
//                 client.update(DEL,keyword,"");
//                 time_end_local = chrono::high_resolution_clock::now();
//                 time_diff_local = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
//                 update_time_total_del += time_diff_local.count();
//                 //reconstruct
//                 vector<string> bucket_content;
//                 time_start_local = chrono::high_resolution_clock::now();
//                 bucket_content = server.reconstruct_send(client.reconstruct_time%server.bucket_number);
//                 time_end_local = chrono::high_resolution_clock::now();
//                 time_diff_local = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
//                 reconstruction_time_server_send_total += time_diff_local.count();
//                 reconstruct_com_size_total += bucket_content.size() * bucket_content[0].size();

//                 vector<string> encrypted_bucket;
//                 time_start_local = chrono::high_resolution_clock::now();
//                 encrypted_bucket = client.reconstruct(bucket_content);
//                 time_end_local = chrono::high_resolution_clock::now();
//                 time_diff_local = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
//                 reconstruction_time_client_total += time_diff_local.count();
//                 reconstruct_com_size_total += encrypted_bucket.size() * encrypted_bucket[0].size();

//                 time_start_local = chrono::high_resolution_clock::now();
//                 server.reconstruct_receive(encrypted_bucket,(client.reconstruct_time-1)%server.bucket_number);
//                 time_end_local = chrono::high_resolution_clock::now();
//                 time_diff_local = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
//                 reconstruction_time_server_receive_total += time_diff_local.count();  
//             }


//             cout<<"Keywords number:"<<Number_of_keywords[i]<<endl;
//             cout<<"update_time_add:"<<float(update_time_total_add/200)<<"micro second"<<endl;
//             cout<<"update_time_del:"<<float(update_time_total_del/200)<<"micro second"<<endl;
//             cout<<"reconstruction_time_server_send:"<<float(reconstruction_time_server_send_total/400)<<"micro second"<<endl;
//             cout<<"reconstruction_time_server_receive:"<<float(reconstruction_time_server_receive_total/400)<<"micro second"<<endl;
//             cout<<"reconstruction_time_client:"<<float(reconstruction_time_client_total/400)<<"micro second"<<endl;
//             cout<<"reconstruct_com_size_single_:"<<float(reconstruct_com_size_total/800)<<"bytes"<<endl;
//         }
        

//         // vector <chrono::microseconds> Setup_time_client,Setup_time_server;
//         // time_start_local = time_end_local = chrono::high_resolution_clock::now();
//         // time_diff_local_q_g = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
//         // client.init();
//         // client.encode_and_encrypt_db();
//         // KVServer server(&client);
//         // server.init(client.parms_stream,client.data_stream);
//         // server.encode_db();

       
//         // cout<<"Test updating and reconstructing."<<endl;

//         // cout<<"generate back up dataset"<<endl;
//         // client.original_KV_db->generate_back_up_dataset(number_of_keywords);
//         // cout<<"generate back up dataset finished"<<endl;
//         // uint64_t number_of_updates_per_reconstruction = 20;
//         // uint64_t reconstruct_times = number_of_keywords/number_of_updates_per_reconstruction;
        
        

        

//         // cout<<client.S2.size()<<endl;
//         // cout<<number_of_keywords<<endl;
        


//         // vector<string> keywords = client.get_random_keywords(query_number);

        

//         // cout<<"To Queried Keywords and its Value:"<<endl;
//         // for(uint64_t i=0;i<keywords.size();i++){
//         //     cout<< "Keywords: " << keywords[i] << ". Value: " << db.get_value_from_keyword(keywords[i]) <<endl;
//         // }
//         // cout<<"Query number: "<<keywords.size()<<endl;

       

//         // client.generate_query(keywords);
//         // server.sv_mul_db(client.query_stream,false);
//         // client.decrypt_response(server.response_stream);

        
//     }
//     if(test_case==3){
//         cout<< "Bucket number:" << bucket_number<<endl;
//         cout<< "Batch Size:" << query_number<<endl;

//         if(reach_max_attempts==true){
//             cout<<"Reach Max Attempts in Cooko hash" <<endl;
//         }
//         cout<< "Noise Budget: "<< noise_budget<<endl;
//         cout<< "Value number:"<<number_of_values<<endl;
//     }
//     else{
//         cout<< "Keyword number:"<<number_of_keywords<<endl;
//         cout<< "Keyword length:" << keywords_bit_length<<"bits"<<endl;
//     }
//     cout<< "Value length:" << value_bit_length<<"bits"<<endl;
//     //cout<< "MPH size:" << hint_size<<endl;
//     cout<< "Database encode time: "<<float(time_diff_local_e_d.count())/1000 << "ms" <<endl;
//     cout<< "Query generation time: "<<float(time_diff_local_q_g.count())/1000<<"ms"<<endl;
//     cout<< "Query size: "<< query_size/1000<<"KB"<<endl;
//     cout<< "Server response time:" << float(time_diff_local_mul_db.count())/1000<<"ms"<<endl;
//     cout<< "Server response size:"<<res_size/1000<<"KB"<<endl;

//     cout<< "Client dec time:"<<float(time_diff_local_c_dec.count())/1000<<"ms"<<endl;
// }