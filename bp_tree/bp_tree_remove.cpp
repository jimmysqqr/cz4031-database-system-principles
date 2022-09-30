#include <cstring>
#include <iostream>
#include <cmath>

#include "bp_tree.h"
#include "../storage/helper_types.h"

using namespace std;

// Function to delete records of a certain key
// Returns no. of tree nodes deleted
int BPTree::remove(int key) {
    // tree is empty
    if (root == nullptr) {
        // throw std::logic_error("Deletion failed: Tree is empty.");
        cout << "Deletion failed: Tree is empty." << endl;
        return 0;
    }

    // load root node from disk into main memory
    // Address addressOfRootDisk{addressOfRoot, 0};
    // root = (TreeNode *)index->readFromDisk(addressOfRootDisk, nodeSize);

    // initalize current node for traversing
    TreeNode *currentNode = root;

    // keep track of parent node in case of updates
    TreeNode *parentNode;

    // keep track of current node's disk address in case of updates
    // void *currentDiskAddress = addressOfRoot;

    // keep track of parent's disk address in case of updates
    // void *parentDiskAddress = addressOfRoot;

    // keep track of index of left and right sibling to borrow a key
    // they will be index of the parentNode (parent of currentNode)
    int leftSibling, rightSibling;

    // keep track of number of deleted nodes
    int numDeletedNodes = 0;


    // find the node - while not leaf node, traverse the tree
    while (!currentNode->isLeaf) {

        // traverse the keys

        // set parent node and disk address in case of updates
        parentNode = currentNode;
        // parentDiskAddress = currentDiskAddress;

        for (int i = 0; i < currentNode->numOfKey; i++) {

            int currentKey = currentNode->dataKey[i];

            // update index of left and right sibling
            leftSibling = i - 1;
            rightSibling = i + 1;

            // if key is smaller than current key, then follow its (left)
            if (key < currentKey) {
                // load current node from disk into main mem
                // TreeNode *tempNode = (TreeNode *)index->readFromDisk(currentNode->pointer[i], nodeSize);

                // store current node's disk address in case of updates
                // currentDiskAddress = currentNode->pointer[i].blockAddress;

                // traverse to node at next level
                currentNode = (TreeNode *)currentNode->pointer[i];
                break;
            }

            // if key is bigger than all keys, then follow the rightmost pointer
            if (i == currentNode->numOfKey - 1) {

                // update index of left and right sibling
                leftSibling = i;
                rightSibling = i + 2; // or use -1 bcos no right sibling //TODO

                // load tree node from disk into main mem
                // TreeNode *tempNode = (TreeNode *)index->readFromDisk(currentNode->pointer[i + 1], nodeSize);

                // store current node's disk address in case of updates
                // currentDiskAddress = currentNode->pointer[i + 1].blockAddress;

                // traverse to node at next level
                currentNode = (TreeNode *)currentNode->pointer[i + 1];
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
    // Address LLheadAddress = currentNode->pointer[keyIdx];
    // TreeNode *LLhead = (TreeNode *)index->readFromDisk(LLheadAddress, nodeSize);
    ListNode *headListNode = (ListNode *)currentNode->pointer[keyIdx];
    ListNode *tempListNode = headListNode;

    // deallocate current record
    // index->deallocateRecord(LLheadAddress, nodeSize);

    // keep accessing the next node to deallocate it
    // while (LLhead->pointer[LLhead->numOfKey].blockAddress != nullptr)
    // {
    //     LLheadAddress = LLhead->pointer[LLhead->numOfKey];
    //     LLhead = (TreeNode *)index->readFromDisk(LLheadAddress, nodeSize);
    //     index->deallocateRecord(LLheadAddress, nodeSize);
    // }

    // keep accessing the next ListNode to deallocate it
    while (headListNode->next != nullptr) {
        tempListNode = headListNode->next;
        free(headListNode);
        headListNode = tempListNode;
    }

    // deallocate the last ListNode that points to NULL
    free(headListNode);

    // we have deleted the linked list pointed by the key
    // now delete the key in the leaf node by moving all keys & pointers forward

    // (currentNode->numOfKey-1) has "-1" in order to not index out of range
    for (int i = keyIdx; i < currentNode->numOfKey - 1; i++) {
        currentNode->dataKey[i] = currentNode->dataKey[i+1];
        currentNode->pointer[i] = currentNode->pointer[i+1];
    }

    // move rightmost pointer
    currentNode->pointer[currentNode->numOfKey - 1] = currentNode->pointer[currentNode->numOfKey];

    // decrement number of keys in the node
    currentNode->numOfKey--;

    // TODO: check whether we need to set every pointer from the last key, bcos every pointer after this one should alr be NULL
    // remove the pointer near the last key, +1 to access the previously occupied pointer
    // Address nullAddr{nullptr, 0};
    currentNode->pointer[currentNode->numOfKey+1] = nullptr;

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
            // index->deallocateRecord(addressOfRootDisk, nodeSize);
            free(root);

            // set root pointers to NULL
            root = nullptr;
            // addressOfRoot = nullptr;
            
            // update stats
            numOfNode--;
            numOfLevel--;
            numDeletedNodes++;

            cout << "Key " << key << " deleted. Root node deleted. B+ tree index is now empty." << endl;

            return numDeletedNodes; // should be 1
        }

        // no stats to update

        // write updated node to disk
        // Address currentNodeAddress = {currentDiskAddress, 0};
        // index->writeToDisk(currentNode, nodeSize, currentNodeAddress);

        cout << "Key " << key << " in root node deleted." << endl;

        return numDeletedNodes; // should be 0
    }
    
    // (2) leaf node, no rules violated
    // check if leaf node has minimum of floor((n+1)/2) keys
    if (currentNode->numOfKey >= floor((maxDataKey + 1) / 2)) {
        // write updated node to disk
        // Address currentNodeAddress = {currentDiskAddress, 0};
        // index->writeToDisk(currentNode, nodeSize, currentNodeAddress);

        // no stats to update

        cout << "Key " << key << " in leaf node deleted." << endl;

        return numDeletedNodes; // should be 0
    }

    // (3) will violate rules, CAN borrow a key from sibling node
    // check left and right sibling to borrow a key

    // check if left sibling exists
    if (leftSibling >= 0) {
        // load left sibling from disk into main mem
        // TreeNode *leftNode = (TreeNode *)index->readFromDisk(parentNode->pointer[leftSibling], nodeSize);
        TreeNode *leftNode = (TreeNode *)parentNode->pointer[leftSibling];
        // check if borrowing a key from left sibling is legal
        if (leftNode->numOfKey >= floor((maxDataKey + 1) / 2)) {

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
            // Address currentNodeAddress = {currentDiskAddress, 0};
            // Address parentNodeAddress = {parentDiskAddress, 0};
            // index->writeToDisk(currentNode, nodeSize, currentNodeAddress);
            // index->writeToDisk(leftNode, nodeSize, parentNode->pointer[leftSibling]);
            // index->writeToDisk(parentNode, nodeSize, parentNodeAddress);

            cout << "Key " << key << " in leaf node deleted." << endl;
            cout << "Borrowed key " << currentNode->dataKey[0] << " from left node." << endl;

            return numDeletedNodes; // should be 0
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
        // TreeNode *rightNode = (TreeNode *)index->readFromDisk(parentNode->pointer[rightSibling], nodeSize);
        TreeNode *rightNode = (TreeNode *)parentNode->pointer[rightSibling];

        // check if borrowing a key from right sibling is legal
        if (rightNode->numOfKey >= floor((maxDataKey + 1) / 2)) {

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
            // Address currentNodeAddress = {currentDiskAddress, 0};
            // Address parentNodeAddress = {parentDiskAddress, 0};
            // index->writeToDisk(currentNode, nodeSize, currentNodeAddress);
            // index->writeToDisk(rightNode, nodeSize, parentNode->pointer[rightSibling]);
            // index->writeToDisk(parentNode, nodeSize, parentNodeAddress);

            cout << "Key " << key << " in leaf node deleted." << endl;
            cout << "Borrowed key " << currentNode->dataKey[currentNode->numOfKey-1] << " from right node." << endl;

            return numDeletedNodes; // should be 0
        } else {
            cout << "Right sibling of current node has no key to borrow from." << endl;
        }
    } else {
        cout << "Current node has no right sibling." << endl;
    }

    // (4) will violate rules, CANNOT borrow a key from sibling node
    // at this point, no siblings exist OR no key can be borrowed from both siblings
    // 1 or more nodes will be deleted

    // check if left sibling exists
    if (leftSibling >= 0) {
        // load left sibling from disk into main mem
        // TreeNode *leftNode = (TreeNode *)index->readFromDisk(parentNode->pointer[leftSibling], nodeSize);
        TreeNode *leftNode = (TreeNode *)parentNode->pointer[leftSibling];

        // merge left and current node by migrating currentNode content into leftNode
        int l = leftNode->numOfKey;
        for (int c = 0; c < currentNode->numOfKey; c++) {
            leftNode->dataKey[l] = currentNode->dataKey[c];
            leftNode->pointer[l] = currentNode->pointer[c];
            l++;
        }

        // update stats
        leftNode->numOfKey += currentNode->numOfKey;

        // change leftNode last pointer to the next leaf node pointed by currentNode
        leftNode->pointer[leftNode->numOfKey] = currentNode->pointer[currentNode->numOfKey];

        // write updated leftNode to disk
        // index->writeToDisk(leftNode, nodeSize, parentNode->pointer[leftSibling]);

        //TODO: update parent node
        numDeletedNodes += recursiveParentUpdate(parentNode->dataKey[leftSibling], leftSibling, (TreeNode *)parentNode, (TreeNode *)currentNode);

        //TODO: delete currentNode from disk
        // Address currentNodeAddress = {currentDiskAddress, 0};
        // index->deallocateRecord(currentNodeAddress, nodeSize);

        numDeletedNodes++;

    } else if (rightSibling <= parentNode->numOfKey && rightSibling >= 0) { // check if right sibling exists
        // load right sibling from disk into main mem
        // TreeNode *rightNode = (TreeNode *)index->readFromDisk(parentNode->pointer[rightSibling], nodeSize);
        TreeNode *rightNode = (TreeNode *)parentNode->pointer[rightSibling];

        // merge right and current node by migrating rightNode content into currentNode
        int c = currentNode->numOfKey;
        for (int r = 0; r < rightNode->numOfKey; r++) {
            currentNode->dataKey[c] = rightNode->dataKey[r];
            currentNode->pointer[c] = rightNode->pointer[r];
            c++;
        }

        // update stats
        currentNode->numOfKey += rightNode->numOfKey;

        // change currentNode last pointer to the next leaf node pointed by rightNode
        currentNode->pointer[currentNode->numOfKey] = rightNode->pointer[rightNode->numOfKey];

        // write updated currentNode to disk
        // index->writeToDisk(currentNode, nodeSize, parentNode->pointer[rightSibling]);

        // update parent node
        // void *rightDiskAddress = parentNode->pointer[rightSibling].blockAddress;
        numDeletedNodes += recursiveParentUpdate(parentNode->dataKey[rightSibling], rightSibling, (TreeNode *)parentNode, (TreeNode *)rightNode);

        // delete rightNode from disk
        // Address rightNodeAddress = {rightDiskAddress, 0};
        // index->deallocateRecord(rightNodeAddress, nodeSize);

        numDeletedNodes++;

    }

    // update stats
    numOfNode -= numDeletedNodes;

    return numDeletedNodes; // should be 1 or more
}


/**
 * TODO
 * 2) calc number of nodes
 * 3) which read/write to remove? how to change function params
 * */

// From the perspective of the parent, update its contents after removal of a child node
// Parent node has been assigned as the current node
// returns number of deleted nodes
int BPTree::recursiveParentUpdate(int key, int keyIdx, TreeNode *currentNode, TreeNode *childNode) {

    int numDeletedNodes = 0;

    // load parent/current and child nodes from disk into main mem
    // Address currentNodeAddress = {currentDiskAddress, 0};
    // Address childNodeAddress = {childDiskAddress, 0};

    // TreeNode *currentNode = (TreeNode *)index->readFromDisk(currentNodeAddress, nodeSize);

    // TODO: huh, why is this needed? it doesnt seem correct
    // check if currentNode is the root
    // Address rootTreeAddress = {addressOfRoot, 0};
    if (currentNode == root) root = currentNode;

    // 2 cases to handle, whether parent node is the root

    // (1) current/parent node is the root AND it has only 1 key
    // replace its child as the new root
    // note: if the root node has 2 or more keys, then move on to move keys & pointers
    if (currentNode == root && currentNode->numOfKey == 1) {

        // delete childNode
        // index->deallocateRecord(childNodeAddress, nodeSize);

        // set parent's left/right child node as the new root
        root = (TreeNode *)currentNode->pointer[keyIdx];
        // addressOfRoot = (TreeNode *)currentNode->pointer[keyIdx].blockAddress;

        cout << "Root node was replaced." << endl;

        numDeletedNodes++;

        return numDeletedNodes; // should be 1
    }

    // update keys & pointers in parent node (can be internal or root)
    // TODO: check int i values

    // move all keys forward to delete the old key // also applies to root nodes with 2 or more keys
    for (int i = keyIdx; i < currentNode->numOfKey - 1; i++)
    {
        currentNode->dataKey[i] = currentNode->dataKey[i+1];
    }

    // move all pointers forward to delete the old pointer
    // pointer to child node is on the right of its key for internal node
    for (int i = keyIdx; i < currentNode->numOfKey; i++)
    {
        currentNode->pointer[i] = currentNode->pointer[i+1];
    }

    // update stats
    currentNode->numOfKey--;

    // check if internal node has minimum of floor(n/2) keys or floor(n/2)+1 pointers
    // if so, no more recursions needed
    if (currentNode->numOfKey >= floor(maxDataKey / 2)) return 1;

    // (2) current/parent node is NOT root AND violates rules
    // may need to delete internal(nodes)

    // find parent of this current node to access its siblings
    TreeNode *parentNode = getParent(currentNode, currentNode->dataKey[0]);
    // Address parentNodeAddress{parentDiskAddress, 0};
    // TreeNode *parentNode = (TreeNode *)index->readFromDisk(parentNodeAddress, nodeSize);
    
    // find left and right sibling of currentNode
    int leftSibling, rightSibling;

    for (int i = 0; i < parentNode->numOfKey + 1; i++) {
        if (parentNode->pointer[i] = currentNode) {
            leftSibling = i - 1;
            rightSibling = i + 1;
            break;
        }
    }

    // (2A) can borrow a key from left/rigth sibling

    // check left and right sibling to borrow a key
    // check if left sibling exists
    if (leftSibling >= 0)
    {
        // load left sibling from disk into main mem
        // TreeNode *leftNode = (TreeNode *)index->readFromDisk(parentNode->pointer[leftSibling], nodeSize);
        TreeNode *leftNode = (TreeNode *)parentNode->pointer[leftSibling];

        // check if borrowing a key from left sibling is legal
        if (leftNode->numOfKey >= floor(maxDataKey / 2)) {

            // insert rightmost key of leftNode into first position of currentNode

            // move currentNode last pointer backward
            currentNode->pointer[currentNode->numOfKey+1] = currentNode->pointer[currentNode->numOfKey];

            // move all other keys & pointers in currentNode backward
            for (int i = currentNode->numOfKey; i > 0; i--) {
                currentNode->dataKey[i] = currentNode->dataKey[i-1];
                currentNode->pointer[i] = currentNode->pointer[i-1];
            }

            // migrate borrowed key & pointer from leftNode into currentNode

            // new (borrowed) key in currentNode becomes the parentNode key
            // parentNode key becomes the borrowed key from leftNode
            currentNode->dataKey[0] = parentNode->dataKey[leftSibling];
            parentNode->dataKey[leftSibling] = leftNode->dataKey[leftNode->numOfKey-1];
            currentNode->pointer[0] = leftNode->pointer[leftNode->numOfKey]; // pointer is to the right of the key

            // update stats
            currentNode->numOfKey++;
            leftNode->numOfKey--;

            // move left node last pointer forware
            // TODO: not sure abt this, check logic again
            // leftNode->pointer[currentNode->numOfKey] = leftNode->pointer[currentNode->numOfKey + 1];

            // write updated current, left, and parent nodes to disk
            // Address currentNodeAddress = {currentDiskAddress, 0};
            // Address parentNodeAddress = {parentDiskAddress, 0};
            // index->writeToDisk(currentNode, nodeSize, currentNodeAddress);
            // index->writeToDisk(leftNode, nodeSize, parentNode->pointer[leftSibling]);
            // index->writeToDisk(parentNode, nodeSize, parentNodeAddress);

            cout << "Borrowed key " << currentNode->dataKey[0] << " from left node." << endl;

            return numDeletedNodes; // should be 0

        } else {
             cout << "Left sibling of current node has no key to borrow from." << endl;
        }
    } else {
        cout << "Current node has no left sibling." << endl;
    }

    // check if right sibling exists
    if (rightSibling <= parentNode->numOfKey && rightSibling >= 0) {
        // load right sibling from disk into main mem
        // TreeNode *rightNode = (TreeNode *)index->readFromDisk(parentNode->pointer[rightSibling], nodeSize);
        TreeNode *rightNode = (TreeNode *)parentNode->pointer[rightSibling];

        // check if borrowing a key from right sibling is legal
        if (rightNode->numOfKey >= floor(maxDataKey / 2)) {

            // insert first key of rightNode into last position of currentNode

            // migrate borrowed key & pointer from leftNode into currentNode

            // new (borrowed) key in currentNode becomes the parentNode key
            // parentNode key becomes the borrowed key from leftNode
            currentNode->dataKey[currentNode->numOfKey] = parentNode->dataKey[rightSibling - 1];
            parentNode->dataKey[rightSibling - 1] = rightNode->dataKey[0];
            currentNode->pointer[currentNode->numOfKey + 1] = rightNode->pointer[0]; // pointer is to the right of the key

            // move all other keys & pointers in rightNode forward
            for (int i = 0; i < rightNode->numOfKey - 1; i++) {
                rightNode->dataKey[i] = rightNode->dataKey[i+1];
                rightNode->pointer[i] = rightNode->pointer[i+1];
            }

            // move rightNode last pointer forward
            // TODO: check index value
            rightNode->pointer[currentNode->numOfKey - 1] = rightNode->pointer[currentNode->numOfKey];

            // update stats
            currentNode->numOfKey++;
            rightNode->numOfKey--;

            // write updated current, right, and parent nodes to disk
            // Address currentNodeAddress = {currentDiskAddress, 0};
            // Address parentNodeAddress = {parentDiskAddress, 0};
            // index->writeToDisk(currentNode, nodeSize, currentNodeAddress);
            // index->writeToDisk(rightNode, nodeSize, parentNode->pointer[rightSibling]);
            // index->writeToDisk(parentNode, nodeSize, parentNodeAddress);

            cout << "Borrowed key " << currentNode->dataKey[currentNode->numOfKey-1] << " from right node." << endl;

            return numDeletedNodes; // should be 0

        } else {
            cout << "Right sibling of current node has no key to borrow from." << endl;
        }
    } else {
        cout << "Current node has no right sibling." << endl;
    }

    // (2B) CANNOT borrow a key from left/right sibling

    // at this point, no siblings exist OR no key can be borrowed from both siblings
    // check if left or right sibling exists to merge with current node

    // check if left sibling exists
    if (leftSibling >= 0) {
        // load left sibling from disk into main mem
        // TreeNode *leftNode = (TreeNode *)index->readFromDisk(parentNode->pointer[leftSibling], nodeSize);
        TreeNode *leftNode = (TreeNode *)parentNode->pointer[leftSibling];

        // move parent node key of the left sibling into the left node
        leftNode->dataKey[leftNode->numOfKey] = parentNode->dataKey[leftSibling];

        // merge left and current node by migrating currentNode content into leftNode
        int l = leftNode->numOfKey + 1;
        for (int c = 0; c < currentNode->numOfKey; c++) {
            leftNode->dataKey[l] = currentNode->dataKey[c];
            leftNode->pointer[l] = currentNode->pointer[c];
            l++;
        }

        // update stats
        leftNode->numOfKey += currentNode->numOfKey + 1;
        // currentNode->numOfKey = 0;

        // move leftNode last pointer to currentNode last pointer
        leftNode->pointer[leftNode->numOfKey] = currentNode->pointer[currentNode->numOfKey];

        // write updated leftNode to disk
        // index->writeToDisk(leftNode, nodeSize, parentNode->pointer[leftSibling]);

        //TODO: update parent node
        numDeletedNodes += recursiveParentUpdate(parentNode->dataKey[leftSibling], leftSibling, (TreeNode *)parentNode, (TreeNode *)currentNode);

        // delete currentNode from disk
        // Address currentNodeAddress = {currentDiskAddress, 0};
        // index->deallocateRecord(currentNodeAddress, nodeSize);

        numDeletedNodes++;

    } else if (rightSibling <= parentNode->numOfKey && rightSibling >= 0) { // check if right sibling exists
        // load right sibling from disk into main mem
        // TreeNode *rightNode = (TreeNode *)index->readFromDisk(parentNode->pointer[rightSibling], nodeSize);
        TreeNode *rightNode = (TreeNode *)parentNode->pointer[rightSibling];

        // move parent node key of the left sibling into current node
        currentNode->dataKey[currentNode->numOfKey] = parentNode->dataKey[rightSibling - 1];

        // merge right and current node by migrating rightNode content into currentNode
        int c = currentNode->numOfKey + 1;
        for (int r = 0; r < rightNode->numOfKey; r++) {
            currentNode->dataKey[c] = rightNode->dataKey[r];
            currentNode->pointer[c] = rightNode->pointer[r];
            c++;
        }

        // update stats
        currentNode->numOfKey += rightNode->numOfKey + 1;
        // currentNode->numOfKey = 0;

        // move rightNode last pointer to currentNode last pointer
        currentNode->pointer[currentNode->numOfKey] = rightNode->pointer[rightNode->numOfKey];

        // write updated currentNode to disk
        // index->writeToDisk(currentNode, nodeSize, parentNode->pointer[rightSibling]);

        // update parent node
        // void *rightDiskAddress = parentNode->pointer[rightSibling].blockAddress;
        numDeletedNodes += recursiveParentUpdate(parentNode->dataKey[rightSibling], rightSibling, (TreeNode *)parentNode, (TreeNode *)rightNode);

        // delete rightNode from disk
        // Address rightNodeAddress = {rightDiskAddress, 0};
        // index->deallocateRecord(rightNodeAddress, nodeSize);

        numDeletedNodes++;
    }

    // update stats
    return numDeletedNodes; // should be 1 or more
}