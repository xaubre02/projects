/* -----------------------
 * Předmět: PRL 2018/2019
 * Projekt: Bucket sort
 * Datum:   2019-03-07
 * Autor:   Tomáš Aubrecht
 * Login:   xaubre02
 * ----------------------- */

#include <mpi.h>
#include <vector>
#include <string>
#include <algorithm>

using int_vec = std::vector<int>;

class Node {
private:

    // Node attributes
    int _rank;
    int _parent;
    int _left_child;
    int _right_child;
    bool _leaf;

    void calcParentRank(int mpi_size);
    void calcChildrenRanks(int mpi_size);

    int count;
    int value;
    MPI_Status stat;

public:

    inline Node(int rank, int mpi_size): _rank(rank) {
        calcParentRank(mpi_size);
        calcChildrenRanks(mpi_size);
    }

    inline int rank() const        {return _rank;}
    inline int parent() const      {return _parent;}
    inline int leftChild() const   {return _left_child;}
    inline int rightChild() const  {return _right_child;}
    inline bool isRoot() const     {return _rank == 0;}
    inline bool isLeaf() const     {return _leaf;}
    
    void recvVector(  int_vec &vec,         int from);
    void sendVector(  int_vec &vec,         int to);
    void printValues( int_vec &vec,        bool input=false);
    void readValues(  int_vec &vec, std::string filename);

    inline void sortVector(int_vec &vec) {
        std::sort(vec.begin(), vec.end());
    }

    inline void mergeVectors(int_vec &vec1,
                             int_vec &vec2, 
                             int_vec &vec_out) {
        std::merge(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(), std::back_inserter(vec_out));
    }
};

void bucketSort(int mpi_rank, int mpi_size);
