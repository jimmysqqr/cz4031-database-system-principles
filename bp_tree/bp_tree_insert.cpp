#include "bp_tree.h"
#include "../storage/helper_types.h"

#include <vector>
#include <cstring>
#include <iostream>

// Function to insert data into nodes
void BPTree::insert(Address address, float key)
{
    if (addressOfRoot == = nullptr)
    {

        TreeNode *nodeLL = new TreeNode(totalDataKey);
        nodeLL->dataKey[0] = key;
        nodeLL->isLeaf = false;
        nodeLL->numOfKey = 1;
        nodeLL->pointer[0] = address; // insert disk address of the key

        // Allocate nodeLL and root address
        Address nodeLLRootAddress = index->writeToDisk((void *)nodeLL, nodeSize);

        root = new TreeNode(totalDataKey);
        root->dataKey[0] = key;
        root->isLeaf = true; // Both root and leaf
        root->numOfKey = 1;
        root->pointer[0] = nodeLLRootAddress; // Assign record disk to pointer

        // Write root node to disk
        // Get disk address after writing to disk and assign it to addressOfRoot
        addressOfRoot = index->writeToDisk(root, nodeSize).blockAddress;
    }
    // else if root exists, traverse the nodes to find proper place to insert key
    else
    {
        TreeNode *curNode = root;                 // inialize current node
        TreeNode *parent;                         // keep track of parent node for updating
        void *parentDiskAddress = addressOfRoot;  // Keep track of parent's disk address for updating
        void *curNodeDiskAddress = addressOfRoot; // Store current node's disk address for updating

        // Go through all nodes till you reach a leaf node
        while (curNode->isLeaf == false)
        {

            // Set parent node for going back and assigning child node later and set parent disk address
            parent = curNode;
            parentDiskAddress = curNodeDiskAddress;
        }
    }
}