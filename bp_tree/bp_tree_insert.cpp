#include "bp_tree.h"
#include "../storage/helper_types.h"

#include <vector>
#include <cstring>
#include <iostream>
#include <cmath>

// Function to insert data into nodes
void BPTree::insertKey(Address address, int key)
{
    if (root == nullptr)
    {
        // Create a new node for the new record
        ListNode *newNode = new ListNode(address);

        // Create a root node and point to the new node
        root = new TreeNode(maxDataKey);
        root->dataKey[0] = key;
        root->isLeaf = true; // Both root and leaf
        root->numOfKey = 1;
        root->pointer[0] = newNode;

        numOfNode++;
    }

    // Else if root exists, traverse the nodes to find proper place to insert key
    else
    {
        TreeNode *curNode = root; // Inialize current node
        TreeNode *parent;         // Keep track of parent node for updating

        // Go through all nodes till you reach a leaf node
        while (curNode->isLeaf == false)
        {
            // Set parent node for going back and assigning child node later and set parent disk address
            parent = curNode;

            for (int i = 0; i < curNode->numOfKey; i++)
            {
                // If key is smaller than current key, go to left node pointer
                if (key < curNode->dataKey[i])
                {
                    // Move to the next node (left ptr)
                    curNode = (TreeNode *)curNode->pointer[i];
                    break;
                }

                // if key is larger than all keys in the node, go to last pointer of last key
                if (i == curNode->numOfKey - 1)
                {
                    // move to next node
                    curNode = (TreeNode *)curNode->pointer[i + 1];
                    break;
                }
            }
        }

        // Reached a leaf node
        // If leaf node have space to insert a key, find out the place and insert record
        if (curNode->numOfKey < maxDataKey)
        {
            int i = 0;

            // While its not the last key and key is larger than current key, keep moving to next key
            while (key > curNode->dataKey[i] && i < curNode->numOfKey)
            {
                i++;
            }

            // Check for duplicates at i
            // This is where datakey[i] has space, insert key at i
            if (curNode->dataKey[i] == key)
            {
                // If duplicate exists, it means linked list exists
                // Insert and update node to the linked list
                insertListNode((ListNode *)curNode->pointer[i], address, key);
            }

            // If no duplicates, we need to make a new linked list
            else
            {
                // Set last node pointer to a new pointer
                TreeNode *nextNode = (TreeNode *)curNode->pointer[curNode->numOfKey];

                // Move key backwards, move pointers also
                for (int j = curNode->numOfKey; j > i; j--)
                {
                    // Preserve index order
                    curNode->dataKey[j] = curNode->dataKey[j - 1];
                    curNode->pointer[j] = curNode->pointer[j - 1];
                }

                // Create a new ListNode for the new record
                ListNode *newNode = new ListNode(address);

                // Update variables
                curNode->dataKey[i] = key;
                curNode->pointer[i] = newNode;
                curNode->numOfKey++;

                // Update next node
                curNode->pointer[curNode->numOfKey] = nextNode;
            }
        }

        // If there is no space to insert new key, split the leaf node into two and update the parent if required (Overflow case)
        else
        {
            // before we split it lets check if there is duplicates
            int b = 0;
            // While its not the last key and key is larger than current key, keep moving to next key
            while (key > curNode->dataKey[b] && b < curNode->numOfKey)
            {
                b++;
            }

            // Check for duplicates at i
            // This is where datakey[i] has space, insert key at i
            if (curNode->dataKey[b] == key)
            {
                // If duplicate exists, it means linked list exists
                // Insert and update node to the linked list
                insertListNode((ListNode *)curNode->pointer[b], address, key);
                return;
            }

            // Create new leaf node to insert keys and pointers
            TreeNode *newLeafNode = new TreeNode(maxDataKey);
            newLeafNode->isLeaf = true;

            // Create temporary list for keys and pointers
            int tempKeyList[maxDataKey + 1];
            // previously was void *
            ListNode *tempPointerList[maxDataKey + 1];

            // Initialize next node, the node that will immediately follow the 2 splitted nodes (may be null)
            TreeNode *nextNode = (TreeNode *)curNode->pointer[curNode->numOfKey];

            // Copy all keys and pointers to temp array
            int i = 0;
            for (i = 0; i < maxDataKey; i++)
            {
                tempKeyList[i] = curNode->dataKey[i];
                tempPointerList[i] = (ListNode *)curNode->pointer[i];
            }

            // Get index where key can be inserted
            i = 0;
            while (key > tempKeyList[i] && i < maxDataKey)
            {
                i++;
            }

            // If there is no duplicate then insert new key
            // First move temp list backwards by 1 key
            for (int j = maxDataKey; j > i; j--)
            {
                tempKeyList[j] = tempKeyList[j - 1];
                tempPointerList[j] = tempPointerList[j - 1];
            }

            // Create a new ListNode for the new record
            ListNode *newListNode = new ListNode(address);

            // Insert new key into temp key and pointer list
            tempKeyList[i] = key;
            tempPointerList[i] = newListNode;

            // first node should have ceiling of ((maxdatakey+1)/2))
            int rightNodeNumOfKey = (maxDataKey + 1) / 2;
            int leftNodeNumOfKey = (maxDataKey + 1) - ((maxDataKey + 1) / 2);
            // DO NOT DIRECTLY CALL floor/ceil on (x/y) where x and y are ints

            curNode->numOfKey = leftNodeNumOfKey;
            newLeafNode->numOfKey = rightNodeNumOfKey;

            // Set last pointer of new leaf node to point to previous last pointer of existing node (curNode)
            // curNode was full, so last pointer would use numOfKey
            // --> curNode --> newLeafNode --> nextNode -->
            newLeafNode->pointer[newLeafNode->numOfKey] = nextNode;

            // Update other pointers, keys and parent nodes
            // First update first node's keys and pointers
            for (i = 0; i < leftNodeNumOfKey; i++)
            {
                curNode->dataKey[i] = tempKeyList[i];
                curNode->pointer[i] = tempPointerList[i];
            }

            // Next update new leaf node
            // Keep track of k index since we are using remaining keys and pointers

            for (int j = 0, k = leftNodeNumOfKey; j < rightNodeNumOfKey; j++, k++)
            {
                newLeafNode->dataKey[j] = tempKeyList[k];
                newLeafNode->pointer[j] = tempPointerList[k];
            }

            // Assign curNode pointer to new leaf address
            // --> curNode --> newLeafNode --> nextNode -->
            curNode->pointer[curNode->numOfKey] = newLeafNode;

            // Don't forget to clean up the extra pointers and keys from curNode
            for (int i = leftNodeNumOfKey; i < maxDataKey; i++)
            {
                curNode->dataKey[i] = int();
            }
            for (int i = leftNodeNumOfKey + 1; i < maxDataKey + 1; i++)
            {
                curNode->pointer[i] = nullptr;
            }

            // Update number of nodes after adding everything
            numOfNode++;

            // If curNode is at root level, then we need to make a new parent root
            if (curNode == root)
            {
                TreeNode *newParentRoot = new TreeNode(maxDataKey);

                // Set new parent root's key to be left bound of the right child (new leaf node)
                newParentRoot->dataKey[0] = newLeafNode->dataKey[0];

                // Point new parent root's children as current node and new node
                curNode->isLeaf = true;
                newParentRoot->pointer[0] = curNode;
                newParentRoot->pointer[1] = newLeafNode;

                // Update new parent root's variables
                newParentRoot->numOfKey = 1;

                // Update root
                root = newParentRoot;

                // We added a node, update the total count
                numOfNode++;
            }

            // If curNode is not root, insert a new parent in the middle levels of the trees
            else
            {
                insertUpdateParent(parent, newLeafNode, newLeafNode->dataKey[0]);
            }
        }
    }
}

