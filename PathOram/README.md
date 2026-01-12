## 一个可以使用的pathoram的库

### 使用示例
```c++
TEST_CASE("test oram")
{
    // Block::BLOCK_SIZE=16;
    int bucket_size=4;
    int payload=256; //每个元素256byte
    Block::BLOCK_SIZE=(payload+AES_BLOCK_SIZE)/sizeof(int); //每个block存储256byte数据，加上iv的16byte
    int num_element=512;
    int res=ceil(log2l(num_element));
    int num_blocks=pow(2,res);
    //初始化ORAM
    Bucket::setMaxSize(bucket_size);	//set the bucket size
    UntrustedStorageInterface* storage = new ServerStorage();
    RandForOramInterface* random = new RandomForOram();
    OramInterface* oram = new OramReadPathEviction(storage, random, bucket_size, num_blocks);
    ((OramReadPathEviction *)oram)->enable_encryption=true;
    std::unique_ptr<int[]> data_ptr;
    data_ptr = std::make_unique<int[]>(payload*num_element/sizeof(int));
    for(int i=0;i<payload*num_element/sizeof(int);i++)
    {
        data_ptr[i]=i+1;
    }
    //写入数据
    for(int i=0;i<num_element;i++)
    {
        int* newdata = new int[Block::BLOCK_SIZE-4];
        memcpy(newdata, data_ptr.get()+i*payload/sizeof(int), payload);
        oram->access(OramInterface::WRITE, i, newdata);
        delete[] newdata;
    }
    //读取数据并验证
    for(int i=0;i<num_element;i++)
    {
        int* res_data = oram->access(OramInterface::READ, i, nullptr);
        for(int j=0;j<payload/sizeof(int);j++)
        {
            REQUIRE(res_data[j]==data_ptr[i*payload/sizeof(int)+j]);
        }
        for(int j=0;j<10;j++)
        {
            std::cout<<res_data[j]<<" ";
        }
        std::cout<<std::endl;
        delete[] res_data;
    }
}
```

### 注意事项
1. `Block::BLOCK_SIZE` 决定了每一个元素的大小为多少个int，在每次使用前，务必确保该参数和oram初始化时候的参数是相同的
2. 当需要开启对oram的加密功能时，设置`oram->enable_encryption=true;`
3. `oram->access`的结果需要手动释放