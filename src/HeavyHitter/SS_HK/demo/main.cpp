#include "../BOBHash32.h"
#include "../heavykeeper.h"
#include "../../definition.h"

int field_num = 3;
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
    // preparing heavykeeper
    const int single_size = DATA_LEN + (4 * (field_num));
    const int hk_M = memory * 1024 / (single_size * L);
    heavykeeper *hk = new heavykeeper(hk_M, CYCLE, field_num);
    hk->clear();

    string *cyc_dat = new string[CYCLE];
    fill(cyc_dat, cyc_dat + CYCLE, "");
    map<string, int> B;

    unsigned long long int num = 0;
    double precision = 0, recall = 0, are = 0;
    int heavy = 0, out_num = 0;

    // Inserting
    FILE *file = fopen(input_file, "rb");
    char packet[DATA_LEN] = {0};

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

        hk->Insert(s, num);

        if (num >= CYCLE && num % EVA_CYCLE == 0) {
            map<string, int> hh_hk;
            map<string, int> all_hk;
            double true_pos = 0, all_pos = 0, all_true = 0;

            for (int i = 0; i < L; i++) {
                for (int j = 0; j < hk_M; j++) {
                    string key = hk->HK[i][j].FP;
                    if (all_hk.find(key) == all_hk.end()) {
                        int est = hk->num_query(key);
                        all_hk[key] = est;
                        if (est > PHI) {
                            hh_hk[key] = est;
                            if (B[key] > PHI) {
                                true_pos++;
                            }
                        }
                    }
                }
            }
            all_pos = hh_hk.size();
            precision += true_pos / all_pos;

            double re = 0;
            for (map<string, int>::iterator sit = B.begin(); sit != B.end(); sit++) {
                int real = sit->second;
                if (real > PHI) {
                    all_true++;
                    re += (double) abs(real - hh_hk[sit->first]) / real;
                }
            }
            heavy += all_true;
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
    delete hk;
    delete[] cyc_dat;
}

void speed(int memory) {
    const int single_size = DATA_LEN + (4 * (field_num));
    const int hk_M = memory * 1024 / (single_size * L);
    heavykeeper *hk = new heavykeeper(hk_M, CYCLE, field_num);
    hk->clear();

    unsigned long long int num = 0;
    FILE *file = fopen(input_file, "rb");
    char packet[DATA_LEN] = {0};

    double start = clock();
    while (fread(packet, DATA_LEN, 1, file) > 0 && num < MAX_FLOWS) {
        string s(packet, DATA_LEN);
        hk->Insert(s, num);
        num++;
    }
    double end = clock();
    double duration = (end - start) / CLOCKS_PER_SEC;
    cout << memory << ' ' << (double) num / duration / 1e6 << endl;

    fclose(file);
    delete hk;
}
