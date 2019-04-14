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

// MPI_Scan je suma prefixu

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
 */
void calculate_performance(timespec start, timespec end);

#endif