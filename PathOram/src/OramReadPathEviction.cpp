//
//
//

#include "OramReadPathEviction.h"
#include <iostream>

#include <string>
#include <sstream>
#include <cmath>

extern long long int commu_cost;
extern double enc_time;
OramReadPathEviction::OramReadPathEviction(UntrustedStorageInterface* storage, RandForOramInterface* rand_gen,
                                           int bucket_size, int num_blocks) {
    this->storage = storage;
    this->rand_gen = rand_gen;
    this->bucket_size = bucket_size;
    this->enable_encryption = true;
    this->num_blocks = num_blocks;
    this->num_levels = ceil(log10(num_blocks) / log10(2)) + 1;
    this->num_buckets = pow(2, num_levels)-1;
    if (this->num_buckets*this->bucket_size < this->num_blocks) //deal with precision loss
    {
        throw new runtime_error("Not enough space for the acutal number of blocks.");
    }
    this->num_leaves = pow(2, num_levels-1);
    Bucket::resetState();
    Bucket::setMaxSize(bucket_size);
    this->rand_gen->setBound(num_leaves);
    this->storage->setCapacity(num_buckets);
    this->position_map = new int[this->num_blocks];
    this->stash = vector<Block>();
    this->key = std::unique_ptr<unsigned char[]>(new unsigned char[16]); //16 bytes key
    RAND_bytes(this->key.get(), AES_KEY_LENGTH);

    for (int i = 0; i < this->num_blocks; i++){
        position_map[i] = rand_gen->getRandomLeaf();
    }

    for(int i = 0; i < num_buckets; i++){

        Bucket init_bkt = Bucket();
        for(int j = 0; j < bucket_size; j++){
            init_bkt.addBlock(Block());
        }
        storage->WriteBucket(i, Bucket(init_bkt));
    }

}

