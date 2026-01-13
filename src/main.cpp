#include <stdio.h>  
#include <unistd.h>  
#include <getopt.h> 
#include <string>
#include <iostream>
#include "spiral.h"
#include "KVserver.h"
#include "KVclient.h"
#include "utils.h"
#include "FakePIR.h"
#include "DBclient.h"
#include "DBserver.h"
#include "Spiral_Instance.h"
#include <omp.h>
using namespace std;

// int main1(int argc, char** argv)
// {
//      for(int i=0;i<argc;i++)
//         cout<<argv[i]<<" ";
//     cout<<endl;
//     size_t num_expansions = 5; // max = 7 //used to be 8
//     size_t further_dims = 4;
//     size_t total_n = (1 << num_expansions) * (1 << further_dims);
//     int IDX_TARGET = 14;
//     int IDX_DIM0 = IDX_TARGET / (1 << further_dims);
//     if (argc > 1) {
//         num_expansions = strtol(argv[1], NULL, 10); // max = 7 //used to be 8
//         further_dims = strtol(argv[2], NULL, 10);
//         total_n = (1 << num_expansions) * (1 << further_dims);
//         IDX_TARGET = strtol(argv[3], NULL, 10);
//         IDX_DIM0 = IDX_TARGET / (1 << further_dims);
//         if (argc >= 5) {
            
//             // if (dbFilename.length() <= 2) has_file = false;
//             for (size_t i = 5; i < argc; i++) {
                
//                 if (strcmp(argv[i], "--nonoise") == 0) {
//                     cout << "Using no noise" << endl;
//                     nonoise = true;
//                 }
                
//                 if (strcmp(argv[i], "--random-data") == 0) {
//                     cout << "Using random data..." << endl;
//                     random_data = true;
//                     dummyWorkingSet = min((1UL << 25) / (total_n), poly_len);
//                     max_trials = (1L << 16) / (total_n);
//                     if (max_trials == 0) {
//                         max_trials = 1;
//                     }
//                 }
//                 if (strcmp(argv[i], "--show-diff") == 0) {
//                     cout << "Showing diff..." << endl;
//                     show_diff = true;
//                 }
                
//                 if (strcmp(argv[i], "--direct-upload") == 0) {
//                     cout << "Direct uploading of query (no compression)" << endl;
//                     direct_upload = true;
//                 }
               
//             }
//         }
//     }
//     PIR::spiral_PIR pir(num_expansions, further_dims);
//     pir.IDX_TARGET = IDX_TARGET;
//     pir.IDX_DIM0 = IDX_DIM0;
//     // if (ubench) {
//     //     testPacking();
//     //     exit(0);
//     // }
//     // cout << "params:" << endl;
//     // cout << "\tnum_expansions: " << num_expansions << endl;
//     // cout << "\tfurther_dims: " << further_dims << endl;
//     // cout << "\ttotal_n: " << total_n << endl;
//     // cout << "\tIDX_TARGET: " << IDX_TARGET << endl;
//     // cout << "\tIDX_DIM0: " << IDX_DIM0 << endl;
//     // cout << "\tthreads: " << omp_get_num_threads() << endl;

    

    
//     // do_MatPol_test();

//     // setup_constants();

//     // generate_gadgets();

    

//     // load_db();
//     vector<string> database;
//     for(size_t i=0;i<total_n;i++)
//     {
//         //生成随机的长度为2048byte的字符串
//         string item(2048,'b');
//         random_device rd;
//         mt19937_64 gen2(rd()); // NOT SECURE
//         uniform_int_distribution<uint64_t> dist(0, 255);
//         for(size_t j=0;j<2048;j++)
//         {
//             item[j]=(char)dist(gen2);
//         }
//         database.push_back(item);
//     }
//     vector<string> database2=database;
//     // do_test();
//     //pir.load_db();
//     // vector<string> database2;
//     //pir.encode_db(database);
//     cout<<"database size: "<<database.size()<<endl;
//     cout<<"first item size: "<<database[0].size()<<endl;
//     double setup_time=0;
//     double query_time=0;
//     start_timing();
//     //pir.set_DB(pir.encoded_DB);


//     //pir.set_DB(database);
//     setup_time+=end_timing();
//     cout<<"setup done."<<endl;
//     cout<<"setup time: "<<setup_time/1000.0<<" ms"<<endl;
//     string res;
//     auto start_stage = chrono::high_resolution_clock::now();
//     auto start = chrono::high_resolution_clock::now();
//     size_t stage = 0;
//     start_stage = chrono::high_resolution_clock::now();
//     start = chrono::high_resolution_clock::now();
//     start_timing();
//     //随机生成查询index；
//     // for(int i=0;i<15;i++)
//     // {
//     //     int query_index = rand() % database.size();
//     //     res.clear();
//     //     pir.query(query_index,res);
//     //     if(memcmp(res.data(),database2[query_index].data(),pir.item_size)==0)
//     //     {
//     //         cout<<"query success!"<<endl;
//     //     }
//     //     else
//     //     {
//     //         cout<<query_index<<endl;
//     //         cout<<"query failed!"<<endl;
//     //     }
//     // }
    
//     // pir.query(IDX_TARGET,res);
//     // uint64_t duration_us = std::chrono::duration_cast<std::chrono::microseconds>(chrono::high_resolution_clock::now() - start_stage).count();
//     // // double duration = (chrono::high_resolution_clock::now() - start_stage).count() / 1000;
//     // // cout << "#\t\t\t\t\t";
//     // // cout << "  >"
//     // //      << duration
//     // //      << endl;
//     // // cout << "#\t\t\t\t\t END" << endl;
//     // start_stage = chrono::high_resolution_clock::now();
//     // start = chrono::high_resolution_clock::now();
//     // stage = 0;
//     // query_time+=duration_us;
//     // cout<<"query done."<<endl;
//     // cout<<"query time: "<<query_time/1000.0<<" ms"<<endl;
//     // cout<<"res size: "<<res.length()<<endl;
//     // memcmp(res.data(),database2[IDX_TARGET].data(),pir.item_size)==0?cout<<"query success!"<<endl:cout<<"query failed!"<<endl;
//     // // memcmp(res.data(),database[IDX_TARGET+1].data(),pir.item_size)==0?cout<<"query success!"<<endl:cout<<"query failed!"<<endl;
//     // // memcmp(res.data(),database[IDX_TARGET-1].data(),pir.item_size)==0?cout<<"query success!"<<endl:cout<<"query failed!"<<endl;
//     // // time_key_gen = 0;
//     // // time_query_gen = 0;
//     // // time_expansion_main = 0;
//     // // time_expansion_further = 0;
//     // // time_conversion = 0;
//     // // time_first_multiply = 0;
//     // // time_folding = 0;
//     // // time_decoding = 0;
//     // // PIR::spiral_PIR pir2(num_expansions+1, further_dims);
//     // // pir2.IDX_TARGET = IDX_TARGET;
//     // // pir2.IDX_DIM0 = IDX_DIM0;
//     // // pir2.load_db();
//     // // pir2.do_test();
//     pir.set_DB(database);
//     // pir.query(105,res);
//     // cout<<IDX_TARGET<<endl;
//     //cout<<res<<endl;
//     // std::strcmp(res.data(),database2[105].data())==0?cout<<"query success!"<<endl:cout<<"query failed!"<<endl;
//     int test_size=100;
//     Timer timer;
//     timer.reset();
//     for(int i=0;i<test_size;i++)
//     {
//         int query_index = rand() % database.size();
//         res.clear();
//         pir.query(query_index,res);
//         std::strcmp(res.data(),database2[query_index].data())==0?cout<<"query "<<i<<" success!"<<endl:cout<<"query "<<i<<" failed!"<<endl;
//     }
//     auto elapsed = timer.elapsed();
//     cout<<"Total query time over "<<test_size<<" queries: "<<elapsed <<" ms"<<endl;
//     //以16进制打印
//     // for(size_t i=0;i<res.size();i++)
//     // {
//     //     printf("%02x ",(unsigned char)res[i]);
//     //     if((i+1)%16==0)
//     //         printf("\n");
//     // }
//     // cout<<endl;
//     // for(size_t i=0;i<res.size();i++)
//     // {
//     //     printf("%02x ",(unsigned char)database2[105][i]);
//     //     if((i+1)%16==0)
//     //         printf("\n");
//     // }
//     // cout<<endl;
//     //pir.query(IDX_TARGET,res);
//     // cout<<"==========================="<<endl;
//     // MatPoly p_tp(n0,n0,false);
//     // generate_random_pt(p_tp);
//     // MatPoly p_tp_1(n0,n0,false);
//     // edb_gen(p_tp_1,database[IDX_TARGET]);
//     // cout<<"original pt vs generated pt:"<<endl;
//     // for(int i=0;i<n0*n0*poly_len;i++)
//     // {
//     //     if(p_tp.data[i]!=p_tp_1.data[i])
//     //     {
//     //         cout<<i<<": "<<p_tp.data[i]<<" vs "<<p_tp_1.data[i]<<endl;
//     //     }
//     // }
// }
extern long long int commu_cost;
extern double enc_time;
// int main(int argc, char *argv[])
// {
//     uint64_t keywords_bit_length=stoi(argv[1]);
//     uint64_t value_bit_length=stoi(argv[2]);
//     uint64_t number_of_keywords=stoi(argv[3]);
//     uint64_t number_of_values = stoi(argv[4]);
//     number_of_keywords=1 <<number_of_keywords;
//     number_of_values=1 <<number_of_values;

