#include "Spiral_Instance.h"


int Spiral_server::dim0=6;
int Spiral_server::further_dims=2;



Spiral_client::Spiral_client(uint64_t index){
    this->query = to_string(index);
}
Spiral_client::~Spiral_client(){

}

string Spiral_client::generate_query(uint64_t index){
    this->query = to_string(index);
    return this->query;
}
string Spiral_client::recovery(string response){
    return response;
}
Spiral_server::Spiral_server(vector<string> Content){
    // Spiral_server::dim0=4;
    // Spiral_server::further_dims=3;
    int item_size=0;
    for(const auto& item : Content) {
        if(item_size==0){
            item_size = item.size();
        }
        else if(item_size != item.size()){
            cout<<"Error: different item size in the database!"<<endl;
            exit(-1);
        }
    }
    this->item_size = item_size;  
    int datasize=Content.size()*item_size;  
    int p_mod=log2(p_db);
    int element_byte_length=n0*n0*poly_len*p_mod/8;  
    //cout<<"Each element size in Spiral_server constructor: "<<element_byte_length<<" bytes."<<endl;
    //cout<<"Database size in Spiral_server constructor: "<<datasize<<" bytes."<<endl;
    size_t num_dim0 = 1 << Spiral_server::dim0; //16
    size_t num_further_dims = 1 << Spiral_server::further_dims; //8
    size_t total_n = num_dim0 * num_further_dims; //128
    //cout<<"Total n in Spiral_server constructor: "<<total_n<<endl;
    if(total_n*element_byte_length < datasize){
        cout<<"Error: Database size exceeds the maximum capacity of Spiral PIR with current parameters!"<<endl;
        exit(-1);
    }
    
    this->pir_ptr = std::make_unique<PIR::spiral_PIR>(Spiral_server::dim0,Spiral_server::further_dims);

    vector<string> db_padded;
    // size_t num_dim0 = 1 << Spiral_server::dim0; //16
    // size_t num_further_dims = 1 << Spiral_server::further_dims; //8
    // size_t total_n = num_dim0 * num_further_dims; //128
    // cout<<"Total n in Spiral_server constructor: "<<total_n<<endl;
    string tmp;
    tmp.resize(element_byte_length,'a');
    int num_per_row=element_byte_length/item_size;
    // int item_per_row=num_per_row*item_size; 
    for(int i=0;i<Content.size();i++){
        int offset = i % num_per_row;
        tmp.replace(offset*item_size,item_size,Content[i]);
        if((i+1)%num_per_row==0){
            db_padded.push_back(tmp);
            tmp.clear();
            tmp.resize(element_byte_length,'a');
        }
    }
    db_padded.push_back(tmp);
    if(db_padded.size()>total_n){
        cout<<"Error: Database size exceeds the maximum capacity of Spiral PIR with current parameters!"<<endl;
        exit(-1);
    }
   
    tmp.clear();
    tmp.resize(element_byte_length,'a');
    this->data_num=db_padded.size();
    while(db_padded.size()<total_n){
        db_padded.push_back(tmp);
    }
    
    this->Content =Content;
    // this->Content = db_padded;
    this->pir_ptr->set_DB(db_padded);

}

string Spiral_server::response(string query){
    size_t index = stoi(query);
    string res;
    
    int p_mod=log2(p_db);
    int element_byte_length=n0*n0*poly_len*p_mod/8;  
    int num_per_row=element_byte_length/item_size;
    int ii=index/num_per_row;
    int offset=index%num_per_row;
    if(ii>this->data_num){
        cout<<"Error: query index exceeds the database size!"<<endl;
        exit(-1);
    }
    string tp;
    this->pir_ptr->query(ii,tp);
    //tp=this->Content[ii];
    res=tp.substr(offset*item_size,item_size);
    return res;
}
