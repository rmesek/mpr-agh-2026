#include <omp.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{

    double start,end;
    start = omp_get_wtime();

    // Beginning of parallel region
    #pragma omp parallel
    {

        #pragma omp for schedule(static,2)
        for (int i=0 ; i < 20 ; i++)
        {
                printf ( "Pierwszy for: iteracje %d wykonuje watek nr %d \n", i , omp_get_thread_num () ) ;
        }

        #pragma omp for schedule(dynamic,2)
        for (int i=0 ; i < 10 ; i++)
        {
                printf ( "Drugi for: iteracje %d wykonuje watek nr %d \n", i , omp_get_thread_num () ) ;
        }
    }
    // Ending of parallel region
    end = omp_get_wtime();
    printf ( "Execution time: %f\n\n", end-start);
}