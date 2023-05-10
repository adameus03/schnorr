#include <iostream>
#include "tests.h"

using namespace std;

int main()
{
    cout << "Hello world!" << endl;

    checker(test_add_buffers(), "add_buffers");

    checker(test_sub_buffers(), "sub_buffers");

    checker(test_mul_buffers(), "mul_buffers");

    checker(test_smod_buffer(), "smod_buffer");

    return 0;
}
