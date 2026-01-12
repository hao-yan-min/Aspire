#ifndef __DBSERVER__H
#define __DBSERVER__H

#include "DBclient.h"
#include "FakePIR.h"
#include "PIRANA_server.h"
#include "PIRANA_pir_parms.h"
#include "utils.h"
#include "Spiral_Instance.h"
#include <sstream>
#include "RecSplit.hpp"
#include "seal/seal.h"
#include <thread>
#include <map>
#include <sys/stat.h> 


class DBServer{

    public:
    bool debug = false;
    vector<vector<string>> Encrypted_db;
    DBClient *client;
    vector<std::unique_ptr<Spiral_server>> Spiral_servers_uptr;
    uint64_t bucket_number;
    uint64_t reconstruct_time = 0;

    string PIRType = "FakePIR";
    vector<std::unique_ptr<oram_server_wrapper>> oram_servers_uptr;
    vector<FakePIRServer> *FakePIRs;
    PIRANA_Server * PIR;

    DBServer(vector<vector<string>> Encrypted_db, DBClient *client);
    ~DBServer();
    void setup();
    vector<string> query_response_FakePIR(vector<string> generated_query_for_pir);
    //vector<string> query_response_SpiralPIR(vector<string> generated_query_for_pir);
    stringstream query_response_PIRANA(stringstream& generated_query_for_pir);
    vector<string> get_bucket_content(bool is_keep_encrypted_DB = true);
    void reconstruct_bucket_content(vector<string> encrypted_bucket_content);
};

#endif