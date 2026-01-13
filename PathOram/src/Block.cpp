//
//
//
#include "Block.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;
int Block::BLOCK_SIZE = 1; // Default block size, can be modified as needed
Block::Block(){//dummy index
    
    this->leaf_id = -1;
    this->index = -1;
    this->data=std::shared_ptr<int[]>(new int[BLOCK_SIZE]);
    for (int i = 0; i < BLOCK_SIZE; i++){
        this->data[i] = 0;
    }
}

Block::Block(int leaf_id, int index, int* data) : leaf_id(leaf_id), index(index)
{
   this->data = std::shared_ptr<int[]>(new int[BLOCK_SIZE]);
   for (int i = 0; i < BLOCK_SIZE; i++){
       this->data[i] = data[i];
   }
}

// Block::Block(Block &other) {
//     this->leaf_id = other.leaf_id;
//     this->index = other.index;
//     this->data = new int[BLOCK_SIZE];
//     for (int i = 0; i < BLOCK_SIZE; i++){
//        this->data[i] = other.data[i];
//    }

// }

// Block& Block::operator=(const Block& other) {
//     if (this != &other) {
//         this->leaf_id = other.leaf_id;
//         this->index = other.index;
//         if(this->data!=NULL)
//         {
//             delete[] this->data; 
//         }
//         this->data = new int[BLOCK_SIZE];
//         for (int i = 0; i < BLOCK_SIZE; i++){
//            this->data[i] = other.data[i];
//         }
//     }
//     return *this;
// }

Block::~Block()
{
    // if(data!=nullptr)
    //     delete[] data;
    // data=nullptr;
}

void Block::printBlock(){
	string data_holder = "";
	for (int i = 0; i<BLOCK_SIZE; i++) {
		data_holder += to_string(this->data[i]);
		data_holder += " ";
	}
	cout << "index: " << to_string(this->index) << " leaf id: " << to_string(this->leaf_id) << " data: " << data_holder << endl;
}