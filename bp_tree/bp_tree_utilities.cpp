#include "bp_tree.h"
#include "helper_types.h"

#include <vector>
#include <cstring>
#include <iostream>

using namespace std;

// Helper function to return the disk address of the parent of the given node
TreeNode *BPTree::FindParent(TreeNode *PointerDiskAddress, TreeNode *ChildDiskAddress, float LowBoundKey)
{
    // Load the pointer into main memory and initialise it to the root
    Address PointerDiskAddress{PointerDiskAddress, 0};
    TreeNode *point = (TreeNode *)index->readFromDisk(PointerDiskAddress, nodeSize);

    // Sanity Check, if the root is a leaf node, there would not be any children and consequently no parent
    if (point->isLeaf)
    {
        return nullptr;
    }

    // The current candidate for the parent (initially the root)
    TreeNode *ParentDiskAddress = PointerDiskAddress;

    while (point->isLeaf == false)
    {
        for (int i = 0; i < pointer->numOfKey + 1; i++)
        {
            // If the current candidate node has a pointer, pointing to the target nnode, it the parent node
            if (point->pointer[i].blockAddress == ChildDiskAddress)
            {
                return ParentDiskAddress;
            }
        }

        // Else we must go down one level, this loop helps us pick the current pointer to follow based on the key.
        // This is in a similar to the search function
        for (int i = 0; i < point->numOfKey; i++)
        {
            if (LowBoundKey < point->key[i])
            {
                // Update the candidate parent
                ParentDiskAddress = (TreeNode *)point->pointer[i].blockAddress;

                // Load the node into the main memory
                TreeNode *MainMemoryNode = (TreeNode *)index->readFromDisk(point->pointer[i], nodeSize);

                // Updating the pointer accordingly
                point = (TreeNode *)MainMemoryNode;

                break;
            }

            // This is the case where we should follow the rightmost pointer in the node
            if (i == point->numOfKey - 1)
            {
                // Update the candidate parent
                ParentDiskAddress = (TreeNode *)point->pointer[i + 1].blockAddress;

                // Load the node into the main memory
                TreeNode *MainMemoryNode = (TreeNode *)index->readFromDisk(point->pointer[i + 1], nodeSize);

                // Updating the pointer accordingly
                point = (TreeNode *)MainMemoryNode;
                break;
            }
        }
    }

    return nullptr;
}
