#ifndef BUFFER_ARITHMETICS_H_INCLUDED
#define BUFFER_ARITHMETICS_H_INCLUDED

typedef unsigned long long ull;

extern "C" void add_buffers(ull* input_buffer_1, ull* input_buffer_2, ull* output_buffer, ull length);

extern "C" void sub_buffers(ull* input_buffer_1, ull* input_buffer_2, ull* output_buffer, ull length);

extern "C" void mul_buffers(ull* input_buffer_1, ull* input_buffer_2, ull* output_buffer, ull input_n_bytes);

#endif // BUFFER_ARITHMETICS_H_INCLUDED
