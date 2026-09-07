#include"oram_wrapper.h"
#include <assert.h>

long long int commu_cost=0;
double enc_time=0.0;
oram_client_wrapper::oram_client_wrapper(uint64_t index){
    this->query = to_string(index);
}
string oram_client_wrapper::generate_query(uint64_t index){
    this->query = to_string(index);
    return this->query;
}
string oram_client_wrapper::recovery(string response){
    return response;    
}



oram_server_wrapper::oram_server_wrapper(vector<string> Content){
    this->Content = Content;
    this->element_size=Content[0].size();
    this->block_size=(element_size+AES_BLOCK_SIZE+3)/sizeof(int);  
    Block::BLOCK_SIZE= this->block_size;  
    for(auto &item:Content){
        if(item.size()!=element_size){
            throw invalid_argument("All elements must have the same length.");
        }
        if(item.size() % sizeof(int) != 0) {
            //padding
            for(int i=0;i<sizeof(int)-(element_size % sizeof(int));i++){
                item.push_back('a');  
            }
        }
        assert(item.size() % sizeof(int) == 0);
    }
    int res=ceil(log2l(Content.size()));
    int numBlocks=1<<res;
    int bucket_size=4;  
    Bucket::setMaxSize(bucket_size);  
    UntrustedStorageInterface* storage = new ServerStorage();
    RandForOramInterface* random = new RandomForOram();
    // this->oram = std::make_unique<OramReadPathEviction>(storage, random, bucket_size, numBlocks);
    // // dynamic_cast<OramReadPathEviction*>(this->oram.get())->enable_encryption = true;  
    // this->oram->enable_encryption = true;  
    vector<vector<int>> init_data;
    for(auto &item:Content){
        vector<int> data_vec((element_size+3)/sizeof(int));
        memcpy(data_vec.data(),item.data(),item.size());
        init_data.push_back(data_vec);;
    }
    
    this->oram = std::make_unique<OramReadPathEviction>(storage, random, bucket_size, numBlocks,init_data);
    
    return;

}

string oram_server_wrapper::response(string query)
{
    Block::BLOCK_SIZE= this->block_size;  
    uint64_t index=stoi(query);
    int* data=this->oram->access(OramInterface::READ, index, nullptr);
    string result;
    result.resize(this->element_size);
    memcpy(result.data(),data,this->element_size);
    delete[] data;
    return result;
}

