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
            if (LowBoundKey < point->key[i])
            {
                Node *MainMemoryNode = (TreeNode *)index->LoadFromDisk(point->pointer[i], nodeSize);

                ParentDiskAddress = (TreeNode *)point->pointer[i].blockAddress;

                point = (TreeNode *)MainMemoryNode;
                break;
            }

            if (i == point->numOfKey - 1)
            {
           
                TreeNode *MainMemoryNode = (TreeNode *)index->LoadFromDisk(point->pointer[i +1], nodeSize);
                
                ParentDiskAddress = (TreeNode *)point->pointer[i + 1].blockAddress;

                point = (TreeNode *)MainMemoryNode;
                break;
            }
        }
    }

  return nullptr;

}
