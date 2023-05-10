#ifndef TESTS_H_INCLUDED
#define TESTS_H_INCLUDED

#include <iostream>
#include "buffer_arithmetics.h"

typedef unsigned char uchar;

void checker(const uchar& ok, const char* description);

uchar test_add_buffers();

uchar test_sub_buffers();

uchar test_mul_buffers();

uchar test_smod_buffer();

#endif // TESTS_H_INCLUDED
