#include"spiral.h"


void enc_str_test()
{
    std::string s = "abcdefghijsdsdsdsopqrstuvwxyz";
    size_t a = 4;
    int mod_len= (int)log2((double)a);
    vector<int> res;
    for(auto & ch:s)
    {
        int ch_int=(int)(unsigned char)ch;
        for(int i=0;i<8*sizeof(char)/mod_len;i++)
        {
            res.push_back(ch_int%(a));
            ch_int=ch_int>>mod_len;
        }
         
    }
    std::string s2;
    for(size_t i=0;i<res.size()/ (8*sizeof(char)/mod_len);i++)
    {
        int ch_int=0;
        for(int j=8*sizeof(char)/mod_len-1;j>=0;j--)
        {
            int tps=res[i*(8*sizeof(char)/mod_len)+j];
            ch_int<<=mod_len;
            ch_int|=tps;
        }
        s2.push_back((char)ch_int);
    }
    cout<<"original string: "<<s<<endl;
    cout<<"decoded string: "<<s2<<endl;
}
int main2()
{
    enc_str_test();
    return 0;
}
int main(int argc, char** argv) {
    for(int i=0;i<argc;i++)
        cout<<argv[i]<<" ";
    cout<<endl;
    size_t num_expansions = 4; // max = 7 //used to be 8
    size_t further_dims = 4;
    size_t total_n = (1 << num_expansions) * (1 << further_dims);
    int IDX_TARGET = 14;
    int IDX_DIM0 = IDX_TARGET / (1 << further_dims);
    if (argc > 1) {
        num_expansions = strtol(argv[1], NULL, 10); // max = 7 //used to be 8
        further_dims = strtol(argv[2], NULL, 10);
        total_n = (1 << num_expansions) * (1 << further_dims);
        IDX_TARGET = strtol(argv[3], NULL, 10);
        IDX_DIM0 = IDX_TARGET / (1 << further_dims);
        if (argc >= 5) {
            
            // if (dbFilename.length() <= 2) has_file = false;
            for (size_t i = 5; i < argc; i++) {
                
                if (strcmp(argv[i], "--nonoise") == 0) {
                    cout << "Using no noise" << endl;
                    nonoise = true;
                }
                
                if (strcmp(argv[i], "--random-data") == 0) {
                    cout << "Using random data..." << endl;
                    random_data = true;
                    dummyWorkingSet = min((1UL << 25) / (total_n), poly_len);
                    max_trials = (1L << 16) / (total_n);
                    if (max_trials == 0) {
                        max_trials = 1;
                    }
                }
                if (strcmp(argv[i], "--show-diff") == 0) {
                    cout << "Showing diff..." << endl;
                    show_diff = true;
                }
                
                if (strcmp(argv[i], "--direct-upload") == 0) {
                    cout << "Direct uploading of query (no compression)" << endl;
                    direct_upload = true;
                }
               
            }
        }
    }
    PIR::spiral_PIR pir(num_expansions, further_dims);
    pir.IDX_TARGET = IDX_TARGET;
    pir.IDX_DIM0 = IDX_DIM0;
    
    

   

    

    
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
    
    
    pir.set_DB(database);
    
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
    
    return 0;
    
}