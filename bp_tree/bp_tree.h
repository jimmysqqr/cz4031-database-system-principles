#ifndef BP_TREE_H
#define BP_TREE_H

#include "../storage/helper_types.h"
#include "../storage/disk_storage.h"
#include <cstddef>
#include <array>

class TreeNode
{
private:
    // pointer to the next node
    // total number of pointer will be = total number of data keys + 1
    // pointer to array of void pointers
    void **pointer; // will be either an array of TreeNode pointers (internal nodes) or ListNode pointers (leaf nodes)
    // Address *pointer;
    
    // Pointer to array of data keys in this tree node
    int *dataKey;

    // Used to keep track the number of data keys we have or have "created" so far
    int numOfKey;

    // Flag indicating if the node is a leaf
    bool isLeaf;

    // We need to allow the BPTree class to access these private variables
    friend class BPTree;

public:
    // Constructor, totalDataKey referes to the number of data keys in a node
    TreeNode(int totalDataKey);
};

class ListNode 
{
public:
    ListNode *next;
    Address *recordAddress;
    
    // Constructor for a ListNode
    ListNode(Address *recordAddress);
};

class BPTree
{
private:
    // Pointer to disk storage for data block
    // DiskStorage *disk;

    // Pointer to disk storage for the index
    // DiskStorage *index;

    // Pointer to main memory
    TreeNode *root;

    // Pointer to root's address on the disk
    // void *addressOfRoot;

    // Max number of keys a node in the B+ tree can hold
    int maxDataKey;

    // Number of level in the B+ tree
    int numOfLevel;

    // Number of nodes in the B+ tree
    int numOfNode;

    // Size of a node in the tree, which is also equal to the size of a block
    std::size_t nodeSize;

    // Tracks the sum of 'averageRating' of records retrieved in search function
    float sumOfAverageRating;

    // Tracks the num of 'averageRating' of records retrieved in search function
    int numOfRecordsRetrieved;

    // Helper function to return the disk address of the immediate parent of the target node
    TreeNode *getParent(TreeNode *rootDiskAddress, TreeNode *targetDiskAddress, int lower);

    // Inserts new parent or updates parent in the tree with child nodes
    void insertUpdateParent(TreeNode *curNodeDiskAddress, TreeNode *childDiskAddress, int key);

    // Function that recursively updates the parent due to the removal of a lower level node in the B+ tree
    int recursiveParentUpdate(int key, int keyIdx, TreeNode *currentDiskAddress, TreeNode *childDiskAddress);

public:
    // Constructor
    BPTree(std::size_t blockSize, DiskStorage *disk, DiskStorage *index);

    // Getter functions
    int getNumOfNodes()
    {
        return numOfNode;
    }

    int getMaxDataKey()
    {
        return maxDataKey;
    }

    float getAverageOfAverageRatings()
    {
        return sumOfAverageRating / numOfRecordsRetrieved;
    }

    // Helper function that returns the height of the B+ Tree Index
    int getHeight();

    // Function that deletes a key from the B+ tree
    int remove(int key);

    // Function that inserts data into nodes of B+ tree
    void insertKey(Address address, int key);

    // Function to insert record into existing linked list
    void insertListNode(ListNode* head, Address address, int key);

    // Search functions that supports a range query on the B+ Tree Index
    void search(int lower, int upper);

    // Function that prints the specified node of the B+ Tree
    void displayNode(TreeNode *node);

    // Function that prints the specified data block and its content
    void displayBlock(void *blockAddress);

    // Function that prints the entire linked list (pointed by the leaves)
    void displayList(Address headAddress);
};

#endif