// Function to insert record into existing linked list
void BPTree::insertListNode(ListNode *head, Address address, int key)
{
    // When you enter this function, the linked list will already have at least 1 list node
    // because this function is only called when there is a duplicate key, meaning the previous key has initialised the first list node
    // Hence it reduces to a simplified insert to LInkedList function from CZ1107 :D

    ListNode *newNode = new ListNode(address);
    ListNode *tempNode = head;

    while (tempNode->next != nullptr)
    {
        tempNode = tempNode->next;
    }

    // We've found teh end, now attatcg the new node
    tempNode->next = newNode;
}

// Function to update parent nodes in the case when there are n+1 children to one node
// It will add new parent nodes if needed and update child nodes respectively
void BPTree::insertUpdateParent(TreeNode *parentNode, TreeNode *childNode, int key)
{
    // If parent (curNode) still has space, add child node as a pointer
    if (parentNode->numOfKey < maxDataKey)
    {
        // Get index to insert new child node
        int i = 0;
        while (key > parentNode->dataKey[i] && i < parentNode->numOfKey)
        {
            i++;
        }

        if (i < parentNode->numOfKey)
        {
            // Shift keys 1 space forward
            for (int j = parentNode->numOfKey; j > i; j--)
            {
                parentNode->dataKey[j] = parentNode->dataKey[j - 1];
            }

            // Shift pointers 1 space forward
            for (int j = parentNode->numOfKey + 1; j > i + 1; j--)
            {
                parentNode->pointer[j] = parentNode->pointer[j - 1];
            }

            // Add in new child node's lower bound key and pointer to parent
        }

        // Update curNode's parameters
        parentNode->dataKey[i] = key;
        parentNode->pointer[i + 1] = childNode;
        parentNode->numOfKey++;
    }

    // Else if parent node does not have space, split parent node and insert more parent nodes
    else
    {
        // Initialize the new internal node
        TreeNode *newInternalNode = new TreeNode(maxDataKey);
        TreeNode *nextNode = (TreeNode *)parentNode->pointer[parentNode->numOfKey];

        // Extra pointer to keep track of new child pointer
        int tempKeyList[maxDataKey + 1];
        void *tempPointerList[maxDataKey + 2];

        // Copy all keys into temp key list
        for (int i = 0; i < maxDataKey; i++)
        {
            tempKeyList[i] = parentNode->dataKey[i];
        }

        for (int i = 0; i < maxDataKey + 1; i++)
        {
            tempPointerList[i] = parentNode->pointer[i];
        }

        // Find index to insert pointer
        int i = 0;
        while (key > tempKeyList[i] && i < maxDataKey)
        {
            i++;
        }

        // Shift temp keys back by 1 space
        for (int j = maxDataKey; j > i; j--)
        {
            tempKeyList[j] = tempKeyList[j - 1];
        }

        // Insert new key
        tempKeyList[i] = key;

        // Shift temp pointer back 1 space
        // im not too sure if it's j > i + 1 but seems correct
        for (int j = maxDataKey + 1; j > i + 1; j--)
        {
            tempPointerList[j] = tempPointerList[j - 1];
        }

        // Insert new pointer to right of child's key
        // im not too sure if it's [i + 1] but seems correct
        tempPointerList[i + 1] = childNode;

        // // Split nodes into 2. floor(n/2) keys for left node
        int rightNodeNumOfKey = maxDataKey / 2;
        int leftNodeNumOfKey = maxDataKey - (maxDataKey / 2);
        // DO NOT DIRECTLY CALL floor/ceil on (x/y) where x and y are ints

        // Update the numOfKey parameters
        // | parentNode | newInternalNode |
        parentNode->numOfKey = leftNodeNumOfKey;
        newInternalNode->numOfKey = rightNodeNumOfKey;

        // Update left keys into parentNode from tempKeyList
        for (i = 0; i < leftNodeNumOfKey; i++)
        {
            parentNode->dataKey[i] = tempKeyList[i];
        }

        // Update left pointer into parentNode from tempPointerList
        for (i = 0; i < leftNodeNumOfKey + 1; i++)
        {
            parentNode->pointer[i] = tempPointerList[i];
        }

        // Update right keys into newInternalNode from tempkeylist
        // Note the starting value of k
        for (int j = 0, k = leftNodeNumOfKey + 1; j < rightNodeNumOfKey; j++, k++)
        {
            newInternalNode->dataKey[j] = tempKeyList[k];
        }

        // Update right pointer into newInternalNode from tempPointerList
        for (int j = 0, k = leftNodeNumOfKey + 1; j < rightNodeNumOfKey + 1; j++, k++)
        {
            newInternalNode->pointer[j] = tempPointerList[k];
        }

        // Don't forget to clean up the extra pointers and keys from parentNode
        for (int i = leftNodeNumOfKey; i < maxDataKey; i++)
        {
            parentNode->dataKey[i] = int();
        }

        for (int i = leftNodeNumOfKey + 1; i < maxDataKey + 1; i++)
        {
            parentNode->pointer[i] = nullptr;
        }

        // We added a node, update the total count
        numOfNode++;

        // If current node is the root, we need to create a new root and call a recursive update
        if (parentNode == root)
        {
            TreeNode *newParentRoot = new TreeNode(maxDataKey);

            // Loop through nodes to get left most leaf node from the right side of the tree
            TreeNode *tempNode = (TreeNode *)newInternalNode->pointer[0];
            while (tempNode->isLeaf == false)
            {
                tempNode = (TreeNode *)tempNode->pointer[0];
            }

            // Update parent root to hold children
            newParentRoot->dataKey[0] = tempNode->dataKey[0]; // to update

            // Update parent root children with previous 2 nodes
            newParentRoot->pointer[0] = parentNode;
            newParentRoot->pointer[1] = newInternalNode;

            // Update parent root variables
            newParentRoot->numOfKey = 1;

            // Update root
            root = newParentRoot;

            // We added a node, update the total count
            numOfNode++;
        }

        // If current node is not parent, need to split again
        // We then call this function recursively
        else
        {
            // Get parent and perform a recursive call. Ensure the node has a parent first!
            TreeNode *parent = getParent(parentNode, parentNode->dataKey[0]);
            if (parent != nullptr)
            {
                insertUpdateParent(parent, (TreeNode *)newInternalNode, tempKeyList[parentNode->numOfKey]);
            }
        }
    }
}
