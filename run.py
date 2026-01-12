import subprocess
from concurrent.futures import ThreadPoolExecutor, as_completed
import time

def run_complie(TEXP,TEXPRIGHT,TCONV,TGSW,QPBITS,PVALUE,QNUMFIRST,QNUMREST,OUTN,dim0,dim1):
    cmds = [
        ["rm","-rf","build"],
        ["cmake", "-S", ".", "-B", "build"],
        ["cmake", "--build", "build", "-j8","--",
         "PARAMSET=PARAMS_DYNAMIC",
         f"TEXP={TEXP}",
         f"TEXPRIGHT={TEXPRIGHT}",
         f"TCONV={TCONV}",
         f"TGSW={TGSW}",
         f"QPBITS={QPBITS}",
         f"PVALUE={PVALUE}",
         f"QNUMFIRST={QNUMFIRST}",
         f"QNUMREST={QNUMREST}",
         f"OUTN={OUTN}",
         "ADDTL_CXX_FLAGS="]
    ]
    last_res = None
    for cmd in cmds:
        last_res = subprocess.run(cmd, capture_output=True, text=True)
        if last_res.returncode != 0:
            # 发生错误时立即返回包含 stdout/stderr 的结果
            return last_res
    return last_res
def run_exe(bn,qn,nk,kbl,vbll,dim0,dim1,num_thread=1,pir_type="ORAM"):
    cmd = ["./build/main",f"{bn}",f"{qn}",f"{nk}",f"{kbl}",f"{vbll}",f"{dim0}",f"{dim1}",f"{num_thread}",pir_type]
    res = subprocess.run(cmd, capture_output=True, text=True)
    return res
