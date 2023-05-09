#include <iostream>
#include "buffer_arithmetics.h"

typedef unsigned char uchar;

void checker(const uchar& ok, const char* description){
    std::cout << '[' << (ok?"OK":"!!") << "]  " << description << std::endl;
}

uchar test_add_buffers(){
    ull* input_1 = new ull[0x4];
    ull* input_2 = new ull[0x4];
    *input_1 = 0x2;
    *(input_1+0x1) = 0x0;
    *(input_1+0x2) = 0xffffffffffffffff;
    *(input_1+0x3) = 0x000000000000000;

    *input_2 = 0x3;
    *(input_2+0x1) = 0x0;
    *(input_2+0x2) = 0x0;
    *(input_2+0x3) = 0x0;
    ull* output = new ull[0x4];
    add_buffers(input_1, input_2, output, 0x4);
    uchar retval = 0x1;
    if(0x5 != *output) retval = 0x0;
    else if(0x0 != *(output+0x1)) retval = 0x0;
    else if(0xffffffffffffffff != *(output+0x2)) retval = 0x0;
    else if(0x0 != *(output+0x3)) retval = 0x0;

    return retval;
}
