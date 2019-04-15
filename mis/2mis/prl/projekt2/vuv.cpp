/* ----------------------------
 * Subject: PRL 2018/2019
 * Project: Compute node level
 * Date:    2019-04-14
 * Author:  Tomas Aubrecht
 * Login:   xaubre02
 * ---------------------------- */
 
#include "vuv.h"
#include <string>
#include <iostream>
#include <math.h>

using namespace std;
#define TAG_NODE_FROM   0
#define TAG_NODE_TO     1
#define TAG_SUFFIX_SUM  2


void Edge::calc_edge_ends(char *str) {
    int nodes = (mpi_size + 2) / 2;  // total number of nodes in a tree
    int next = mpi_rank + 1;         // next process

    // edge is specified by <from node, to node>
    int prev_from, prev_to;  // previous edge

    if (mpi_rank == 0) {
        from = 1;
        to = nodes > 1 ? 2 : 1;
    }
    else {
        MPI_Recv(&prev_from, 1, MPI_INT, mpi_rank - 1, TAG_NODE_FROM, MPI_COMM_WORLD, &stat);  // receive node ID from which the previous edge is starting
        MPI_Recv(&prev_to,   1, MPI_INT, mpi_rank - 1, TAG_NODE_TO,   MPI_COMM_WORLD, &stat);  // receive node ID to where the previous edge is leading (and this is starting at)
        
        from = prev_to;
        // leads to the left child
        if (prev_from < prev_to) { 
            to = prev_to * 2;
        }
        // leads to the right child
        else {
            to = prev_to * 2 + 1;
            // leads to the parent (already visited)
            if (to == prev_from) {
                to = from % 2 ? (from - 1) / 2 : from / 2;
            }
        }
        
        // no children -> go back to the parent
        if (to > nodes) {
            to = from % 2 ? (from - 1) / 2 : from / 2;
        }        
    }

    // there is another process waiting
    if (next < mpi_size) {
        MPI_Send(&from, 1, MPI_INT, next, TAG_NODE_FROM, MPI_COMM_WORLD);  // send node ID from which this edge is starting
        MPI_Send(&to,   1, MPI_INT, next, TAG_NODE_TO,   MPI_COMM_WORLD);  // send node ID to where this edge is leading
    }

    // store the vertex symbol
    symbol = str[to-1];
}

int Edge::calc_suffix_sum(void) {
    int ssum = weight;  // suffix sum
    int next_ssum;      // suffix sum of the next processor
    
    int iters = ceil(log2(mpi_size));  // number of iterations = log(n)
    
    int powr;  // power
    int next;  // ID of the next processor
    int prev;  // ID of the previous processor

    for (int i = 0; i < iters; i++) {
        powr = pow(2, i);
        prev = mpi_rank - powr;  // ID - 2^i
        next = mpi_rank + powr;  // ID + 2^i

        // send data to the previous processor
        if (prev > -1) {
            MPI_Send(&ssum, 1, MPI_INT, prev, TAG_SUFFIX_SUM, MPI_COMM_WORLD);
        }
        
        // receive data from the next proccesor
        if (next < mpi_size) {
            MPI_Recv(&next_ssum, 1, MPI_INT, next, TAG_SUFFIX_SUM, MPI_COMM_WORLD, &stat);
            ssum += next_ssum;
        }
    }

    return ssum;
}

void Edge::print_vertex(void) {
    // iterate through all processes
    for (int i = 0; i < mpi_size; i++) {
        // synchronize
        MPI_Barrier(MPI_COMM_WORLD);
        // print the node on turn
        if (is_forward() && to == i + 1) {
            cout << "," << symbol << ":" << weight;
            fflush(stdout);
        }
    }

    // add one end of line in the end
    if (mpi_rank == 0) {
        cout << endl;
    }
}

void compute_node_level(char *str, int mpi_rank, int mpi_size) {
    // initialize edge
    Edge edge(mpi_rank, mpi_size);
    
    // timestamps
    timespec start;
    timespec end;
    
    // algorithm
    // step 0: calculate ends of each edge (Etour)
    edge.calc_edge_ends(str);

    // start measuring the time
    if (MEASURE_PERFORMANCE && mpi_rank == 0){
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    }
        
    // step 1: initialize weights
    edge.init_weight();

    // step 2: calculate prefix sum
    int ssum = edge.calc_suffix_sum();
       
    // step 3: correct the weights
    edge.update_weight(ssum + 1);

    // calculate and print the measured time
    if (MEASURE_PERFORMANCE && mpi_rank == 0){
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
        calculate_performance(start, end);
    }
    
    // step 4: print the results
    if (!MEASURE_PERFORMANCE) {
        // print the root
        if (mpi_rank == 0) {
            cout << str[0] << ":" << 0;
            fflush(stdout);
        }

        edge.print_vertex();
    }
}

void calculate_performance(timespec start, timespec end, bool nsec_only) {
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
    if (nsec_only) {
        cout << temp.tv_nsec << endl;
    }
    else {
        cout << "Run time: " << temp.tv_sec << "s, " << temp.tv_nsec << "ns" << endl;
    }
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
        compute_node_level(argv[1], mpi_rank, mpi_size);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}