if __name__ == "__main__":
    params = [{"TEXP":4, "TEXPRIGHT":56, "TCONV":4, "TGSW":4, "QPBITS":14, "PVALUE":4, "QNUMFIRST":1, "QNUMREST":0, "OUTN":2, "dim0":6,"dim1":2},
              {"TEXP":4, "TEXPRIGHT":56, "TCONV":4, "TGSW":5, "QPBITS":15, "PVALUE":16, "QNUMFIRST":1, "QNUMREST":0, "OUTN":2,"dim0":6,"dim1":2},
              {"TEXP":8, "TEXPRIGHT":56, "TCONV":4, "TGSW":6, "QPBITS":20, "PVALUE":256, "QNUMFIRST":1, "QNUMREST":0, "OUTN":2,"dim0":6,"dim1":2},
              {"TEXP":8, "TEXPRIGHT":56, "TCONV":4, "TGSW":7, "QPBITS":20, "PVALUE":256, "QNUMFIRST":1, "QNUMREST":0, "OUTN":2,"dim0":7,"dim1":2},
              {"TEXP":8, "TEXPRIGHT":56, "TCONV":4, "TGSW":7, "QPBITS":20, "PVALUE":256, "QNUMFIRST":1, "QNUMREST":0, "OUTN":2,"dim0":8,"dim1":2},
              {"TEXP":8, "TEXPRIGHT":56, "TCONV":4, "TGSW":7, "QPBITS":20, "PVALUE":256, "QNUMFIRST":1, "QNUMREST":0, "OUTN":2,"dim0":9,"dim1":2},              
              
              {"TEXP":4, "TEXPRIGHT":56, "TCONV":4, "TGSW":5, "QPBITS":15, "PVALUE":16, "QNUMFIRST":1, "QNUMREST":0, "OUTN":2,"dim0":6,"dim1":2},
              {"TEXP":8, "TEXPRIGHT":56, "TCONV":4, "TGSW":6, "QPBITS":20, "PVALUE":256, "QNUMFIRST":1, "QNUMREST":0, "OUTN":2,"dim0":6,"dim1":2},
              {"TEXP":8, "TEXPRIGHT":56, "TCONV":4, "TGSW":7, "QPBITS":20, "PVALUE":256, "QNUMFIRST":1, "QNUMREST":0, "OUTN":2,"dim0":7,"dim1":2},
              {"TEXP":8, "TEXPRIGHT":56, "TCONV":4, "TGSW":7, "QPBITS":20, "PVALUE":256, "QNUMFIRST":1, "QNUMREST":0, "OUTN":2,"dim0":8,"dim1":2},
              {"TEXP":8, "TEXPRIGHT":56, "TCONV":4, "TGSW":8, "QPBITS":20, "PVALUE":256, "QNUMFIRST":1, "QNUMREST":0, "OUTN":2,"dim0":10,"dim1":2},
              
              {"TEXP":8, "TEXPRIGHT":56, "TCONV":4, "TGSW":8, "QPBITS":20, "PVALUE":256, "QNUMFIRST":1, "QNUMREST":0, "OUTN":2,"dim0":10,"dim1":3},
              {"TEXP":8, "TEXPRIGHT":56, "TCONV":4, "TGSW":8, "QPBITS":20, "PVALUE":256, "QNUMFIRST":1, "QNUMREST":0, "OUTN":2,"dim0":10,"dim1":2},
              {"TEXP":8, "TEXPRIGHT":56, "TCONV":4, "TGSW":7, "QPBITS":20, "PVALUE":256, "QNUMFIRST":1, "QNUMREST":0, "OUTN":2,"dim0":9,"dim1":2},
              {"TEXP":8, "TEXPRIGHT":56, "TCONV":4, "TGSW":7, "QPBITS":20, "PVALUE":256, "QNUMFIRST":1, "QNUMREST":0, "OUTN":2,"dim0":8,"dim1":2},
              {"TEXP":4, "TEXPRIGHT":56, "TCONV":4, "TGSW":5, "QPBITS":16, "PVALUE":16, "QNUMFIRST":1, "QNUMREST":0, "OUTN":2,"dim0":8,"dim1":2},
              {"TEXP":4, "TEXPRIGHT":56, "TCONV":4, "TGSW":5, "QPBITS":15, "PVALUE":16, "QNUMFIRST":1, "QNUMREST":0, "OUTN":2,"dim0":7,"dim1":2}
              ]
    

    idx=0
    num_threads=1
    pir_type="ORAM"
    kv=[18,19,20,21,22,23]
    # kv=[22,23]
    # kv=[21,22,23]
     
    for nk in kv:
        res=run_complie(params[idx]["TEXP"],params[idx]["TEXPRIGHT"],params[idx]["TCONV"],params[idx]["TGSW"],params[idx]["QPBITS"],params[idx]["PVALUE"],params[idx]["QNUMFIRST"],params[idx]["QNUMREST"],params[idx]["OUTN"],params[idx]["dim0"],params[idx]["dim1"])
        if res.returncode != 0:
            print(f"Error during compilation for kv=18:")
            print("STDOUT:", res.stdout)
            print("STDERR:", res.stderr)  
        # //     main_0(512,300,nk,32,2048);
        res=run_exe(512,512/1.5,nk,32,2048,params[idx]["dim0"],params[idx]["dim1"],num_threads,pir_type)
        print(f"Completed execution for nk={nk}")
        print(f"run_exe(512,300,{nk},32,2048,{params[idx]['dim0']},{params[idx]['dim1']})")
        print(f"Return code: {res.returncode}")
        print(f"STDOUT:", res.stdout)
        print(f"STDERR:", res.stderr)
        # with open(f"result_nk_{100}.txt", "w") as f:
        #     f.write("STDOUT:\n")
        #     f.write(res.stdout)
        #     f.write("\nSTDERR:\n")
        #     f.write(res.stderr)
        idx+=1
        
    exit(0)
    # # # exit(0)
    # vl=[1024,2048,4096,8192,16384,32768,65536]
    vl=[32768,65536]
    res=run_complie(params[idx]["TEXP"],params[idx]["TEXPRIGHT"],params[idx]["TCONV"],params[idx]["TGSW"],params[idx]["QPBITS"],params[idx]["PVALUE"],params[idx]["QNUMFIRST"],params[idx]["QNUMREST"],params[idx]["OUTN"],params[idx]["dim0"],params[idx]["dim1"])
    if res.returncode != 0:
        print(f"Error during compilation for vbl={1024}:")
        print("STDOUT:", res.stdout)
        print("STDERR:", res.stderr)
    # main_0(512,300,20,32,vbl);
    for vbl in vl:
        run_exe(512,300,20,32,vbl,params[idx]["dim0"],params[idx]["dim1"],num_threads,pir_type)
        print(f"run_exe(512,300,20,32,{vbl},{params[idx]['dim0']},{params[idx]['dim1']})")
        idx+=1
    num_threads=16
    res=run_complie(params[idx]["TEXP"],params[idx]["TEXPRIGHT"],params[idx]["TCONV"],params[idx]["TGSW"],params[idx]["QPBITS"],params[idx]["PVALUE"],params[idx]["QNUMFIRST"],params[idx]["QNUMREST"],params[idx]["OUTN"],params[idx]["dim0"],params[idx]["dim1"])
    if res.returncode != 0:
        print(f"Error during compilation for vbl={1024}:")
        print("STDOUT:", res.stdout)
        print("STDERR:", res.stderr)
    # main_0(512,300,20,32,vbl);
    for vbl in vl:
        run_exe(512,300,20,32,vbl,params[idx]["dim0"],params[idx]["dim1"],num_threads,pir_type)
        print(f"run_exe(512,300,20,32,{vbl},{params[idx]['dim0']},{params[idx]['dim1']})")
        idx+=1


    # idx=11
    # bn=[64,128,256,512,1024,2048]
    # bn=[64,128,256,512,1024,2048,4096,8192]
    # # # bn=[8192]
    # # bn=[4096,8192]
    # res=run_complie(params[idx]["TEXP"],params[idx]["TEXPRIGHT"],params[idx]["TCONV"],params[idx]["TGSW"],params[idx]["QPBITS"],params[idx]["PVALUE"],params[idx]["QNUMFIRST"],params[idx]["QNUMREST"],params[idx]["OUTN"],params[idx]["dim0"],params[idx]["dim1"])
    # if res.returncode != 0:
    #     print(f"Error during compilation for idx={idx}:")
    #     print("STDOUT:", res.stdout)
    #     print("STDERR:", res.stderr)
    # for b in bn:
        
    #     res=run_exe(b, int(b/1.5), 22, 32, 2048, params[idx]["dim0"], params[idx]["dim1"], num_threads, pir_type)
    #     print(f"run_exe({b},{int(b/1.5)},22,32,2048,{params[idx]['dim0']},{params[idx]['dim1']})")
    #     idx+=1
    #     if res.returncode != 0:
    #         print("Error during compilation:")
    #         print("STDOUT:", res.stdout)
    #         print("STDERR:", res.stderr)
    #     # main_0(512,300,20,32,vbl);
    

    # res=run_complie(params[0]["TEXP"],params[0]["TEXPRIGHT"],params[0]["TCONV"],params[0]["TGSW"],params[0]["QPBITS"],params[0]["PVALUE"],params[0]["QNUMFIRST"],params[0]["QNUMREST"],params[0]["OUTN"],params[0]["dim0"],params[0]["dim1"])
    # if res.returncode != 0:
    #     print("Error during compilation:")
    #     print("STDOUT:", res.stdout)
    #     print("STDERR:", res.stderr)
    # else:
    #     print("Compilation succeeded:")
    #     print("STDOUT:", res.stdout)
    #     print("STDERR:", res.stderr)