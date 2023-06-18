#ifndef _PARAMS_H
#define _PARAMS_H

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory.h>

#include <iostream>
#include <algorithm>
#include <string>
#include <list>
#include <queue>
#include <map>
#include <unordered_map>
#include <fstream>
#include <iomanip>
#include <thread>
#include <mutex>

#define TEST_ERROR_RATE
#define MAX_MEM 1000000 // maximum memory size
#define DATA_LEN 8

/*const int CYCLE = 100000;
const int MAX_FLOWS = 5 * CYCLE;
const int EVA_CYCLE = CYCLE / 5;
const int MEMORY_MIN = 100; //KB
const int MEMORY_MAX = 250;
const int MEMORY_STEP = 50;
const int PHI = 50;
const char *input_file = "../data/formatted00.dat";*/


const int CYCLE = 500000;
const int MAX_FLOWS = 5 * CYCLE;
const int EVA_CYCLE = CYCLE / 5;
const int MEMORY_MIN = 100; //KB
const int MEMORY_MAX = 250;
const int MEMORY_STEP = 50;
const int PHI = 250;
const char *input_file = "../data/CAIDA.dat";
//const char * input_file = "../data/UNIV.dat";
//const char *input_file = "../data/UCLA.dat";


using namespace std;

#endif //_PARAMS_H
