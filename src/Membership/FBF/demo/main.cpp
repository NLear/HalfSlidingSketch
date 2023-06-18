#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <string.h>
#include <time.h>
#include <math.h>
#include <queue>
#include <unordered_map>
#include <random>
#include <thread>
#include <mutex>

#include "../forget_bloom.h"
#include "../../definition.h"

using namespace std;

uint HashNum = 8;
uint FBFNum = 40;
mutex g_mutex;

void Error_Rate(int memory);

void speed(int memory);

int main(int argc, char **argv) {
#ifdef TEST_ERROR_RATE
    thread ths[(MEMORY_MAX - MEMORY_MIN) / MEMORY_STEP + 1];
    int th_cnt = 0;
    for (int memory = MEMORY_MIN; memory <= MEMORY_MAX; memory += MEMORY_STEP) {
        ths[th_cnt] = thread(Error_Rate, memory);
        th_cnt++;
    }
    for (auto &th: ths) {
        th.join();
    }
#else
    for (int memory = MEMORY_MIN; memory <= MEMORY_MAX; memory += MEMORY_STEP) {
        speed(memory);
    }
#endif
    return 0;
}


void Error_Rate(int memory) {
    queue<string> que;
    unordered_map<string, int> mp;
    Forget_Bloom forget((double) memory * 1024 / FBFNum, HashNum, FBFNum, CYCLE);

    FILE *input;

    input = fopen(input_file, "rb");

    uint num = 0;
    uint slide_err1 = 0, slide_err2 = 0;
    double ave_err1 = 0, ave_err2 = 0;
    int tests = 0;
    char cur_packet[8];

    while (fread(cur_packet, KEY_LEN, 1, input) > 0 && num < MAX_FLOWS) {

        if (num >= que.size() && !que.empty()) {
            string out_packet = que.front();
            mp[out_packet] -= 1;
            if (mp[out_packet] == 0) {
                mp.erase(out_packet);
            }
            que.pop();
        }

        string cur_string = string(cur_packet, KEY_LEN);
        que.push(cur_string);
        if (mp.find(cur_string) == mp.end()) {
            mp[cur_string] = 1;
        } else {
            mp[cur_string]++;
        }

        forget.Init(cur_packet, num);

        if (num >= CYCLE && num % EVA_CYCLE == 0) {

            for (auto &imp : mp) {
                if (!forget.Query(imp.first.c_str()))
                    slide_err1 += 1;
            }

            int cnt = 0;
            char temp[8] = {0};
            random_device rd;
            default_random_engine dre(rd());
            uniform_int_distribution<unsigned long long int> intDistribution(0, UINT64_MAX);

            while (cnt < TEST_EXAMPLES) {
                unsigned long long int rand = intDistribution(dre);
                memcpy(temp, (char *) &rand, KEY_LEN);
                if (mp.find(temp) == mp.end()) {
                    cnt++;
                    if (forget.Query(temp))
                        slide_err2 += 1;
                }
            }

            ave_err1 += (double) slide_err1 / mp.size();
            ave_err2 += (double) slide_err2 / TEST_EXAMPLES;
            slide_err1 = 0, slide_err2 = 0;
            tests++;
        }
        num++;
    }
    lock_guard<mutex> lockGuard(g_mutex);
    cout << memory << " " << ave_err1 / tests << " " << ave_err2 / tests << endl;
    fclose(input);
}

void speed(int memory) {
    FILE *file = fopen(input_file, "rb");
    Forget_Bloom forget((double) memory * 1024 / FBFNum, HashNum, FBFNum, CYCLE);

    int cnt = 0;
    char cur_packet[8];
    double start = clock(), end = 0;

    while (fread(cur_packet, KEY_LEN, 1, file) > 0) {
        //read a packet
        fread(cur_packet, 8, 1, file);
        forget.Init(cur_packet, cnt);
        cnt++;
    }

    end = clock();
    double duration = (end - start) / CLOCKS_PER_SEC;
    cout << memory << ' ' << (double) cnt / duration / 1e6 << endl;
    fclose(file);
}
