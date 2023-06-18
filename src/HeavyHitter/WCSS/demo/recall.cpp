#define _CRT_SECURE_NO_DEPRECATE

#include "../wcss.h"
#include "../data.h"
#include "../../definition.h"

int hash_number = 3;
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
    wcss cm(CYCLE, memory * 1024, DATA_LEN);
    map<Data, int> mp;

    Data *dat = new Data[CYCLE];
    memset(dat, 0, CYCLE * sizeof(Data));

    unsigned long long int num = 0;
    double precision = 0, recall = 0, are = 0;
    int heavy = 0, out_num = 0;

    FILE *file = fopen(input_file, "rb");
    Data packet;

    while (fread(packet.str, DATA_LEN, 1, file) > 0 && num < MAX_FLOWS) {
        unsigned int pos = num % CYCLE;
        if (num >= CYCLE) {
            mp[dat[pos]] -= 1;
        }
        dat[pos] = packet;
        if (mp.find(packet) == mp.end()) {
            mp[packet] = 1;
        } else {
            mp[packet] += 1;
        }

        cm.add((char *) packet.str, DATA_LEN);

        if (num && num % EVA_CYCLE == 0) {
            map<string, int> mp_hk;
            double true_pos = 0, all_pos = 0, all_true = 0;

            mp_hk = cm.getHHs();
            all_pos = mp_hk.size();

            double re = 0;
            for (auto &imp : mp) {
                int real = imp.second;
                if (real > PHI) {
                    all_true++;
                    int est = mp_hk[string((char *) imp.first.str, DATA_LEN)];
                    re += (double) abs(real - est) / real;
                    if (est > PHI) {
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
    delete[] dat;
    fclose(file);
}

void speed(int memory) {

    wcss cm(CYCLE, memory * 1024, DATA_LEN);

    unsigned long long int num = 0;

    FILE *file = fopen(input_file, "rb");
    Data packet;

    double start = clock();
    while (fread(packet.str, DATA_LEN, 1, file) > 0 && num < MAX_FLOWS) {
        cm.add((char *) packet.str, DATA_LEN);
        num++;
    }
    double end = clock();
    double duration = (end - start) / CLOCKS_PER_SEC;
    cout << memory << ' ' << (double) num / duration / 1e6 << endl;

    fclose(file);
}
