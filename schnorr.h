#ifndef SCHNORR_H_INCLUDED
#define SCHNORR_H_INCLUDED

#include "buffer_arithmetics.h"
#include <random>
#include <cstring>
#include "sha256.h"

void schnorr_sign(uchar* M, const ull& M_length, ull* p, ull* q, ull* h, ull* a, const uint& blks_cnt, ull* s1, ull* s2);

#endif // SCHNORR_H_INCLUDED
