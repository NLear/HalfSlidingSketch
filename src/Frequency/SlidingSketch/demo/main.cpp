#include "../clock.h"
#include "../data.h"
#include "../../definition.h"

mutex g_mutex;

void Error_Rate(int memory);

void speed(int memory);

int main() {
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
    int hash_number = 10, field_num = 2;
    int row_length = (memory * 1024) / hash_number / (4 * field_num);

    Recent_Counter CM_Counter(CYCLE, hash_number * row_length, row_length, hash_number, field_num);
    Recent_Counter CU_Counter(CYCLE, hash_number * row_length, row_length, hash_number, field_num);
    Recent_Counter CO_Counter(CYCLE, hash_number * row_length, row_length, hash_number, field_num);

    Data *dat = new Data[CYCLE + 1];

    unordered_map<Data, int, My_Hash> mp;

    unsigned long long int num = 0;
    double CM_re = 0, CU_re = 0, CO_re = 0;
    double ave_err_CM = 0, ave_err_CU = 0, ave_err_CO = 0;
    int tests = 0;

    FILE *file = fopen(input_file, "rb");
    Data packet;

    while (fread(packet.str, DATA_LEN, 1, file) > 0 && num < MAX_FLOWS) {
        unsigned int pos = num % CYCLE;
        if (num >= CYCLE) {
            mp[dat[pos]] -= 1;
        }

        dat[pos] = packet;

        CM_Counter.CM_Init(packet.str, DATA_LEN, num);
        CU_Counter.CU_Init(packet.str, DATA_LEN, num);
        CO_Counter.CO_Init(packet.str, DATA_LEN, num);

        if (mp.find(packet) == mp.end())
            mp[packet] = 1;
        else
            mp[packet] += 1;

        int CM_guess = CM_Counter.Query(packet.str, DATA_LEN);
        int CU_guess = CU_Counter.Query(packet.str, DATA_LEN);
        int CO_guess = CO_Counter.CO_Query(packet.str, DATA_LEN);

        int real = mp[packet];

        CM_re += (double) abs(CM_guess - real) / real;
        CU_re += (double) abs(CU_guess - real) / real;
        CO_re += (double) abs(CO_guess - real) / real;

        if (num >= CYCLE && num % EVA_CYCLE == 0) {
            ave_err_CM += CM_re / CYCLE;
            ave_err_CU += CU_re / CYCLE;
            ave_err_CO += CO_re / CYCLE;
            CM_re = 0;
            CU_re = 0;
            CO_re = 0;
            tests++;
        }
        num++;
    }
    lock_guard<mutex> lockGuard(g_mutex);
    cout << memory << " " << ave_err_CM / tests << " " << ave_err_CU / tests << " " << ave_err_CO / tests << endl;
    fclose(file);
    delete[] dat;
}

void speed(int memory) {
    int hash_number = 10, field_num = 2;
    int row_length = (memory * 1024) / hash_number / (4 * field_num);

    Recent_Counter CM_Counter(CYCLE, hash_number * row_length, row_length, hash_number, field_num);
//    Recent_Counter CU_Counter(CYCLE, hash_number * row_length, row_length, hash_number, field_num);
//    Recent_Counter CO_Counter(CYCLE, hash_number * row_length, row_length, hash_number, field_num);

    unsigned long long int num = 0;
    FILE *file = fopen(input_file, "rb");
    Data packet;

    double start = clock();
    while (fread(packet.str, DATA_LEN, 1, file) > 0 && num < MAX_FLOWS) {
        CM_Counter.CM_Init(packet.str, DATA_LEN, num);
//        CU_Counter.CU_Init(packet.str, DATA_LEN, num);
//        CO_Counter.CO_Init(packet.str, DATA_LEN, num);
        num++;
    }
    double end = clock();
    double duration = (end - start) / CLOCKS_PER_SEC;
    lock_guard<mutex> lockGuard(g_mutex);
    cout << memory << ' ' << (double) num / duration / 1e6 << endl;
    fclose(file);
}
