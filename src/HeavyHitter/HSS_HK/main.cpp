
#include "heavykeeper.h"
#include "../definition.h"


mutex g_mutex;

void Error_Rate(int memory);

void speed(int memory);

int main(int argc, char **argv) {
#ifdef TEST_ERROR_RATE
    cout << "memory #HHs  recall  precision  f1  ARE" << endl;
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
    int single_size = DATA_LEN + 4;
    int hk_M = memory * 1024 / (single_size * HK_d * LEVEL);
    HSS_HeavyKeeper *hssHeavyKeeper = new HSS_HeavyKeeper(LEVEL, hk_M);

    string *cyc_dat = new string[CYCLE];
//    fill(cyc_dat, cyc_dat + CYCLE, "");
    map<string, int> B;

    unsigned long long int num = 0;
    double precision = 0, recall = 0, f1 = 0, are = 0;
    int heavy = 0, out_num = 0;

    FILE *file = fopen(input_file, "rb");
    char packet[DATA_LEN] = {0};
    if (file == NULL) {
        cout << "open file error" << endl;
        exit(-1);
    }

    // Inserting
    while (fread(packet, DATA_LEN, 1, file) > 0 && num < MAX_FLOWS) {
        string s(packet, DATA_LEN);

        unsigned int pos = num % CYCLE;
        if (num >= CYCLE) {
            B[cyc_dat[pos]]--;
        }

        cyc_dat[pos] = s;
        if (B.find(s) == B.end())
            B[s] = 1;
        else
            B[s] += 1;
        hssHeavyKeeper->Insert(s, num);

        if (num >= CYCLE && num % EVA_CYCLE == 0) {
            double true_pos = 0, all_pos = 0, all_true = 0;
            map<string, int> HK_HH = hssHeavyKeeper->GetHHs();
            all_pos = HK_HH.size();

            for (auto &hh: HK_HH) {
                if (B[hh.first] > PHI) {
                    true_pos++;
                }
            }
            precision += true_pos / all_pos;

            double re = 0;
            for (map<string, int>::iterator sit = B.begin(); sit != B.end(); sit++) {
                int real = sit->second;
                if (real > PHI) {
                    all_true++;
                    re += (double) abs(real - HK_HH[sit->first]) / real;
//                    cout << real << " " << HK_HH[sit->first] << endl;
                }
            }
//            cout << endl;
            heavy += all_true;
            recall += true_pos / all_true;
            are += re / all_true;
            out_num++;
        }
        num++;
    }
    {
        lock_guard<mutex> lockGuard(g_mutex);
        recall /= out_num;
        precision /= out_num;
        f1 = 2 * precision * recall / (precision + recall);

        cout << memory << " " << heavy / out_num << " " << recall << " " << precision << " "
             << f1 << " " << are / out_num << endl;
    }
    fclose(file);
    delete hssHeavyKeeper;
    delete[] cyc_dat;
}
