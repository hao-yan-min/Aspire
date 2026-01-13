//
//
//

#ifndef PORAM_BLOCK_H
#define PORAM_BLOCK_H

#include <algorithm>
#include <memory>
using namespace std;

class Block {
public:
    static int BLOCK_SIZE; 
    int leaf_id;
    int index;
    // int *data;
    // int data[BLOCK_SIZE];
    std::shared_ptr<int[]> data; 
    Block();
    // Block(const Block &other);
    Block(int leaf_id, int index, int data[]);
    //Block& operator=(const Block& other);
    void printBlock();
    virtual ~Block();
};

#endif //PORAM_BLOCK_H
