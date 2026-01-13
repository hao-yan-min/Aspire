#include <catch2/catch_test_macros.hpp>
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
#include "OramInterface.h"
#include "OramReadPathEviction.h"
#include "UntrustedStorageInterface.h"
#include "ServerStorage.h"
#include "Bucket.h"
#include "oram_wrapper.h"
#include "ServerStorage.h"
#include "OramInterface.h"
#include "RandForOramInterface.h"
#include "RandomForOram.h"
#include "UntrustedStorageInterface.h"
#include "OramReadPathEviction.h"
using namespace std;

extern long long int commu_cost;



// int main()
// {
//     printf("hello world\n");
//     int element_length=273; 
//     int element_num=24897;
//     vector<string> database;
//     for(int i=0;i<element_num;i++)
//     {
//         string item(element_length,'a');
//         RAND_bytes((unsigned char*)item.data(), element_length);
//         database.push_back(item);
//     }
//     oram_client_wrapper client(6);
    
//     oram_server_wrapper server(database);
//     Timer time;
//     time.reset();
//     for(int i=0;i<100;i++)
//     {
//         string query=client.generate_query(i);
//         string response=server.response(query);
//         string recovered=client.recovery(response);
//         REQUIRE(recovered==database[i]);
//     }
//     auto duration = time.elapsed();
//     cout<<"Total query time for "<<element_num<<" items: "<<duration<<" ms."<<endl;




//     return 0;
// }

TEST_CASE("spiral_parallel")
{
    
}

TEST_CASE("test oram")
{
    // Block::BLOCK_SIZE=16;
    int bucket_size=4;
    int payload=256; 
    Block::BLOCK_SIZE=(payload+AES_BLOCK_SIZE)/sizeof(int); 
    int num_element=512;
    int res=ceil(log2l(num_element));
    int num_blocks=pow(2,res);
    
    Bucket::setMaxSize(bucket_size);	//set the bucket size
    UntrustedStorageInterface* storage = new ServerStorage();
    RandForOramInterface* random = new RandomForOram();
    OramInterface* oram = new OramReadPathEviction(storage, random, bucket_size, num_blocks);
    ((OramReadPathEviction *)oram)->enable_encryption=true;
    std::unique_ptr<int[]> data_ptr;
    data_ptr = std::make_unique<int[]>(payload*num_element/sizeof(int));
    for(int i=0;i<payload*num_element/sizeof(int);i++)
    {
        data_ptr[i]=i+1;
    }
    
    for(int i=0;i<num_element;i++)
    {
        int* newdata = new int[Block::BLOCK_SIZE-4];
        memcpy(newdata, data_ptr.get()+i*payload/sizeof(int), payload);
        oram->access(OramInterface::WRITE, i, newdata);
        delete[] newdata;
    }
    
    for(int i=0;i<num_element;i++)
    {
        int* res_data = oram->access(OramInterface::READ, i, nullptr);
        for(int j=0;j<payload/sizeof(int);j++)
        {
            REQUIRE(res_data[j]==data_ptr[i*payload/sizeof(int)+j]);
        }
        for(int j=0;j<10;j++)
        {
            std::cout<<res_data[j]<<" ";
        }
        std::cout<<std::endl;
        delete[] res_data;
    }
    delete oram;
}
std::ofstream outfile("oram_s_test_results5.txt",std::ios::app);
extern double enc_time;
void test_ORAM_S(int key_size,int value_size,int num_elements,int num_queries)
{
    int k_length=1<<key_size; 
    int v_length=1<<value_size; 
    int n_elements=1<<num_elements; 
    cout<<"Testing ORAM with "<<n_elements<<" elements, key size 2^"<<key_size<<" bytes, value size 2^"<<value_size<<" bytes."<<endl;
    outfile<<"Testing ORAM with "<<n_elements<<" elements, key size 2^"<<key_size<<" bytes, value size 2^"<<value_size<<" bytes."<<endl;
    vector<string> database;
    int element_length=k_length+v_length; 
    cout<<"Each element size: "<<element_length<<" bytes."<<endl;
    for(int i=0;i<n_elements;i++)
    {
        string item(element_length,'a');
        RAND_bytes((unsigned char*)item.data(), element_length);
        database.push_back(item);
    }
    Timer time;
    time.reset();
    oram_client_wrapper client(6);
    commu_cost=0;
    oram_server_wrapper server(database);
    auto setup_duration = time.elapsed();
    auto setup_commu = commu_cost;
    commu_cost=0;
    time.reset();
    int trail_num=10;
    enc_time=0.0;
    outfile<<"ORAM setup time: "<<setup_duration<<" ms, communication: "<<(double)setup_commu/1024.0/1024.0<<" MB."<<endl;
    cout<<"ORAM setup time: "<<setup_duration<<" ms, communication: "<<(double)setup_commu/1024.0/1024.0<<" MB."<<endl;
    for(int i=0;i<num_queries*trail_num;i++)
    {
        string query=client.generate_query(i);
        string response=server.response(query);
        string recovered=client.recovery(response);
        REQUIRE(recovered==database[i]);
    }    

    auto queery_duration = time.elapsed()/(double)trail_num;
    auto query_commu = commu_cost/(double)trail_num;
    enc_time/= (double)trail_num;
    cout<<"ORAM total query time for "<<num_queries<<" queries: "<<queery_duration<<" ms, communication: "<<(double)query_commu/1024.0/1024.0<<" MB."<<endl;
    outfile<<"ORAM total query time for "<<num_queries<<" queries: "<<queery_duration<<" ms, communication: "<<(double)query_commu/1024.0/1024.0<<" MB."<<endl;
    outfile<<"total encryption time: "<<enc_time<<" ms."<<endl;
    cout<<"total encryption time: "<<enc_time<<" ms."<<endl;
}

