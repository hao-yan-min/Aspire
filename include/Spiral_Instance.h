#pragma once
#include"spiral.h"
#include <memory>

class Spiral_client{
    public:
    uint64_t index;
    string query;
    ~Spiral_client();
    Spiral_client(uint64_t index);
    string generate_query(uint64_t index);
    string recovery(string response);
};

class Spiral_server{
    public:
    vector<string> Content;
    int item_size;
    int data_num;
    
    std::unique_ptr<PIR::spiral_PIR> pir_ptr;
    static int dim0;
    static int further_dims;
    Spiral_server(vector<string> Content);  
    string response(string query);
};