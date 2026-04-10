#include <stdio.h>
#include <omp.h>

int main() {
    int shared_var = 0;
    int private_var = 0;

    // Parallel region starts here
    #pragma omp parallel private(private_var) shared(shared_var)
    {
        int thread_id = omp_get_thread_num();

        // Modify shared variable (unsafe without synchronization)
        shared_var += 1;

        // Modify private variable (each thread has its own copy)
        private_var += 1;

        printf("Thread %d: shared_var = %d, private_var = %d\n",
                thread_id, shared_var, private_var);
    }

    // Final value of shared variable after parallel region
    printf("Final shared_var = %d (should equal number of threads)\n", shared_var);

    return 0;
}