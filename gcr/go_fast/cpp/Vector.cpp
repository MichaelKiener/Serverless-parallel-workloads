#include <iostream>

int main(int argc, char const *argv[])
{
    int A[1000];
    int B[1000];
    int C[1000];

    for(int i = 0; i<1000; i++) {
        A[i] = i;
        B[i] = i * 2;
        C[i] = (i + 4) / 2;
    }

    #pragma ivdep
    for(int i = 0; i<1000; i++) {
        A[i] = B[i] + C[i];
    }

    std::cout << A[1] << std::endl;

    return 0;
}
