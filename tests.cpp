#include <iostream>
#include "buffer_arithmetics.h"

typedef unsigned char uchar;

void checker(const uchar& ok, const char* description){
    std::cout << '[' << (ok?"OK":"!!") << "]  " << description << std::endl;
}

uchar test_add_buffers(){
    ull* input_1 = new ull[0x4];
    ull* input_2 = new ull[0x4];
    *input_1 = 0xffffffffffffffff;
    *(input_1+0x1) = 0xffffffffffffffff;
    *(input_1+0x2) = 0xffffffffffffffff;
    *(input_1+0x3) = 0xffffffffffffffff;

    *input_2 = 0xffffffffffffffff;
    *(input_2+0x1) = 0xffffffffffffffff;
    *(input_2+0x2) = 0xffffffffffffffff;
    *(input_2+0x3) = 0xffffffffffffffff;
    ull* output = new ull[0x4];
    add_buffers(input_1, input_2, output, 0x4);
    uchar retval = 0x1;
    if(0xfffffffffffffffe != *output) retval = 0x0;
    else if(0xffffffffffffffff != *(output+0x1)) retval = 0x0;
    else if(0xffffffffffffffff != *(output+0x2)) retval = 0x0;
    else if(0xffffffffffffffff != *(output+0x3)) retval = 0x0;

    return retval;
}

uchar test_sub_buffers(){
    ull* input_1 = new ull[0x4];
    ull* input_2 = new ull[0x4];
    *input_1 = 0xef6d;
    *(input_1+0x1) = 0x5;
    *(input_1+0x2) = 0x5;
    *(input_1+0x3) = 0xef6d;

    *input_2 = 0xfffffffffffffffb;
    *(input_2+0x1) = 0xffffffffffffffff;
    *(input_2+0x2) = 0xffffffffffffffff;
    *(input_2+0x3) = 0xffffffffffffffff;
    ull* output = new ull[0x4];
    sub_buffers(input_1, input_2, output, 0x4);
    uchar retval = 0x1;
    if(0xef72 != *output) retval = 0x0;
    else if(0x5 != *(output+0x1)) retval = 0x0;
    else if(0x5 != *(output+0x2)) retval = 0x0;
    else if(0xef6d != *(output+0x3)) retval = 0x0;

    return retval;
}