OramReadPathEviction::OramReadPathEviction(UntrustedStorageInterface* storage, RandForOramInterface* rand_gen,
                                           int bucket_size, int num_blocks,vector<vector<int>> init_data) {
    this->storage = storage;
    this->rand_gen = rand_gen;
    this->bucket_size = bucket_size;
    this->enable_encryption = true;
    this->num_blocks = num_blocks;
    this->num_levels = ceil(log10(num_blocks) / log10(2)) + 1;
    this->num_buckets = pow(2, num_levels)-1;
    if (this->num_buckets*this->bucket_size < this->num_blocks) //deal with precision loss
    {
        throw new runtime_error("Not enough space for the acutal number of blocks.");
    }
    this->num_leaves = pow(2, num_levels-1);
    Bucket::resetState();
    Bucket::setMaxSize(bucket_size);
    this->rand_gen->setBound(num_leaves);
    this->storage->setCapacity(num_buckets);
    this->position_map = new int[this->num_blocks];
    this->stash = vector<Block>();
    this->key = std::unique_ptr<unsigned char[]>(new unsigned char[16]); //16 bytes key
    RAND_bytes(this->key.get(), AES_KEY_LENGTH);

    for (int i = 0; i < this->num_blocks; i++){
        position_map[i] = rand_gen->getRandomLeaf();  
    }
    
    for(int i = 0; i < num_buckets; i++){

        Bucket init_bkt = Bucket();
        for(int j = 0; j < bucket_size; j++){
            init_bkt.addBlock(Block());
        }
        storage->WriteBucket(i, Bucket(init_bkt));
    }


    
    vector<Block> init_blocks;
    // std::unique_ptr<int[]> newdata_ptr;
    // newdata_ptr = std::unique_ptr<int[]>(new int[Block::BLOCK_SIZE]);
    unsigned char* ciphertext = new unsigned char[Block::BLOCK_SIZE * sizeof(int)-16];
    unsigned char* plaintext = new unsigned char[Block::BLOCK_SIZE * sizeof(int)-16];
    for(int i=0;i<init_data.size();i++){
        int* raw_data_ptr = new int[Block::BLOCK_SIZE]; 
        memset(raw_data_ptr,0,Block::BLOCK_SIZE*sizeof(int));
        memset(ciphertext,0,Block::BLOCK_SIZE * sizeof(int)-16);
        memset(plaintext,0,Block::BLOCK_SIZE * sizeof(int)-16);
        unsigned char iv[AES_KEY_LENGTH];
        RAND_bytes(iv, AES_KEY_LENGTH);
        memcpy(reinterpret_cast<unsigned char*>(raw_data_ptr),iv,AES_KEY_LENGTH);
        
        
        memcpy(plaintext, reinterpret_cast<unsigned char*>(init_data[i].data()), Block::BLOCK_SIZE * sizeof(int)-16);
        
        auto start = std::chrono::high_resolution_clock::now();
        aes_encrypt(plaintext, Block::BLOCK_SIZE * sizeof(int)-16, this->key.get(), iv, ciphertext);
        auto end = std::chrono::high_resolution_clock::now();
        enc_time += std::chrono::duration<double, std::milli>(end - start).count();
         
        memcpy(reinterpret_cast<unsigned char*>(raw_data_ptr)+16, ciphertext, Block::BLOCK_SIZE * sizeof(int)-16);
        Block newBlock = Block(position_map[i], i, raw_data_ptr);
        delete[] raw_data_ptr;
        init_blocks.push_back(newBlock);
        
    }
    delete[] plaintext;
    delete[] ciphertext;
    
    // for(int leaf_id=0;leaf_id<num_leaves;leaf_id++){
    //     for(int level=num_levels-1;level>=0;level--){
    //         int Pxl = OramReadPathEviction::P(leaf_id, level);
    //         Bucket bucket = this->storage->ReadBucket(Pxl);
    //         int counter = bucket.getBlocks().size();
    //         for(auto b_block:bucket.getBlocks()){
    //             if(b_block.index==-1){
    //                 counter--;
    //             }
    //         }
    //         vector<int> bid_evicted = vector<int>();
    //         for(auto & b:init_blocks){
    //             if(counter>=bucket_size){
    //                 break;
    //             }
    //             if(Pxl == OramReadPathEviction::P(b.leaf_id, level)){
    //                 bucket.removeBlock(Block()); //remove a dummy block
    //                 bucket.addBlock(b);
    //                 bid_evicted.push_back(b.index);
    //                 counter++;
    //             }
    //         }
    //         //remove from init_blocks
    //         for(int i = 0; i < bid_evicted.size(); i++)
    //         {   
                
    //             for(int j=0; j<init_blocks.size(); j++)
    //             {
    //                 Block b_instash = init_blocks[j];
    //                 if(b_instash.index == bid_evicted[i]){
    //                     init_blocks.erase(init_blocks.begin() + j);
    //                     break;
    //                 }
    //             }
    //         }
    //         storage->WriteBucket(Pxl, bucket);
            
    //     }

    // }
    vector<int> bid_evicted = vector<int>();
    for(auto & b:init_blocks)
    {
        for(int level=num_levels-1;level>=0;level--){
            int Pxl = OramReadPathEviction::P(b.leaf_id, level); 
            Bucket bucket = this->storage->ReadBucket(Pxl);
            int counter = bucket.getBlocks().size();
            for(auto b_block:bucket.getBlocks()){
                if(b_block.index==-1){
                    counter--;
                }
            }
            if(counter>=bucket_size){
                continue;
            }
            bucket.removeBlock(Block()); //remove a dummy block
            bucket.addBlock(b);
            bid_evicted.push_back(b.index);
            this->storage->WriteBucket(Pxl, bucket);
            break;            
        }
    }
    if(init_blocks.size()>bid_evicted.size())
    {
        //remove from init_blocks
        for(int i = 0; i < bid_evicted.size(); i++)
        {   
            
            for(int j=0; j<init_blocks.size(); j++)
            {
                Block b_instash = init_blocks[j];
                if(b_instash.index == bid_evicted[i]){
                    init_blocks.erase(init_blocks.begin() + j);
                    break;
                }
            }
        }
    }
    else if(init_blocks.size()==bid_evicted.size())
    {
        init_blocks.clear();
    }
    if(init_blocks.size()>0)
    {
        cout<<"init_blocks size: "<<init_blocks.size()<<endl;
    }
   
    for(auto & b:init_blocks){
        Block b_instash = b;
        stash.push_back(b);
    }
    
    commu_cost += (long long)num_buckets * bucket_size * (Block::BLOCK_SIZE)*4; //count the number of bytes read/written during initialization

    

}


