#include <iostream>
#include <cstring>
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

uchar test_mul_buffers(){
    ull* input_1 = new ull[0x4];
    ull* input_2 = new ull[0x4];
    *input_1 = 0x5;
    *(input_1+0x1) = 0x0;
    *(input_1+0x2) = 0x0;
    *(input_1+0x3) = 0x0;

    *input_2 = 0x0;
    *(input_2+0x1) = 0x1;
    *(input_2+0x2) = 0x0;
    *(input_2+0x3) = 0x0;
    ull* output = new ull[0x9];
    memset(output, 0, 0x9<<0x3);
    mul_buffers(input_1, input_2, output, 0x20);
    uchar retval = 0x1;
    if(0x0 != *output) retval = 0x0;
    else if(0x5 != *(output+0x1)) retval = 0x0;
    else if(0x0 != *(output+0x2)) retval = 0x0;
    else if(0x0 != *(output+0x3)) retval = 0x0;

    return retval;
}

uchar test_mod_buffer(){
    /*ull* iobuf = new ull[0x4];
    ull* modbuf = new ull[0x4];
    *iobuf       = 0xd;
    *(iobuf+0x1) = 0x0;
    *(iobuf+0x2) = 0x0;
    *(iobuf+0x3) = 0x0;

    *modbuf      = 0x7;
    *(modbuf+0x1)= 0b0000000000000000000000000000000000000000000000000000000000000000;
    *(modbuf+0x2)= 0b0000000000000000000000000000000000000000000000000000000000000000;
    *(modbuf+0x3)= 0b0000000000000000000000000000000000000000000000000000000000000000;
    smod_buffer(iobuf, modbuf, 0x20);
    uchar retval = 0x1;
    if(            0x6 !=       *iobuf) retval = 0x0;
    else if(       0b0000000000000000000000000000000000000000000000000000000000000000 != *(iobuf+0x1)) retval = 0x0;
    else if(       0b0000000000000000000000000000000000000000000000000000000000000000 != *(iobuf+0x2)) retval = 0x0;
    else if(       0b0000000000000000000000000000000000000000000000000000000000000000 != *(iobuf+0x3)) retval = 0x0;

    return retval;*/

    ull* input_1 = new ull[0x4];
    ull* input_2 = new ull[0x4];
    *input_1 = /*0xd;*/0xdd1;
    *(input_1+0x1) = /*0x0;*/0xb245525465768967;
    *(input_1+0x2) = /*0x0;*/0x114356ff5eab44c5;
    *(input_1+0x3) = /*0x0; */0x0245ccea31d3f8a3;

    *input_2 = /*0x7;*/0x2;
    *(input_2+0x1) = 0x0;
    *(input_2+0x2) = 0x0;
    *(input_2+0x3) = 0x0;

    // 00001101
    // 00000111


    ull* output = new ull[0xd];
    memset(output, 0, 0xd<<0x3);
    mod_buffer(input_1, input_2, output, /*0x20*/0x4);
    uchar retval = 0x1;
    if(/*0x6*/0x1 != *output) retval = 0x0;
    else if(0x0 != *(output+0x1)) retval = 0x0;
    else if(0x0 != *(output+0x2)) retval = 0x0;
    else if(0x0 != *(output+0x3)) retval = 0x0;

    return retval;
}

uchar test_powermod_buffer(){
    ull* input_1 = new ull[0x4];
    ull* input_2 = new ull[0x4];
    *input_1 = 0x2;
    *(input_1+0x1) = 0x0;
    *(input_1+0x2) = 0x0;
    *(input_1+0x3) = 0x0;

    *input_2 = 0xa;//0x3;
    *(input_2+0x1) = 0x0;
    *(input_2+0x2) = 0x0;
    *(input_2+0x3) = 0x0;

    ull* output = new ull[0x1b];
    memset(output, 0, 0x1b<<0x3);
    powermod_buffer(input_1, input_2, output, 0x4);
    uchar retval = 0x1;
    if(1024u != *output) retval = 0x0;
    else if(0x0 != *(output+0x1)) retval = 0x0;
    else if(0x0 != *(output+0x2)) retval = 0x0;
    else if(0x0 != *(output+0x3)) retval = 0x0;

    return retval;
}
