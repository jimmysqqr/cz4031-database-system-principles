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
    if (addressOfRoot == nullptr)
    {
        throw logic_error("Tree is empty!");
    }

    // Else we shall traverse the tree by following the appropriate pointers till we reach a leaf node
    else
    {
        // Load the root node from disk into main memory
        Address rootDiskAddress{addressOfRoot, 0};
        root = (TreeNode *)index->readFromDisk(rootDiskAddress, nodeSize);

        // Display the root (it was accessed in the search process)
        cout << "Root accessed. Contents.....";
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
                    curr = (TreeNode *)index->readFromDisk(curr->pointer[i], nodeSize);

                    // Display the current node (it was accessed in the search process)
                    cout << "Non-Leaf node accessed. Contents.....";
                    displayNode(curr);

                    break;
                }

                // If lower is larger than all keys, we follow right pointer (rightmost pointer actually!).
                if (i == curr->numOfKey - 1)
                {
                    // Load node from disk into main memory and update curr accordingly
                    curr = (TreeNode *)index->readFromDisk(curr->pointer[i + 1], nodeSize); // Notice i+1 here

                    // Display the current node (it was accessed in the search process)
                    cout << "Non-Leaf node accessed. Contents.....";
                    displayNode(curr);

                    break;
                }
            }
        }

        // We have now hit the leaf node with the first key that is withing the range.
        // We shall now traverse the linked list and display the records with their key within the range.

        // Flag indicating whether we're within range (current key < upper)
        bool flag = true;

        while (flag)
        {
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
                    // Display the current node (it was accessed in the search process)
                    cout << "Leaf node accessed. Contents.....";
                    displayNode(curr);

                    cout << "\nRecords for key: " << curr->dataKey[i] << " -> ";

                    // Access the linked list node and print all corresponding records
                    displayList(curr->pointer[i]);
                }
            }

            // Only continue iterating if the leaf node is not the last one (meaning the last pointer of the node points to another leaf node and isn't null)
            // Also check if the key is still within hte range
            if (curr->pointer[curr->numOfKey].blockAddress != nullptr && curr->dataKey[curr->numOfKey] <= upper)
            {
                // Load node from disk into main memory and update curr accordingly
                curr = (TreeNode *)index->readFromDisk(curr->pointer[curr->numOfKey], nodeSize);

                // Display the current node (it was accessed in the search process)
                cout << "Leaf node accessed. Contents.....";
                displayNode(curr);
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
