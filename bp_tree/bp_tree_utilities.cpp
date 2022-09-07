#include "bp_tree.h"
#include "helper_types.h"
#include <vector>
#include <cstring>
#include <iostream>

using namespace std;

TreeNode *BPTree::FindParent(TreeNode *PointerDiskAddress, TreeNode *ChildDiskAddress, float LowBoundKay)
{
    Address PointerDiskAddress{PointerDiskAddress, 0};
    TreeNode *point = (TreeNode *)index -> LoadFromDisk(PointerAddress, nodeSize);

    if(point -> isLeaf)
    {
        return nullptr;
    }

    TreeNode *ParentDiskAddress = PointerDiskAddress;

    while(point->isLeaf == false)
    {
        for (int i = 0; i < pointer->numOfKey + 1; i++)
        {

            if (point->pointer[i].blockAddress == ChildDiskAddress)
            {
                return ParentDiskAddress;
            }
        }


        for (int i = 0; i < point->numOfKey; i++)
        {
        // If key is lesser than current key, go to the left pointer's node.
        if (LowBoundKey < point->key[i])
        {
            // Load node in from disk to main memory.
            Node *MainMemoryNode = (TreeNode *)index->LoadFromDisk(point->pointer[i], nodeSize);

            // Update parent address.
            ParentDiskAddress = (TreeNode *)point->pointer[i].blockAddress;

            // Move to new node in main memory.
            point = (TreeNode *)MainMemoryNode;
            break;
        }

        // Else if key larger than all keys in the node, go to last pointer's node (rightmost).
        if (i == point->numOfKey - 1)
        {
            // Load node in from disk to main memory.
            TreeNode *MainMemoryNode = (TreeNode *)index->LoadFromDisk(point->pointer[i +1], nodeSize);

            // Update parent address.
            ParentDiskAddress = (TreeNode *)point->pointer[i + 1].blockAddress;

            // Move to new node in main memory.
            point = (TreeNode *)MainMemoryNode;
            break;
        }
        }
    }

  return nullptr;

}