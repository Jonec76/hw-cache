#include<iostream>
#include<string.h>
#include <stdio.h>
#include<fstream>

using namespace std;
int cache_size = -1, block_size = -1, associativity=-1, algo=-1;
int bit_tag = -1;
int bit_index = -1;
int bit_asso = -1;
int bit_byte = 4;

struct Cache{
    int ptr;
    unsigned long* set;
    bool* sec_chance;
};

void get_bits(int block_bits, int associativity, int* bit_tag, int* bit_index, int* bit_asso){
    *bit_tag = 32 - block_bits - bit_byte;
    *bit_index = block_bits;
    switch(associativity) {
        case 0: // direct
            *bit_asso = 0;
            break;
        case 1: // 4-way
            *bit_asso = 2;
            *bit_tag += *bit_asso;
            *bit_index -= *bit_asso;
            break;
        case 2: // full
            *bit_asso = *bit_index;
            *bit_tag += *bit_asso;
            *bit_index = 0;
            break;
        default:
            cout<<"wrong associative setting";
            break;
    }
}


string hb_table(char h){
    switch (h) {
        case '0':
            return "0000";
            break;
        case '1':
            return "0001";
            break;
        case '2':
            return "0010";
            break;
        case '3':
            return "0011";
            break;
        case '4':
            return "0100";
            break;
        case '5':
            return "0101";
            break;
        case '6':
            return "0110";
            break;
        case '7':
            return "0111";
            break;
        case '8':
            return "1000";
            break;
        case '9':
            return "1001";
            break;
        case 'A':
        case 'a':
            return "1010";
            break;
        case 'B':
        case 'b':
            return "1011";
            break;
        case 'C':
        case 'c':
            return "1100";
            break;
        case 'D':
        case 'd':
            return "1101";
            break;
        case 'E':
        case 'e':
            return "1110";
            break;
        case 'F':
        case 'f':
            return "1111";
            break;
        default:
            return "\nInvalid hexadecimal digit ";
    }
}


string hex_to_bin(string hex){
    string bin = "";
    hex = hex.substr(2, hex.length());
    for(int i=0;i<hex.length();i++)
        bin += hb_table(hex[i]);
    return bin;
}

unsigned long bin_to_dec(string b){
    if(b=="")
        return 0;
    unsigned long dec = 0;
    int idx = 0;
    for(int i=b.length()-1;i>=0;i--){
        dec += (b[i] - '0') * (1<<idx);
        idx++;
    }
    return dec;
}

long update(Cache** cache, unsigned long tag, unsigned long index, int algo, int width){
    long ans = -1;
    int ptr = (*cache)[index].ptr;
    unsigned long victim;
    unsigned long temp;

    switch (algo){
        case 0: // FIFO
            for(int i=0;i<width;i++){
                if((*cache)[index].set[i] == tag) // If cache hit
                    return ans;
            }
            // If cache miss
            victim = (*cache)[index].set[(*cache)[index].ptr % width];
            ans = (victim == 0)?-1:victim;
            (*cache)[index].set[(*cache)[index].ptr % width] = tag;
            (*cache)[index].ptr++ ;
            break;
        case 1: // LRU
            for(int i=0;i<width;i++){
                if((*cache)[index].set[i] == tag){ // If cache hit
                    unsigned long temp;
                    for(int j = i; j >= 1; j--){
                        temp = (*cache)[index].set[j];
                        (*cache)[index].set[j] = (*cache)[index].set[j-1];
                        (*cache)[index].set[j-1] = temp;
                    }
                    return ans;
                }
            }
            // If cache miss
            victim = (*cache)[index].set[(*cache)[index].ptr % width];
            ans = (victim == 0)?-1:victim;

            (*cache)[index].set[(*cache)[index].ptr % width] = tag;

            for(int j = (*cache)[index].ptr % width; j >= 1; j--){
                temp = (*cache)[index].set[j];
                (*cache)[index].set[j] = (*cache)[index].set[j-1];
                (*cache)[index].set[j-1] = temp;
            }

            (*cache)[index].ptr = ((*cache)[index].ptr != width-1)?(*cache)[index].ptr+1:(*cache)[index].ptr ;
            break;
        case 2: // Customize: Second Chance FIFO
            for(int i=0;i<width;i++){
                if((*cache)[index].set[i] == tag){ // If cache hit
                    (*cache)[index].sec_chance[i] = true;
                    return ans;
                }
            }
            // If cache miss
            while((*cache)[index].sec_chance[(*cache)[index].ptr % width]){
                (*cache)[index].sec_chance[(*cache)[index].ptr % width] = false;
                (*cache)[index].ptr++;
            }
            victim = (*cache)[index].set[(*cache)[index].ptr % width];
            ans = (victim == 0)?-1:victim;
            (*cache)[index].set[(*cache)[index].ptr % width] = tag;
            (*cache)[index].ptr++ ;
            break;
    }
    return ans;
}

int main(int argc, char *argv[]){
    if(argc != 3){
        cout<<"Wrong input args";
        return 0 ;
    }

    ifstream cin_(argv[1]);
    string x;
    getline(cin_, x);
    cache_size = stoi(x);
    getline(cin_, x);
    block_size = stoi(x);
    getline(cin_, x);
    associativity = stoi(x);
    getline(cin_, x);
    algo = stoi(x);
    int block_bits = __builtin_ctz(cache_size) + 10 - __builtin_ctz(block_size);
    get_bits(block_bits, associativity, &bit_tag, &bit_index, &bit_asso);

    Cache* cache = new Cache[1<<bit_index];
    for(int i = 0; i < 1<<bit_index; ++i){
        cache[i].set = new unsigned long [1<<bit_asso];
        cache[i].sec_chance = new bool [1<<bit_asso];
        cache[i].ptr = 0;
        memset(cache[i].set, 0, (1<<bit_asso)*sizeof(unsigned long));
        memset(cache[i].sec_chance, false, (1<<bit_asso)*sizeof(bool));
    }
    string line;
    ofstream ans_file;
    ans_file.open (argv[2]);

    while (getline(cin_, line)){
        if(line == "")
            break;
        string bin = hex_to_bin(line);
        unsigned long tag = bin_to_dec(bin.substr(0, bit_tag));
        unsigned long index = bin_to_dec(bin.substr(bit_tag, bit_index));
        long ans = update(&cache, tag, index, algo, 1<<bit_asso);
        ans_file<<ans<<endl;
    }
    for(int i = 0; i < 1<<bit_index; ++i){
        delete [] cache[i].set;
        delete [] cache[i].sec_chance;
    }
    delete [] cache;
}