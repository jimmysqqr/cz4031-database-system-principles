#ifndef BP_TREE_H
#define BP_TREE_H

#include "helper_types.h"
#include "disk_storage.h"
#include <cstddef>
#include <array>

class TreeNode
{
private:
    // pointer to the next node
    // total number of pointer will be = total number of data keys + 1
    Address *pointer;

    // Pointer to array of data keys in this tree node
    float *dataKey;

    // Used to keep track the number of data keys we have or have "created" so far
    int numOfKey;

    // Flag indicating if the node is a leaf
    bool isLeaf;

    // We need to allow the BPTree class to access these private variables
    friend class BPTree;

public:
    // totalDataKey referes to the number of data keys in a node
    TreeNode(int totalDataKey);
};

class BPTree
{
private:
    // Pointer to Disk storage for data block
    DiskStorage *disk;

    // Pointer to disk storage in the disk
    DiskStorage *index;

    // Pointer to main memory
    TreeNode *root;

    // Pointer to root's address on the disk
    void *addressOfRoot;

    // Total number of data key in the node
    int totalDataKey;

    // Number of level in the B+ tree
    int numOflevel;

    // Number of nodes in the B+ tree
    int numOfNode;

    // Size of a node in the tree, which is also equal to the size of a block
    std::size_t nodeSize;

public:
    BPTree(std::size blockSize, DiskStorage *disk, DiskStorage *index);

    // Getter functions
    int getNumOfNodes()
    {
        return numOfNode;
    }

    int getTotalDataKey()
    {
        return totalDataKey;
    }

    insert(Address address, float key);

    search(float lower, floar upper);
}

#endif