static std::once_flag flag;
int* OramReadPathEviction::access(Operation op, int blockIndex, int *newdata) {
    std::unique_ptr<int[]> newdata_ptr;
    std::call_once(flag, [](){
        printf("ORAM write with encryption enabled\n");
        });
    if(this->enable_encryption&& op==WRITE){
        
        
        // printf("encrypting data before writing\n");
        newdata_ptr = std::unique_ptr<int[]>(new int[Block::BLOCK_SIZE]);
        unsigned char iv[AES_KEY_LENGTH];
        RAND_bytes(iv, AES_KEY_LENGTH);
        memcpy(reinterpret_cast<unsigned char*>(newdata_ptr.get()),iv,AES_KEY_LENGTH);
        unsigned char* plaintext = new unsigned char[Block::BLOCK_SIZE * sizeof(int)-16];
        
        memcpy(plaintext, reinterpret_cast<unsigned char*>(newdata), Block::BLOCK_SIZE * sizeof(int)-16);
        unsigned char* ciphertext = new unsigned char[Block::BLOCK_SIZE * sizeof(int)-16];
        auto start = std::chrono::high_resolution_clock::now();
        aes_encrypt(plaintext, Block::BLOCK_SIZE * sizeof(int)-16, this->key.get(), iv, ciphertext);
        auto end = std::chrono::high_resolution_clock::now();
        enc_time += std::chrono::duration<double, std::milli>(end - start).count();
        
        memcpy(reinterpret_cast<unsigned char*>(newdata_ptr.get())+16, ciphertext, Block::BLOCK_SIZE * sizeof(int)-16);
        delete[] plaintext;
        delete[] ciphertext;
    }
    
    int *data = new int[Block::BLOCK_SIZE];
    int oldLeaf = position_map[blockIndex];
    position_map[blockIndex] = rand_gen->getRandomLeaf();
    for (int i = 0; i < num_levels; i++) {
        vector<Block> blocks = storage->ReadBucket(OramReadPathEviction::P(oldLeaf, i)).getBlocks();
        commu_cost += blocks.size() * (Block::BLOCK_SIZE+2)*4; //count the number of bytes read
        for (Block b: blocks) {
            if (b.index != -1) {
                stash.push_back(Block(b));
            }
        }

    }

    Block *targetBlock = NULL;
    int targetPos = 0;
    for (int i = 0; i < stash.size(); i++) {
        Block b = stash[i];
        if (b.index == blockIndex) {
            targetBlock = &b;
            targetPos = i;
            break;
        }
    }


    if (op == Operation::WRITE) {
        
        int* data_tp=new int[Block::BLOCK_SIZE];
        if(this->enable_encryption){
            memcpy(data_tp,newdata_ptr.get(),Block::BLOCK_SIZE*sizeof(int));
        }
        else{
            memcpy(data_tp,newdata,Block::BLOCK_SIZE*sizeof(int));
        }
        if (targetBlock == NULL) {
            Block newBlock = Block(position_map[blockIndex], blockIndex, data_tp);
            stash.push_back(newBlock); 
        } else {
            for(int i =0; i < Block::BLOCK_SIZE; i++){
                targetBlock->data[i] = data_tp[i];
            }
            stash[targetPos] = Block(*targetBlock);
        }
        delete[] data_tp;
    } 
    else {
        if(targetBlock == NULL){
            delete[] data;
            data = NULL;
        }
        else {
            for(int i = 0; i < Block::BLOCK_SIZE; i++){
                data[i] = targetBlock->data[i];
            }
        }
       
    }
    if(this->enable_encryption)
    {
        int plainblock[Block::BLOCK_SIZE];
        
        auto start = std::chrono::high_resolution_clock::now();
        for(auto & n_block:stash)  
        {
            block_decrypt(n_block.data.get(), this->key.get(), plainblock);
            block_encrypt(plainblock, this->key.get(), n_block.data.get());
        }
        auto end = std::chrono::high_resolution_clock::now();
        enc_time += std::chrono::duration<double, std::milli>(end - start).count();
    }
    // for(auto & n_block:stash)  
    // {

    // }


    // Eviction steps: write to the same path that was read from.
    for (int l = num_levels - 1; l >= 0; l--) {

        vector<int> bid_evicted = vector<int>();
        Bucket bucket = Bucket();
        int Pxl = P(oldLeaf, l);
        int counter = 0;

        for (Block b_instash : stash) {
            
            if (counter >= bucket_size) {
                break;
            }
            Block be_evicted = Block(b_instash);
            if (Pxl == P(position_map[be_evicted.index], l)) {
                bucket.addBlock(be_evicted);

                bid_evicted.push_back(be_evicted.index);
                counter++;
            }
        }

        //remove from the stash
        for(int i = 0; i < bid_evicted.size(); i++)
        {   
            
            for(int j=0; j<stash.size(); j++)
            {
                Block b_instash = stash.at(j);
                if(b_instash.index == bid_evicted.at(i))
                {   
                    this->stash.erase(this->stash.begin() + j);

                    break;
                }
            }
            
        }

        while(counter < bucket_size)
        {
            bucket.addBlock(Block()); //dummy block
            counter++;
        }
        storage->WriteBucket(Pxl, bucket);

    }
    if(this->enable_encryption&& op==READ&&data!=NULL){
        int plainblock[Block::BLOCK_SIZE-4];
        auto start = std::chrono::high_resolution_clock::now();
        block_decrypt(data, this->key.get(), plainblock);
        auto end = std::chrono::high_resolution_clock::now();
        enc_time += std::chrono::duration<double, std::milli>(end - start).count();
        delete[] data;
        data = new int[Block::BLOCK_SIZE-4];
        for(int i=0;i<Block::BLOCK_SIZE-4;i++){
            data[i]=plainblock[i];
        }
    }
    return data;
    
}

