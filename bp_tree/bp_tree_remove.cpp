#include <cstring>
#include <iostream>

#include "bp_tree.h"
#include "helper_types.h"

using namespace std;

// Function to delete records of a certain key
int BPTree::remove(float key) {
    // tree is empty
    if (addressOfRoot == nullptr) {
        // throw std::logic_error("Deletion failed: Tree is empty.");
        cout << "Deletion failed: Tree is empty." << endl;
        return 0;
    }

    // load root node from disk into main memory
    Address rootDiskAddress{addressOfRoot, 0};
    root = (TreeNode *)index->readFromDisk(rootDiskAddress, nodeSize);

    // initalize current node for traversing
    TreeNode *currentNode = root;

    // keep track of parent node in case of updates
    TreeNode *parentNode;

    // store current node's disk address in case of updates
    void *currentDiskAddress = addressOfRoot;

    // keep track of parent's disk address in case of updates
    void *parentDiskAddress = addressOfRoot;

    // keep track of number of deleted nodes
    int numDeletedNodes = 0;

    // find the node - while not leaf node, traverse the tree
    while (!currentNode->isLeaf) {
        // traverse the keys
        int numNodeKeys = currentNode->numOfKey;

        for (int i = 0; i < numNodeKeys; i++) {
            float currentKey = currentNode->dataKey[i];

            // if key is smaller than current key, then follow its (left)
            if (key < currentKey) {
                // load tree node from disk into main mem
                TreeNode *tempNode = (TreeNode *)index->readFromDisk(
                    currentNode->pointer[i], nodeSize);

                // store current node's disk address in case of updates
                currentDiskAddress = currentNode->pointer[i].blockAddress;

                // traverse to node at next level
                currentNode = tempNode;
                break;
            }

            // if key is bigger than all keys, then follow the rightmost pointer
            if (i == numNodeKeys - 1) {
                // load tree node from disk into main mem
                TreeNode *tempNode = (TreeNode *)index->readFromDisk(
                    currentNode->pointer[i + 1], nodeSize);

                // store current node's disk address in case of updates
                currentDiskAddress = currentNode->pointer[i + 1].blockAddress;

                // traverse to node at next level
                currentNode = tempNode;
                break;
            }
        }
    }

    // currentNode is a leaf node
    // now find the key in this leaf node

    int keyIdx;
    bool keyFound = false;

    for (keyIdx = 0; i < currentNode->numOfKey; i++) {
        if (currentNode->dataKey[i] == key) {
            keyFound = true;
            break;
        }
    }

    // key to remove does not exist, no key removed
    if (!keyFound) {
        // throw std::logic_error("Deletion failed: Tree is empty.");
        cout << "Deletion failed: Key " << key << " does not exist." << endl;
        return 0;
    }

    // we have found the key to remove
    // now remove the entire linked list pointed by this key

    // now remove the key in the leaf node

    // rule 1: each leaf node (except root) must have >= floor((n+1)/2) keys
    // rule 2: each internal node (except root) must have >= floor(n/2) keys

    // there are 3 cases to handle

    // (1) no rules violated

    // (2) will violate rules, CAN borrow a key from sibling node

    // (3) will violate rules, CANNOT borrow a key from sibling node
}