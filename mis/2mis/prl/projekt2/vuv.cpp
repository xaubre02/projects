/* ----------------------------
 * Subject: PRL 2018/2019
 * Project: Compute node level
 * Date:    2019-04-14
 * Author:  Tomas Aubrecht
 * Login:   xaubre02
 * ---------------------------- */
 
#include "vuv.h"
#include <mpi.h>
#include <string>
#include <iostream>

using namespace std;


void compute_node_level(char *str, int mpi_rank, int mpi_size) {
    cout << "Proc #" << mpi_rank << ": " << str << endl;
}

void calculate_performance(timespec start, timespec end) {
    timespec temp;
    if ((end.tv_nsec-start.tv_nsec) < 0) {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    }
    else {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }

    // print the result
    cout << "Sorting time: " << temp.tv_sec << "s, " << temp.tv_nsec << "ns" << endl;
}

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

    if (argc > 1) {
        nejakafunkce(argv[1], mpi_rank, mpi_size);
    }
    
    MPI_Finalize();
    return EXIT_SUCCESS;
}