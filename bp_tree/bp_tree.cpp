#include "bp_tree.h"
#include "disk_storage.h"
#include "helper_types.h"

#include <tuple>
#include <iostream>
#include <array>
#include <unordered_map>
#include <cstring>

using namespace std;

// initialise pointer to null at the beginning
bool nullPointer = false;

// Function to create individual tree node
TreeNode::TreeNode(int totalDataKey)
{
    // Dynamic allocation of a pointer to an array, size of array = total number of data key 
    // float *DataKey = new float[totalDataKey]
    dataKey = new float[totalDataKey];

    // Dynamic allocation of a pointer to an array, size of array = total number of data key + 1
    // Address *pointer = new Address[totalDataKey]
    pointer = new Address[totalDataKey + 1];

    // using a for loop, for each of the pointer in the node, initialise it to NULL
    for(int i = 0; i < totalDataKey + 1; i++)
    {
        Address nullAddress{(void *)nullPointer, 0};
        pointer[i] = nullAddress;
    }

    // initialise the number of key that we have used or "created" for this node to be 0
    // Because we are only creating a node and have not insert any record yet
    numOfkey = 0;
}

// Function to create a tree
BPTree::BPTree(std::size blockSize, DiskStorage *disk, DiskStorage *index)
{
    // Amount fo size left after minusing off size used to keep track if node is a leaf node and number of keys
    size sizeOfNodeBuffer = blockSize - sizeof(bool) - sizeof(int);

    size sum = sizeof(Address);
    maxKey = 0;

    while(sum + sizeof(Address) + sizeof(float) <= NodeBufferSize)
    {
        sum += (sizeof(Address) + sizeof(float));
        maxKey += 1;
    }

    if(maxKey == 0)
    {
        throw std::overflow_error("ERROR! Number of keys and pointers are too large!");
    }

    // Set the address of the root node and root to NULL
    addressOfRoot = nullptr;
    root = nullptr;

    nodeSize = blockSize;

    // At the creation of the tree, number of level and node will be = 0
    numOflevel = 0;
    numOfNode = 0;

    // Initialisation of disk space for index and set reference to disk
    this->disk = disk;
    this->index = index;
}
