#include <cstring>
#include <iostream>

#include "bp_tree.h"
#include "helper_types.h"

using namespace std;

// Function to delete records of a certain key, returns no. of tree nodes deleted
int BPTree::remove(float key) {
    // tree is empty
    if (addressOfRoot == nullptr) {
        // throw std::logic_error("Deletion failed: Tree is empty.");
        cout << "Deletion failed: Tree is empty." << endl;
        return 0;
    }

    // load root node from disk into main memory
    Address addressOfRootDisk{addressOfRoot, 0};
    root = (TreeNode *)index->readFromDisk(addressOfRootDisk, nodeSize);

    // initalize current node for traversing
    TreeNode *currentNode = root;

    // keep track of parent node in case of updates
    TreeNode *parentNode;

    // store current node's disk address in case of updates
    void *currentDiskAddress = addressOfRoot;

    // keep track of parent's disk address in case of updates
    void *parentDiskAddress = addressOfRoot;

    // keep track of index of left and right sibling to borrow a key
    // they will be index of the parentNode (parent of currentNode)
    int leftSibling, rightSibling;

    // keep track of number of deleted nodes
    int numDeletedNodes = 0;


    // find the node - while not leaf node, traverse the tree
    while (!currentNode->isLeaf) {
        // traverse the keys
        int numNodeKeys = currentNode->numOfKey;

        // set parent node and disk address in case of updates
        parentNode = currentNode;
        parentDiskAddress = currentDiskAddress;

        for (int i = 0; i < numNodeKeys; i++) {

            float currentKey = currentNode->dataKey[i];

            // update index of left and right sibling
            leftSibling = i - 1;
            rightSibling = i + 1;

            // if key is smaller than current key, then follow its (left)
            if (key < currentKey) {
                // load current node from disk into main mem
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

                // update index of left and right sibling
                leftSibling = i;
                rightSibling = i + 2; // or use -1 bcos no right sibling //TODO

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
    bool FLAG_keyFound = false;

    for (keyIdx = 0; keyIdx < currentNode->numOfKey; keyIdx++) {
        if (currentNode->dataKey[keyIdx] == key) {
            FLAG_keyFound = true;
            break;
        }
    }

    // key to remove does not exist, no key removed
    if (!FLAG_keyFound) {
        // throw std::logic_error("Deletion failed: Tree is empty.");
        cout << "Deletion failed: Key " << key << " does not exist." << endl;
        return 0;
    }

    // we have found the key to remove
    // now remove the entire linked list pointed by this key

    // get pointer to first record from disk
    Address *LLheadAddress = currentNode->pointer[keyIdx];
    TreeNode *LLhead = (TreeNode *)index->readFromDisk(LLheadAddress, nodeSize);

    // deallocate current record
    index->deallocateRecord(LLheadAddress, nodeSize);

    // keep accessing the next node to deallocate it
    while (LLhead->pointer[LLhead->numOfKey].blockAddress != nullptr)
    {
        LLheadAddress = LLhead->pointer[LLhead->numOfKey];
        LLhead = (TreeNode *)index->readFromDisk(LLheadAddress, nodeSize);
        index->deallocateRecord(LLheadAddress, nodeSize);
    }

    // we have deleted the records pointed by the key
    // now delete the key in the leaf node by moving all keys & pointers forward

    // (currentNode->numOfKey-1) has "-1" in order to not index out of range
    for (int i = keyIdx; i < currentNode->numOfKey-1; i++) {
        currentNode->dataKey[i] = currentNode->dataKey[i+1];
        currentNode->pointer[i] = currentNode->pointer[i+1];
    }
    // move rightmost pointer
    currentNode->pointer[currentNode->numOfKey-1] = currentNode->pointer[currentNode->numOfKey];

    // decrement number of keys in the node
    currentNode->numOfKey--;

    // TODO: check whether we need to set every pointer from the last key, bcos every pointer after this one should alr be NULL
    // remove the pointer near the last key, +1 to access the previously occupied pointer
    Address nullAddr{nullptr, 0};
    currentNode->pointer[currentNode->numOfKey+1] = nullAddr;

    // we have deleted the key from the leaf node
    // now re-organise the B+ tree by following 2 rules

    // rule 1: each leaf node (except root) must have >= floor((n+1)/2) keys
    // rule 2: each internal node (except root) must have >= floor(n/2) keys

    // there are 4 cases to handle

    // (1) leaf node is also the root, if so, will NOT violate any rules
    if (currentNode == root) {

        cout << "Leaf node is the root of the tree." << endl;

        // check whether the root node is empty
        if (root->numOfKey == 0) {
            // deallocate root node
            index->deallocateRecord(addressOfRootDisk, nodeSize);

            // set root pointers to NULL
            root = nullptr;
            addressOfRoot = nullptr;
            
            // update stats
            numOfNode--;
            numOflevel--;
            cout << "Key " << key << " deleted. Root node deleted. B+ tree index is now empty." << endl;

            return 1;
        }

        // no stats to update

        // write updated node to disk
        Address currentNodeAddress = {currentDiskAddress, 0};
        index->writeToDisk(currentNode, nodeSize, currentNodeAddress);

        cout << "Key " << key << " in root node deleted." << endl;

        return 0;
    }
    
    // (2) leaf node, no rules violated
    // check if leaf node has minimum of floor((n+1)/2) keys
    if (currentNode->numOfKey >= floor((totalDataKey + 1) / 2)) {
        // write updated node to disk
        Address currentNodeAddress = {currentDiskAddress, 0};
        index->writeToDisk(currentNode, nodeSize, currentNodeAddress);

        // no stats to update

        cout << "Key " << key << " in leaf node deleted." << endl;

        return 0;
    }

    // (3) will violate rules, CAN borrow a key from sibling node
    // check left and right sibling to borrow a key

    // check if left sibling exists
    if (leftSibling >= 0) {
        // load left sibling from disk into main mem
        TreeNode *leftNode = (TreeNode *)index->readFromDisk(parentNode->pointer[leftSibling], nodeSize);

        // check if borrowing a key from left sibling is legal
        if (leftNode->numOfKey >= floor((totalDataKey + 1) / 2)) {

            // insert rightmost key of leftNode into first position of currentNode

            // move currentNode last pointer backward
            currentNode->pointer[currentNode->numOfKey+1] = currentNode->pointer[currentNode->numOfKey];

            // move all other keys & pointers in currentNode backward
            for (int i = currentNode->numOfKey; i > 0; i--) {
                currentNode->dataKey[i] = currentNode->dataKey[i-1];
                currentNode->pointer[i] = currentNode->pointer[i-1];
            }

            // migrate borrowed key & pointer from leftNode into currentNode
            currentNode->dataKey[0] = leftNode->dataKey[leftNode->numOfKey-1];
            currentNode->pointer[0] = leftNode->pointer[leftNode->numOfKey-1];

            // update stats
            currentNode->numOfKey++;
            leftNode->numOfKey--;

            // move leftNode last pointer forward
            leftNode->pointer[leftNode->numOfKey] = leftNode->pointer[leftNode->numOfKey+1];

            // update parentNode key bcos first key of currentNode has changed
            parentNode->dataKey[leftSibling] = currentNode->dataKey[0];

            // write updated current, left, and parent nodes to disk
            Address currentNodeAddress = {currentDiskAddress, 0};
            Address parentNodeAddress = {parentDiskAddress, 0};
            index->writeToDisk(currentNode, nodeSize, currentNodeAddress);
            index->writeToDisk(leftNode, nodeSize, parentNode->pointer[leftSibling]);
            index->writeToDisk(parentNode, nodeSize, parentNodeAddress);

            cout << "Key " << key << " in leaf node deleted." << endl;
            cout << "Borrowed key " << currentNode->dataKey[0] << " from left node." << endl;

            return 0;
        } else {
            cout << "Left sibling of current node has no key to borrow from." << endl;
        }
    } else {
        cout << "Current node has no left sibling." << endl;
    }

    // at this point, left sibling does not exist OR does not have a key to borrow from
    
    // check if right sibling exists
    if (rightSibling <= parentNode->numOfKey && rightSibling >= 0) {
        // load right sibling from disk into main mem
        TreeNode *rightNode = (TreeNode *)index->readFromDisk(parentNode->pointer[rightSibling], nodeSize);

        // check if borrowing a key from right sibling is legal
        if (rightNode->numOfKey >= floor((totalDataKey + 1) / 2)) {

            // insert first key of rightNode into last position of currentNode

            // move currentNode last pointer backward
            currentNode->dataKey[currentNode->numOfKey+1] = currentNode->dataKey[currentNode->numOfKey];

            // migrate borrowed key & pointer from rightNode into currentNode
            currentNode->dataKey[currentNode->numOfKey] = rightNode->dataKey[0];
            currentNode->pointer[currentNode->numOfKey] = rightNode->pointer[0];

            // update stats
            currentNode->numOfKey++;
            rightNode->numOfKey--;

            // move rightNode last pointer forward
            rightNode->pointer[currentNode->numOfKey] = rightNode->pointer[currentNode->numOfKey+1];

            // move all other keys & pointers in rightNode backward
            for (int i = 0; i < rightNode->numOfKey; i++) {
                rightNode->dataKey[i] = rightNode->dataKey[i+1];
                rightNode->pointer[i] = rightNode->pointer[i+1];
            }

            // write updated current, right, and parent nodes to disk
            Address currentNodeAddress = {currentDiskAddress, 0};
            Address parentNodeAddress = {parentDiskAddress, 0};
            index->writeToDisk(currentNode, nodeSize, currentNodeAddress);
            index->writeToDisk(rightNode, nodeSize, parentNode->pointer[rightSibling]);
            index->writeToDisk(parentNode, nodeSize, parentNodeAddress);

            cout << "Key " << key << " in leaf node deleted." << endl;
            cout << "Borrowed key " << currentNode->dataKey[currentNode->numOfKey-1] << " from right node." << endl;

            return 0;
        } else {
            cout << "Right sibling of current node has no key to borrow from." << endl;
        }
    } else {
        cout << "Current node has no right sibling." << endl;
    }

    // (4) will violate rules, CANNOT borrow a key from sibling node
    // at this point, no siblings exist OR no key can be borrowed from both siblings

    // check if right sibling exists
    if (leftSibling >= 0) {
        
    }



}