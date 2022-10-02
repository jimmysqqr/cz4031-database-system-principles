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
    // will be either an array of TreeNode pointers (internal nodes) or ListNode pointers (linked list of a leaf node)
    // hence the use of void *
    // total number of pointer will be = total number of data keys + 1
    void **pointer;

    // Pointer to array of data keys in this tree node
    int *dataKey;

    // Used to keep track the number of data keys we have or have "created" so far
    int numOfKey;

    // Flag indicating if the node is a leaf
    bool isLeaf;

    // We need to allow the BPTree class to access these private variables
    friend class BPTree;

public:
    // Constructor, maxDataKey referes to the number of data keys in a node
    TreeNode(int maxDataKey);

    // Helper function to retrieve the first child of a node
    TreeNode *getFirstChild()
    {
        return (TreeNode *)this->pointer[0];
    }
};

class ListNode
{
public:
    ListNode *next;
    Address recordAddress;

    // Constructor for a ListNode
    ListNode(Address recordAddress);
};

class BPTree
{
private:
    // Pointer to disk storage for data block
    DiskStorage *disk;

    // Root of B+ Tree
    TreeNode *root;

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

    // Tracks the num of index nodes (index blocks) accessed in search function
    int numOfNodesAccessed;

    // Helper function to return the disk address of the immediate parent of the target node
    TreeNode *getParent(TreeNode *targetNode, int lower);

    // Inserts new parent or updates parent in the tree with child nodes
    void insertUpdateParent(TreeNode *curNodeDiskAddress, TreeNode *childDiskAddress, int key);

    // Function that recursively updates the parent due to the removal of a lower level node in the B+ tree
    int recursiveParentUpdate(int key, int keyIdx, TreeNode *currentDiskAddress, TreeNode *childDiskAddress);

public:
    // Constructor
    BPTree(std::size_t blockSize, DiskStorage *disk);

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

    int getNumIndexNodesAccessed()
    {
        return numOfNodesAccessed;
    }

    void resetNumIndexNodesAccessed()
    {
        numOfNodesAccessed = 0;
    }

    TreeNode *getRoot()
    {
        return this->root;
    }

    // Helper function that returns the height of the B+ Tree Index
    int getHeight();

    // Function that deletes a key from the B+ tree
    int remove(int key);

    // Function that inserts data into nodes of B+ tree
    void insertKey(Address address, int key);

    // Function to insert record into existing linked list
    void insertListNode(ListNode *head, Address address, int key);

    // Search functions that supports a range query on the B+ Tree Index
    void search(int lower, int upper);

    // Function that prints the specified node of the B+ Tree
    void displayNode(TreeNode *node);

    // Function that prints the entire linked list (pointed by the leaves)
    // print flag used to limit the terminal output to first 5 leaf nodes only
    void displayList(ListNode *leafNode, bool print);
};

#endif
