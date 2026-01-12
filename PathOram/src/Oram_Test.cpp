#include"Oram_Test.h"
using namespace std;
// long long int commu_cost=0;
extern long long int commu_cost;
void test_oram(int l,int x,OramInterface* oram)
{
    // commu_cost=0;
    // int bucketSize = 4;
    // int total=(l*x+7)/8;
    // int res=ceil(log2l(total));
    // int numBlocks = pow(2, res);
    // Bucket::setMaxSize(bucketSize);
    
    
    int total=(l*x+7)/8;
    int res=ceil(log2l(total));
    int numBlocks = pow(2, res);
    
    cout << "Warming up the stash by writing blocks" << endl;
    CorrectnessTester1 * tester1 = new CorrectnessTester1();
    for(int i = 0; i < total; i++){
        int* accessed = oram->access(OramInterface::Operation::WRITE, i % numBlocks, tester1->sampleData(i));
        delete[] accessed;
        if (oram->getStashSize() != 0 && i%10000 == 0) {
            cout << "Writing Block " << i % numBlocks << " to ORAM. The stash size is: " << to_string(oram->getStashSize()) << std::endl;
        }
    }
    Timer timer;
    timer.reset();
    commu_cost=0;
    for(int i = 0; i < total; i++){
        int* accessed = oram->access(OramInterface::Operation::READ, i , NULL);
        string holder = "";
        if (accessed == NULL) {
            cout << "Reading Block " << i << " from ORAM. Block not found!" << std::endl;
        }
        else {
            for (unsigned int j = 0; j<Block::BLOCK_SIZE; ++j) {
                int temp = accessed[j];
                holder += to_string(temp);
                holder += " ";
            }
            delete[] accessed;
            if (i % 10000 == 0) {
                cout << "Reading Block " << i << " from ORAM. Value is : " << holder << std::endl;
                cout << "Stash size is " << to_string(oram->getStashSize()) << std::endl;
            }
        }
    }
    auto search_time = timer.elapsed();
    cout << "Total time for reading "<<x<<" Bytes: " << search_time/l << " ms" << endl;

    long long int ave_commu_cost=commu_cost;
    cout << "Average communication cost: " << ave_commu_cost/(l*1024) << " KB"<<endl;
    delete tester1;
    // delete random;
    // delete storage;
    return;

}
// int main(int argc, char *argv[]) {
//     // cout << "Example Test: Writing fewer than reading" << endl;
//     // CorrectnessTester1 * tester1 = new CorrectnessTester1();
//     // tester1->runCorrectnessTest();
    
//     /*
//     Make sure to delete the tester and reset various static variables, 
//     this allows you to seemlessly run multiple tests and simulations
//     */
//     // delete tester1;
//     // Bucket::resetState();
//     // ServerStorage::is_initialized = false;
//     // ServerStorage::is_capacity_set = false;
//     // RandomForOram::is_initialized = false;
//     // RandomForOram::bound = -1; 

    
//     /*
//     Testing with a timer. Note that clock_t is not part of all implementations
//     of C++, so you may have to check which clock works best for you.
//     */
//     // std::clock_t start = std::clock();
//     // cout << "Another Sample Test: Running a long run from CorrectnessTester5" << endl;
//     // CorrectnessTester5 * tester5 = new CorrectnessTester5();
//     // tester5->runCorrectnessTest();
//     // std::clock_t end = std::clock();
//     // double total = (double)(end - start)/CLOCKS_PER_SEC;
//     // cout << "total: " << total << endl;

//     // delete tester5;
//     // Bucket::resetState();
//     // ServerStorage::is_initialized = false;
//     // ServerStorage::is_capacity_set = false;
//     // RandomForOram::is_initialized = false;
//     // RandomForOram::bound = -1;

//     /*
//     Simulations for stash sizes. 
//     See StashSizeSimulator.cpp for details on the StashSizeSimulator. 
//     */
//     // std::clock_t start = std::clock();
//     // cout << "Running simulation: Z=2, Eviction Algorithm: Deterministic Eviction" << endl;
//     // StashSizeSimulator * sim1 = new StashSizeSimulator(2, pow(2, 20), 500000000, 1, 1);
//     // sim1->runSimulation();
//     // // std::clock_t end = std::clock();
//     // // double total = (double)(end - start)/CLOCKS_PER_SEC;
//     // // cout << "total: " << total << endl;

