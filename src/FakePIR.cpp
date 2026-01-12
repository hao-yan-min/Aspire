#include "FakePIR.h"


FakePIRClient::FakePIRClient(uint64_t index){
    this->query = to_string(index);
}
string FakePIRClient::generate_query(uint64_t index){
    this->query = to_string(index);
    return this->query;
}
string FakePIRClient::recovery(string response){
    return response;
}

FakePIRServer::FakePIRServer(vector<string> Content){
    this->Content = Content;
}
string FakePIRServer::response(string query){
    uint64_t index = stoi(query);
    return this->Content[index];
}