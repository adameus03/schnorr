#include "buffer_arithmetics.h"
#include <random>
#include <cstring>
#include "sha256.h"

#define P_LENGTH 512
#define Q_LENGTH 140
#define PRIMA_CHECK_REP 0x4

typedef unsigned long long ull;
typedef unsigned int uint;
typedef unsigned char uchar;



void generate_q(ull* q){
    uint p_blks_cnt = P_LENGTH << 0x6;
    uint q_blks_cnt = Q_LENGTH << 0x6;

    ull* q_head = q + ++q_blks_cnt;
    while(q_head != q){
        q_head--;
        *q_head = (((ull)rand()) << 0x20) | (ull)rand();
    }

    q_head += q_blks_cnt;
    ull* q_tail = q + p_blks_cnt;

    while(q_head != q_tail){
        *q_head++ = 0x0;
    }
    --q_head;
    *q_head |= 0x8000000000000000;
}

void generate_N(ull* n, const uint& blks_cnt){
    ull* n_head = n + blks_cnt;
    while(n_head != n){
        n_head--;
        *n_head = (((ull)rand()) << 0x20) | (ull)rand();
    }
    n_head += blks_cnt;
    n_head--;
    *n_head |= 0x8000000000000000;
}



void incrementation(ull* x, ull* inc_x, const uint& blks_cnt){
    ull* one = new ull[blks_cnt<<0x3];
    memset(one, 0, blks_cnt);
    *one |= 0x1;
    add_buffers(x, one, inc_x, blks_cnt);
}

void decrementation(ull* x, ull* dec_x, const uint& blks_cnt){
    ull* one = new ull[blks_cnt<<0x3];
    memset(one, 0, blks_cnt);
    *one |= 0x1;
    sub_buffers(x, one, dec_x, blks_cnt);
}

uchar primality_test(ull* p, ull* dec_p, const uint& blks_cnt){
    // dec_p = r*2^s
    //ull* r = new ull[blks_cnt];
    //ull* s = new ull[blks_cnt];
    //ull* prod = new ull[(blks_cnt<<0x1)+0x1];

    //memset(r, 0, blks_cnt<<0x3);
    //*r |= 0x1;
    //memset(s, 0, blks_cnt<<0x3);
    //*s = (ull)(blks_cnt)-0x1;

    ull* a = new ull[blks_cnt];
    ull* a_head = a + blks_cnt;
    while(a_head != a){
        a_head--;
        *a_head = (((ull)rand()) << 0x20) | (ull)rand();
    }


    /*while(!(r & 0x1)){
        decrementation(s, s, blks_cnt); // OK??
    }*/

    if(!(*a & 0x1)) return 0x0;
    ull* exponent = new ull[blks_cnt];
    memset(exponent, 0, blks_cnt<<0x3);
    *exponent |= 0x1;

    ull* pow_buf = new ull[blks_cnt*0x6+0x3];
    ull* rem_buf = new ull[blks_cnt*0x6+0x1];
    ull* mod_buf = new ull[(blks_cnt<<0x1)];
    memset(mod_buf, 0, (blks_cnt<<0x1)<<0x3);
    memcpy(mod_buf, p, blks_cnt<<0x3);

    ull* test_buf = new ull[blks_cnt*0x6+0x1];
    ull* test_head;
    for(uint j=0x0; j<blks_cnt; j++){
        memset(pow_buf, 0, (blks_cnt*0x6+0x3)<<0x3);
        powermod_buffer(a, exponent, pow_buf, blks_cnt);
        memset(rem_buf, 0, (blks_cnt*0x6+0x1)<<0x3);
        mod_buffer(pow_buf, mod_buf, rem_buf, blks_cnt<<0x1);
        incrementation(rem_buf, test_buf, blks_cnt*0x6+0x1);

        test_head = test_buf + blks_cnt*0x6 + 0x1;
        uchar breakout = 0x0;
        while(test_head != test_buf){
            test_head--;
            if(*test_head){
                breakout = 0x1;
                break;
            }
        }
        if(!breakout) return 0x0;
    }
    return 0x1;
}

uchar repeated_primality_test(ull* p, ull* dec_p, const uint& blks_cnt){
    for(uint i=0x0; i<PRIMA_CHECK_REP; i++){
        if(!primality_test(p, dec_p, blks_cnt)) return 0x0;
    }
    return 0x1;
}

