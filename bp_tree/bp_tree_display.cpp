#include "bp_tree.h"
#include "../storage/helper_types.h"

#include <iostream>
#include <cstring>

using namespace std;

// Function that prints the specified node of the B+ Tree
void BPTree::displayNode(TreeNode *curr)
{
    // First let's print all filled keys and pointers
    cout << "[";
    for (int i = 0; i < curr->numOfKey; i++)
    {
        //cout << &curr->pointer[i] << "| ";
        cout << "#| ";
        cout << curr->dataKey[i] << " |";
    }

    // For the last filled pointer we must be wary of it being Null
    void *lastPointer = (void *)&curr->pointer[curr->numOfKey];
    if (lastPointer == nullptr)
    {
        cout << "null";
    }
    else
    {
        // cout << lastPointer;
        cout << "#";
    }

    // Printing remaining empty keys/pointers (if any)
    for (int i = curr->numOfKey; i < maxDataKey; i++)
    {
        cout << "| empty ";
        cout << "|null";
    }

    // Formatting
    cout << "]\n" << endl;

    return;
}

// Function that prints the entire linked list (pointed by the leaves)
// It also computes and displays the average of “averageRating’s” of the records that are returned
void BPTree::displayList(ListNode *head)
{
    // Create a temp variable to traverse the linked list
    ListNode *temp = head;
    
    int numDisplayed = 0;

    // cout << "Traversing the linked list..." << endl;

    // Computing the average of “averageRating’s” of the records that are returned
    while (temp != nullptr)
    {
        // Use the adress to read the record from the disk
        Record *record = disk->readFromDisk(temp->recordAddress, sizeof(Record));

        // Only displaying 5 records
        if(numDisplayed < 5){
            // Print the contents of the record
            cout << "[" << record->tconst << "|" << record->averageRating << "|" << record->numVotes << "]-->";
            numDisplayed++;
        }
        
        // Increment the sum of averageRating and numofRecordsRetrieved
        sumOfAverageRating += record->averageRating;
        numOfRecordsRetrieved += 1;

        // Increment temp
        temp = temp->next;
    }

    // Formatting
    cout << endl;
    return;
}

// Debug function to print out all leaf nodes
void BPTree::displayAllLeafNodes() {
    // Edge Case, tree is empty
    if (root == nullptr) {
        throw logic_error("Tree is empty!");
        return;
    }

    // Else we shall traverse the tree by always following the first pointers till we reach the start of the chain of leaf nodes
        
    // Pointer to current node
    TreeNode *curr = root;
    TreeNode *prev = curr;

    while (!curr->isLeaf) {
        curr = (TreeNode *)curr->pointer[0];
    }

    int numLeaf = 0;
    // now curr is a leaf node
    while (curr->pointer[curr->numOfKey] != nullptr) {
        cout << "Leaf node " << numLeaf << endl;
        displayNode(curr);
        prev = curr;
        curr = (TreeNode *)curr->pointer[curr->numOfKey];
        numLeaf++;
    }

    // print parents for last 2 leaf nodes as they have some kind of ordering problem
    cout << "Parent node of leaf node " << numLeaf - 2 << endl;
    TreeNode *parent1 = getParent(prev, prev->dataKey[0]);
    displayNode(parent1);

    cout << "Parent node of leaf node " << numLeaf - 1 << endl;
    // cout << "hi" << endl;
    TreeNode *parent2 = getParent(curr, curr->dataKey[0]);
    // cout << "hey" << endl;
    displayNode(parent2);
}