int OramReadPathEviction::P(int leaf, int level) {
    /*
    * This function should be deterministic. 
    * INPUT: leaf in range 0 to num_leaves - 1, level in range 0 to num_levels - 1. 
    * OUTPUT: Returns the location in the storage of the bucket which is at the input level and leaf.
    */
    return (1<<level) - 1 + (leaf >> (this->num_levels - level - 1));
}


/*
The below functions are to access various parameters, as described by their names.
INPUT: No input
OUTPUT: Value of internal variables given in the name.
*/

int* OramReadPathEviction::getPositionMap() {
    return this->position_map;
}

vector<Block> OramReadPathEviction::getStash() {
    return this->stash;
}
    
int OramReadPathEviction::getStashSize() {
    return (this->stash).size();
}
    
int OramReadPathEviction::getNumLeaves() {
    return this->num_leaves;

}

int OramReadPathEviction::getNumLevels() {
    return this->num_levels;

}

int OramReadPathEviction::getNumBlocks() {
    return this->num_blocks;

}

int OramReadPathEviction::getNumBuckets() {
    return this->num_buckets;

}

OramReadPathEviction::~OramReadPathEviction() {
    //printf("delete oram\n");
    delete[] this->position_map;
    delete this->storage;
    delete this->rand_gen;
}
