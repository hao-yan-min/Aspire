#ifndef __FAKEPIR__H
#define __FAKEPIR__H
#include <vector>
#include <string>
#include <iostream>
using namespace std;

class FakePIRClient{

public:
    uint64_t index;
    string query;
    FakePIRClient(uint64_t index);
    string generate_query(uint64_t index);
    string recovery(string response);
};

class FakePIRServer{

public:
    vector<string> Content;

    FakePIRServer(vector<string> Content);
    string response(string query);
};

#endif
