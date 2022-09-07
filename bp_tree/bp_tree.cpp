#include "bp_tree.h"
#include "disk_storage.h"
#include "helper_types.h"

#include <tuple>
#include <iostream>
#include <array>
#include <unordered_map>
#include <cstring>

using namespace std;

bool nullPointer = false;

TreeNode::TreeNode(int maxDataKey)
{
    key = new float[maxDataKey];
    pointer = new Address[maxDataKey + 1];

    for(int i = 0; i < maxDataKey + 1; i++)
    {
        Address nullAddress{(void *)nullPOinter, 0};
        pointer = [i] = nullAddress;
    }
    numOfkey = 0;
}

BPTree::BPTree(std::size blockSize, DiskStorage *disk, DiskStorage *index)
{
    size NodeBufferSize = blockSize - sizeof(bool) - sizeof(int);

    size sum = sizeof(Address);
    maxKey = 0;

    while(sum + sizeof(Address) + sizeof(float) <= NodeBufferSize)
    {
        sum += (sizeof(Address) + sizeof(float));
        maxKey += 1;
    }

    if(maxKey == 0)
    {
        throw std::overflow_error("ERROR! Number of keys and pointers are too large!");
    }

    RootAddress = nullptr;
    root = nullptr;

    nodeSize = blockSize;

    level = 0;
    numOfNode = 0;

    this->disk = disk;
    this->index = index;
}
