//
// Created by 63130 on 2021/4/4.
//

#ifndef SLIDING_SKETCH_MASTER_DEFINITION_H
#define SLIDING_SKETCH_MASTER_DEFINITION_H

#define TEST_ERROR_RATE

const unsigned int KEY_LEN = 8;
const unsigned int SL_Hash = 7;
const unsigned int LO = 6;


const int CYCLE = 500000;
const int EVA_CYCLE = CYCLE / 5;
const int MEMORY_MIN = 100;
const int MEMORY_MAX = 250;
const int MEMORY_STEP = 50;
const int MAX_FLOWS = 5 * CYCLE;
const int TEST_EXAMPLES = 100000;
const char *input_file = "../data/CAIDA.dat";


//const int CYCLE = 500000;
//const int EVA_CYCLE = CYCLE/5;
//const int MEMORY_MIN = 15;
//const int MEMORY_MAX = 30;
//const int MEMORY_STEP = 5;
//const int MAX_FLOWS = 5 * CYCLE;
//const int TEST_EXAMPLES = 100000;
//const char * input_file = "../data/UCLA.dat";
//const char * input_file = "../data/UNIV.dat";

//const int CYCLE = 100000;
//const int EVA_CYCLE = CYCLE / 5;
//const int MEMORY_MIN = 100;
//const int MEMORY_MAX = 250;
//const int MEMORY_STEP = 50;
//const int MAX_FLOWS = 5 * CYCLE;
//const int TEST_EXAMPLES = 100000;
//const char *input_file = "../data/formatted00.dat";

#endif //SLIDING_SKETCH_MASTER_DEFINITION_H
