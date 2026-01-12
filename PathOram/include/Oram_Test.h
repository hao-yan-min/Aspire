#pragma once
#include <iostream>
#include <cmath>
#include "Bucket.h"
#include "Block.h"
#include "RandomForOram.h"
#include "OramReadPathEviction.h"
#include "OramDeterministic.h"
#include "OramInterface.h"
#include "RandForOramInterface.h"
#include "UntrustedStorageInterface.h"
#include "ServerStorage.h"
#include "CorrectnessTester1.h"
#include "CorrectnessTester2.h"
#include "CorrectnessTester3.h"
#include "CorrectnessTester4.h"
#include "CorrectnessTester5.h"
#include "StashSizeSimulator.h"
using namespace std;


#include <chrono>
#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>
#include<fstream>
#include <string>
#include <unordered_map>
class Timer
{
public:
  Timer() { _start = std::chrono::high_resolution_clock::now(); }

  inline double elapsed()
  {
    _end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(_end - _start)
               .count() /
           1000.0;
  }

  inline void reset() { _start = std::chrono::high_resolution_clock::now(); }
  ~Timer(){};

private:
  std::chrono::high_resolution_clock::time_point _start, _end;
};

void test_oram(int l,int x,OramInterface* oram);
