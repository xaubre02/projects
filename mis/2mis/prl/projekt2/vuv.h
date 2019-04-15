/* ----------------------------
 * Subject: PRL 2018/2019
 * Project: Compute node level
 * Date:    2019-04-14
 * Author:  Tomas Aubrecht
 * Login:   xaubre02
 * ---------------------------- */
 
#ifndef _VUV_H_
#define _VUV_H_

#define MEASURE_PERFORMANCE false // performance measurement

#include <mpi.h>
#include <time.h>


class Edge {
public:
    int mpi_rank;     // MPI rank
    int mpi_size;     // total number of processors
    int weight;       // node weight/level
    unsigned from;    // ID of node this edge is going from (starting at 1)
    unsigned to;      // ID of node this edge is going to (starting at 1)
    char symbol;      // node name

    MPI_Status stat;  // MPI stat for receiving data

public:

    /**
     * Edge constructor.
     * @param mpi_rank rank in the MPI environment
     * @param mpi_size total number of processors in the MPI environment
     */
    Edge(int mpi_rank, int mpi_size): mpi_rank(mpi_rank), mpi_size(mpi_size) {}
    
    /**
     * Calculate IDs of nodes from which this edge is going and IDs where this edge is going.
     * @param str input string specifing the tree and names of its nodes
     */
    void calc_edge_ends(char *str);

    /**
     * Calculate the suffix sum.
     * @return suffix sum
     */
    int calc_suffix_sum(void);

    /**
     * Print the vertex. Prints ",name:level".
     */
    void print_vertex(void);
    
    /**
     * Return the flag if this edge is a forward edge.
     * @return forward flag
     */
    inline bool is_forward(void) { return to > from; }

    /**
     * Initialize the weight of this edge to -1 if it is a forward edge, otherwise 1.
     */
    inline void init_weight(void) { weight = is_forward() ? -1 : 1; }

    /**
     * Update the weight of the node this edge is leading to.
     * @param ssum
     */
    inline void update_weight(int ssum) { 
        if (is_forward()) {
            weight = ssum;
        }
    }
};

/**
 * Compute a level of each node in a tree.
 * @param str string specifing the tree
 * @param mpi_rank ID of the current processor
 * @param mpi_size total number of processors
 */
void compute_node_level(char *str, int mpi_rank, int mpi_size);

/**
 * Calculate the difference between start and end time of the sorting and print the result.
 * @param start start time
 * @param end end time
 * @param nsec_only print out only the number of nanoseconds
 */
void calculate_performance(timespec start, timespec end, bool nsec_only=false);

#endif