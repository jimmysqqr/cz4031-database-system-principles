#include "bp_tree.h"
#include "../storage/helper_types.h"

#include <iostream>

using namespace std;

// We shall implement a function for range queries only. The function can also search a specific key by setting lower = upper = key.
// In brief the function works by traversing the B+ tree till it finds the first leaf node with a key <= lower.
// Then it traverse the leaves via the linked list as long as the keys are <= upper.
void BPTree::search(int lower, int upper)
{
    // Edge Case, tree is empty
    if (root == nullptr)
    {
        throw logic_error("Tree is empty!");
    }

    // Else we shall traverse the tree by following the appropriate pointers till we reach a leaf node
    else
    {
        // Display the root (it was accessed in the search process)
        cout << ++numOfNodesAccessed <<  " - Root accessed. Contents..." << endl;
        displayNode(root);
        
        // Pointer to current node
        TreeNode *curr = root;

        while (!curr->isLeaf)
        {
            // Iterate through the keys in the current node. Once we find the appropriate key, we shall load the node, it's pointer points to, into memory
            for (int i = 0; i < curr->numOfKey; i++)
            {
                // If lower is smaller than the current key, we follow the left pointer.
                if (lower < curr->dataKey[i])
                {   
                    // Load node from disk into main memory and update curr accordingly
                    curr = (TreeNode *)curr->pointer[i];

                    // Display the current node (it was accessed in the search process)
                    numOfNodesAccessed++;
                    if(numOfNodesAccessed <= 5){
                        if(!curr->isLeaf){
                            cout << numOfNodesAccessed << " - Non-Leaf node accessed. Contents..." << endl;
                            displayNode(curr);
                            break;
                        }
                        else {
                            cout << numOfNodesAccessed << " - Leaf node accessed. Contents..." << endl;
                            displayNode(curr);
                            break;
                        }
                    }
                }

                // If lower is larger than all keys, we follow right pointer (rightmost pointer actually!).
                if (i == curr->numOfKey - 1)
                {
                    // Load node from disk into main memory and update curr accordingly
                    curr = (TreeNode *)curr->pointer[i + 1];

                    // Display the current node (it was accessed in the search process)
                    numOfNodesAccessed++;
                    if(numOfNodesAccessed <= 5){
                        if(!curr->isLeaf){
                            cout << numOfNodesAccessed << " - Non-Leaf node accessed. Contents..." << endl;
                            displayNode(curr);
                            break;
                        }
                        else {
                            cout << numOfNodesAccessed << " - Leaf node accessed. Contents..." << endl;
                            displayNode(curr);
                            break;
                        }
                    }
                }
            }
        }

        // We have now hit the leaf node with the first key that is withing the range.
        // We shall now traverse the linked list of TreeNodes (lowest level of B+ tree) and display the records with their key within the range.

        // Flag indicating whether we're within range (current key < upper)
        bool flag = true;
        // numOfNodesAccessed++; // Cuz no matter what 1 leaf node will be accessed

        // Reset the variables that help us return the average 'averageRating'
        sumOfAverageRating = 0.0;
        numOfRecordsRetrieved = 0;

        // for expt 4
        int numOfLeafNodesAccessed = 0;

        while (flag)
        {
            // expt 4
            if (numOfLeafNodesAccessed == 5) break;
            
            for (int i = 0; i < curr->numOfKey; i++)
            {
                // If we've exceeded the range, switch the flag
                if (curr->dataKey[i] > upper)
                {
                    flag = false;
                    break;
                }

                // Else only display the records if the key is within the range
                if (lower <= curr->dataKey[i] && curr->dataKey[i] <= upper)
                {
                    cout << "Records with key (numVotes) = " << curr->dataKey[i] << ": ";

                    // Access the linked list node and print all corresponding records
                    displayList((ListNode *)curr->pointer[i]);
                    numOfLeafNodesAccessed++;
                }
            }

            // Only continue iterating if the leaf node is not the last one (meaning the last pointer of the node points to another leaf node and isn't null)
            // Also check if the key is still within the range
            if (curr->pointer[curr->numOfKey] != nullptr && curr->dataKey[curr->numOfKey - 1] <= upper)
            {
                // Load node from disk into main memory and update curr accordingly
                curr = (TreeNode *)curr->pointer[curr->numOfKey];

                // Display the current node (it was accessed in the search process)
                cout << "\n" << ++numOfNodesAccessed << " - Moving to next leaf node" << endl;
            }

            // Else switch the flag and the loop exits
            else
            {
                flag = false;
            }
        }
    }
    return;
}
