#include <iostream>
#include "tests.h"

using namespace std;

int main()
{
    checker(test_add_buffers(), "add_buffers");

    checker(test_sub_buffers(), "sub_buffers");

    checker(test_mul_buffers(), "mul_buffers");

    checker(test_mod_buffer(), "mod_buffer");

    checker(test_powermod_buffer(), "powermod_buffer");

    return 0;
}
