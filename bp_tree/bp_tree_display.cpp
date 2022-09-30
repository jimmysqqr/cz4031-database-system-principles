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
        // Previously it was curr->pointer[i].blockAddress 
        cout << &curr->pointer[i] << " | ";
        cout << curr->dataKey[i] << " | ";
    }

    // For the last filled pointer we must be wary of it being Null
    void *lastPointer = (void *)&curr->pointer[curr->numOfKey];
    if (lastPointer == nullptr)
    {
        cout << " NULL ";
    }
    else
    {
        cout << lastPointer;
    }

    // Printing remaining empty keys/pointers (if any)
    for (int i = curr->numOfKey; i < maxDataKey; i++)
    {
        cout << "|\t";
        cout << "| NULL";
    }

    // Formatting
    cout << " ]" << endl;
    return;
}

// // Function that prints the specified data block and its content
// void BPTree::displayBlock(void *blockAddress)
// {
//     // Loading block into memory by creating a shallow copy of it
//     void *currBlock = operator new(nodeSize);
//     memcpy(currBlock, blockAddress, nodeSize);

//     unsigned char emptyBlock[nodeSize];
//     memset(emptyBlock, '\0', nodeSize);

//     // Case when current block is empty (note the use of memcmp here)
//     if (memcmp(emptyBlock, currBlock, nodeSize) == 0)
//     {
//         cout << "Block is Empty!" << endl;
//         return;
//     }

//     // Iniitializing the loop variable and block pointer
//     size_t i = 0;
//     unsigned char *blockPtr = (unsigned char *)currBlock;

//     while (i < nodeSize)
//     {
//         // Loading each record into memory by creating a shallow copy of it
//         void *recordAddress = operator new(sizeof(Record));
//         memcpy(recordAddress, blockPtr, sizeof(Record));
//         Record *record = (Record *)recordAddress;

//         // Formatting the output and incrementing the pointer and loop variable by the record size
//         cout << "[ " << record->tconst << " | " << record->averageRating << " | " << record->numVotes << " ]\t";
//         blockPtr += sizeof(Record);
//         i += sizeof(Record);
//     }
//     return;
// }

// Function that prints the entire linked list (pointed by the leaves)
// It also computes and displays the average of “averageRating’s” of the records that are returned
void BPTree::displayList(ListNode *curr)
{
    // Create a temp variable to traverse the linked list
    ListNode *temp = curr;

    // Computing the average of “averageRating’s” of the records that are returned
    while(temp!=NULL)
    {   
        // Get the address of the record
        Address recordAdd = *temp->recordAddress;
        
        // Use the adress to read the record from the disk
        Record record = *(Record *)disk->readFromDisk(recordAdd, sizeof(Record));

        cout << "\nReading record from Disk. Contents....";

        // Print the contents of the record
        cout << "[ " << record.tconst << " | " << record.averageRating << " | " << record.numVotes << " ]\t";

        // Increment the sum of averageRating and numofRecordsRetrieved
        sumOfAverageRating += record.averageRating;
        numOfRecordsRetrieved += 1;

        // Increment temp
        temp = temp->next;
    };

    return;
}