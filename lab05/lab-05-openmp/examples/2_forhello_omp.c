#include <omp.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{

    #pragma omp parallel for
        for (int i=0 ; i < 3 ; i++)
        {
                printf("Hello World... %d \n", i);
        }
}