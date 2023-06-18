//
// Created by 63130 on 2021/3/31.
//

#ifndef SLIDING_SKETCH_MASTER_DEFINITION_H
#define SLIDING_SKETCH_MASTER_DEFINITION_H

#include <iostream>
#include <map>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <vector>
#include <thread>
#include <mutex>
#include <deque>

#include <cstring>
#include <cmath>

#define min(x, y) (x < y? x:y)
#define max(x, y) (x > y? x:y)

//#define  RUN_AQS
//#define  RUN_HW

#define TEST_ERROR_RATE
#define DATA_LEN 8


const int CYCLE = 500000;
const int EVA_CYCLE = CYCLE / 5;
const int MEMORY_MIN = 1 * 1024;
const int MEMORY_MAX = 3 * 1024;
const int MEMORY_STEP = 1 * 1024;
const int MAX_FLOWS = 5 * CYCLE;
const char *input_file = "../data/CAIDA.dat";

//const int CYCLE = 100000;
//const int MAX_FLOWS = 5 * CYCLE;
//const int EVA_CYCLE = CYCLE / 5;
//const int MEMORY_MIN = 1 * 1024;
//const int MEMORY_MAX = 3 * 1024;
//const int MEMORY_STEP = 1 * 1024;
//const char *input_file = "../data/formatted00.dat";

//const int CYCLE = 500000;
//const int EVA_CYCLE = CYCLE / 5;
//const int MEMORY_MIN = 10;
//const int MEMORY_MAX = 25;
//const int MEMORY_STEP = 5;
//const int MAX_FLOWS = 5 * CYCLE;
////const char * input_file = "../data/UNIV.dat";
//const char *input_file = "../data/UCLA.dat";


using namespace std;

#endif //SLIDING_SKETCH_MASTER_DEFINITION_H
