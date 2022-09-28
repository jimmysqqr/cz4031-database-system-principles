#include "bp_tree.h"
#include "../storage/disk_storage.h"
#include "../storage/helper_types.h"

#include <array>
#include <iostream>
#include <cstring>

using namespace std;

// initialise pointer to null at the beginning
bool nullPointer = false;

// Function to create individual tree node
TreeNode::TreeNode(int totalDataKey)
{
    // Dynamic allocation of a pointer to an array, size of array = total number of data key
    // float *DataKey = new float[totalDataKey]
    dataKey = new int[totalDataKey];

    // Dynamic allocation of a pointer to an array, size of array = total number of data key + 1
    // Address *pointer = new Address[totalDataKey]
    pointer = new Address[totalDataKey + 1];

    // using a for loop, for each of the pointer in the node, initialise it to NULL
    for (int i = 0; i < totalDataKey + 1; i++)
    {
        Address nullAddress{(void *)nullPointer, 0};
        pointer[i] = nullAddress;
    }

    // initialise the number of key that we have used or "created" for this node to be 0
    // Because we are only creating a node and have not insert any record yet
    numOfKey = 0;
}

// Function to create a tree
BPTree::BPTree(std::size_t blockSize, DiskStorage *disk, DiskStorage *index)
{
    // Amount of size left after subtracting off size used to keep track if node is a leaf node (boolean) and number of keys (integer)
    size_t sizeOfNodeBuffer = blockSize - sizeof(bool) - sizeof(int);

    // Left pointer in node
    size_t sum = sizeof(Address);
    totalDataKey = 0;

    while (sum + sizeof(Address) + sizeof(float) <= sizeOfNodeBuffer)
    {
        sum += (sizeof(Address) + sizeof(float));
        totalDataKey += 1;
    }

    if (totalDataKey == 0)
    {
        throw overflow_error("ERROR! Number of keys and pointers are too large!");
    }

    // Set the address of the root node and root to NULL
    addressOfRoot = nullptr;
    root = nullptr;

    nodeSize = blockSize;

    // At the creation of the tree, number of level and node will be = 0
    numOfLevel = 0;
    numOfNode = 0;

    // Initialisation of disk space for index and set reference to disk
    this->disk = disk;
    this->index = index;
}
