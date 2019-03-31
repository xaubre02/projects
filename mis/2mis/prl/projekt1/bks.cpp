/* -----------------------
 * Subject: PRL 2018/2019
 * Project: Bucket sort
 * Date:    2019-03-07
 * Author:  Tomas Aubrecht
 * Login:   xaubre02
 * ----------------------- */

//#define MEASURE_PERFORMANCE  // performance measurement

#define INPUT_FILE    "numbers"  // input file
#define TAG_COUNT     (0)        // sending total count of values
#define TAG_VALUE     (1)        // sending a single value

#include "bks.h"
#include <math.h>
#include <fstream>
#include <iostream>

using namespace std;

void Node::calcParentRank(int mpi_size) {
    if (_rank == 0) _parent = -1; // root node
    else            _parent = (_rank % 2) == 0 ? (_rank - 2) / 2 : (_rank - 1) / 2; 
}

void Node::calcChildrenRanks(int mpi_size) {
    // children
    _left_child  = _rank * 2 + 1;
    
    // leaf node
    if (_left_child >= mpi_size) {
        _left_child  = -1;
        _right_child = -1;
        _leaf = true;
        return;
    }
    else {
        _right_child = _rank * 2 + 2;
        _leaf = false;
    }
}

void Node::recvVector(int_vec &vec, int from) {
    if (from >= 0) {
        // receive the number of values in the vector
        MPI_Recv(&count, 1, MPI_INT, from, TAG_COUNT, MPI_COMM_WORLD, &stat);
        // receive values one by one
        for (int i = 0; i < count; i++) {
            MPI_Recv(&value, 1, MPI_INT, from, TAG_VALUE, MPI_COMM_WORLD, &stat);
            vec.push_back(value);
        }
    }
}

void Node::sendVector(int_vec &vec, int to) {
    if (to >= 0) {
        // send the number of values in the vector
        count = vec.size();
        MPI_Send(&count, 1, MPI_INT, to, TAG_COUNT, MPI_COMM_WORLD);
        // send values one by one
        for (int i = 0; i < count; i++) {
            MPI_Send(&vec[i], 1, MPI_INT, to, TAG_VALUE, MPI_COMM_WORLD);
        }
    }
}

void Node::printValues(int_vec &vec, bool one_line) {
    // print the vector on one line
    if (one_line) {
        for (int i = 0; i < vec.size() - 1; i++) {
            cout << vec[i] << " ";
        }
        cout << vec[vec.size() - 1] << endl;
    }
    // print each value in the vector on a new line
    else {
        for (int i = 0; i < vec.size(); i++) {
            cout << vec[i] << endl;
        }
    }
}

void Node::readValues(int_vec &vec, std::string filename) {
    char c;
    // read the values from the file
    ifstream fin(filename.c_str(), ifstream::binary);
    while (fin.get(c)) {
        vec.push_back((int)c);
    }
}

void bucketSort(std::string filename, int mpi_rank, int mpi_size) {
    // initialize tree node
    Node node(mpi_rank, mpi_size);
    
    int_vec vector1; // values from the left child
    int_vec vector2; // values from the right child
    int_vec merged;  // merged values of both children
    
    // root node
    if (node.isRoot()) {
        // read values from the file and print them on one line
        node.readValues(vector1, filename);
        node.printValues(vector1, true);
        
        int val_count  = vector1.size();                   // total count of values
        int leaves     = (mpi_size + 1) / 2;               // number of leaf nodes
        int bucket_cap = ceil((double)val_count / leaves); // bucket capacity

        if (leaves > 1) {
            // split and send the values to the leaves
            int_vec bucket;
            for (int leaf = mpi_size - leaves; leaf < mpi_size; leaf++) {
                // fill the bucket
                for (int i = 0; i < bucket_cap; i++) {
                    // vector of input values is not empty
                    if (vector1.size()) {
                        // add a value from input vector to bucket and remove it from the input vector
                        bucket.push_back(vector1[0]);
                        vector1.erase(vector1.begin());
                    }
                }
                // send the bucket and then clear it
                node.sendVector(bucket, leaf);
                bucket.clear();
            }
        }
    }

    // receive a vector of values from the first processor
    if (node.isLeaf()) {
        // if leaf node is also the root node -> there is only one node
        if (node.isRoot()) {
            node.sortVector(vector1);
            node.printValues(vector1);
        }
        else {
            // receive values from the root
            node.recvVector(vector1, 0);
            // sort the values
            node.sortVector(vector1);
            // send sorted values to parent
            node.sendVector(vector1, node.parent());
        }
    }
    else {
        // receive vectors from children
        node.recvVector(vector1, node.leftChild());
        node.recvVector(vector2, node.rightChild());
        // merge vectors
        node.mergeVectors(vector1, vector2, merged);
        // if the node is root, print the values, otherwise send the vector to parent
        if (node.isRoot()) {
            node.printValues(merged);
        }
        else {
            node.sendVector(merged, node.parent());
        }
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
    
    // bucket sort
    bucketSort(INPUT_FILE, mpi_rank, mpi_size);

    MPI_Finalize();
    return EXIT_SUCCESS;
}