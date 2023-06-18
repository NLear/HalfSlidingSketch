//
// Created by 63130 on 2021/3/31.
//

#ifndef SLIDING_SKETCH_MASTER_CLOCK_H
#define SLIDING_SKETCH_MASTER_CLOCK_H


#include "../definition.h"
#include "hash_class.h"


class Recent_Sketch {
public :
    int len, row_length, hash_number, field_num;
    int cycle, clock_pos;
    int query_hash_number;
    double step;
    long long int last_time;

    Recent_Sketch(int c, int _row_length, int _hash_number, int _field_num,
                  int _query_hash_number) :
            cycle(c), row_length(_row_length), hash_number(_hash_number), field_num(_field_num),
            query_hash_number(_query_hash_number), len(_row_length * _hash_number),
            step((double) _row_length * _hash_number / c) {
        clock_pos = 0;
        last_time = 0;
    }

    int Mid(vector<int> num, unsigned int n_idx);
};

class Recent_Counter : public Recent_Sketch {
public:

    struct Unit {
        int *count;
        int field_num;
    };

    Unit *counter;

    Recent_Counter(int c, int _row_length, int _hash_number, int _field_num, int _query_hash_number);

    ~Recent_Counter();

    void Clock_Go(unsigned long long int num);

    void CM_Init(const unsigned char *str, int length, unsigned long long int num);//CM Sketch update an item

    int Query(const unsigned char *str, int length);//CM(CU) Sketch update an item

    void CU_Init(const unsigned char *str, int length, unsigned long long int num);//CU Sketch update an item

    void CO_Init(const unsigned char *str, int length, unsigned long long int num);//Count Sketch update an item

    int CO_Query(const unsigned char *str, int length);//Count Sketch query an item
};

Recent_Counter::Recent_Counter(int c, int _row_length, int _hash_numberber, int _field_num, int _query_hash_number) :
        Recent_Sketch(c, _row_length, _hash_numberber, _field_num, _query_hash_number) {
    counter = new Unit[len];
    field_num = _field_num;
    for (int i = 0; i < len; i++) {
        counter[i].count = new int[_field_num];
        counter[i].field_num = _field_num;
        memset(counter[i].count, 0, _field_num * sizeof(int));
    }
}

Recent_Counter::~Recent_Counter() {
    for (int i = 0; i < len; i++) {
        delete[] counter[i].count;
    }
    delete[] counter;
}

void Recent_Counter::CM_Init(const unsigned char *str, int length, unsigned long long int num) {
#ifdef RUN_HW
    if (num >= cycle && (num % (CYCLE / hash_number) == 0)) {
#else
    if (num >= cycle) {
#endif
        Clock_Go(num * step);
    }

    unsigned int position;

    for (unsigned int i = 0; i < hash_number; ++i) {
        for (unsigned int j = 0; j < field_num; ++j) {
            position = Hash(str, (i * field_num + j) % 13, length) % row_length + i * row_length;
            counter[position].count[j] += 1;
        }
    }
}

void Recent_Counter::CU_Init(const unsigned char *str, int length, unsigned long long int num) {
#ifdef RUN_HW
    if (num >= cycle && (num % (CYCLE / hash_number) == 0)) {
#else
    if (num >= cycle) {
#endif
        Clock_Go(num * step);
    }

    int k = clock_pos / row_length;
    unsigned int position = Hash(str, k, length) % row_length + k * row_length;
    if (position < clock_pos) {
        k = (k + 1) % hash_number;
        position = Hash(str, k, length) % row_length + k * row_length;
    }

    unsigned int height = counter[position].count[0];
//    int height = this->Query(str, length);

    for (
            unsigned int i = 0;
            i < hash_number;
            ++i) {
        for (
                unsigned int j = 0;
                j < field_num;
                ++j) {
            position = Hash(str, (i * field_num + j) % 13, length) % row_length + i * row_length;
            if (counter[position].count[j] <= height) {
                counter[position].count[j]++;
            }
        }
    }
}


int Recent_Counter::Query(const unsigned char *str, int length) {
    unsigned int k = clock_pos / row_length;
    unsigned int position = 0;
    int min_num = INT32_MAX;

#ifdef RUN_AQS
    for (unsigned int j = 0; j < field_num; ++j) {
        position = Hash(str, (k * field_num + j) % 13, length) % row_length + k * row_length;
        if (position > clock_pos) {
            min_num = min(counter[position].count[j], min_num);
        }
    }
#endif

    k = (k + 1) % hash_number;
    for (unsigned int i = k; (i + hash_number - k) % hash_number <= query_hash_number; i = (i + 1) % hash_number) {
        for (unsigned int j = 0; j < field_num; ++j) {
            position = Hash(str, (i * field_num + j) % 13, length) % row_length + i * row_length;
            min_num = min(counter[position].count[j], min_num);
        }
    }
    return min_num;
}


static int Count_Hash[2] = {-1, 1};

void Recent_Counter::CO_Init(const unsigned char *str, int length, unsigned long long num) {
    unsigned int position;
#ifdef RUN_HW
    if (num >= cycle && (num % (CYCLE / hash_number) == 0)) {
#else
    if (num >= cycle) {
#endif
        Clock_Go(num * step);
    }

    for (
            unsigned int i = 0;
            i < hash_number;
            ++i) {
        for (
                unsigned int j = 0;
                j < field_num;
                ++j) {
            position = Hash(str, (i * field_num + j) % 13, length) % row_length + i * row_length;
            counter[position].count[j] += Count_Hash[(str[length - 1] + position) & 1];
        }
    }
}


int Recent_Sketch::Mid(vector<int> num, unsigned int n_idx) {
    if (n_idx & 1) {
        return max(num[n_idx >> 1], 0);
    } else {
        return max(0, (num[n_idx >> 1] + num[(n_idx >> 1) - 1]) >> 1);
    }
}


int Recent_Counter::CO_Query(const unsigned char *str, int length) {
    vector<int> est;
    unsigned int k = clock_pos / row_length;
#ifdef RUN_AQS
    for (unsigned int j = 0; j < field_num; ++j) {
        unsigned int position = Hash(str, (k * field_num + j) % 13, length) % row_length + k * row_length;
        if (position > clock_pos) {
            est.push_back(counter[position].count[j] * Count_Hash[(str[length - 1] + position) & 1]);
        }
    }
#endif

    k = (k + 1) % hash_number;
    for (int i = k; (i + hash_number - k) % hash_number <= 4; i = (i + 1) % hash_number) {
        for (unsigned int j = 0; j < field_num; ++j) {
            unsigned int position = Hash(str, (i * field_num + j) % 13, length) % row_length + i * row_length;
            est.push_back(counter[position].count[j] * Count_Hash[(str[length - 1] + position) & 1]);
        }
    }

    sort(est.begin(), est.end());
    return Mid(est, est.size());
}


void Recent_Counter::Clock_Go(unsigned long long int num) {
    for (; last_time < num; ++last_time) {
        for (int i = 0; i < field_num; ++i) {
            counter[clock_pos].count[i] = 0;
        }
        clock_pos = (clock_pos + 1) % len;
    }
}


#endif //SLIDING_SKETCH_MASTER_CLOCK_H
