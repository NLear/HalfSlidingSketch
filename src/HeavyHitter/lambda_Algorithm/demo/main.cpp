#include "../summary.h"
#include "../data.h"
#include "../../definition.h"

int para_lambda = 100;
mutex g_mutex;

void Error_Rate(int memory);

void speed(int memory);

int main(int argc, char **argv) {
#ifdef TEST_ERROR_RATE
    cout << "memory #HHs  recall  precision  ARE" << endl;
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

    Summary summary(memory * 1024, DATA_LEN, CYCLE, para_lambda);

    Data *dat = new Data[CYCLE + 1];
    memset(dat, 0, (CYCLE + 1) * sizeof(Data));
    unordered_map<Data, int, My_Hash> mp;

    unsigned long long int num = 0;
    int heavy = 0, out_num = 0;
    double precision = 0, recall = 0, are = 0;

    FILE *file = fopen(input_file, "rb");
    Data packet;

    while (fread(packet.str, DATA_LEN, 1, file) > 0 && num < MAX_FLOWS) {
        unsigned int pos = num % CYCLE;
        if (num >= CYCLE) {
            mp[dat[pos]]--;
        }

        dat[pos] = packet;
        if (mp.find(packet) == mp.end())
            mp[packet] = 1;
        else
            mp[packet] += 1;

        summary.Init(packet, num);

        if (num && num % EVA_CYCLE == 0) {
            map<string, int> map_hk;
            double true_pos = 0, all_pos = 0, all_true = 0;

            map<Data, int> heavy_mp = summary.GetHeavy(num);
            all_pos = heavy_mp.size();

            double re = 0;
            unordered_map<Data, int, My_Hash>::iterator it;
            for (it = mp.begin(); it != mp.end(); ++it) {
                int real = it->second;
                if (real > PHI) {
                    all_true++;
                    re += (double) fabs(real - heavy_mp[it->first]) / real;
                    if (heavy_mp[it->first] > PHI) {
                        true_pos++;
                    }
                }
            }
            heavy += all_true;
            precision += true_pos / all_pos;
            recall += true_pos / all_true;
            are += re / all_true;

            out_num++;
        }
        num++;
    }
    {
        lock_guard<mutex> lockGuard(g_mutex);
        cout << memory << " " << heavy / out_num << " " << recall / out_num << " " << precision / out_num << " "
             << are / out_num << endl;
    }
    fclose(file);
    delete[] dat;
}

void speed(int memory) {
    Summary summary(memory * 1024, DATA_LEN, CYCLE, para_lambda);
    unsigned long long int num = 0;

    FILE *file = fopen(input_file, "rb");
    Data packet;

    double start = clock();
    while (fread(packet.str, DATA_LEN, 1, file) > 0 && num < MAX_FLOWS) {

        summary.Init(packet, num);
        num++;
    }
    double end = clock();
    double duration = (end - start) / CLOCKS_PER_SEC;
    cout << memory << ' ' << (double) num / duration / 1e6 << endl;

    fclose(file);
}