//     // delete sim1;
//     // Bucket::resetState();
//     // ServerStorage::is_initialized = false;
//     // ServerStorage::is_capacity_set = false;
//     // RandomForOram::is_initialized = false;
//     // RandomForOram::bound = -1;
    

//     // std::clock_t start = std::clock();
//     // cout << "Running simulation: Z=4, Eviction Algorithm: Read Path Eviction" << endl;
//     // StashSizeSimulator * sim2 = new StashSizeSimulator(4, pow(2, 20), 500000000, 2, 0);
//     // sim2->runSimulation();
//     // // std::clock_t end = std::clock();
//     // // double total = (double)(end - start)/CLOCKS_PER_SEC;
//     // // cout << "total: " << total << endl;

//     // delete sim2;
//     // Bucket::resetState();
//     // ServerStorage::is_initialized = false;
//     // ServerStorage::is_capacity_set = false;
//     // RandomForOram::is_initialized = false;
//     // RandomForOram::bound = -1;

//     // std::clock_t start = std::clock();
//     // cout << "Running simulation: Z=5, Eviction Algorithm: Read Path Eviction" << endl;
//     // StashSizeSimulator * sim3 = new StashSizeSimulator(5, pow(2, 20), 500000000, 3, 0);
//     // sim3->runSimulation();
//     // // std::clock_t end = std::clock();
//     // // double total = (double)(end - start)/CLOCKS_PER_SEC;
//     // // cout << "total: " << total << endl;

//     // delete sim3;
//     // Bucket::resetState();
//     // ServerStorage::is_initialized = false;
//     // ServerStorage::is_capacity_set = false;
//     // RandomForOram::is_initialized = false;
//     // RandomForOram::bound = -1;


//     // std::clock_t start = std::clock();
//     // cout << "Running simulation: Z=6, Eviction Algorithm: Read Path Eviction" << endl;
//     // StashSizeSimulator * sim4 = new StashSizeSimulator(6, pow(2, 20), 10000000, 4, 0);
//     // sim4->runSimulation();
//     // // std::clock_t end = std::clock();
//     // // double total = (double)(end - start)/CLOCKS_PER_SEC;
//     // // cout << "total: " << total << endl;

//     // delete sim4;
//     // Bucket::resetState();
//     // ServerStorage::is_initialized = false;
//     // ServerStorage::is_capacity_set = false;
//     // RandomForOram::is_initialized = false;
//     // RandomForOram::bound = -1;
//     int l=20;
//     int x=32;
    
//     if(argc == 3)
//     {
//         l=atoi(argv[1]);
//         x=atoi(argv[2]);
        
//     }
//     commu_cost=0;
//     int bucketSize = 4;
//     int total=(l*x+7)/8;
//     int res=ceil(log2l(total));
//     int numBlocks = pow(2, res);
    
//     Bucket::setMaxSize(bucketSize);	//set the bucket size
//     vector<OramInterface *> ORAM;
//     for(int i=0;i<2;i++)
//     {
//         UntrustedStorageInterface* storage = new ServerStorage();

// 	    RandForOramInterface* random = new RandomForOram();
//         numBlocks=numBlocks*(i+1);
// 	    OramInterface* oram = new OramReadPathEviction(storage, random, bucketSize, numBlocks);
//         ORAM.push_back(oram);
//     }
//     for(int i=0;i<2;i++)
//     {
//         test_oram(l,x,ORAM[i]);
//     }
//     for(int i=0;i<2;i++)
//     {
//         delete ORAM[i];
//     }
//     UntrustedStorageInterface* storage = new ServerStorage();

// 	    RandForOramInterface* random = new RandomForOram();
// 	    OramInterface* oram = new OramReadPathEviction(storage, random, bucketSize, numBlocks);
//         test_oram(l,x,oram);
//         delete oram;
//     return 0;
    
// }