TEST_CASE("Test ORAM S","[oram_wrapper]")
{
    

    cout<<"Starting ORAM S tests..."<<endl;
    outfile<<"Starting ORAM S tests..."<<endl;
    cout<<"-------------------stage 1-------------------"<<endl;
    outfile<<"-------------------stage 1-------------------"<<endl;
    auto element_nums={18,19,20,21,22,23};
    for(auto num:element_nums)
    {
        test_ORAM_S(2,8,num,341); //key size 4 bytes, value size 256 bytes, 2^num elements, 100 queries
        cout<<endl;
    }
    cout<<endl;
    outfile<<endl;


    //**********continue */
    // cout<<"-------------------stage 2-------------------"<<endl;
    // outfile<<"-------------------stage 2-------------------"<<endl;
    // auto bucket_num={64,128,256,512,1024,2048,4096,8192};
    // for(auto length:bucket_num)
    // {
    //     test_ORAM_S(2,8,22,length/1.5); //key size 2^length bytes, value size 2^length bytes, 16384 elements, 100 queries
    //     cout<<endl;
    //     outfile<<endl;
    // }
    // cout<<"-------------------stage 3-------------------"<<endl;
    // outfile<<"-------------------stage 3-------------------"<<endl;
    // auto element_length={10,11,12,13,14,15,16};
    // // auto element_length={15,16};
    // for(auto length:element_length)
    // {
    //     test_ORAM_S(2,length-3,20,341); //key size 2^length bytes, value size 2^length bytes, 16384 elements, 100 queries
    //     cout<<endl;
    //     outfile<<endl;
    // }
    
}
TEST_CASE("Test oram_wrapper","[oram_wrapper]")
{
    int element_length=273; 
    int element_num=25130;
    vector<string> database;
    for(int i=0;i<element_num;i++)
    {
        string item(element_length,'a');
        RAND_bytes((unsigned char*)item.data(), element_length);
        database.push_back(item);
    }
    oram_client_wrapper client(6);
    oram_server_wrapper server(database);
    Timer time;
    time.reset();
    for(int i=0;i<100;i++)
    {
        string query=client.generate_query(i);
        string response=server.response(query);
        string recovered=client.recovery(response);
        REQUIRE(recovered==database[i]);
    }
    auto duration = time.elapsed();
    cout<<"Total query time for "<<element_num<<" items: "<<duration<<" ms."<<endl;
}

