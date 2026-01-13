#pragma once
#include "OramInterface.h"
#include "OramReadPathEviction.h"
#include "UntrustedStorageInterface.h"
#include "ServerStorage.h"
#include "Bucket.h"
#include "ServerStorage.h"
#include "OramInterface.h"
#include "RandForOramInterface.h"
#include "RandomForOram.h"
#include "UntrustedStorageInterface.h"
#include "OramReadPathEviction.h"

class oram_client_wrapper {
public:
    uint64_t index;
    string query;
    oram_client_wrapper(uint64_t index);
    string generate_query(uint64_t index);
    string recovery(string response);
};
class oram_server_wrapper {
public:
    vector<string> Content;
    int block_size;  //element size
    int element_size;
    oram_server_wrapper(vector<string> Content);
    ~oram_server_wrapper(){};
    std::unique_ptr<OramReadPathEviction> oram;
    string response(string query);
};
