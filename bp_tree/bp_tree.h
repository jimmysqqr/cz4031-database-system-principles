ifndef BP_TREE_H
#define BP_TREE_H

#include "helper_types.h"
#include "disk_storage.h"
#include <cstddef>
#include <array>

class TreeNode
{
    private:
        Address *pointer;
        float *key;
        int numOfKey;
        bool isLeaf;
        friend class BPTree;

    public:
        TreeNode(int maxDataKey);
};

class BPTree
{
    private:
        DiskStorage *disk;
        DiskStorage *index;
        TreeNode *root;
        void *rootAddress
        int maxKey;
        int level;
        int numOfNode;
        std::size nodeSize;

    public:
        BPTree(std::size blockSize, DiskStorage *disk, DiskStorage *index);


}

#endif