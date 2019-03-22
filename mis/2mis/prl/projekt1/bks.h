/* -----------------------
 * Subject: PRL 2018/2019
 * Project: Bucket sort
 * Date:    2019-03-07
 * Author:  Tomas Aubrecht
 * Login:   xaubre02
 * ----------------------- */

#include <mpi.h>
#include <vector>
#include <string>
#include <algorithm>

// type alias
using int_vec = std::vector<int>;

class Node {
private:

    int _rank;         // node rank
    int _parent;       // rank of the node's parent
    int _left_child;   // rank of the node's left child
    int _right_child;  // rank of the node's right child
    bool _leaf;        // flag whether the node is the tree leaf

    /**
     * Calculate the rank of the node's parent.
     * @param mpi_size total number of processors
     */
    void calcParentRank(int mpi_size);
    
    /**
     * Calculate the ranks of the node's children.
     * @param mpi_size total number of processors
     */
    void calcChildrenRanks(int mpi_size);

    int count;         // number of values in a vector (send/receive)
    int value;         // single value in a vector
    MPI_Status stat;   // MPI status

public:

    /**
     * Class constructor. Calculate node attributes.
     * @param rank rank of the current processor
     * @param mpi_size total number of processors
     */
    inline Node(int rank, int mpi_size): _rank(rank) {
        calcParentRank(mpi_size);
        calcChildrenRanks(mpi_size);
    }

    /**
     * Return node's rank.
     * @return node's rank     
     */
    inline int rank() const {return _rank;}
    
    /**
     * Return rank of the node's parent.
     * @return rank of the node's parent.
     */
    inline int parent() const {return _parent;}
    
    /**
     * Return rank of the node's left child.
     * @return rank of the node's left child.
     */
    inline int leftChild() const {return _left_child;}
    
    /**
     * Return rank of the node's right child.
     * @return rank of the node's right child.
     */
    inline int rightChild() const {return _right_child;}
    
    /**
     * Return true if the node is root, false otherwise.
     * @return flag, whether the node is root
     */
    inline bool isRoot() const {return _rank == 0;}
    
    /**
     * Return true if the node is leaf, false otherwise.
     * @return flag, whether the node is leaf
     */
    inline bool isLeaf() const {return _leaf;}
    
    /**
     * Receive a vector of int values.
     * @param vec vector which receives int values
     * @param from ID of the sender
     */
    void recvVector(  int_vec &vec, int from);
    
    /**
     * Send a vector of int values.
     * @param vec int vector to be send
     * @param to ID of the receiver
     */
    void sendVector(  int_vec &vec, int to);
    
    /**
     * Print int values of a vector.
     * @param vec vector to be printed
     * @param input flag, whether the vector values should be on one line or not
     */
    void printValues( int_vec &vec, bool one_line=false);
    
    /**
     * Read values from a binary file to vector.
     * @param vec output vector of read values
     * @param filename input file
     */
    void readValues(  int_vec &vec, std::string filename);

    /**
     * Sort a vector.
     * @param vec vector to sort
     */
    inline static void sortVector(int_vec &vec) {
        std::sort(vec.begin(), vec.end());
    }

    /**
     * Merge two vectors.
     * @param vec1 first vector
     * @param vec2 second vector
     * @param vec_out output merged vector
     */
    inline static void mergeVectors(int_vec &vec1,
                             int_vec &vec2, 
                             int_vec &vec_out) {
        std::merge(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(), std::back_inserter(vec_out));
    }
};

/**
 * Sort the values from a file using bucket sort.
 * @param filename input file containg values for sort
 * @param mpi_rank ID of the current processor
 * @param mpi_size total number of processors
 */
void bucketSort(std::string filename, int mpi_rank, int mpi_size);
