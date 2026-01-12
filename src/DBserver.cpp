#include "DBserver.h"

DBServer::DBServer(vector<vector<string>> Encrypted_db, DBClient *client){
    this->Encrypted_db = Encrypted_db;
    client->Encrypted_KV.clear();
    this->client = client;
    this->debug = client->debug;
    this->bucket_number = client->bucket_number;
    this->PIRType = client->PIRType;
    this->FakePIRs =nullptr;
    this->PIR = nullptr;
}
DBServer::~DBServer(){
    if (this->FakePIRs != NULL){
        delete[] this->FakePIRs;
        this->FakePIRs = nullptr;
    }
    else if (this->PIR !=nullptr){
        delete this->PIR;
        this->PIR = nullptr;
    }
}
void DBServer::setup(){
    //setup the PIR
    this->FakePIRs = new vector<FakePIRServer>[this->bucket_number];
    if (this->PIRType == "FakePIR"){
        for(uint64_t i = 0; i < this->bucket_number; i++){
            FakePIRServer fakepirserver(this->Encrypted_db[i]);
            this->FakePIRs->emplace_back(fakepirserver);
        }
    }
    else if(this->PIRType == "SpiralPIR"){
        this->Spiral_servers_uptr.clear();
        this->Spiral_servers_uptr.resize(this->bucket_number);
        #pragma omp parallel for
        for(uint64_t i = 0; i < this->bucket_number; i++){
            auto ptr= std::make_unique<Spiral_server>(this->Encrypted_db[i]);
            this->Spiral_servers_uptr[i] = std::move(ptr);
        }
    }
    else if(this->PIRType == "ORAM"){
        this->oram_servers_uptr.clear();
        this->oram_servers_uptr.resize(this->bucket_number);
        #pragma omp parallel for
        for(uint64_t i = 0; i < this->bucket_number; i++){
            std::unique_ptr<oram_server_wrapper> oram_server = std::make_unique<oram_server_wrapper>(this->Encrypted_db[i]);
            this->oram_servers_uptr[i] = std::move(oram_server);
        }
    }
    else if (this->PIRType == "PIRANA"){
        this->client->PIRANA_params->set_bucket_size(this->Encrypted_db[0].size());
        this->client->PIRANA_params->set_encode_size();
        this->PIR = new PIRANA_Server(*this->client->PIRANA_params,true,0,this->Encrypted_db);
        this->PIR->set_bucket_size(this->Encrypted_db[0].size());
        this->PIR->set_encode_size();
        this->PIR->set_keys(this->client->keys);
    }
    else{
        cout<<"Error: The PIR type is not supported."<<endl;
        exit(1);
    }
    // if (!this->debug){
    //     this->Encrypted_db.clear();
    // }
    if (this->debug){
        cout<<"Server Setup finished."<<endl;
    }
}

stringstream DBServer::query_response_PIRANA(stringstream& generated_query_for_pir){
    //generate the response for each query
    if (this->debug){
        cout<<"Query response begin"<<endl;
    }
    stringstream response = this->PIR->gen_batch_response(generated_query_for_pir);
    return response;
}


vector<string> DBServer::query_response_FakePIR(vector<string> generated_query_for_pir){
    //generate the response for each query
    if (this->debug){
        cout<<"Query response begin"<<endl;
    }
    vector<string> response(this->bucket_number);
    if(this->PIRType =="FakePIR")
    {
        #pragma omp parallel for
        for (uint64_t i=0;i<this->bucket_number;i++){
            response[i] = (*this->FakePIRs)[i].response(generated_query_for_pir[i]);
        }
    }
    else if(this->PIRType =="SpiralPIR")
    {
        #pragma omp parallel for
        for (uint64_t i=0;i<this->bucket_number;i++){
            response[i] = this->Spiral_servers_uptr[i]->response(generated_query_for_pir[i]);
        }
    }
    else if(this->PIRType =="ORAM")
    {
        #pragma omp parallel for
        for (uint64_t i=0;i<this->bucket_number;i++){
            response[i] = this->oram_servers_uptr[i]->response(generated_query_for_pir[i]);
        }
    }
    else{
        cout<<"Error: The PIR type is not supported."<<endl;
        exit(1);
    }
   
    if (this->debug){
        cout<<"Query response finished."<<endl;
    }
    return response;
}

vector<string> DBServer::get_bucket_content(bool is_keep_encrypted_DB){
    //get the content of the bucket
    if (this->debug){
        cout<<"Get bucket content begin"<<endl;
    }
    uint64_t bucket_id = this->reconstruct_time%this->bucket_number;

    vector<string> content;
    if (this->PIRType == "FakePIR"){
        content = (*this->FakePIRs)[bucket_id].Content;
    }
    else if (this->PIRType == "SpiralPIR"){
        content = this->Spiral_servers_uptr[bucket_id]->Content;
    }
    else if (this->PIRType == "ORAM"){
        content = this->oram_servers_uptr[bucket_id]->Content;
    }
    else if (this->PIRType == "PIRANA"){
        content = this->PIR->get_bucket_content(bucket_id, is_keep_encrypted_DB);
    }
    else{
        cout<<"Error: The PIR type is not supported."<<endl;
        exit(1);
    }
    
    if (this->debug){
        cout<<"Get bucket content finished."<<endl;
    }
    return content;
}

void DBServer::reconstruct_bucket_content(vector<string> encrypted_bucket_content){
    //reconstruct the bucket content
    if (this->debug){
        cout<<"Reconstruct bucket content begin"<<endl;
    }
    
    if (this->PIRType == "FakePIR"){
        FakePIRServer fakepirserver(encrypted_bucket_content);
        (*this->FakePIRs)[this->reconstruct_time%this->bucket_number] = fakepirserver;
    }
    else if (this->PIRType == "SpiralPIR"){
        // Spiral_server spiral_server(encrypted_bucket_content);
        //Warning: 该功能可能会报错
        auto ptr= std::make_unique<Spiral_server>(encrypted_bucket_content);
        this->Spiral_servers_uptr[this->reconstruct_time%this->bucket_number] = std::move(ptr);
    }
    else if (this->PIRType == "ORAM"){
        auto ptr = std::make_unique<oram_server_wrapper>(encrypted_bucket_content);
        this->oram_servers_uptr[this->reconstruct_time%this->bucket_number] = std::move(ptr);
    }
    else if (this->PIRType == "PIRANA"){
        #ifdef TEST_FOR_DEBUG
        int num=std::stoi(encrypted_bucket_content[0]);
        // this->reconstruct_time=this->reconstruct_time-num+1;
        this->PIR->reconstruct(encrypted_bucket_content, (this->reconstruct_time-num+1)%this->bucket_number);
        #else
        this->PIR->reconstruct(encrypted_bucket_content, this->reconstruct_time%this->bucket_number);
        #endif
        //this->Encrypted_db[this->reconstruct_time%this->bucket_number] = encrypted_bucket_content;
    }
    else{
        cout<<"Error: The PIR type is not supported."<<endl;
        exit(1);
    }

    if (this->debug){
        cout<<"Reconstruct bucket content finished."<<endl;
    }

    this->reconstruct_time += 1;
}