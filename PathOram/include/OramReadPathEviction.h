//
//
//

#ifndef PORAM_ORAMREADPATHEVICTION_H
#define PORAM_ORAMREADPATHEVICTION_H
#include<mutex>
#include "OramInterface.h"
#include "RandForOramInterface.h"
#include "UntrustedStorageInterface.h"
#include "oram_encryption.h"
#include <cmath>

extern long long int commu_cost;
extern double enc_time;
class OramReadPathEviction : public OramInterface {
public:
    UntrustedStorageInterface* storage;
    RandForOramInterface* rand_gen;
    bool enable_encryption;
    int bucket_size;
    int num_levels;
    int num_leaves;
    int num_blocks;
    int num_buckets;
    std::unique_ptr<unsigned char[]> key;  
    int *position_map; //array
    vector<Block> stash;

    OramReadPathEviction(UntrustedStorageInterface* storage,
            RandForOramInterface* rand_gen, int bucket_size, int num_blocks);
    OramReadPathEviction(UntrustedStorageInterface* storage, 
            RandForOramInterface* rand_gen, int bucket_size, int num_blocks,vector<vector<int>> init_data);
    int* access(Operation op, int blockIndex, int newdata[]);
    int P(int leaf, int level);
    int* getPositionMap();
    vector<Block> getStash();
    int getStashSize();
    int getNumLeaves();
    int getNumLevels();
    int getNumBlocks();
    int getNumBuckets();
    ~OramReadPathEviction();

};


#endif 
