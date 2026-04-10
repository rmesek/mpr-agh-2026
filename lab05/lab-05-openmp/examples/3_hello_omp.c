#include <omp.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{

    #pragma omp parallel
    {
        printf("Hello World... from Thread %d\n", omp_get_thread_num());
    }
}

// KOMPILACJA:
// gcc 4_omphello.c -o 4_omphello -fopenmp

// URUCHOMIENIE: mateu@DESKTOP-1VIJCQS:~/rownolegle/helloworld
// $  OMP_NUM_THREADS=2 ./4_omphello
// Hello World...
// Hello World...