TEST_CASE("Test database","[spiral_pir]") {
    Spiral_client client(6);
    vector<string> database;
    int item_size=273;
    int item_num=24897;
    for(int i=0;i<item_num;i++)
    {
        string item(item_size,'c');
        for(int j=0;j<item_size;j++)
        {
            item[j]=(char)((rand())%256);
        }
        database.push_back(item);
    }
    Spiral_server server(database);
    Timer time;
    time.reset();
    for(int i=0;i<100;i++)
    {
        string query=client.generate_query(i);
        string response=server.response(query);
        string recovered=client.recovery(response);
        REQUIRE(recovered==database[i]);
       
            
    }
    auto duration = time.elapsed();
    cout<<"Total query time for "<<item_num<<" items: "<<duration<<" ms."<<endl;
}


TEST_CASE("Spiral PIR basic functionality test", "[spiral_pir]") {
    printf("Starting Spiral PIR basic functionality test...\n");
    size_t num_expansions = 4; // max = 7 //used to be 8
    size_t further_dims = 3;
    size_t total_n = (1 << num_expansions) * (1 << further_dims);
    int IDX_TARGET = 14;
    int IDX_DIM0 = IDX_TARGET / (1 << further_dims);
   
    PIR::spiral_PIR pir(num_expansions, further_dims);
    pir.IDX_TARGET = IDX_TARGET;
    pir.IDX_DIM0 = IDX_DIM0;
    // if (ubench) {
    //     testPacking();
    //     exit(0);
    // }
    // cout << "params:" << endl;
    // cout << "\tnum_expansions: " << num_expansions << endl;
    // cout << "\tfurther_dims: " << further_dims << endl;
    // cout << "\ttotal_n: " << total_n << endl;
    // cout << "\tIDX_TARGET: " << IDX_TARGET << endl;
    // cout << "\tIDX_DIM0: " << IDX_DIM0 << endl;
    // cout << "\tthreads: " << omp_get_num_threads() << endl;

    

    
    // do_MatPol_test();

    // setup_constants();

    // generate_gadgets();

    

    // load_db();
    vector<string> database;
    for(size_t i=0;i<total_n;i++)
    {
       
        string item(2048,'b');
        random_device rd;
        mt19937_64 gen2(rd()); // NOT SECURE
        uniform_int_distribution<uint64_t> dist(0, 255);
        for(size_t j=0;j<2048;j++)
        {
            item[j]=(char)dist(gen2);
        }
        database.push_back(item);
    }
    vector<string> database2=database;
    // do_test();
    //pir.load_db();
    // vector<string> database2;
    //pir.encode_db(database);
    cout<<"database size: "<<database.size()<<endl;
    cout<<"first item size: "<<database[0].size()<<endl;
    double setup_time=0;
    double query_time=0;
    start_timing();
    //pir.set_DB(pir.encoded_DB);


    //pir.set_DB(database);
    setup_time+=end_timing();
    cout<<"setup done."<<endl;
    cout<<"setup time: "<<setup_time/1000.0<<" ms"<<endl;
    string res;
    auto start_stage = chrono::high_resolution_clock::now();
    auto start = chrono::high_resolution_clock::now();
    size_t stage = 0;
    start_stage = chrono::high_resolution_clock::now();
    start = chrono::high_resolution_clock::now();
    start_timing();
    
    // for(int i=0;i<15;i++)
    // {
    //     int query_index = rand() % database.size();
    //     res.clear();
    //     pir.query(query_index,res);
    //     if(memcmp(res.data(),database2[query_index].data(),pir.item_size)==0)
    //     {
    //         cout<<"query success!"<<endl;
    //     }
    //     else
    //     {
    //         cout<<query_index<<endl;
    //         cout<<"query failed!"<<endl;
    //     }
    // }
    
    // pir.query(IDX_TARGET,res);
    // uint64_t duration_us = std::chrono::duration_cast<std::chrono::microseconds>(chrono::high_resolution_clock::now() - start_stage).count();
    // // double duration = (chrono::high_resolution_clock::now() - start_stage).count() / 1000;
    // // cout << "#\t\t\t\t\t";
    // // cout << "  >"
    // //      << duration
    // //      << endl;
    // // cout << "#\t\t\t\t\t END" << endl;
    // start_stage = chrono::high_resolution_clock::now();
    // start = chrono::high_resolution_clock::now();
    // stage = 0;
    // query_time+=duration_us;
    // cout<<"query done."<<endl;
    // cout<<"query time: "<<query_time/1000.0<<" ms"<<endl;
    // cout<<"res size: "<<res.length()<<endl;
    // memcmp(res.data(),database2[IDX_TARGET].data(),pir.item_size)==0?cout<<"query success!"<<endl:cout<<"query failed!"<<endl;
    // // memcmp(res.data(),database[IDX_TARGET+1].data(),pir.item_size)==0?cout<<"query success!"<<endl:cout<<"query failed!"<<endl;
    // // memcmp(res.data(),database[IDX_TARGET-1].data(),pir.item_size)==0?cout<<"query success!"<<endl:cout<<"query failed!"<<endl;
    // // time_key_gen = 0;
    // // time_query_gen = 0;
    // // time_expansion_main = 0;
    // // time_expansion_further = 0;
    // // time_conversion = 0;
    // // time_first_multiply = 0;
    // // time_folding = 0;
    // // time_decoding = 0;
    // // PIR::spiral_PIR pir2(num_expansions+1, further_dims);
    // // pir2.IDX_TARGET = IDX_TARGET;
    // // pir2.IDX_DIM0 = IDX_DIM0;
    // // pir2.load_db();
    // // pir2.do_test();
    pir.set_DB(database);
    // pir.query(105,res);
    // cout<<IDX_TARGET<<endl;
    //cout<<res<<endl;
    // std::strcmp(res.data(),database2[105].data())==0?cout<<"query success!"<<endl:cout<<"query failed!"<<endl;
    int test_size=100;
    Timer timer;
    timer.reset();
    for(int i=0;i<test_size;i++)
    {
        int query_index = rand() % database.size();
        res.clear();
        pir.query(query_index,res);
        std::strcmp(res.data(),database2[query_index].data())==0?cout<<"query "<<i<<" success!"<<endl:cout<<"query "<<i<<" failed!"<<endl;
    }
    auto elapsed = timer.elapsed();
    cout<<"Total query time over "<<test_size<<" queries: "<<elapsed <<" ms"<<endl;
    
    // for(size_t i=0;i<res.size();i++)
    // {
    //     printf("%02x ",(unsigned char)res[i]);
    //     if((i+1)%16==0)
    //         printf("\n");
    // }
    // cout<<endl;
    // for(size_t i=0;i<res.size();i++)
    // {
    //     printf("%02x ",(unsigned char)database2[105][i]);
    //     if((i+1)%16==0)
    //         printf("\n");
    // }
    // cout<<endl;
    //pir.query(IDX_TARGET,res);
    // cout<<"==========================="<<endl;
    // MatPoly p_tp(n0,n0,false);
    // generate_random_pt(p_tp);
    // MatPoly p_tp_1(n0,n0,false);
    // edb_gen(p_tp_1,database[IDX_TARGET]);
    // cout<<"original pt vs generated pt:"<<endl;
    // for(int i=0;i<n0*n0*poly_len;i++)
    // {
    //     if(p_tp.data[i]!=p_tp_1.data[i])
    //     {
    //         cout<<i<<": "<<p_tp.data[i]<<" vs "<<p_tp_1.data[i]<<endl;
    //     }
    // }

}