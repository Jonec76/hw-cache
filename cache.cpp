#include<iostream>
#include<string.h>
#include <stdio.h>
using namespace std;
int cache_size = -1, block_size = -1, associativity=-1, algo=-1;
int bit_tag = -1;
int bit_index = -1;
int bit_asso = -1;
int bit_byte = 4;

void get_bits(int block_bits, int associativity, int* bit_tag, int* bit_index, int* bit_asso){
     switch(associativity) {
        case 0: // direct
            *bit_asso  = 0;
            *bit_index = block_bits;
            break;
        case 1: // 4-way
            *bit_asso  = 2;
            *bit_index = block_bits - *bit_asso;
            break;
        case 2: // full
            *bit_asso  = block_bits;
            *bit_index = 0;
            break;
        default:
            cout<<"wrong associative setting";
            break;
    }
    *bit_tag = 32 - *bit_index - *bit_asso - bit_byte;
}

int main(){
    cin >> cache_size >> block_size >> associativity >> algo;
    int block_bits = __builtin_ctz(cache_size) + 10 - __builtin_ctz(block_size);
    get_bits(block_bits, associativity, &bit_tag, &bit_index, &bit_asso);

    int** cache = new int*[1<<bit_index];
    for(int i = 0; i < 1<<bit_index; ++i)
        cache[i] = new int[1<<bit_asso];

    // for(int i = 0; i < 1<<bit_index; i++){
    //     for(int j = 0; j < 3; j++){
    //         cout<<cache[i][j]<<" ";
    //     }cout<<endl;
    // }

    delete [] cache;
}