#include "bp_tree.h"
#include "../storage/helper_types.h"

#include <vector>
#include <iostream>

using namespace std;

// Helper function to return the disk address of the immediate parent of the target node. It leverages a key present in the target node to prune the search.
TreeNode *BPTree::getParent(TreeNode *targetNode, int key)
{
    // Sanity Check, if the target is the root. It means the tree has only 1 level and hence no 'parent'.
    if (targetNode == root)
    {
        return nullptr;
    }

    // Else let us initialize the parent at the root, we shall now check and update it.
    TreeNode *parent = root;

    while (!parent->isLeaf)
    {
        for (int i = 0; i < parent->numOfKey + 1; i++)
        {
            // If the current candidate node has a pointer, pointing to the target node, it is the immediate parent of the target node
            if ((TreeNode *)parent->pointer[i] == targetNode)
            {
                return parent;
            }
        }

        // Else we must go down one level, this loop helps us pick the current pointer to follow based on the key.
        // This is similar to the search function
        for (int i = 0; i < parent->numOfKey; i++)
        {
            if (key < parent->dataKey[i])
            {
                // Updating the pointer accordingly
                parent = (TreeNode *)parent->pointer[i];
                break;
            }

            // This is the case where we should follow the rightmost pointer in the node
            if (i == parent->numOfKey - 1)
            {
                // Updating the pointer accordingly
                parent = (TreeNode *)parent->pointer[i + 1];
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
    if (root == nullptr)
    {
        return 0;
    }

    TreeNode *currentNode = root;

    // Initialised to 1 (root)
    numOfLevel = 1;

    // Traversing to the leaves by always following the leftmost pointer
    while (!currentNode->isLeaf)
    {
        currentNode = (TreeNode *)currentNode->pointer[0];
        numOfLevel++;
    }

    // Don't forget to count the leaf node as a level
    return numOfLevel;
}
