#include "../sketch.h"
#include "../data.h"
#include "../../definition.h"

map<Data, int> mp;

void Error_Rate();

void speed();


int main(int argc, char **argv) {
#ifdef TEST_ERROR_RATE
    Error_Rate();
#else
    speed();
#endif
    return 0;
}

void Error_Rate() {
    int cycle = 50000;
    Data *dat = new Data[cycle];
    int update_k = 368;
    int hash_number = 3;
    int row_length = 40;

    ECM ecm(cycle, update_k, hash_number, row_length);

//    ECU ecu(cycle, update_k, hash_number, row_length);
//
//    ECO eco(cycle, update_k, hash_number, row_length);

    unsigned long long int input_num_max = 500000;
    unsigned long long int input_num = 0;
    FILE *file = fopen(input_file, "rb");
    Data packet;
    double are_ecm = 0, are_ecu = 0, are_eco = 0;
    double aae_ecm = 0, aae_ecu = 0, aae_eco = 0;
    while (fread(packet.str, DATA_LEN, 1, file) > 0) {
        if (input_num >= input_num_max) {
            break;
        }
        if (input_num >= cycle) {
            mp[dat[input_num % cycle]] -= 1;
        }
        dat[input_num % cycle] = packet;
        if (mp.find(packet) == mp.end()) {
            mp[packet] = 1;
        } else {
            mp[packet] += 1;
        }
        ecm.update(packet.str, DATA_LEN, input_num);
//        ecu.update(packet.str, DATA_LEN, input_num);
//        eco.update(packet.str, DATA_LEN, input_num);
        double dif_ecm = fabs(ecm.query(packet.str, DATA_LEN, input_num) - mp[packet]);
//        double dif_ecu = fabs(ecu.query(packet.str, DATA_LEN, input_num) - mp[packet]);
//        double dif_eco = fabs(eco.query(packet.str, DATA_LEN, input_num) - mp[packet]);

        are_ecm = are_ecm + (dif_ecm / mp[packet]);
        aae_ecm = aae_ecm + dif_ecm;
//        are_ecu = are_ecu + (dif_ecu / mp[packet]);
//        aae_ecu = aae_ecu + dif_ecu;
//        are_eco = are_eco + (dif_eco / mp[packet]);
//        aae_eco = aae_eco + dif_eco;

        if (input_num % cycle == 0 && input_num / cycle >= 1) {
            //fout << argv[9] << "," << (double)input_num  << "," << are_ecm/input_num << endl;
            cout << (double) ecm.q_memory(input_num) << " " << are_ecm / input_num << endl;
            //cout << mp.size() << endl;
        }
        input_num++;
    }
}

void speed() {
    int update_k = 368;
    int hash_number = 3;
    int row_length = 600;

    ECM ecm(CYCLE, update_k, hash_number, row_length);

    unsigned long long int num = 0;
    FILE *file = fopen(input_file, "rb");
    Data packet;

    double start = clock();
    while (fread(packet.str, DATA_LEN, 1, file) > 0 && num < MAX_FLOWS) {
        ecm.update(packet.str, DATA_LEN, num);
        num++;
    }
    double end = clock();
    double duration = (end - start) / CLOCKS_PER_SEC;
    cout << (double) ecm.q_memory(0) << ' ' << (double) num / duration / 1e6 << endl;
    fclose(file);
}