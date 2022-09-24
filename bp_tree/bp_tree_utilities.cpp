#include "bp_tree.h"
#include "helper_types.h"

#include <vector>
#include <iostream>

using namespace std;

// Helper function to return the disk address of the immediate parent of the target node
TreeNode *BPTree::getParent(TreeNode *rootDiskAddress, TreeNode *targetDiskAddress, float lower)
{
    // Load the pointer into main memory and initialise it to the root
    Address rootAddress{rootDiskAddress, 0};
    TreeNode *curr = (TreeNode *)index->readFromDisk(rootAddress, nodeSize);

    // Sanity Check, if the root is a leaf node, there would not be any children and consequently no parent
    if (curr->isLeaf)
    {
        return nullptr;
    }

    // The current candidate for the parent (initially the root)
    TreeNode *parentDiskAddress = rootDiskAddress;

    while (!curr->isLeaf)
    {
        for (int i = 0; i < curr->numOfKey + 1; i++)
        {
            // If the current candidate node has a pointer, pointing to the target node, it is the immediate parent of the target node
            if (curr->pointer[i].blockAddress == targetDiskAddress)
            {
                return parentDiskAddress;
            }
        }

        // Else we must go down one level, this loop helps us pick the current pointer to follow based on the key.
        // This is in a similar to the search function
        for (int i = 0; i < curr->numOfKey; i++)
        {
            if (lower < curr->dataKey[i])
            {
                // Update the candidate parent
                parentDiskAddress = (TreeNode *)curr->pointer[i].blockAddress;

                // Load the node into the main memory
                TreeNode *MainMemoryNode = (TreeNode *)index->readFromDisk(curr->pointer[i], nodeSize);

                // Updating the pointer accordingly
                curr = (TreeNode *)MainMemoryNode;

                break;
            }

            // This is the case where we should follow the rightmost pointer in the node
            if (i == curr->numOfKey - 1)
            {
                // Update the candidate parent
                parentDiskAddress = (TreeNode *)curr->pointer[i + 1].blockAddress;

                // Load the node into the main memory
                TreeNode *MainMemoryNode = (TreeNode *)index->readFromDisk(curr->pointer[i + 1], nodeSize);

                // Updating the pointer accordingly
                curr = (TreeNode *)MainMemoryNode;
                break;
            }
        }
    }

    return nullptr;
}

// Helper function that returns the height of the B+ Tree Index
int BPTree::getHeight()
{
    // Edge case when tree has no nodes, hence 0 height
    if (addressOfRoot == nullptr)
    {
        return 0;
    }

    // Load the root node from disk into main memory
    Address rootDiskAddress{addressOfRoot, 0};
    root = (TreeNode *)index->readFromDisk(rootDiskAddress, nodeSize);
    TreeNode *curr = root;

    // Initialised to 1 (root)
    numOfLevel = 1;

    // Traversing to the leaves by always following the leftmost pointer
    while (!curr->isLeaf)
    {
        curr = (TreeNode *)index->readFromDisk(curr->pointer[0], nodeSize);
        numOfLevel++;
    }

    // Don't forget to count the leaf node as a level
    return ++numOfLevel;
}