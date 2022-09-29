#include "bp_tree.h"
#include "../storage/helper_types.h"

#include <iostream>
#include <cstring>

using namespace std;

// Function that prints the specified node of the B+ Tree
void BPTree::displayNode(TreeNode *curr)
{
    // First let's print all filled keys and pointers
    cout << "[ ";
    for (int i = 0; i < curr->numOfKey; i++)
    {
        cout << curr->pointer[i].blockAddress << " | ";
        cout << curr->dataKey[i] << " | ";
    }

    // For the last filled pointer we must be weary of it being Null
    void *lastPointer = curr->pointer[curr->numOfKey].blockAddress;
    if (lastPointer == nullptr)
    {
        cout << " NULL ";
    }
    else
    {
        cout << lastPointer;
    }

    // Printing remaining empty keys/pointers (if any)
    for (int i = curr->numOfKey; i < totalDataKey; i++)
    {
        cout << "|\t";
        cout << "| NULL";
    }

    // Formatting
    cout << " ]" << endl;
    return;
}

// Function that prints the specified data block and its content
void BPTree::displayBlock(void *blockAddress)
{
    // Loading block into memory by creating a shallow copy of it
    void *currBlock = operator new(nodeSize);
    memcpy(currBlock, blockAddress, nodeSize);

    unsigned char emptyBlock[nodeSize];
    memset(emptyBlock, '\0', nodeSize);

    // Case when current block is empty (note the use of memcmp here)
    if (memcmp(emptyBlock, currBlock, nodeSize) == 0)
    {
        cout << "Block is Empty!" << endl;
        return;
    }

    // Iniitializing the loop variable and block pointer
    size_t i = 0;
    unsigned char *blockPtr = (unsigned char *)currBlock;

    while (i < nodeSize)
    {
        // Loading each record into memory by creating a shallow copy of it
        void *recordAddress = operator new(sizeof(Record));
        memcpy(recordAddress, blockPtr, sizeof(Record));
        Record *record = (Record *)recordAddress;

        // Formatting the output and incrementing the pointer and loop variable by the record size
        cout << "[ " << record->tconst << " | " << record->averageRating << " | " << record->numVotes << " ]\t";
        blockPtr += sizeof(Record);
        i += sizeof(Record);
    }
    return;
}

// Function that prints the entire linked list (pointed by the leaves)
// It also computes and displays the average of “averageRating’s” of the records that are returned
void BPTree::displayList(Address headAddress)
{
    // Load the head of the linked list into main memory.
    TreeNode *head = (TreeNode *)index->readFromDisk(headAddress, nodeSize);

    // Computing the average of “averageRating’s” of the records that are returned
    for (int i = 0; i < head->numOfKey; i++)
    {

        cout << "\nData block accessed. Contents....";
        displayBlock(head->pointer[i].blockAddress);
        cout << endl;

        Record record = *(Record *)(disk->readFromDisk(head->pointer[i], sizeof(Record)));
        sumOfAverageRating += record.averageRating;
        numOfRecordsRetrieved += 1;
    }

    // If we reach the end of the linked list return
    if (head->pointer[head->numOfKey].blockAddress == nullptr)
    {
        cout << "End of linked list" << endl;
        return;
    }

    // Else recursively print the next node
    else
    {
        displayList(head->pointer[head->numOfKey]);
    }

    return;
}