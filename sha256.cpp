#include "SHA256.h"
#include <bitset>
#include <iostream>
#include <sstream>
#include <iomanip>

typedef unsigned long long ull;
using namespace std;

void zero_bits(unsigned char* byte_ptr, unsigned char left, unsigned char right) {
    /*
        ********
        ***0000*

        ********
       &11100001
    */
    unsigned char ones = 255;
    unsigned char mask = ones << (8 - left);
    mask += ones >> (8 - right);
    //cout << "Mask: " << bitset<8>(mask) << endl;
    *byte_ptr &= mask;
}
void zero(unsigned char* arr, unsigned char phase, short int zero_length) {// know how to use it!
    /*
        +++aaaaa aaabbbbb bbbaaaaa aaabbbbb bbbaaaaa aaabbbbb bbbaaaaa aaabbb++
    */
    //unsigned char* ptr = arr;
    /*char cophase = 8-phase;
    unsigned char mask = ((unsigned char) 255) << cophase; // 11100000
    if(zero_length < cophase) {
        mask += ((unsigned char) 255) >> (phase+zero_length);
        zero_length
    }
    arr[0] &= mask;*/
    //cout << "{" << (unsigned int)phase << ", " << zero_length << "}" << endl;
    if (zero_length < (8 - phase)) zero_bits(arr, phase, 8 - phase - zero_length);
    else {
        zero_bits(arr, phase, 0);
        zero(arr + 1, 0, zero_length - (8 - phase));
    }

}
unsigned char* get_chunks(unsigned char* data, unsigned long int data_length, int& n) {
    int chunks_count = 1 + (data_length + 65) / 512;
    short int zero_padding = 512 - (data_length + 65) % 512;

    //cout << "Data (as text): " << data << endl;
    //cout << "Data length: " << data_length << endl;
    //cout << "Chunks count: " << chunks_count << endl;
    //cout << "Zero padding: " << zero_padding << endl;

    unsigned char* chunks = new unsigned char [chunks_count<<0x6];
    for (int i = 0; i < chunks_count - 1; i++) {
        //chunks[i] = new unsigned char[64];
        for (int j = 0; j < 64; j++) {
            chunks[(i<<0x6)+j] = data[64 * i + j];
        }
    }

    short int last_chunk_data_length = 447 - zero_padding; //512-65=447
    short int last_chunk_full_data_bytes_count = last_chunk_data_length / 8;
    char last_data_byte_incomplete_bits_count = last_chunk_data_length % 8;
    //chunks[chunks_count - 1] = new unsigned char[64];

    unsigned char* last_chunk = &chunks[(chunks_count - 1)<<0x6];
    unsigned char* last_chunk_data = data + 64 * (chunks_count - 1);

    //cout << "Last chunk data length: " << last_chunk_data_length << endl;
    //cout << "Last chunk full data bytes count: " << last_chunk_full_data_bytes_count << endl;
    //cout << "Last byte incomplete bits count: " << (unsigned int)last_data_byte_incomplete_bits_count << endl;


    for (int i = 0; i < last_chunk_full_data_bytes_count; i++) {
        last_chunk[i] = last_chunk_data[i];   //do it also for the incomplete byte, hence <=
    } // fixed 14.02.2022 ( do it also for the incomplete byte, but only if the incomplete part is non-zero
    if (last_data_byte_incomplete_bits_count > 0) last_chunk[last_chunk_full_data_bytes_count] = last_chunk_data[last_chunk_full_data_bytes_count];
    // +++-----
    // 11011001
    //|00011111
    // rrrrrrrr
    // 11011111
    // +++&----
    // +++&---- ----/---- ----/---- ----/---- ...
    last_chunk[last_chunk_full_data_bytes_count] |= ((unsigned char)255) >> last_data_byte_incomplete_bits_count;
    //cout << "Resu: " << bitset<8>(last_chunk[last_chunk_full_data_bytes_count]) << endl;
    if (last_data_byte_incomplete_bits_count == 7) zero(last_chunk + last_chunk_full_data_bytes_count + 1, 0, zero_padding);
    else zero(last_chunk + last_chunk_full_data_bytes_count, last_data_byte_incomplete_bits_count + 1, zero_padding);
    //511-64=447
    //64-8=56
    for (int i = 63; i > 55; i--) {
        last_chunk[i] = data_length;
        data_length >>= 8;
        //last_chunk[i]=255;
    }
    n = chunks_count;
    return chunks;
}
/*void print_chunks(unsigned char** chunks, int chunks_count) {
    for (int i = 0; i < chunks_count; i++) {
        for (int j = 0; j < 64; j++) {
            std::cout << std::bitset<8>(chunks[i][j]) << ' ';
            //cout << chunks[i][j] << ' ';
            if (j % 8 == 7) std::cout << std::endl;
        }
        //cout << "-------- -------- -------- -------- -------- -------- -------- --------" << endl;
        cout << endl;
    }
    cout << endl;
}*/
/*void deallocate_chunks_memory(unsigned char** chunks, int chunks_count) {
    for (int i = 0; i < chunks_count; i++) {
        delete[] chunks[i];
    }
    delete[] chunks;
}*/
/*void deallocate_message_schedule_memory(unsigned int* message_schedule) {
    delete[] message_schedule;
}*/
unsigned int rightrotate(unsigned int a, unsigned int b) {
    /*
        11011001    rr2
        110110-01
        01-110110
       =01110110

        11011001
       &00000011
       =00000001

        00000001 ls6
       =01000000

        11011001 rs2
       =00110110

        00110110
       |01000000
       =01110110

    */

    /*
        11011001 ls2
       =01100100

        11011001 rs6
       =00000011

        compare
        01110110
    */
    unsigned int ones = 0xffffffff;
    unsigned int right_blueprint = a & (ones >> (32 - b));
    //cout << "Right blueprint: " << bitset<32>(right_blueprint) << endl;
    unsigned int left_blueprint = right_blueprint << (32 - b);
    //cout << "Left blueprint: " << bitset<32>(left_blueprint) << endl;
    return a >> b | left_blueprint;
}
unsigned int* get_message_schedule(unsigned char* chunk) {
    //print_chunks(&chunk, 1);
    unsigned int* w = new unsigned int[64];
    for (int i = 0; i < 16; i++) {
        w[i] = chunk[4 * i];
        for (int j = 1; j < 4; j++) {
            w[i] <<= 8;
            w[i] |= chunk[4 * i + j];
        }
    }

    unsigned int s0;
    unsigned int s1;
    for (int i = 16; i < 64; i++) {
        //w[i] = 0;
        s0 = rightrotate(w[i - 15], 7) ^ rightrotate(w[i - 15], 18) ^ (w[i - 15] >> 3);
        s1 = rightrotate(w[i - 2], 17) ^ rightrotate(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }
    return w;
}
void print_message_schedule(unsigned int* message_schedule) {
    for (int i = 0; i < 64; i++) {
        std::cout << std::bitset<32>(message_schedule[i]) << ' ';
        if (i % 2) cout << endl;
    }
    cout << endl;
}
void compress(unsigned int* w, unsigned int* k, unsigned int& a, unsigned int& b, unsigned int& c, unsigned int& d, unsigned int& e, unsigned int& f, unsigned int& g, unsigned int& h) {
    unsigned int S1, ch, temp1, S0, maj, temp2;
    for (int i = 0; i < 64; i++) {
        S1 = rightrotate(e, 6) ^ rightrotate(e, 11) ^ rightrotate(e, 25);
        ch = (e & f) ^ (~e & g);
        temp1 = h + S1 + ch + k[i] + w[i];
        S0 = rightrotate(a, 2) ^ rightrotate(a, 13) ^ rightrotate(a, 22);
        maj = (a & b) ^ (a & c) ^ (b & c);
        temp2 = S0 + maj;
        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }
}
/*void print_hashes(unsigned int h0, unsigned int h1, unsigned int h2, unsigned int h3, unsigned int h4, unsigned int h5, unsigned int h6, unsigned int h7) {
    cout << "h0 = " << bitset<32>(h0) << endl;
    cout << "h1 = " << bitset<32>(h1) << endl;
    cout << "h2 = " << bitset<32>(h2) << endl;
    cout << "h3 = " << bitset<32>(h3) << endl;
    cout << "h4 = " << bitset<32>(h4) << endl;
    cout << "h5 = " << bitset<32>(h5) << endl;
    cout << "h6 = " << bitset<32>(h6) << endl;
    cout << "h7 = " << bitset<32>(h7) << endl;
    cout << endl;
}*/
void sha256hash(unsigned char* data, ull* hashed, ull length/*, bool verbose*/) {

    /*
        todo:
            [done] free allocated memory for the chunks array

    */
    int chunks_count;
    unsigned char* chunks = get_chunks(data, length, chunks_count);
    //if (verbose) print_chunks(chunks, chunks_count);


    unsigned int h0 = 0x6a09e667;
    unsigned int h1 = 0xbb67ae85;
    unsigned int h2 = 0x3c6ef372;
    unsigned int h3 = 0xa54ff53a;
    unsigned int h4 = 0x510e527f;
    unsigned int h5 = 0x9b05688c;
    unsigned int h6 = 0x1f83d9ab;
    unsigned int h7 = 0x5be0cd19;

    unsigned int k[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };


    for (int i = 0; i < chunks_count; i++) {

        unsigned int* w = get_message_schedule(&chunks[i]);

        //if (verbose) print_message_schedule(w);

        unsigned int a = h0;
        unsigned int b = h1;
        unsigned int c = h2;
        unsigned int d = h3;
        unsigned int e = h4;
        unsigned int f = h5;
        unsigned int g = h6;
        unsigned int h = h7;

        compress(w, k, a, b, c, d, e, f, g, h);

        //deallocate_message_schedule_memory(w);
        delete[] w;

        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
        h4 += e;
        h5 += f;
        h6 += g;
        h7 += h;

    }



    //if (verbose) print_hashes(h0, h1, h2, h3, h4, h5, h6, h7);



    //deallocate_chunks_memory(chunks, chunks_count);
    delete[] chunks;

    *hashed = ((ull)h0) | (ull)h1;
    *(hashed+0x1) = ((ull)h2) | (ull)h3;
    *(hashed+0x2) = ((ull)h4) | (ull)h5;
    *(hashed+0x3) = ((ull)h6) | (ull)h7;
    /*cout << hex
        << setfill('0') << setw(8) << h0
        << setfill('0') << setw(8) << h1
        << setfill('0') << setw(8) << h2
        << setfill('0') << setw(8) << h3
        << setfill('0') << setw(8) << h4
        << setfill('0') << setw(8) << h5
        << setfill('0') << setw(8) << h6
        << setfill('0') << setw(8) << h7
        << endl << dec;*/

    //cout << flush;


    //return "ok";
    //return (char*)ss.str().c_str();
}
