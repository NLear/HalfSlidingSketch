#include "../splitter.h"
#include "../data.h"
#include "../../definition.h"

mutex g_mutex;

void Error_Rate();

void speed();


int main () {
#ifdef TEST_ERROR_RATE
    Error_Rate();

#else
    speed();
#endif
    return 0;
}

void Error_Rate() {
    double tau = 0.05;
    double mu = 1.5;
    int hash_number = 3;
    int row_length = 600; // 200 400 600

    Bucket cm(CYCLE, tau, mu, hash_number, row_length);
    cout << cm.q_memory() << " ";

    Data *dat = new Data[CYCLE];
    map<Data, int> mp;
    memset(dat, 0, CYCLE * sizeof(Data));

    unsigned long long int num = 0;
    double CM_re = 0, ave_err_CM = 0;
    int tests = 0;

    FILE *file = fopen(input_file, "rb");
    Data packet;

    while (fread(packet.str, DATA_LEN, 1, file) > 0 && num < MAX_FLOWS) {
        if (num >= CYCLE) {
            mp[dat[num % CYCLE]] -= 1;
        }
        dat[num % CYCLE] = packet;

        cm.update(packet.str, DATA_LEN, num);

        if (mp.find(packet) == mp.end()) {
            mp[packet] = 1;
        } else {
            mp[packet] += 1;
        }

        int CM_guess = cm.query(packet.str, DATA_LEN);
        int real = mp[packet];
        CM_re += (double) abs(CM_guess - real) / real;

        if (num >= CYCLE && num % EVA_CYCLE == 0) {
            ave_err_CM += CM_re / CYCLE;
            CM_re = 0;
            tests++;
        }
        num++;
    }
    lock_guard<mutex> lockGuard(g_mutex);
    cout << " " << ave_err_CM / tests << endl;
    fclose(file);
    delete[] dat;
}

void speed() {
    double tau = 0.05;
    double mu = 1.5;
    int hash_number = 3;
    int row_length = 400;   // 200 400 600

    Bucket cm(CYCLE, tau, mu, hash_number, row_length);
    cout << cm.q_memory() << " ";

    unsigned long long int num = 0;

    FILE *file = fopen(input_file, "rb");
    Data packet;

    double start = clock();
    while (fread(packet.str, DATA_LEN, 1, file) > 0 && num < MAX_FLOWS) {
        cm.update(packet.str, DATA_LEN, num);
        num++;
    }
    double end = clock();

    double duration = (end - start) / CLOCKS_PER_SEC;
    cout << (double) num / duration / 1e6 << endl;
    fclose(file);
}