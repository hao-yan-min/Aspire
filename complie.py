import subprocess
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

if __name__=="__main__":

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
    res=run_complie(params[idx]["TEXP"],params[idx]["TEXPRIGHT"],params[idx]["TCONV"],params[idx]["TGSW"],params[idx]["QPBITS"],params[idx]["PVALUE"],params[idx]["QNUMFIRST"],params[idx]["QNUMREST"],params[idx]["OUTN"],params[idx]["dim0"],params[idx]["dim1"])
    if res.returncode != 0:
        print(f"Error during compilation for kv=18:")
        print("STDOUT:", res.stdout)
        print("STDERR:", res.stderr)