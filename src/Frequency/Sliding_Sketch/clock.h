#ifndef CLOCK_H
#define CLOCK_H

#include "hash_class.h"
#include "../definition.h"

using namespace std;


class Recent_Sketch {
public :
    unsigned int clock_pos;
    unsigned int len;
    double step;
    unsigned int cycle_num;
    int row_length;
    int hash_number;
    int field_num;
    unsigned long long int last_time;

    Recent_Sketch(unsigned int c, unsigned int l, int _row_length, int _hash_number, int _field_num) :
            len(l), step((double) l * (_field_num - 1) / c), row_length(_row_length), hash_number(_hash_number),
            field_num(_field_num) {
        clock_pos = 0;
        last_time = 0;
        cycle_num = 0;
    }

    int Mid(int *num);
};

class Recent_Counter : public Recent_Sketch {
public:
    int field_num;

    struct Unit {
        int *count;
        int field_num;

        int Total() {
            int ret = 0;
            for (int i = 0; i < field_num; ++i) {
                ret += count[i];
            }
            return ret;
        }
    };

    Unit *counter;


    Recent_Counter(int c, int l, int _row_length, int _hash_number, int _field_num);

    ~Recent_Counter();

    void Clock_Go(unsigned long long int num);

    void CM_Init(const unsigned char *str, int length, unsigned long long int num);//CM Sketch update an item
    void CO_Init(const unsigned char *str, int length, unsigned long long int num);//Count Sketch update an item
    void CU_Init(const unsigned char *str, int length, unsigned long long int num);//CU Sketch update an item
    int CO_Query(const unsigned char *str, int length);//Count Sketch query an item
    unsigned int Query(const unsigned char *str, int length);//CM(CU) Sketch update an item
};


struct Place {
    unsigned int serial;
    unsigned int pos;
};

bool operator<(Place a, Place b) {
    return a.serial < b.serial;
}

int Recent_Sketch::Mid(int *num) {
    if (hash_number & 1) {
        return max(num[hash_number >> 1], 0);
    } else {
        return max(0, (num[hash_number >> 1] + num[(hash_number >> 1) - 1]) >> 1);
    }
}

Recent_Counter::Recent_Counter(int c, int l, int _row_length, int _hash_number, int _field_num) :
        Recent_Sketch(c, l, _row_length, _hash_number, _field_num) {
    counter = new Unit[l];
    field_num = _field_num;
    for (int i = 0; i < l; i++) {
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
    unsigned int position;
    Clock_Go(num * step);
    for (int i = 0; i < hash_number; ++i) {
        position = Hash(str, i, length) % row_length + i * row_length;
        counter[position].count[(cycle_num + (position < clock_pos)) % field_num] += 1;
    }
}

void Recent_Counter::CU_Init(const unsigned char *str, int length, unsigned long long int num) {
    int k = clock_pos / row_length;
    Clock_Go(num * step);
    unsigned int position = Hash(str, k, length) % row_length + k * row_length;
    if (position < clock_pos) {
        k = (k + 1) % hash_number;
        position = Hash(str, k, length) % row_length + k * row_length;
    }

    unsigned int height = counter[position].count[(cycle_num + (position < clock_pos)) % field_num];
    counter[position].count[(cycle_num + (position < clock_pos)) % field_num] += 1;

    for (int i = (k + 1) % hash_number; i != k; i = (i + 1) % hash_number) {
        position = Hash(str, i, length) % row_length + i * row_length;
        if (counter[position].count[(cycle_num + (position < clock_pos)) % field_num] <= height) {
            height = counter[position].count[(cycle_num + (position < clock_pos)) % field_num];
            counter[position].count[(cycle_num + (position < clock_pos)) % field_num] += 1;
        }
    }
}

unsigned int Recent_Counter::Query(const unsigned char *str, int length) {
    unsigned int min_num = 0x7fffffff;
    for (int i = 0; i < hash_number; ++i) {
        unsigned int position = Hash(str, i, length) % row_length + i * row_length;
        int est = counter[position].Total();
        min_num = min(est, min_num);
    }
    return min_num;
}

static int Count_Hash[2] = {-1, 1};

void Recent_Counter::CO_Init(const unsigned char *str, int length, unsigned long long num) {
    unsigned int position;
    Clock_Go(num * step);
    for (int i = 0; i < hash_number; ++i) {
        position = Hash(str, i, length) % row_length + i * row_length;
        counter[position].count[(cycle_num + (position < clock_pos)) % field_num] +=
                Count_Hash[(str[length - 1] + position) & 1];
    }
}


int Recent_Counter::CO_Query(const unsigned char *str, int length) {
    int *n = new int[hash_number];
    memset(n, 0, hash_number * sizeof(int));

    for (int i = 0; i < hash_number; ++i) {
        unsigned int position = Hash(str, i, length) % row_length + i * row_length;
        /*
        if(clock_pos >= position){
            n[i] = (counter[position].Total() * Count_Hash[(str[length - 1] + position) & 1]) * row_length * hash_number / (row_length * hash_number + clock_pos - position);
        }else{
            n[i] = (counter[position].Total() * Count_Hash[(str[length - 1] + position) & 1]) * row_length * hash_number / (row_length * hash_number * 2 - (position - clock_pos));

        }*/
        n[i] = counter[position].Total() * Count_Hash[(str[length - 1] + position) & 1];

    }

    std::sort(n, n + hash_number);

    return Mid(n);
}

void Recent_Counter::Clock_Go(unsigned long long int num) {
    for (; last_time < num; ++last_time) {
        counter[clock_pos].count[(cycle_num + 1) % field_num] = 0;
        clock_pos = (clock_pos + 1) % len;
        if (clock_pos == 0) {
            cycle_num = (cycle_num + 1) % field_num;
        }
    }
}


#endif // CLOCK_H