void generate_p(ull* q, ull* p, ull* n, const uint& p_blks_cnt, const uint& q_blks_cnt){
    uint n_blks_cnt = p_blks_cnt - q_blks_cnt;

    //ull* n = new ull[p_blks_cnt];

    uint prod_blks_cnt = (p_blks_cnt<<0x1)+0x1;
    ull* nq = new ull[prod_blks_cnt];
    //ull* p = new ull[p_blks_cnt];

    while(true){
        generate_N(n, n_blks_cnt);
        memset(nq, 0, prod_blks_cnt<<0x3);
        mul_buffers(n, q, nq, p_blks_cnt<<0x3);
        incrementation(nq, p, p_blks_cnt);
        if((*p & 0x1) && repeated_primality_test(p, nq, p_blks_cnt)) break;
    }
}

void generate_h(ull* p, ull* q, ull* n, ull* h, const uint& p_blks_cnt, const uint& q_blks_cnt){
    ull* seed = new ull[p_blks_cnt];
    memset(seed, 0, p_blks_cnt<<0x3);
    ull* seed_head = seed + q_blks_cnt;
    while(seed_head != seed){
        seed_head--;
        *seed_head = (((ull)rand()) << 0x20) | (ull)rand();
    }
    ull* pow_buf = new ull[p_blks_cnt*0x6+0x3];
    memset(pow_buf, 0, (p_blks_cnt*0x6+0x3)<<0x3);
    powermod_buffer(seed, n, pow_buf, p_blks_cnt);
    memcpy(h, pow_buf, p_blks_cnt);
}

// a - private key
// v - public key
void generate_av(ull* p, ull* h, ull* a, ull* v, const uint& blks_cnt){
    ull* _a = new ull[blks_cnt];
    ull* _a_head = _a + blks_cnt;
    while(_a_head != _a){
        _a_head--;
        *_a_head = (((ull)rand()) << 0x20) | (ull)rand();
    }
    ull* _a_ = new ull[blks_cnt*0x3+0x1];
    memset(_a_, 0, (blks_cnt*0x3+0x1)<<0x3);
    mod_buffer(_a, p, _a_, blks_cnt);
    memcpy(_a_, a, blks_cnt<<0x3);

    ull* exponent = new ull[blks_cnt];
    decrementation(p, exponent, blks_cnt);
    sub_buffers(exponent, a, exponent, blks_cnt);

    ull* pow_buf = new ull[blks_cnt*0x6+0x3];
    memset(pow_buf, 0, (blks_cnt*0x6+0x3)<<0x3);
    powermod_buffer(h, exponent, pow_buf, blks_cnt);
    memcpy(v, pow_buf, blks_cnt<<0x3);
}

// M - data buffer
// s1 still blks_cnt
void schnorr_sign(uchar* M, const ull& M_length, ull* p, ull* q, ull* h, ull* a, const uint& blks_cnt, ull* s1, ull* s2){
    ull* _r = new ull[blks_cnt];

    ull* _r_head = _r + blks_cnt;
    while(_r_head != _r){
        _r_head--;
        *_r_head = (((ull)rand()) << 0x20) | (ull)rand();
    }

    ull* r = new ull[blks_cnt*0x3+0x1];
    memset(r, 0, (blks_cnt*0x3+0x1)<<0x3);
    mod_buffer(_r, q, r, blks_cnt);
    ull* _x = new ull[blks_cnt*0x6+0x3];
    memset(_x, 0, (blks_cnt*0x6+0x3)<<0x3);
    powermod_buffer(h, r, _x, blks_cnt);
    ull* x = new ull[blks_cnt*0x3+0x1];
    memset(x, 0, (blks_cnt*0x3+0x1)<<0x3);
    mod_buffer(_x, p, x, blks_cnt);
    // X generated

    ull MX_length = M_length + (blks_cnt<<0x3);
    uchar* MX = new uchar[MX_length];
    memcpy(MX, M, M_length);
    memcpy(MX+M_length, x, blks_cnt<<0x3);
    //MX ready
    //ull* s1 = new ull[0x4];
    sha256hash(MX, s1, MX_length);

    ull* accumulator = new ull[(blks_cnt<<0x1)+0x1];
    memset(accumulator, 0, ((blks_cnt<<0x1)+0x1)<<0x3);
    mul_buffers(a, s1, accumulator, blks_cnt<<0x3);
    add_buffers(accumulator, r, accumulator, blks_cnt);

    ull* modbuf = new ull[blks_cnt*0x3+0x1];
    memset(modbuf, 0, (blks_cnt*0x3+0x1)<<0x3);
    mod_buffer(accumulator, q, modbuf, blks_cnt);
    memcpy(s2, modbuf, blks_cnt<<0x3);
}
