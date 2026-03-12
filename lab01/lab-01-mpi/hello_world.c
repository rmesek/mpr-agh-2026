// $ mpicc -o hello_world_c hello_world.c
// $ mpiexec [-machinefile ./allnodes] -np [liczba procesow] ./hello_world_c
#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size, len;
    char hostname[MPI_MAX_PROCESSOR_NAME];
    
    MPI_Init(&argc, &argv);  /* starts MPI */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  /* get current process id */
    MPI_Comm_size(MPI_COMM_WORLD, &size);  /* get number of processes */
    MPI_Get_processor_name(hostname, &len);  
    printf("[%s] Hello world from process %d of %d\n", hostname, rank, size);
    MPI_Finalize();
    return 0;
}
