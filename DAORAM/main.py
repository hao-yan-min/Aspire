from tests.test_omaps import TestOramOdsOmap
import glob
import math
import os
import sys
import time
import pickle
# sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from daoram.dependency import InteractLocalServer
from daoram.omaps import AVLOdsOmap, BPlusOdsOmap, OramTreeOdsOmap
from daoram.orams import DAOram
# 写入文件


def OMap_test(num_bit:int,value_bit:int,key_bit:int,query_num:int):
    client = InteractLocalServer()
    item_num=pow(2,num_bit)
    value_size=pow(2,value_bit-3)  #比特数
    key_size=pow(2,key_bit-3)        #比特数
    # Create the ods object.
    # ods = BPlusOdsOmap(order=70, num_data=item_num, key_size=key_size, data_size=value_size, client=client, use_encryption=True)

    # # Create the oram object.
    # oram = DAOram(num_data=item_num, data_size=value_size, client=client, use_encryption=True)

    # # Create the omap object.
    # omap = OramTreeOdsOmap(num_data=item_num, ods=ods, oram=oram)

    # Create a client object for shared usage.
    # client = InteractLocalServer()

    # Create the ods object.
    ods = AVLOdsOmap(num_data=item_num, key_size=key_size, data_size=value_size, client=client, use_encryption=True)

    # Create the oram object.
    oram = DAOram(num_data=item_num, data_size=value_size, client=client, use_encryption=True)

    # Create the omap object.
    omap = OramTreeOdsOmap(num_data=item_num, ods=ods, oram=oram)

    # Initialize an empty storage.
    
    

    print("Initialization done.")
    # Initialize the omap with some integer keys.
    # omap.init_server_storage(data=[(f"Key {i}", f"Value {i}") for i in range(item_num // 2)])
    data_dic={}
    data=[]
    while True:
        #随机生成长度为value_size的byte数据
        value = os.urandom(value_size)
        key = os.urandom(key_size)
        value=int.from_bytes(value, byteorder='big')
        key=int.from_bytes(key, byteorder='big')
        data_dic[key]=value
        if  len(data_dic)==item_num:
            break
    data=list(data_dic.items())
    if len(data)!=item_num:
        print("数据生成错误")
        print(f"期望数据量:{item_num}")
        print(f"实际数据量:{len(data)}")
        return
        
    del data_dic
    serialized=pickle.dumps(data)
    print(f"数据生成完成,data size: {len(serialized)/(1024*1024):.2f} MB")
    del serialized
    start_time = time.time()
    omap.init_server_storage(data=data)
    end_time = time.time()
    print(f"初始化时间: {end_time - start_time:.2f} 秒")
        # Issue some search queries.
    print(f"初始化通信开销: {client.commmunication_cost / (1024 * 1024):.2f} MB")
    print(f"初始化通信轮数: {client.commmunication_round}")
    client.commmunication_cost=0
    client.commmunication_round=0
    # 将信息写入文件
    with open("communication_cost_init.txt", "a") as f:
        f.write("开始查询\n")
    print("开始查询")
    start_time = time.time()
    for cnt,item in enumerate(data[:query_num]):
        if omap.search(key=item[0]) != item[1]:
            print("查询错误")
            print(f"key:{item[0]}")
            print(f"expected value:{item[1]}")
            print(f"returned value:{omap.search(key=item[0])}")
        assert omap.search(key=item[0]) == item[1]
        # print(omap.search(key=item[0]))
        if cnt==query_num-1:
            print("查询完成")
            break
    end_time = time.time()
    print(f"查询时间: {end_time - start_time:.2f} 秒")
    # 在主流程最后输出通信开销
    print(f"搜索通信开销: {client.commmunication_cost / (1024 * 1024):.2f} MB")
    print(f"搜索通信轮数: {client.commmunication_round}")
    with open("communication_cost_init.txt", "a") as f:
        f.write(f"搜索通信开销: {client.commmunication_cost / (1024 * 1024):.2f} MB\n")
        f.write(f"搜索通信轮数: {client.commmunication_round}\n")
    del omap
    del oram
    del ods
    del client
if __name__ == "__main__":


    
    


    # print("OMap 测试")
    # # print(f"K-V对数目：2^16,Keyword bit长度：32bit, Value bit 长度：1024 bit")
    # # for i in range(5):       
    # #     print(f"第{i+1}次测试")
    # #     OMap_test(num_bit=16, value_bit=10, key_bit=5, query_num=5461)
    # print("------------------stage1------------------")
    # num_bits=[18,19,20,21,22,23]
    # #num_bits=[20,21,22,23]
    # for num_bit in num_bits:
    #     print(f"K-V对数目：2^{num_bit},Keyword bit长度：32bit, Value bit 长度：1024 bit")
    #     OMap_test(num_bit=num_bit, value_bit=11, key_bit=5, query_num=1365)
    #     print('\n')
    print("------------------stage2------------------")
    with open("communication_cost_init.txt", "w") as f:
        f.write("------------------stage2------------------\n")
    # value_bits=[7,8,9,10,11,12,13]
    value_bits=[14,15,16]
    for value_bit in value_bits:
        print(f"K-V对数目：2^20,Keyword bit长度：32bit, Value bit 长度：{2**value_bit} bit")
        OMap_test(num_bit=20, value_bit=value_bit, key_bit=5, query_num=341)
        print('\n')
    # print(f"K-V对数目：2^24,Keyword bit长度：32bit, Value bit 长度：1024 bit")
    # for i in range(5):
    #     print(f"第{i+1}次测试")
    #     OMap_test(num_bit=24, value_bit=10, key_bit=5, query_num=5461)
    # print(f"K-V对数目：2^20,Keyword bit长度：32bit, Value bit 长度：2048 bit")
    # for i in range(5):
    #     print(f"第{i+1}次测试")
    #     OMap_test(num_bit=20, value_bit=11, key_bit=5, query_num=5461)
    # print(f"K-V对数目：2^20,Keyword bit长度：32bit, Value bit 长度：4096 bit")
    # for i in range(5):
    #     print(f"第{i+1}次测试")
    #     OMap_test(num_bit=20, value_bit=12, key_bit=5, query_num=5461)
    # print(f"K-V对数目：2^20,Keyword bit长度：32bit, Value bit 长度：8192 bit")
    # for i in range(5):
    #     print(f"第{i+1}次测试")
    #     OMap_test(num_bit=20, value_bit=13, key_bit=5, query_num=5461)
        # # Create a client object for shared usage.
        # client = InteractLocalServer()
        # item_num=pow(2,20)
        # # Create the ods object.
        # ods = BPlusOdsOmap(order=70, num_data=item_num, key_size=10, data_size=10, client=client, use_encryption=False)

        # # Create the oram object.
        # oram = DAOram(num_data=item_num, data_size=10, client=client, use_encryption=False)

        # # Create the omap object.
        # omap = OramTreeOdsOmap(num_data=item_num, ods=ods, oram=oram)
        # print("Initialization done.")
        # # Initialize the omap with some integer keys.
        # omap.init_server_storage(data=[(f"Key {i}", f"Value {i}") for i in range(item_num // 2)])

        # # Keep inserting more values.
        # for i in range(item_num // 2, item_num):
        #     omap.insert(key=f"Key {i}", value=f"Value {i}")
        #     if i % 1000 == 0:
        #         print(f"Inserted {i} items.")

        # # Issue some search queries.
        # for i in range(item_num):
        #     assert omap.search(key=f"Key {i}") == f"Value {i}"
        #     print(omap.search(key=f"Key {i}"))