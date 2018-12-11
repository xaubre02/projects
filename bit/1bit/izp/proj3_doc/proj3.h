
/**
 * @mainpage Documentation
 * @brief Documentation of source code of Project #3. Describes each function, structure and variable and their parameters.
 * @author Tomas Aubrecht (xaubre02)
 * @version 1.0
 * @date 15.12.2015
 */

struct obj_t {
    /// number representing an unique name of an object
    int id;
    /// X coordinate
    float x;
    /// Y coordinate
    float y;
};

struct cluster_t {
    /// size of cluster
    int size;
    /// capacity(for how many objects should memory be reserved); < INT_MAX
    int capacity;
    /// array of objects in the cluster (NULL if uninitialized)
    struct obj_t *obj;
};

/**
 * @defgroup cluster Operations over a cluster
 * @{
 */

/**
 * Initialization of cluster 'c'. Allocate memory for capacity of objects.
 * @param  c cluster which will be initialized
 * @param  cap capacity of cluster
 * @pre    c != NULL
 * @pre    cap >= 0
 * @post   c->capacity = cap
 */
void init_cluster(struct cluster_t *c, int cap);

/**
 * Removing all object of cluster 'c' and initialization to an empty cluster.
 * @param c cluster to be cleared
 * @pre  c != NULL
 * @post c->obj=NULL
 * @post c->size = 0
 * @post c->capacity = 0
 */
void clear_cluster(struct cluster_t *c);

/// Chunk of cluster objects. Value recommended for reallocation.
extern const int CLUSTER_CHUNK;

/**
 * Reallocate memory for a new number of objects.
 * @param  c
 * @param  new_cap
 * @return c the cluster with new capacity
 * @pre    new_cap >= 0
 * @pre    c->capacity >= 0
 * @post   c->capacity = new_cap
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap);

/**
 * Add object 'obj' to the end of cluster.
 * @param  c cluster to which the object will be added
 * @param  obj object which will be appended
 * @pre    none
 * @post   c->size = c->size + 1
 */
void append_cluster(struct cluster_t *c, struct obj_t obj);

/**
 * Add object from cluster 'c2' to cluster 'c1'
 * @param c1 cluster to be expanded
 * @param c2 cluster which will be merged to 'c1'
 * @pre c1 != NULL
 * @pre c2 != NULL
 * @post cluster 'c2' remains unmodified
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2);

///@}

/**
 * @defgroup cluster2 Operations over an array of clusters
 * @{
 */

/**
 * Removing cluster from array of clusters. Array contains 'narr' clusters.
 * @param  carr Array of clusters where a cluster will be removed.
 * @param  narr Number of clusters in array 'carr'
 * @param  idx  Position of cluster in array to be removed.
 * @pre    idx < narr
 * @pre    narr > 0
 * @post   carr->size = carr->size - 1
 * @return Returns new count of clusters in array.
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx);

/**
 * Calculates distance between two objects using Euclidean algorithm.
 * @param  o1 Pointer to the first object 'o1'
 * @param  o2 Pointer to the second object 'o2'
 * @pre    o1 != NULL
 * @pre    o2 != NULL
 * @post   distance >= 0
 * @return Distance between two objects.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2);

/**
 * Calculates distance between two clusters based on the nearest neighbours (objects)
 * @param  c1 Pointer to the first cluster 'c1' where distance will be calculated.
 * @param  c2 Pointer to the second cluster 'c2' where distance will be calculated.
 * @pre    c1 != NULL
 * @pre    c2 != NULL
 * @pre    c1->size > 0
 * @pre    c2->size > 0
 * @post   distance >= 0
 * @return Minimal distance between two clusters.
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2);

/**
 * Finds two nearest neighbours.
 * @param carr  cluster array
 * @param narr  number of clusters
 * @param c1    pointer to index of the first neighbour
 * @param c2    pointer to index of the second neighbour
 * @pre   0 < narr
 * @post  0 <= *c1 < *c2 < narr
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2);

/**
 * Sorting of objects in cluster 'c' from the highest ID to lowest.
 * @param  c Pointer to the cluster to be sorted.
 * @pre    c != NULL
 * @post   Cluster is sorted.
 */
void sort_cluster(struct cluster_t *c);

/**
 * Printing of all object of cluster 'c' to stdout.
 * @param  c Pointer to the cluster to be printed.
 * @pre    c != NULL
 * @post   Cluster is printed.
 */
void print_cluster(struct cluster_t *c);

/**
 * Loads objects from file 'filename'. Creates new cluster for each object and stores it to the array of clusters 'carr'.
 * Allocates memory for array of all clusters and pointer to the first one saves in memory, where 'arr' pointing to.
 * @param  filename Name of file to load objects from.
 * @param  arr Pointer to pointer to array of clusters
 * @pre    arr != NULL
 * @pre    filename exists
 * @post   loaded objects
 * @return Number of successfully loaded objects.
 */
int load_clusters(char *filename, struct cluster_t **arr);

/**
 * Printing of array 'carr'
 * @param  carr Pointer to the first cluster to be printed.
 * @param  narr Number of clusters to be printed
 * @pre    carr != NULL
 * @pre    narr > 0
 * @post   Printed clusters.
 */
void print_clusters(struct cluster_t *carr, int narr);

///@}

