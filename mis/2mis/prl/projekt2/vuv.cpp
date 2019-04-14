/* --------------------------------
 * Subject: PRL 2018/2019
 * Project: Vypocet urovne vrcholu
 * Date:    2019-04-14
 * Author:  Tomas Aubrecht
 * Login:   xaubre02
 * -------------------------------- */
 
#include "vuv.h"
#include <mpi.h>
 

/**
 * Main function
 * @param argc number of input arguments
 * @param argv list of arguments
 * @return sucess flag
 */
int main(int argc, char* argv[]) {
    int mpi_rank;
    int mpi_size;

    // initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  
    MPI_Finalize();
    return EXIT_SUCCESS;
}