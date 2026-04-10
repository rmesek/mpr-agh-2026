#include <omp.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{

    #pragma omp parallel
    {
        srand((unsigned int)time(NULL));

        int i=0;
        for(i=0;i<5;i++)
        {
            printf("Random number: %d by thread %d\n", rand(), omp_get_thread_num());
        }
    }

}