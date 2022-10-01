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

                // // If key is equal to the current key, go to right node pointer
                // if(key == curNode->dataKey[i])
                // {
                //     // Move to the next node (right ptr)
                //     curNode = (TreeNode *)curNode->pointer[i+1];
                //     break;
                // }

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

        // If there is no space to insert new key, split the node into two and update the parent if required
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

            // Initialize next node
            TreeNode *nextNode = (TreeNode *)curNode->pointer[curNode->numOfKey];

            // Copy all keys and pointers to temp array
            int i;
            for (i = 0; i < maxDataKey; i++)
            {
                tempKeyList[i] = curNode->dataKey[i];
                tempPointerList[i] = (ListNode *)curNode->pointer[i];
            }

            // Get index where key can be inserted
            i = b;

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
            int leftNodeNumOfKey = (int)ceil((maxDataKey + 1) / 2);
            int rightNodeNumOfKey = (int)floor((maxDataKey + 1) / 2);
            curNode->numOfKey = leftNodeNumOfKey;
            newLeafNode->numOfKey = rightNodeNumOfKey;

            // Set last pointer of new leaf node to point to previous last pointer of existing node (curNode)
            // curNode was full, so last pointer would use numOfKey
            newLeafNode->pointer[newLeafNode->numOfKey] = nextNode;

            // Update other pointers, keys and parent nodes
            // First update first node's keys and pointers
            for (i = 0; i < leftNodeNumOfKey; i++)
            {
                curNode->dataKey[i] = tempKeyList[i];
                curNode->pointer[i] = tempPointerList[i];
            }

            // Next update new leaf node
            // Keep track of i index since we are using remaining keys and pointers
            for (int j = 0; j < rightNodeNumOfKey; i++, j++)
            {
                newLeafNode->dataKey[j] = tempKeyList[i];
                newLeafNode->pointer[j] = tempPointerList[i];
            }

            // Assign curNode pointer to new leaf address
            curNode->pointer[curNode->numOfKey] = newLeafNode;

            // Update number of nodes after adding everything
            numOfNode++;

            // If curNode is at root level, then we need to make a new parent root
            if (curNode == root)
            {
                TreeNode *newParentRoot = new TreeNode(maxDataKey);

                // Set new parent root's key to be left bound of the right child (new leaf node)
                newParentRoot->dataKey[0] = newLeafNode->dataKey[0];

                // Point new parent root's children as current node and new node
                // Address curNodeDisk{curNodeDiskAddress, 0};
                curNode->isLeaf = true;
                newParentRoot->pointer[0] = curNode;
                newParentRoot->pointer[1] = newLeafNode;

                // Update new parent root's variables
                newParentRoot->numOfKey = 1;

                root = newParentRoot;

                numOfNode++;
            }

            // If curNode is not root, insert a new parent in the middle levels of the trees
            else
            {
                insertUpdateParent((TreeNode *)parent, (TreeNode *)newLeafNode, newLeafNode->dataKey[0]);
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
void BPTree::insertUpdateParent(TreeNode *curNode, TreeNode *childNode, int key)
{
    // // Set root to current node if current node disk address equals root address
    // if (curNode == root)
    // {
    //     root = curNode;
    // }

    if (curNode == childNode) {
        std::cout << std::endl << "##### ???? #####" << std::endl;
    }

    // If parent (curNode) still has space, add child node as a pointer
    if (curNode->numOfKey < maxDataKey)
    {
        int a;
        while (a < curNode->numOfKey)
        {
            if (key == curNode->dataKey[a])
                return;
            a++;
        }

        // Get index to insert new child node
        int i = 0;
        while (key > curNode->dataKey[i] && i < curNode->numOfKey)
        {
            i++;
        }

        if (i < curNode->numOfKey)
        {
            // Shift keys 1 space forward
            for (int j = curNode->numOfKey; j > i; j--)
            {
                curNode->dataKey[j] = curNode->dataKey[j - 1];
            }

            // Shift pointers 1 space forward
            for (int j = curNode->numOfKey + 1; j > i + 1; j--)
            {
                curNode->pointer[j] = curNode->pointer[j - 1];
            }

            // Add in new child node's lower bound key and pointer to parent
        }

        curNode->dataKey[i] = key;
        curNode->pointer[i + 1] = childNode;
        curNode->numOfKey++;
    }

    // Else if parent node does not have space, split parent node and insert more parent nodes
    else
    {

        // Initialize next node
        TreeNode *newInternalNode = new TreeNode(maxDataKey);
        TreeNode *nextNode = (TreeNode *)curNode->pointer[curNode->numOfKey];

        // Extra pointer to keep track of new child pointer
        int tempKeyList[maxDataKey + 2];
        void *tempPointerList[maxDataKey + 2];
        
        // Copy all pointers into temp pointer list
        // Copy all child nodes left most key into temp key list
        for (int i = 0; i < maxDataKey + 1; i++)
        {
            tempPointerList[i] = curNode->pointer[i];
            tempKeyList[i] = ((TreeNode *)(curNode->pointer[i]))->dataKey[0];
        }

        // Find index to insert pointer
        int i = 0;
        while (key > tempKeyList[i] && i < maxDataKey + 1)
        {
            i++;
        }

        // Shift temp keys and insert child key
        for (int j = maxDataKey + 1; j > i; j--)
        {
            tempKeyList[j] = tempKeyList[j - 1];
            tempPointerList[j] = tempPointerList[j-1];
        }

        // Insert new key
        tempKeyList[i] = key;
        tempPointerList[i] = childNode;


        int leftNodeNumOfKey = (int)ceil(maxDataKey / 2);
        int rightNodeNumOfKey = (int)floor(maxDataKey / 2);

        int leftNodeNumOfPointer = leftNodeNumOfKey + 1;
        int rightNodeNumOfPointer = rightNodeNumOfKey + 1;


        // Update left pointer into curNode from tempPointerList
        for (i = 0; i < leftNodeNumOfPointer; i++)
        {
            curNode->pointer[i] = tempPointerList[i];
        }

        // Update right pointers into newInternalNode from tempPointerlist
        for (int j = 0; j < rightNodeNumOfPointer; i++, j++)
        {
            newInternalNode->pointer[j] = tempPointerList[i];
        }

        // Next Update the keys of the parent node
        // Skip first index because parent's first key usually take second child's key
        for (i = 0; i < leftNodeNumOfKey; i++){
            curNode->dataKey[i] = tempKeyList[i+1];
        }
        // have not reach the last index yet so increment by 1
        ++i;

        // Next Update right node key
        // Skip first index because parent's first key usually take second child's key
        for (int j = 0; j < rightNodeNumOfKey; j++, i++){
            newInternalNode->dataKey[j] = tempKeyList[i+1];
        }

        // update numOfKey
        curNode->numOfKey = leftNodeNumOfKey;
        newInternalNode->numOfKey = rightNodeNumOfKey;

        // Update end pointer of newInternalNode to next node
        newInternalNode->pointer[newInternalNode->numOfKey] = nextNode;
        newInternalNode->isLeaf = false;

        // internal nodes should NOT be pointing to each other on the same level
        // increment num of nodes
        numOfNode++;

        // if current node is root, we need to create new root
        if (curNode == root)
        {
            TreeNode *newParentRoot = new TreeNode(maxDataKey);

            // Loop through nodes to get left most leaf node from the right side of the tree
            TreeNode *tempNode = (TreeNode *)newInternalNode->pointer[0];
            while (tempNode->isLeaf == false)
            {
                tempNode = (TreeNode *)tempNode->pointer[0];
            }

            // Update parent root to hold children
            newParentRoot->dataKey[0] = tempNode->dataKey[0];

            // Update parent root children with previous 2 nodes
            newParentRoot->pointer[0] = curNode;
            newParentRoot->pointer[1] = newInternalNode;

            // Update parent root variables
            newParentRoot->isLeaf = false;
            newParentRoot->numOfKey = 1;

            root = newParentRoot;
            numOfNode++;
        }

        // If current node is not parent, need to split again
        // call this function recursively
        else
        {
            // Get parent and perform a recursive call. Ensure the node has a parent first!
            TreeNode *parent = getParent(curNode, curNode->dataKey[0]);
            if (parent != nullptr)
            {
                insertUpdateParent(parent, (TreeNode *)newInternalNode, tempKeyList[curNode->numOfKey]);
            }
        }
    }
}