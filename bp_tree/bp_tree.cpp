#include "bp_tree.h"
#include "../storage/disk_storage.h"
#include "../storage/helper_types.h"

#include <array>
#include <iostream>
#include <cstring>

using namespace std;

// initialise pointer to null at the beginning
bool nullPointer = false;

// Constructor to create individual tree node
TreeNode::TreeNode(int maxDataKey)
{
    // Dynamic allocation of a pointer to an array, size of array = total number of data key
    // float *DataKey = new float[maxDataKey]
    dataKey = new int[maxDataKey];

    // Dynamic allocation of a pointer to an array, size of array = total number of data key + 1
    // Address *pointer = new Address[maxDataKey]
    pointer = new void *[maxDataKey + 1];

    // using a for loop, for each of the pointer in the node, initialise it to NULL
    for (int i = 0; i < maxDataKey + 1; i++)
    {
        // Address nullAddress{(void *)nullPointer, 0};
        // pointer[i] = nullAddress;
        pointer[i] = nullptr;
    }

    // initialise the number of key that we have used or "created" for this node to be 0
    // Because we are only creating a node and have not insert any record yet
    numOfKey = 0;

    // Don't forget to initialize the isLeaf boolean lest we encounter undefined behaviour
    isLeaf = false;
}

// Constructor to create a ListNode
ListNode::ListNode(Address recordAddress)
{
    this->recordAddress = recordAddress;
    this->next = nullptr;
}

// Constructor to create a tree
BPTree::BPTree(std::size_t blockSize, DiskStorage *disk)
{
    // Amount of size left after subtracting off size used to keep track if node is a leaf node (boolean) and number of keys (integer)
    size_t sizeOfNodeBuffer = blockSize - sizeof(bool) - sizeof(int);

    // Left pointer in node
    size_t sum = sizeof(void *);
    maxDataKey = 0;

    while (sum + sizeof(void *) + sizeof(int) <= sizeOfNodeBuffer)
    {
        sum += (sizeof(void *) + sizeof(int));
        maxDataKey += 1;
    }

    if (maxDataKey == 0)
    {
        throw overflow_error("ERROR! Number of keys and pointers are too large!");
    }

    // Set the address of the root node and root to NULL
    // addressOfRoot = nullptr;
    root = nullptr;

    nodeSize = blockSize;

    // At the creation of the tree, number of level and node will be = 0
    numOfLevel = 0;
    numOfNode = 0;

    // At the creation of the tree, the search variables are initialized to 0
    sumOfAverageRating = 0.0;
    numOfRecordsRetrieved = 0;
    numOfNodesAccessed = 0;

    // Initialisation of disk space for index and set reference to disk
    this->disk = disk;
}
