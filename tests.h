#ifndef TESTS_H_INCLUDED
#define TESTS_H_INCLUDED

#include <iostream>
#include "buffer_arithmetics.h"

typedef unsigned char uchar;

void checker(const uchar& ok, const char* description);

uchar test_add_buffers();

#endif // TESTS_H_INCLUDED