//     return 0;
// }
extern std::ofstream logs;
int main_0(int bn,int qn,int nk,
          int kbl,int vbl,string pir_type);
int main(int argc, char** argv)
{
    
    for(int i=0;i<argc;i++)
        cout<<argv[i]<<" ";
    printf("\n");
    int bn,qn,nk,
        kbl,vbl;
    if(argc<10)
    {
        omp_set_num_threads(16);
        cout<<"Usage: "<<argv[0]<<" <bucket_number> <query_number> <log2(number_of_keywords/values)> <keyword_bit_length> <value_bit_length>"<<endl;
        main_0(512,341,23,32,2048,"ORAM");//512,300,nk,32,2048
        Spiral_server::dim0=6;
        Spiral_server::further_dims=2;
        return 0;
    }
    
    bn=stoi(argv[1]);
    qn=stoi(argv[2]);
    nk=stoi(argv[3]);
    kbl=stoi(argv[4]);
    vbl=stoi(argv[5]);
    int dim0=stoi(argv[6]);
    int dim1=stoi(argv[7]);
    int num_thread=stoi(argv[8]);
    string pir_type=argv[9];
    Spiral_server::dim0=dim0;
    Spiral_server::further_dims=dim1;
    omp_set_num_threads(num_thread);
    // auto kv={18,19,20,21,22,23};
    // auto kv={18};
    // cout<<"----------------test stage 1----------------"<<endl;
    // logs<<"----------------test stage 1----------------"<<endl;
    logs<<"----------------New Test----------------"<<endl;
    main_0(bn,qn,nk,kbl,vbl,pir_type);
    logs.close();
    // for(auto nk:kv){
    //     commu_cost=0;
    //     // cout<<"----------------test stage 1----------------"<<endl;
    //     // logs<<"----------------test stage 1----------------"<<endl;
    //     cout<<"number of keywords/values: "<<(1<<nk)<<endl;
    //     logs<<"number of keywords/values: "<<(1<<nk)<<endl;
    //     cout<<"Keyword length: 32 bit, Value length: 2048 bit"<<endl;
    //     logs<<"Keyword length: 32 bit, Value length: 2048 bit"<<endl;
    //     cout<<"Bucket number: 512, Query number: 300"<<endl;
    //     logs<<"Bucket number: 512, Query number: 300"<<endl;
    //     main_0(512,300,nk,32,2048);
    //     cout<<endl;
    // }
    // auto bn={64,128,256,512,1024,2048};
    //auto bn={512,1024,2048};

    // auto bn={4096};
    // cout<<"----------------test stage 2----------------"<<endl;
    // logs<<"----------------test stage 2----------------"<<endl;
    // for(auto item:bn)
    // {
    //     commu_cost=0;
        
    //     cout<<"number of keywords/values: "<<(1<<22)<<endl;
    //     logs<<"number of keywords/values: "<<(1<<22)<<endl;
    //     cout<<"Keyword length: 32 bit, Value length: 2048 bit"<<endl;
    //     logs<<"Keyword length: 32 bit, Value length: 2048 bit"<<endl;
    //     cout<<"Bucket number: "<<item<<", Query number: "<<item/1.5<<endl;
    //     logs<<"Bucket number: "<<item<<", Query number: "<<item/1.5<<endl;
    //     main_0(item,1365,22,32,2048);
    //     cout<<endl;
        
    // }
    // auto vl={1024,2048,4096,8192,16384};
    // cout<<"----------------test stage 3----------------"<<endl;
    // for(auto vbl:vl){
    //     cout<<"number of keywords/values: "<<(1<<20)<<endl;
    //     cout<<"Keyword length: 32 bit, Value length: "<<vbl<<" bit"<<endl;
    //     cout<<"Bucket number: 512, Query number: 300"<<endl;
    //     commu_cost=0;
    //     main_0(512,300,20,32,vbl);
    //     cout<<endl;
    // }
    
    return 0;

}
int main_0(int bn,int qn,int nk,
          int kbl,int vbl,string pir_type){ 
    logs<<"Running "<<pir_type<<"..."<<endl;
    uint64_t test_case = 1;
    if(pir_type=="PIRANA"){
        test_case=6;

    }
    
    
    bool debug=false;


    // uint64_t poly_module_degree = 1<<13;
    // uint64_t batch_prime_bitlength = 23;
    // uint64_t number_of_keywords=1<<20; //
    // uint64_t number_of_values = 1<<20; //
    // uint64_t keywords_bit_length=1024;
    // uint64_t value_bit_length=1024;

    // //bucket number needs to divide the poly_module_degree
    // uint64_t bucket_number=128;

    // uint64_t query_number = 80;


    uint64_t poly_module_degree = 1<<13;
    uint64_t batch_prime_bitlength = 23;
    uint64_t number_of_keywords=1<<nk; //20
    uint64_t number_of_values = 1<<nk; //20
    uint64_t keywords_bit_length=kbl;
    uint64_t value_bit_length=vbl;

    //bucket number needs to divide the poly_module_degree
    uint64_t bucket_number=bn;

    uint64_t query_number = qn;



    // uint64_t poly_module_degree = 1<<13;
    // uint64_t batch_prime_bitlength = 23;
    // uint64_t number_of_keywords=2048; //
    // uint64_t number_of_values = 2048; //
    // uint64_t keywords_bit_length=32;
    // uint64_t value_bit_length=32;

    // //bucket number needs to divide the poly_module_degree
    // uint64_t bucket_number=2048;

    // uint64_t query_number = 1024;

    uint64_t hash_func_number=3;
    uint64_t hamming_weight = 2;

    // const char * type = argv[1];

    int opt;
    // while(1){

    //     int option_index = 0;
    //     static struct option long_options[] = 
    //     {
    //         {"test_case",optional_argument, NULL,'t' },
    //         {"debug",optional_argument, NULL,'d' },
    //         {"keyword_bit_length",optional_argument, NULL,'k' },
    //         {"bucket_number",optional_argument, NULL,'c'},
    //         {"value_bit_length",optional_argument, NULL,'v' },
    //         {"number_of_keywords",optional_argument, NULL,'n' },
    //         {"number_of_values",optional_argument, NULL,'a' },
    //         {"poly_module_degree",optional_argument, NULL,'p' },
    //         {"batch_prime_bitlength",optional_argument, NULL,'l' }
    //     };

    //     opt = getopt_long(argc, argv, "c:t", long_options, &option_index);
    //     if (opt == -1)
    //         break;
    //     switch(opt){  
    //         case 't':
    //             test_case = stoi(optarg);
    //             break;
    //         case 'd':
    //             debug = stoi(optarg);
    //             break;
    //         case 'k':
    //             keywords_bit_length = stoi(optarg);
    //             break;
    //         case 'v':
    //             value_bit_length = stoi(optarg);
    //             break;
    //         case 'n':
    //             number_of_keywords = stoi(optarg);
    //             break;
    //         case 'a':
    //             number_of_values = stoi(optarg);
    //             break;
    //         case 'p':
    //             poly_module_degree = stoi(optarg);
    //             break;
    //         case 'l':
    //             batch_prime_bitlength = stoi(optarg);
    //             break;
    //         case 'c':
    //             bucket_number = stoi(optarg);
    //             query_number = bucket_number/1.5;
    //         case '?':
    //             printf("unknown option: %c\n", optopt); 
    //             break; 
            
    //     }
    // }
    cout << "Test: " << test_case << endl;


    chrono::high_resolution_clock::time_point time_start_local, time_end_local;
    chrono::microseconds time_diff_local_e_d;
    chrono::microseconds time_diff_local_q_g;
    chrono::microseconds time_diff_local_mul_db;
    chrono::microseconds time_diff_local_c_dec;
    bool reach_max_attempts;
    uint64_t query_size;
    uint64_t res_size;
    uint64_t hint_size,noise_budget;
    
    if (test_case==1){
        debug=false;
        cout<<"Test DB"<<endl;
        cout<< "Generate Database" << endl;
        Timer timer;
        timer.reset();
        Database db(number_of_keywords,keywords_bit_length,value_bit_length);
       
        cout<< "Initiate Client"<<endl;
        BatchCodeParams batch_code_params;
        batch_code_params.bucket_number = bucket_number;
        batch_code_params.hash_func_number = hash_func_number;
        batch_code_params.query_number = query_number;
        DBClient client(&db,batch_code_params,pir_type);
        client.debug = debug;
        
        client.setup();
        
        cout<< "Initiate Server"<<endl;
        commu_cost = 0;
        enc_time = 0;
        DBServer server(client.Encrypted_KV,&client);
        server.setup();
        double setup_time = timer.elapsed();
        int sz=0;
        int bucket_sz=0;
        for(auto &item: server.Encrypted_db){
            if(bucket_sz==0)
            {
                bucket_sz = item.size();
            }
            else if(bucket_sz != item.size())
            {
                cout<<"Error: different bucket size!"<<endl;
                return -1;
            }
            for(auto item_t: item){
                if(sz==0)
                {
                    sz = item_t.size();
                    printf("Ciphertext size: %d bytes\n",sz);
                }
                else if(sz != item_t.size())
                {
                    cout<<"Error: different ciphertext size in the same bucket!"<<endl;
                    return -1;
                }
            }
        }
        cout<<"Bucket size: "<<server.Encrypted_db[0].size()<<endl;
        logs<<"Bucket size: "<<server.Encrypted_db[0].size()<<endl;
        cout<<"Ciphertext size in each bucket: "<<sz<<endl;
        logs<<"Ciphertext size in each bucket: "<<sz<<endl;
        cout<< "Test query" << endl;
        logs<<"Test query" << endl;
        double setup_cost = commu_cost/(1024.0*1024.0);//MB
        commu_cost = 0;
        enc_time = 0;
        Timer time_client;
        time_client.reset();
        double search_local_time=0;
        timer.reset();
        int test_queries = 1;
        for(int num_test=0;num_test<test_queries;num_test++)
        {
            time_client.reset();
            vector<string> keywords_query1=db.get_multi_random_keyword(query_number);
            vector<string> queries_query1 = client.query_generate_query_FakePIR(keywords_query1);
            search_local_time+=time_client.elapsed();
            vector<string> responses_query1 = server.query_response_FakePIR(queries_query1);
            time_client.reset();
            vector<string> results_query1 = client.query_recovery_FakePIR(responses_query1);
            search_local_time+=time_client.elapsed();
        }
        double query_time = timer.elapsed();
        query_time/=(double)test_queries;
        double query_cost = commu_cost/(1024.0*1024.0*(double)test_queries);//MB
        if(server.PIRType=="ORAM")
        {
            cout<<"Setup time: "<<setup_time<<" ms"<<endl;
            logs<<"Setup time: "<<setup_time<<" ms"<<endl;
            cout<<"Query time for "<<query_number<<" queries: "<<query_time<<" ms"<<endl;
            logs<<"Query time for "<<query_number<<" queries: "<<query_time<<" ms"<<endl;
            
            cout<<"Setup communication cost: "<<setup_cost<<" MB"<<endl;
            logs<<"Setup communication cost: "<<setup_cost<<" MB"<<endl;
            cout<<"Query communication cost for "<<query_number<<" queries: "<<query_cost<<" MB"<<endl;
            logs<<"Query communication cost for "<<query_number<<" queries: "<<query_cost<<" MB"<<endl;
            // cout<<"Average Client local search time: "<<search_local_time/(double)test_queries<<" ms"<<endl;
            // logs<<"Average Client local search time: "<<search_local_time/(double)test_queries<<" ms"<<endl;
        }
        vector<double> test_results;
        if(server.PIRType=="SpiralPIR")
        {
            
            server.Spiral_servers_uptr[0]->pir_ptr->print_summary(test_results);
            cout << fixed << setprecision(4);
            logs<<"[Spiral PIR parameters] "<<endl;
            cout<<"[Spiral PIR parameters] "<<endl;
            logs<<"setup_time(ms), setup_communications(MB),query_time(ms),query_communications(MB)"<<endl;
            cout<<"setup_time(ms), setup_communications(MB),query_time(ms),query_communications(MB)"<<endl;
            double raw_data_size=server.Encrypted_db[0].size()*sz*bucket_number;
            double setup_communication_cost=test_results[1];
            setup_communication_cost/=(double)test_queries;
            setup_communication_cost+=raw_data_size;
            setup_communication_cost/=(1024.0*1024.0);
            
            double query_comm_cost=test_results[3];
            query_comm_cost*=(double)bucket_number;
            query_comm_cost=query_comm_cost/1024.0/1024.0;
            test_results[3]=static_cast<double>(test_results[3])*((double)bucket_number)/1024.0/1024.0;
            cout<<"["<<(test_results[0]/double(test_queries)/1000.0)+setup_time<<","<<setup_communication_cost<<","<<query_time<<","<<query_comm_cost<<"]"<<endl<<endl;
            logs<<"["<<(test_results[0]/double(test_queries)/1000.0)+setup_time<<","<<setup_communication_cost<<","<<query_time<<","<<query_comm_cost<<"]"<<endl<<endl;
            logs<<"client search time(ms): "<<search_local_time/(double)test_queries+test_results[4]/double(test_queries)/1000.0<<endl;
            cout<<"client search time(ms): "<<search_local_time/(double)test_queries+test_results[4]/double(test_queries)/1000.0<<endl;
        }
        else if(server.PIRType=="ORAM")
        {
            logs<<"client search time(ms): "<<search_local_time/(double)test_queries+enc_time/(double)test_queries<<endl;
        }

        cout<<"Bucket size: "<<server.Encrypted_db[0].size()<<endl;
        logs<<"Bucket size: "<<server.Encrypted_db[0].size()<<endl;
        cout<<"Ciphertext size in each bucket: "<<sz<<endl;
        logs<<"Ciphertext size in each bucket: "<<sz<<endl;
        logs<<"Client Storage: "<<client.get_storage_size()/(1024.0*1024.0)<<" MB"<<endl;
        if(pir_type=="ORAM")
        {
            logs<<"["<<query_cost<<","<<query_time<<",2"<<"]"<<endl<<endl;
        }
        int num_in_stash=0;
        for(auto item:client.S1)
        {
            num_in_stash+=item.size();
        }
        logs<<"stash size: "<<num_in_stash<<endl;
       
    //     {
    //     ////////test query//////////
    //     cout<< "Test query" << endl;
    //     vector<string> keywords_query1=db.get_multi_random_keyword(query_number);
    //     vector<string> queries_query1 = client.query_generate_query_FakePIR(keywords_query1);
    //     vector<string> responses_query1 = server.query_response_FakePIR(queries_query1);
    //     vector<string> results_query1 = client.query_recovery_FakePIR(responses_query1);

    //     ///////test query keywords that are not in the database////////
    //     cout<< "Test query keywords that are not in the database" << endl;
    //     vector<string> keywords_query2=db.get_multi_random_keyword(query_number/2);
    //     string keyword_not_in_db,value;
    //     for (uint64_t i=0;i<query_number-query_number/2;i++){
    //         auto it = db.generate_new_kv_pair();
    //         keyword_not_in_db = it.first;
    //         keywords_query2.push_back(keyword_not_in_db);
    //     }
    //     vector<string> queries_query2 = client.query_generate_query_FakePIR(keywords_query2);
    //     vector<string> responses_query2 = server.query_response_FakePIR(queries_query2);
    //     vector<string> results_query2 = client.query_recovery_FakePIR(responses_query2);

    //     ////////test add//////////
    //     cout<< "Test add" << endl;
    //     string keyword_add,value_add;
    //     map<string,string> added_keywords_values_map;
    //     uint64_t query_number_add = 1000;
    //     for (uint64_t i=0;i<query_number_add;i++){
    //         auto it = db.generate_new_kv_pair();
    //         keyword_add = it.first;
    //         value_add = it.second;
    //         db.add(keyword_add,value_add);
    //         client.add(keyword_add,value_add);
    //         added_keywords_values_map[keyword_add] = value_add;
    //     }
    //     vector<string> keywords_query3 = db.get_multi_random_keyword(query_number/2);
    //     for (uint64_t i=0;i<query_number-query_number/2;i++){
    //         keywords_query3.push_back(added_keywords_values_map.begin()->first);
    //         added_keywords_values_map.erase(added_keywords_values_map.begin());
    //     }
    //     vector<string> queries_query3 = client.query_generate_query_FakePIR(keywords_query3);
    //     vector<string> responses_query3 = server.query_response_FakePIR(queries_query3);
    //     vector<string> results_query3 = client.query_recovery_FakePIR(responses_query3);

    //     /////////test delete////////
    //     cout<< "Test delete" << endl;
    //     string keyword_del;
    //     uint64_t query_number_del = 1000;
    //     vector<string> keywords_to_del = db.get_multi_random_keyword(query_number_del);
    //     for (uint64_t i=0;i<query_number_del;i++){
    //         keyword_del = keywords_to_del[i];
    //         db.del(keyword_del);
    //         client.del(keyword_del);
    //     }
    //     vector<string> keywords_query4 = db.get_multi_random_keyword(query_number/2);
    //     for (uint64_t i=0;i<query_number-query_number/2;i++){
    //         keywords_query4.push_back(keywords_to_del[i]);
    //     }
    //     vector<string> queries_query4 = client.query_generate_query_FakePIR(keywords_query4);
    //     vector<string> responses_query4 = server.query_response_FakePIR(queries_query4);    
    //     vector<string> results_query4 = client.query_recovery_FakePIR(responses_query4);

    //     ////////test reconstruction//////////
    //     cout<< "Test reconstruction" << endl;
    //     for (uint64_t i=0;i<bucket_number;i++){
    //         vector<string> bucket_content = server.get_bucket_content();
    //         vector<string> encrypted_bucket_content = client.reconstruct(bucket_content);
    //         server.reconstruct_bucket_content(encrypted_bucket_content);
    //     }
        
    //     vector<string> keywords_query5 = db.get_multi_random_keyword(query_number);
    //     vector<string> queries_query5 = client.query_generate_query_FakePIR(keywords_query5);
    //     vector<string> responses_query5 = server.query_response_FakePIR(queries_query5);
    //     vector<string> results_query5 = client.query_recovery_FakePIR(responses_query5);

    //     for (uint64_t i=0;i<bucket_number;i++){
    //         vector<string> bucket_content = server.get_bucket_content();
    //         vector<string> encrypted_bucket_content = client.reconstruct(bucket_content);
    //         server.reconstruct_bucket_content(encrypted_bucket_content);
    //     }

    //     vector<string> keywords_query6 = db.get_multi_random_keyword(query_number);
    //     vector<string> queries_query6 = client.query_generate_query_FakePIR(keywords_query6);
    //     vector<string> responses_query6 = server.query_response_FakePIR(queries_query6);
    //     vector<string> results_query6 = client.query_recovery_FakePIR(responses_query6);
    // }

    // ///////test round 2 (repeat above tests)//////////
    //     {
    //     ////////test round 2//////////
    //     cout<< "Test round 2" << endl;
    //     ////////test query//////////
    //     cout<< "Test query" << endl;
    //     vector<string> keywords_query1=db.get_multi_random_keyword(query_number);
    //     vector<string> queries_query1 = client.query_generate_query_FakePIR(keywords_query1);
    //     vector<string> responses_query1 = server.query_response_FakePIR(queries_query1);
    //     vector<string> results_query1 = client.query_recovery_FakePIR(responses_query1);

    //     ///////test query keywords that are not in the database////////
    //     cout<< "Test query keywords that are not in the database" << endl;
    //     vector<string> keywords_query2=db.get_multi_random_keyword(query_number/2);
    //     string keyword_not_in_db,value;
    //     for (uint64_t i=0;i<query_number-query_number/2;i++){
    //         auto it = db.generate_new_kv_pair();
    //         keyword_not_in_db = it.first;
    //         keywords_query2.push_back(keyword_not_in_db);
    //     }
    //     vector<string> queries_query2 = client.query_generate_query_FakePIR(keywords_query2);
    //     vector<string> responses_query2 = server.query_response_FakePIR(queries_query2);
    //     vector<string> results_query2 = client.query_recovery_FakePIR(responses_query2);

    //     ////////test add//////////
    //     cout<< "Test add" << endl;
    //     string keyword_add,value_add;
    //     map<string,string> added_keywords_values_map;
    //     uint64_t query_number_add = 1000;
    //     for (uint64_t i=0;i<query_number_add;i++){
    //         auto it = db.generate_new_kv_pair();
    //         keyword_add = it.first;
    //         value_add = it.second;
    //         db.add(keyword_add,value_add);
    //         client.add(keyword_add,value_add);
    //         added_keywords_values_map[keyword_add] = value_add;
    //     }
    //     vector<string> keywords_query3 = db.get_multi_random_keyword(query_number/2);
    //     for (uint64_t i=0;i<query_number-query_number/2;i++){
    //         keywords_query3.push_back(added_keywords_values_map.begin()->first);
    //         added_keywords_values_map.erase(added_keywords_values_map.begin());
    //     }
    //     vector<string> queries_query3 = client.query_generate_query_FakePIR(keywords_query3);
    //     vector<string> responses_query3 = server.query_response_FakePIR(queries_query3);
    //     vector<string> results_query3 = client.query_recovery_FakePIR(responses_query3);

    //     /////////test delete////////
    //     cout<< "Test delete" << endl;
    //     string keyword_del;
    //     uint64_t query_number_del = 1000;
    //     vector<string> keywords_to_del = db.get_multi_random_keyword(query_number_del);
    //     for (uint64_t i=0;i<query_number_del;i++){
    //         keyword_del = keywords_to_del[i];
    //         db.del(keyword_del);
    //         client.del(keyword_del);
    //     }
    //     vector<string> keywords_query4 = db.get_multi_random_keyword(query_number/2);
    //     for (uint64_t i=0;i<query_number-query_number/2;i++){
    //         keywords_query4.push_back(keywords_to_del[i]);
    //     }
    //     vector<string> queries_query4 = client.query_generate_query_FakePIR(keywords_query4);
    //     vector<string> responses_query4 = server.query_response_FakePIR(queries_query4);    
    //     vector<string> results_query4 = client.query_recovery_FakePIR(responses_query4);

        ////////test reconstruction//////////
        cout<< "Test reconstruction" << endl;
        commu_cost = 0;
        timer.reset();
        for (uint64_t i=0;i<5;i++){
            vector<string> bucket_content = server.get_bucket_content();
            vector<string> encrypted_bucket_content = client.reconstruct(bucket_content);
            server.reconstruct_bucket_content(encrypted_bucket_content);
        }
        auto reconstruction_time = timer.elapsed()/5.0;
        commu_cost = commu_cost/5;
        cout << "Reconstruction time: " << reconstruction_time << " ms" << endl;
        logs<<"Reconstruction time: "<<reconstruction_time<<" ms"<<endl;
        if(server.PIRType=="ORAM")
        {
            cout<<"Reconstruction communication cost: "<<(double)commu_cost/(1024.0*1024.0)<<" MB"<<endl;
            logs<<"Reconstruction communication cost: "<<(double)commu_cost/(1024.0*1024.0)<<" MB"<<endl;
        }
        
        vector<string> keywords_query5 = db.get_multi_random_keyword(query_number);
        vector<string> queries_query5 = client.query_generate_query_FakePIR(keywords_query5);
        vector<string> responses_query5 = server.query_response_FakePIR(queries_query5);
        vector<string> results_query5 = client.query_recovery_FakePIR(responses_query5);
        vector<double> test_results1;
        if(server.PIRType=="SpiralPIR")
        {
            
            server.Spiral_servers_uptr[0]->pir_ptr->print_summary(test_results1);
            double Reconstruction_comm_cost=test_results1[1]-test_results[1];
            Reconstruction_comm_cost/=5.0;
            Reconstruction_comm_cost+=double(server.Encrypted_db[0].size()*(double)sz);
            Reconstruction_comm_cost/=(1024.0*1024.0);
            cout << fixed << setprecision(4);
            cout<<"[Spiral PIR Reconstruction parameters] "<<endl;
            logs<<"[Spiral PIR Reconstruction parameters] "<<endl;
            logs<<"reconstruction_communications(MB): "<<Reconstruction_comm_cost<<endl;
            cout<<"reconstruction_communications(MB): "<<Reconstruction_comm_cost<<endl;
            cout << "Reconstruction time: " << reconstruction_time << " ms" << endl;
            logs<<"Reconstruction time: "<<reconstruction_time<<" ms"<<endl;
        
        }
    // //     for (uint64_t i=0;i<bucket_number/2;i++){
    // //         vector<string> bucket_content = server.get_bucket_content();
    // //         vector<string> encrypted_bucket_content = client.reconstruct(bucket_content);
    // //         server.reconstruct_bucket_content(encrypted_bucket_content);
    // //     }

    // //     vector<string> keywords_query6 = db.get_multi_random_keyword(query_number);
    // //     vector<string> queries_query6 = client.query_generate_query_FakePIR(keywords_query6);
    // //     vector<string> responses_query6 = server.query_response_FakePIR(queries_query6);
    // //     vector<string> results_query6 = client.query_recovery_FakePIR(responses_query6);
    // //     cout<< "Test round 2 finished" << endl;
    // // }
        cout<< "Test Finished" <<endl;
        logs<<"Test Finished" <<endl<<endl;
        cout<<endl;
    }
    else if (test_case == 2)
    {
        debug = true;
        cout<<"Test DB"<<endl;
        cout<< "Generate Database" << endl;
        Database db(number_of_keywords,keywords_bit_length,value_bit_length);
        
        cout<< "Initiate Client"<<endl;
        BatchCodeParams batch_code_params;
        batch_code_params.bucket_number = bucket_number;
        batch_code_params.hash_func_number = hash_func_number;
        batch_code_params.query_number = query_number;
        DBClient client(&db,batch_code_params,"PIRANA");
        client.debug = debug;
        client.setup();
        
        cout<< "Initiate Server"<<endl;
        DBServer server(client.Encrypted_KV,&client);

        chrono::high_resolution_clock::time_point time_start_local = chrono::high_resolution_clock::now();
        server.setup();
        chrono::high_resolution_clock::time_point time_end_local = chrono::high_resolution_clock::now();
        chrono::microseconds time_diff_local = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
        cout << "Time to setup server: " << float(time_diff_local.count()) / 1000000 << " seconds" << endl;

        



        //////test query//////////
        cout<< "Test query" << endl;
        commu_cost = 0;
        Timer time;
        time.reset();
        vector<string> keywords_query=db.get_multi_random_keyword(query_number);
        time_start_local = chrono::high_resolution_clock::now();
        stringstream query_stream = client.query_generate_query_PIRANA(keywords_query);
        commu_cost += query_stream.str().size();
        time_end_local = chrono::high_resolution_clock::now();
        time_diff_local = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
        cout << "Time to generate query: " << float(time_diff_local.count()) / 1000000 << " seconds" << endl;

        time_start_local = chrono::high_resolution_clock::now();
        stringstream responses_query = server.query_response_PIRANA(query_stream);
        commu_cost += responses_query.str().size();
        time_end_local = chrono::high_resolution_clock::now();
        time_diff_local = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
        cout << "Time to process query: " << float(time_diff_local.count()) / 1000000 << " seconds" << endl;

        time_start_local = chrono::high_resolution_clock::now();
        vector<string> results_query = client.query_recovery_PIRANA(responses_query);
        time_end_local = chrono::high_resolution_clock::now();
        time_diff_local = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
        cout << "Time to recover query: " << float(time_diff_local.count()) / 1000000 << " seconds" << endl;
        cout<<"Total time for query: "<<time.elapsed()<<" ms"<<endl;
        cout<<"commu cost for query: "<<commu_cost/(1024.0*1024.0)<<" MB"<<endl;
        cout<<"Test Finished"<<endl;
        //TODO: add correctness check
        // cout<< "Test add" << endl;
        // string keyword_add,value_add;
        // map<string,string> added_keywords_values_map;
        // uint64_t query_number_add = 1000;
        // for (uint64_t i=0;i<query_number_add;i++){
        //     auto it = db.generate_new_kv_pair();
        //     keyword_add = it.first;
        //     value_add = it.second;
        //     db.add(keyword_add,value_add);
        //     client.add(keyword_add,value_add);
        //     added_keywords_values_map[keyword_add] = value_add;
        // }
        // vector<string> keywords_query3 = db.get_multi_random_keyword(query_number/2);
        // for (uint64_t i=0;i<query_number-query_number/2;i++){
        //     keywords_query3.push_back(added_keywords_values_map.begin()->first);
        //     added_keywords_values_map.erase(added_keywords_values_map.begin());
        // }
        // stringstream queries_query3 = client.query_generate_query_PIRANA(keywords_query3);
        // stringstream responses_query3 = server.query_response_PIRANA(queries_query3);
        // vector<string> results_query3 = client.query_recovery_PIRANA(responses_query3);

        // /////////test delete////////
        // cout<< "Test delete" << endl;
        // string keyword_del;
        // uint64_t query_number_del = 1000;
        // vector<string> keywords_to_del = db.get_multi_random_keyword(query_number_del);
        // for (uint64_t i=0;i<query_number_del;i++){
        //     keyword_del = keywords_to_del[i];
        //     db.del(keyword_del);
        //     client.del(keyword_del);
        // }
        // vector<string> keywords_query4 = db.get_multi_random_keyword(query_number/2);
        // for (uint64_t i=0;i<query_number-query_number/2;i++){
        //     keywords_query4.push_back(keywords_to_del[i]);
        // }
        // stringstream queries_query4 = client.query_generate_query_PIRANA(keywords_query4);
        // stringstream responses_query4 = server.query_response_PIRANA(queries_query4);
        // vector<string> results_query4 = client.query_recovery_PIRANA(responses_query4);

        // ////test reconstruction//////////
        // cout<< "Test reconstruction" << endl;
        // for (uint64_t i=0;i<2000;i++){
        //     cout << "Reconstructing bucket " << i+1 << " of " << bucket_number << endl;
        //     vector<string> bucket_content = server.get_bucket_content();
        //     vector<string> encrypted_bucket_content = client.reconstruct(bucket_content);
        //     server.reconstruct_bucket_content(encrypted_bucket_content);
        // }
        // vector<string> keywords_query5 = db.get_multi_random_keyword(query_number);
        // stringstream queries_query5 = client.query_generate_query_PIRANA(keywords_query5);
        // stringstream responses_query5 = server.query_response_PIRANA(queries_query5);
        // vector<string> results_query5 = client.query_recovery_PIRANA(responses_query5);

        // ////test reconstruction//////////
        // // cout<< "Test reconstruction" << endl;
        // // for (uint64_t i=0;i<300;i++){
        // //     cout << "Reconstructing bucket " << i+1 << " of " << bucket_number << endl;
        // //     vector<string> bucket_content = server.get_bucket_content();
        // //     vector<string> encrypted_bucket_content = client.reconstruct(bucket_content);
        // //     server.reconstruct_bucket_content(encrypted_bucket_content);
        // // }
        // vector<string> keywords_query6 = db.get_multi_random_keyword(query_number);
        // stringstream queries_query6 = client.query_generate_query_PIRANA(keywords_query6);
        // stringstream responses_query6 = server.query_response_PIRANA(queries_query6);
        // vector<string> results_query6 = client.query_recovery_PIRANA(responses_query6);

    }
    else if (test_case == 3)
    {
        debug = false;
        cout<<"Formal Test DB"<<endl;
        vector<uint64_t> number_of_keywords_list = {1<<16};//,1<<18,1<<20,1<<22};
        uint64_t bucket_number = 4096;
        uint64_t query_number = bucket_number/1.5;
        uint64_t keywords_bit_length = 32;
        uint64_t value_bit_length = 32;
        uint64_t repeat_times = 2;

        map<uint64_t,vector<uint64_t>> runtime_setup_client;
        map<uint64_t,vector<uint64_t>> runtime_setup_server;
        map<uint64_t,vector<uint64_t>> runtime_query_generate;
        map<uint64_t,vector<uint64_t>> runtime_query_response,runtime_query_response_2;
        map<uint64_t,vector<uint64_t>> runtime_query_recovery;
        cout<< "Generate Database" << endl;
        for (uint64_t number_of_keywords : number_of_keywords_list){
            for(uint64_t i=0;i<repeat_times;i++){
                cout << "Testing with " << number_of_keywords << " keywords, repeat " << i+1 << endl;
                Database db(number_of_keywords,keywords_bit_length,value_bit_length);
                cout<< "Initiate Client"<<endl;
                BatchCodeParams batch_code_params;
                batch_code_params.bucket_number = bucket_number;
                batch_code_params.hash_func_number = hash_func_number;
                batch_code_params.query_number = query_number;
                DBClient client(&db,batch_code_params,"PIRANA");
                client.debug = debug;
                chrono::high_resolution_clock::time_point time_start_local = chrono::high_resolution_clock::now();
                client.setup();
                chrono::high_resolution_clock::time_point time_end_local = chrono::high_resolution_clock::now();
                chrono::microseconds time_diff_local = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
                runtime_setup_client[number_of_keywords].push_back(time_diff_local.count());
                
                cout<< "Initiate Server"<<endl;
                DBServer server(client.Encrypted_KV,&client);
                time_start_local = chrono::high_resolution_clock::now();
                server.setup();
                time_end_local = chrono::high_resolution_clock::now();
                time_diff_local = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
                runtime_setup_server[number_of_keywords].push_back(time_diff_local.count());

                ////////test query//////////
                cout<< "Test query" << endl;
                vector<string> keywords_query=db.get_multi_random_keyword(query_number);
                stringstream query_stream = client.query_generate_query_PIRANA(keywords_query);
                stringstream responses_query = server.query_response_PIRANA(query_stream);
                vector<string> results_query = client.query_recovery_PIRANA(responses_query);
                for (uint64_t j=0;j<3;j++){
                    vector<string> keywords_query=db.get_multi_random_keyword(query_number);
                    time_start_local = chrono::high_resolution_clock::now();
                    stringstream query_stream = client.query_generate_query_PIRANA(keywords_query);
                    time_end_local = chrono::high_resolution_clock::now();
                    time_diff_local = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
                    runtime_query_generate[number_of_keywords].push_back(time_diff_local.count());
                    
                    time_start_local = chrono::high_resolution_clock::now();
                    stringstream responses_query = server.query_response_PIRANA(query_stream);
                    time_end_local = chrono::high_resolution_clock::now();
                    time_diff_local = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
                    runtime_query_response[number_of_keywords].push_back(time_diff_local.count());

                    time_start_local = chrono::high_resolution_clock::now();
                    vector<string> results_query = client.query_recovery_PIRANA(responses_query);
                    time_end_local = chrono::high_resolution_clock::now();
                    time_diff_local = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local);
                    runtime_query_recovery[number_of_keywords].push_back(time_diff_local.count());
                }
            }
            cout << "Runtime Setup Client:" << endl;
            for (const auto& [keywords, runtimes] : runtime_setup_client) {
                cout << "Number of keywords: " << keywords << ", Average time: " 
                    << accumulate(runtimes.begin(), runtimes.end(), 0.0) / runtimes.size() << " microseconds" << endl;
            }

            cout << "Runtime Setup Server:" << endl;
            for (const auto& [keywords, runtimes] : runtime_setup_server) {
                cout << "Number of keywords: " << keywords << ", Average time: " 
                    << accumulate(runtimes.begin(), runtimes.end(), 0.0) / runtimes.size() << " microseconds" << endl;
            }

            cout << "Runtime Query Generate:" << endl;
            for (const auto& [keywords, runtimes] : runtime_query_generate) {
                cout << "Number of keywords: " << keywords << ", Average time: " 
                    << accumulate(runtimes.begin(), runtimes.end(), 0.0) / runtimes.size() << " microseconds" << endl;
            }

            cout << "Runtime Query Response:" << endl;
            for (const auto& [keywords, runtimes] : runtime_query_response) {
                cout << "Number of keywords: " << keywords << ", Average time: " 
                    << accumulate(runtimes.begin(), runtimes.end(), 0.0) / runtimes.size() << " microseconds" << endl;
            }

            cout << "Runtime Query Recovery:" << endl;
            for (const auto& [keywords, runtimes] : runtime_query_recovery) {
                cout << "Number of keywords: " << keywords << ", Average time: " 
                    << accumulate(runtimes.begin(), runtimes.end(), 0.0) / runtimes.size() << " microseconds" << endl;
            }
        }
        
    }
    else if (test_case == 4)
    {
        debug = false;
        uint64_t bucket_number = 8192;
        cout<<"Formal Test DB based on PIRANA"<<endl;

        cout<<"Storage and Reconstruct Cost Test"<<endl;
        vector<map<string,uint64_t>> test_parameters = {
            // {{"number_of_keywords",1<<16},{"bucket_number",bucket_number},{"keywords_bit_length",32},{"value_bit_length",32}},
            // {{"number_of_keywords",1<<16},{"bucket_number",bucket_number},{"keywords_bit_length",32},{"value_bit_length",1024}},
            // {{"number_of_keywords",1<<16},{"bucket_number",bucket_number},{"keywords_bit_length",1024},{"value_bit_length",1024}},
            // {{"number_of_keywords",1<<20},{"bucket_number",bucket_number},{"keywords_bit_length",32},{"value_bit_length",32}},

            //{{"number_of_keywords",1<<20},{"bucket_number",bucket_number},{"keywords_bit_length",32},{"value_bit_length",1024}},
            //{{"number_of_keywords",1<<20},{"bucket_number",bucket_number},{"keywords_bit_length",1024},{"value_bit_length",1024}},
            {{"number_of_keywords",1<<20},{"bucket_number",bucket_number},{"keywords_bit_length",1024},{"value_bit_length",65536}},

            // {{"number_of_keywords",1<<24},{"bucket_number",bucket_number},{"keywords_bit_length",1024},{"value_bit_length",1024}},
            // {{"number_of_keywords",1<<20},{"bucket_number",bucket_number},{"keywords_bit_length",1024},{"value_bit_length",20*1024*8}},
        };
        uint64_t updates_per_reconstruct = 10;
        uint64_t reconstruction_times = 10;
        for (const auto& params : test_parameters) {
            uint64_t number_of_keywords = params.at("number_of_keywords");
            uint64_t bucket_number = params.at("bucket_number");
            uint64_t keywords_bit_length = params.at("keywords_bit_length");
            uint64_t value_bit_length = params.at("value_bit_length");

            cout << "Testing with " << number_of_keywords << " keywords, bucket number: " << bucket_number 
                 << ", keywords bit length: " << keywords_bit_length << ", value bit length: " << value_bit_length << endl;
            cout << "updates_per_reconstruct: " << updates_per_reconstruct << endl;

            Database db(number_of_keywords,keywords_bit_length,value_bit_length);
            BatchCodeParams batch_code_params;
            batch_code_params.bucket_number = bucket_number;
            batch_code_params.hash_func_number = hash_func_number;
            batch_code_params.query_number = query_number;
            DBClient client(&db,batch_code_params,"PIRANA");
            client.debug = debug;
            uint64_t size =  client.setup();
            cout<< "Client setup finished. Storage size: " << size << " bytes" << endl;
            DBServer server(client.Encrypted_KV,&client);
            server.setup();

            chrono::high_resolution_clock::time_point time_start_local = chrono::high_resolution_clock::now();
            chrono::high_resolution_clock::time_point time_end_local = chrono::high_resolution_clock::now();
            float update_time = 0.0;
            float reconstruct_time_C = 0.0;
            float reconstruct_time_S = 0.0;
            for (uint64_t i=0;i<reconstruction_times;i++){
                cout<< "Reconstructing bucket " << i+1 << " of " << reconstruction_times << endl;
                for (uint64_t j=0;j<updates_per_reconstruct;j++){
                    if (random()%2==0){
                        //add
                        auto it = db.generate_new_kv_pair();
                        string keyword_add = it.first;
                        string value_add = it.second;
                        db.add(keyword_add,value_add);
                        time_start_local = chrono::high_resolution_clock::now();
                        client.add(keyword_add,value_add);
                        time_end_local = chrono::high_resolution_clock::now();
                        update_time += chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local).count();
                    }
                    else{
                        //del
                        string keyword_del = db.get_random_keyword();
                        db.del(keyword_del);
                        time_start_local = chrono::high_resolution_clock::now();
                        client.del(keyword_del);
                        time_end_local = chrono::high_resolution_clock::now();
                        update_time += chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local).count();
                    }
                }
                vector<string> bucket_content = server.get_bucket_content();
                time_start_local = chrono::high_resolution_clock::now();
                vector<string> encrypted_bucket_content = client.reconstruct(bucket_content);
                time_end_local = chrono::high_resolution_clock::now();
                reconstruct_time_C += chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local).count();
                time_start_local = chrono::high_resolution_clock::now();
                server.reconstruct_bucket_content(encrypted_bucket_content);
                time_end_local = chrono::high_resolution_clock::now();
                reconstruct_time_S += chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local).count();
            }

            // cout << "Total update time: " << update_time << " microseconds" << endl;
            // cout << "Total reconstruct time (Client): " << reconstruct_time_C << " microseconds" << endl;
            // cout << "Total reconstruct time (Server): " << reconstruct_time_S << " microseconds" << endl;
            //cout << "Average update time: " << update_time / (reconstruction_times * updates_per_reconstruct) << " microseconds" << endl;
            //cout << "Average reconstruct time (Client): " << reconstruct_time_C / reconstruction_times << " microseconds" << endl;
            cout << "Average reconstruct time (Server): " << reconstruct_time_S / reconstruction_times << " microseconds" << endl;
            //cout << "Storage cost (Client, Stash and MPH): " << client.get_storage_size() << " bytes" << endl;
            cout << "Storage cost (Server, Database): " << server.PIR->get_storage_size() << " bytes" << endl;
            cout << "----------------------------------------" << endl;
        }
    }
    else if (test_case == 5)
    {
        debug = true;
        cout<<"Formal Test DB Storage size(the same for FakePIR and PIRANA)"<<endl;

        cout<<"Storage and Reconstruct Cost Test"<<endl;
        vector<map<string,uint64_t>> test_parameters = {
            // {{"number_of_keywords",1<<16},{"bucket_number",bucket_number},{"keywords_bit_length",32},{"value_bit_length",32}},
            // {{"number_of_keywords",1<<16},{"bucket_number",bucket_number},{"keywords_bit_length",32},{"value_bit_length",1024}},
            // {{"number_of_keywords",1<<16},{"bucket_number",bucket_number},{"keywords_bit_length",1024},{"value_bit_length",1024}},
            // {{"number_of_keywords",1<<20},{"bucket_number",bucket_number},{"keywords_bit_length",32},{"value_bit_length",32}},

            // {{"number_of_keywords",1<<20},{"bucket_number",bucket_number},{"keywords_bit_length",32},{"value_bit_length",1024}},
            // {{"number_of_keywords",1<<20},{"bucket_number",bucket_number},{"keywords_bit_length",1024},{"value_bit_length",1024}},
            // {{"number_of_keywords",1<<20},{"bucket_number",bucket_number},{"keywords_bit_length",1024},{"value_bit_length",65536}}

            {{"number_of_keywords",1<<24},{"bucket_number",bucket_number},{"keywords_bit_length",1024},{"value_bit_length",1024}},
            // {{"number_of_keywords",1<<20},{"bucket_number",bucket_number},{"keywords_bit_length",1024},{"value_bit_length",20*1024*8}},
        };
        uint64_t updates_per_reconstruct = 10;
        uint64_t reconstruction_times = 8192;
        for (const auto& params : test_parameters) {
            uint64_t number_of_keywords = params.at("number_of_keywords");
            uint64_t bucket_number = params.at("bucket_number");
            uint64_t keywords_bit_length = params.at("keywords_bit_length");
            uint64_t value_bit_length = params.at("value_bit_length");

            cout << "Testing with " << number_of_keywords << " keywords, bucket number: " << bucket_number 
                 << ", keywords bit length: " << keywords_bit_length << ", value bit length: " << value_bit_length << endl;
            cout << "updates_per_reconstruct: " << updates_per_reconstruct << endl;

            Database db(number_of_keywords,keywords_bit_length,value_bit_length);
            BatchCodeParams batch_code_params;
            batch_code_params.bucket_number = bucket_number;
            batch_code_params.hash_func_number = hash_func_number;
            batch_code_params.query_number = query_number;
            DBClient client(&db,batch_code_params,"FakePIR");
            client.debug = debug;
            client.setup();
            
            DBServer server(client.Encrypted_KV,&client);
            server.setup();

            chrono::high_resolution_clock::time_point time_start_local = chrono::high_resolution_clock::now();
            chrono::high_resolution_clock::time_point time_end_local = chrono::high_resolution_clock::now();
            float update_time = 0.0;
            float reconstruct_time_C = 0.0;
            float reconstruct_time_S = 0.0;
            for (uint64_t i=0;i<reconstruction_times;i++){
                cout<< "Reconstructing bucket " << i+1 << " of " << reconstruction_times << endl;
                for (uint64_t j=0;j<updates_per_reconstruct;j++){
                    if (random()%2==0){
                        //add
                        auto it = db.generate_new_kv_pair();
                        string keyword_add = it.first;
                        string value_add = it.second;
                        db.add(keyword_add,value_add);
                        time_start_local = chrono::high_resolution_clock::now();
                        client.add(keyword_add,value_add);
                        time_end_local = chrono::high_resolution_clock::now();
                        update_time += chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local).count();
                    }
                    else{
                        //del
                        string keyword_del = db.get_random_keyword();
                        db.del(keyword_del);
                        time_start_local = chrono::high_resolution_clock::now();
                        client.del(keyword_del);
                        time_end_local = chrono::high_resolution_clock::now();
                        update_time += chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local).count();
                    }
                }
                vector<string> bucket_content = server.get_bucket_content();
                time_start_local = chrono::high_resolution_clock::now();
                vector<string> encrypted_bucket_content = client.reconstruct(bucket_content);
                time_end_local = chrono::high_resolution_clock::now();
                reconstruct_time_C += chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local).count();
                time_start_local = chrono::high_resolution_clock::now();
                server.reconstruct_bucket_content(encrypted_bucket_content);
                time_end_local = chrono::high_resolution_clock::now();
                reconstruct_time_S += chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local).count();
            }
            cout << "Average update time: " << update_time / (reconstruction_times * updates_per_reconstruct) << " microseconds" << endl;
            cout << "Average reconstruct time (Client): " << reconstruct_time_C / reconstruction_times << " microseconds" << endl;
            cout << "Storage cost (Client, Stash and MPH): " << client.get_storage_size() << " bytes" << endl;
            //cout << "Storage cost (Server, Database): " << server.PIR->get_storage_size() << " bytes" << endl;
            cout << "----------------------------------------" << endl;
        }
    }
    else if (test_case == 6){
        debug = false;
        // uint64_t bucket_number = bn;
        bucket_number = 8192;
        cout<<"Formal Test DB based on PIRANA"<<endl;

        cout<<"Storage and Communication Cost Test"<<endl;
        vector<map<string,uint64_t>> test_parameters = {
             {{"number_of_keywords",number_of_keywords},{"bucket_number",bucket_number},{"keywords_bit_length",keywords_bit_length},{"value_bit_length",value_bit_length}},
            //{{"number_of_keywords",1<<16},{"bucket_number",bucket_number},{"keywords_bit_length",32},{"value_bit_length",1024}},
            // {{"number_of_keywords",1<<16},{"bucket_number",bucket_number},{"keywords_bit_length",1024},{"value_bit_length",1024}},
            // {{"number_of_keywords",1<<20},{"bucket_number",bucket_number},{"keywords_bit_length",32},{"value_bit_length",32}},

            //{{"number_of_keywords",1<<20},{"bucket_number",bucket_number},{"keywords_bit_length",32},{"value_bit_length",1024}},
            // {{"number_of_keywords",1<<20},{"bucket_number",bucket_number},{"keywords_bit_length",1024},{"value_bit_length",1024}},
            //{{"number_of_keywords",1<<20},{"bucket_number",bucket_number},{"keywords_bit_length",1024},{"value_bit_length",65536}},

            //{{"number_of_keywords",1<<24},{"bucket_number",bucket_number},{"keywords_bit_length",1024},{"value_bit_length",1024}},
            // {{"number_of_keywords",1<<20},{"bucket_number",bucket_number},{"keywords_bit_length",1024},{"value_bit_length",20*1024*8}},
        };
        uint64_t updates_per_reconstruct = 10;
        uint64_t reconstruction_times = 5;
        for (const auto& params : test_parameters) {
            uint64_t number_of_keywords = params.at("number_of_keywords");
            uint64_t bucket_number = params.at("bucket_number");
            uint64_t keywords_bit_length = params.at("keywords_bit_length");
            uint64_t value_bit_length = params.at("value_bit_length");

            cout << "Testing with " << number_of_keywords << " keywords, bucket number: " << bucket_number 
                 << ", keywords bit length: " << keywords_bit_length << ", value bit length: " << value_bit_length << endl;
            cout << "updates_per_reconstruct: " << updates_per_reconstruct << endl;

            float update_time = 0.0;
            float reconstruct_time_C = 0.0;
            float reconstruct_time_S = 0.0;

            uint64_t setup_communication_cost = 0;
            
            uint64_t reconstruct_communication_cost_C2S = 0;
            uint64_t reconstruct_communication_cost_S2C = 0;

            float setup_time_C = 0.0;
            float setup_time_S = 0.0;
            

            chrono::high_resolution_clock::time_point time_start_local = chrono::high_resolution_clock::now();
            chrono::high_resolution_clock::time_point time_end_local = chrono::high_resolution_clock::now();

            Database db(number_of_keywords,keywords_bit_length,value_bit_length);
            BatchCodeParams batch_code_params;
            batch_code_params.bucket_number = bucket_number;
            batch_code_params.hash_func_number = hash_func_number;
            // TODO:调一下
            batch_code_params.query_number = query_number;
            logs<<"Testing with " << number_of_keywords << " keywords, bucket number: " << bucket_number 
                 << ", keywords bit length: " << keywords_bit_length << ", value bit length: " << value_bit_length << endl;
            logs << "updates_per_reconstruct: " << updates_per_reconstruct << endl;
            logs<<"start setup"<<endl;
            Timer time;
            time.reset();
            DBClient client(&db,batch_code_params,"PIRANA");
            client.debug = debug;

            auto setup_duration = time.elapsed();

            time_start_local = chrono::high_resolution_clock::now();
            uint64_t size =  client.setup();
            time_end_local = chrono::high_resolution_clock::now();
            setup_time_C = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local).count();
            setup_communication_cost = size;
            cout<< "Client setup finished. Storage size: " << size << " bytes" << endl;
            logs << "Client setup finished. Storage size: " << size << " bytes" << endl;
            cout << "Client setup time: " << float(setup_time_C) << " microseconds" << endl;
            time.reset();
            DBServer server(client.Encrypted_KV,&client);
            time_start_local = chrono::high_resolution_clock::now();
            server.setup(); 
            setup_duration+= time.elapsed();
            time_end_local = chrono::high_resolution_clock::now();
            setup_time_S = chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local).count();

            cout << "Storage cost (Server, Database): " << server.PIR->get_storage_size() << " bytes" << endl;
            logs << "Storage cost (Server, Database): " << server.PIR->get_storage_size() << " bytes" << endl;
            logs << "Total setup time: " << setup_duration << " ms" << endl;
            for (uint64_t i=0;i<reconstruction_times;i++){
                cout<< "Reconstructing bucket " << i+1 << " of " << reconstruction_times << endl;
                for (uint64_t j=0;j<updates_per_reconstruct;j++){
                    if (random()%2==0){
                        //add
                        auto it = db.generate_new_kv_pair();
                        string keyword_add = it.first;
                        string value_add = it.second;
                        db.add(keyword_add,value_add);
                        time_start_local = chrono::high_resolution_clock::now();
                        client.add(keyword_add,value_add);
                        time_end_local = chrono::high_resolution_clock::now();
                        update_time += chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local).count();
                    }
                    else{
                        //del
                        string keyword_del = db.get_random_keyword();
                        db.del(keyword_del);
                        time_start_local = chrono::high_resolution_clock::now();
                        client.del(keyword_del);
                        time_end_local = chrono::high_resolution_clock::now();
                        update_time += chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local).count();
                    }
                }
                vector<string> bucket_content = server.get_bucket_content();
                reconstruct_communication_cost_S2C += bucket_content.size() * bucket_content[0].size();

                time_start_local = chrono::high_resolution_clock::now();
                #ifdef TEST_FOR_DEBUG
                vector<string> encrypted_bucket_content = client.reconstruct(bucket_content);
                vector<string> all_bucket_contents;
                int add_bucket_num=8192/bn;
                if(add_bucket_num==0) add_bucket_num=1;
                all_bucket_contents.push_back(std::to_string(add_bucket_num));
                all_bucket_contents.push_back(std::to_string(encrypted_bucket_content.size()));
                all_bucket_contents.insert(all_bucket_contents.end(),encrypted_bucket_content.begin(),encrypted_bucket_content.end());
                reconstruct_communication_cost_C2S += encrypted_bucket_content.size() * encrypted_bucket_content[0].size();
                for(int i=1;i<8192/bn;i++)
                {
                    server.reconstruct_time+=1;
                    bucket_content=server.get_bucket_content();
                    encrypted_bucket_content=client.reconstruct(bucket_content);
                    all_bucket_contents.push_back(std::to_string(encrypted_bucket_content.size()));
                    all_bucket_contents.insert(all_bucket_contents.end(),encrypted_bucket_content.begin(),encrypted_bucket_content.end());
                    reconstruct_communication_cost_C2S += encrypted_bucket_content.size() * encrypted_bucket_content[0].size();
                }
                encrypted_bucket_content.clear();
                encrypted_bucket_content=all_bucket_contents;

                #else
                vector<string> encrypted_bucket_content = client.reconstruct(bucket_content);
                reconstruct_communication_cost_C2S += encrypted_bucket_content.size() * encrypted_bucket_content[0].size();
                #endif
                time_end_local = chrono::high_resolution_clock::now();
                reconstruct_time_C += chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local).count();
                

                time_start_local = chrono::high_resolution_clock::now();
                server.reconstruct_bucket_content(encrypted_bucket_content);
                time_end_local = chrono::high_resolution_clock::now();
                reconstruct_time_S += chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local).count();
            }
            reconstruct_communication_cost_C2S /= reconstruction_times;
            reconstruct_communication_cost_S2C /= reconstruction_times;
            reconstruct_time_C /= reconstruction_times;
            reconstruct_time_S /= reconstruction_times;

            cout<< "Test query" << endl;
            logs<<endl<<"Start query test"<<endl;
            // vector<uint64_t> query_number_list = {64*2/3,128*2/3,(int)256*2/3,(int)512*2/3,(int)1024*2/3,(int)2048*2/3};
            uint64_t repeat_times = 10;
            vector<uint64_t> query_number_list={bn/1.5};
            float query_time_C_generate_query = 0.0;
            float query_time_S_response = 0.0;
            float query_time_C_recovery = 0.0;
            uint64_t query_communication_cost_C2S = 0;
            uint64_t query_communication_cost_S2C = 0;
            for (uint64_t query_number : query_number_list){
                
                for (uint64_t j=0;j<repeat_times;j++){
                    cout << "Testing with " << query_number << " queries, repeat " << j+1 << endl;
                    logs << "Testing with " << query_number << " queries, repeat " << j+1 << endl;
                    // Generate random keywords for the query
                    vector<string> keywords_query = db.get_multi_random_keyword(query_number);

                    time_start_local = chrono::high_resolution_clock::now();
                    time.reset();
                    stringstream query_stream = client.query_generate_query_PIRANA(keywords_query);
                    time_end_local = chrono::high_resolution_clock::now();
                    query_time_C_generate_query += chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local).count();

                    auto query_gen_duration = time.elapsed();

                    query_communication_cost_C2S += query_stream.str().size();

                    time.reset();

                    time_start_local = chrono::high_resolution_clock::now();
                    stringstream responses_query = server.query_response_PIRANA(query_stream);

                    auto query_response_duration = time.elapsed();

                    time_end_local = chrono::high_resolution_clock::now();
                    query_time_S_response += chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local).count();
                    query_communication_cost_S2C += responses_query.str().size();
                    time.reset();
                    time_start_local = chrono::high_resolution_clock::now();
                    vector<string> results_query = client.query_recovery_PIRANA(responses_query);
                    auto query_recovery_duration = time.elapsed();
                    time_end_local = chrono::high_resolution_clock::now();
                    query_time_C_recovery += chrono::duration_cast<chrono::microseconds>(time_end_local - time_start_local).count();
                }
                query_time_C_generate_query /= repeat_times;
                query_time_S_response /= repeat_times;
                query_time_C_recovery /= repeat_times;
                query_communication_cost_C2S /= repeat_times;
                query_communication_cost_S2C /= repeat_times;
                cout << "Query number: " << query_number << endl;
                cout << "Average query time (Client, generate query): " << query_time_C_generate_query << " microseconds" << endl;
                logs << "Average query time (Client, generate query): " << query_time_C_generate_query/1000.0 << " ms" << endl;
                cout << "Average query time (Server, response  ): " << query_time_S_response << " microseconds" << endl;
                logs << "Average query time (Server, response  ): " << query_time_S_response/1000.0 << " ms" << endl;
                cout << "Average query time (Client, recovery): " << query_time_C_recovery << " microseconds" << endl;
                logs << "Average query time (Client, recovery): " << query_time_C_recovery/1000.0 << " ms" << endl;
                logs << "Total query time: " << (query_time_C_generate_query + query_time_S_response + query_time_C_recovery)/1000.0 << " ms" << endl;
                cout << "Average query communication cost (Client to Server): " << query_communication_cost_C2S << " bytes" << endl;
                logs << "Average query communication cost (Client to Server): " << query_communication_cost_C2S/1024.0/1024.0 << " MB" << endl;
                cout << "Average query communication cost (Server to Client): " << query_communication_cost_S2C << " bytes" << endl;
                logs << "Average query communication cost (Server to Client): " << query_communication_cost_S2C/1024.0/1024.0 << " MB" << endl;
                logs << "Total query communication cost: " << (query_communication_cost_C2S + query_communication_cost_S2C)/1024.0/1024.0 << " MB" << endl;
                logs <<"Total local query time: ["<<query_time_C_generate_query/1000.0+query_time_C_recovery/1000.0<<"] ms"<<endl;
            }

            cout << "Setup communication cost: " << setup_communication_cost << " bytes" << endl;
            logs << "Setup communication cost: " << setup_communication_cost/1024.0/1024.0 << " MB" << endl;
            cout << "Setup time (Client): " << setup_time_C << " microseconds" << endl;
            logs << "Setup time (Client): " << setup_time_C/1000.0 << " ms" << endl;
            cout << "Setup time (Server): " << setup_time_S << " microseconds" << endl;
            logs << "Setup time (Server): " << setup_time_S/1000.0 << " ms" << endl;
            logs << "Setup total time: " << (setup_time_C + setup_time_S)/1000.0 << " ms" << endl;
            cout << "Reconstruct communication cost (Client to Server): " << reconstruct_communication_cost_C2S << " bytes" << endl;
            logs << "Reconstruct communication cost (Client to Server): " << reconstruct_communication_cost_C2S/1024.0/1024.0 << " MB" << endl;
            cout << "Reconstruct communication cost (Server to Client): " << reconstruct_communication_cost_S2C << " bytes" << endl;
            logs << "Reconstruct communication cost (Server to Client): " << reconstruct_communication_cost_S2C/1024.0/1024.0 << " MB" << endl;
            logs << "Reconstruct total communication cost: " << (reconstruct_communication_cost_C2S + reconstruct_communication_cost_S2C)/1024.0/1024.0 << " MB" << endl;
            cout << "Reconstruct time (Client): " << reconstruct_time_C << " microseconds" << endl;
            logs << "Reconstruct time (Client): " << reconstruct_time_C/1000.0 << " ms" << endl;
            cout << "Reconstruct time (Server): " << reconstruct_time_S << " microseconds" << endl;
            logs << "Reconstruct time (Server): " << reconstruct_time_S/1000.0 << " ms" << endl;
            logs << "Reconstruct total time: " << (reconstruct_time_C + reconstruct_time_S)/1000.0 << " ms" << endl;

            cout << "Total update time: " << update_time << " microseconds" << endl;
            logs << "Total update time: " << update_time/1000.0 << " ms" << endl;
            cout << "Total reconstruct time (Client): " << reconstruct_time_C << " microseconds" << endl;
            logs << "Total reconstruct time (Client): " << reconstruct_time_C/1000.0 << " ms" << endl;
            cout << "Total reconstruct time (Server): " << reconstruct_time_S << " microseconds" << endl;
            logs << "Total reconstruct time (Server): " << reconstruct_time_S/1000.0 << " ms" << endl;
            cout << "Average update time: " << update_time / (reconstruction_times * updates_per_reconstruct) << " microseconds" << endl;
            logs << "Average update time: " << update_time / (reconstruction_times * updates_per_reconstruct)/1000.0 << " ms" << endl;
            cout << "Average reconstruct time (Client): " << reconstruct_time_C / bucket_number << " microseconds" << endl;
            logs << "Average reconstruct time (Client): " << reconstruct_time_C / bucket_number/1000.0 << " ms" << endl;
            cout << "Average reconstruct time (Server): " << reconstruct_time_S / bucket_number << " microseconds" << endl;
            logs << "Average reconstruct time (Server): " << reconstruct_time_S / bucket_number/1000.0 << " ms" << endl;
            cout << "Storage cost (Client, Stash and MPH): " << client.get_storage_size() << " bytes" << endl;
            logs << "Storage cost (Client, Stash and MPH): " << client.get_storage_size() << " bytes" << endl;
            cout << "Storage cost (Server, Database): " << server.PIR->get_storage_size() << " bytes" << endl;
            logs << "Storage cost (Server, Database): " << server.PIR->get_storage_size() << " bytes" << endl;
            cout << "----------------------------------------" << endl;
            logs<<"test_result:setup time(ms),setup_commnication(MB),query_time(ms),query_communication(MB)"<<endl;
            logs<<"["<<setup_duration<<","<<setup_duration<<","<<(query_time_C_generate_query + query_time_S_response + query_time_C_recovery)/1000.0<<","<<(query_communication_cost_C2S + query_communication_cost_S2C)/1024.0/1024.0<<"]"<<endl;
            logs << "----------------------------------------" << endl;
        }
    }
    else
    {
        cout<<"Invalid test case number."<<endl;
    }
    return 0;
}