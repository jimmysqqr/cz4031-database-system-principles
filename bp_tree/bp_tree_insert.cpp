#include "bp_tree.h"
#include "../storage/helper_types.h"

#include <vector>
#include <cstring>
#include <iostream>
#include <cmath>

// Function to insert data into nodes
void BPTree::insertKey(Address address, int key)
{
    if (root == nullptr) // why need nodeLL and root? // HELP
    {
        // // Creating the head of the linked list (re-using TreeNode * struct)
        // TreeNode *nodeLL = new TreeNode(maxDataKey);
        // nodeLL->dataKey[0] = key;
        // nodeLL->isLeaf = false;
        // nodeLL->numOfKey = 1;
        // nodeLL->pointer[0] = &address; // Insert disk address of the key

        // // Allocate nodeLL and root address
        // Address nodeLLRootAddress = index->writeToDisk((void *)nodeLL, nodeSize);

        // root = new TreeNode(maxDataKey);
        // root->dataKey[0] = key;
        // root->isLeaf = true; // Both root and leaf
        // root->numOfKey = 1;
        // root->pointer[0] = nodeLLRootAddress; // Assign record disk to pointer

        // // Write root node to disk
        // // Get disk address after writing to disk and assign it to addressOfRoot
        // addressOfRoot = index->writeToDisk(root, nodeSize).blockAddress;
        
        // create a new node for the new record
        ListNode *newNode = new ListNode(&address);

        // curNode.next = new ListNode(&address);
        
        // create a root node and point to the new node
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
        TreeNode *curNode = root;                 // Inialize current node
        TreeNode *parent;                         // Keep track of parent node for updating
        // void *parentDiskAddress = addressOfRoot;  // Keep track of parent's disk address for updating
        // void *curNodeDiskAddress = addressOfRoot; // Store current node's disk address for updating

        // Go through all nodes till you reach a leaf node
        while (curNode->isLeaf == false)
        {
            // Set parent node for going back and assigning child node later and set parent disk address
            parent = curNode;
            // parentDiskAddress = curNodeDiskAddress;

            for (int i = 0; i < curNode->numOfKey; i++)
            {

                // If key is smaller than current key, go to left node pointer
                if (key < curNode->dataKey[i])
                {
                    // move to next node
                    curNode = (TreeNode *)curNode->pointer[i];
                    break;
                }

                // if key is larger than all keys in the node, point to last pointer of last key
                if (i == curNode->numOfKey - 1)
                {
                    // Read from disk and load it to main memory
                    // TreeNode *mainMemoryNode = (TreeNode *)index->readFromDisk(curNode->pointer[i + 1], nodeSize);
                    // TreeNode *mainMemoryNode = (TreeNode *)curNode->pointer[i + 1];

                    // Update curNodeDiskAddress to pointer address for updates later
                    // curNodeDiskAddress = curNode->pointer[i + 1].blockAddress;

                    // Move to new node in main memory
                    // curNode = (TreeNode *)mainMemoryNode;

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

                // // Create a new linked list
                // ListNode *nodeLL = new ListNode(&)
                // TreeNode *nodeLL = new TreeNode(maxDataKey);
                // nodeLL->dataKey[0] = key;
                // nodeLL->isLeaf = false;
                // nodeLL->numOfKey = 1;
                // nodeLL->pointer[0] = &address;

                // Create a new ListNode for the new record
                ListNode *newNode = new ListNode(&address);

                // Update variables
                curNode->dataKey[i] = key;
                curNode->pointer[i] = newNode;
                curNode->numOfKey++;

                // Update next node
                curNode->pointer[curNode->numOfKey] = nextNode;

                // Address curNodeOriginalAddress{curNodeDiskAddress, 0};
                // index->writeToDisk(curNode, nodeSize, curNodeOriginalAddress);
            }
        }

        // If there is no space to insert new key, split the node into two and update the parent if required
        else
        {

            //before we split it lets check if there is duplicates
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
            void *tempPointerList[maxDataKey + 1]; // should be Address[], not TreeNode[]

            // Initialize next node
            TreeNode *nextNode = (TreeNode *)curNode->pointer[curNode->numOfKey];

            // Copy all keys and pointers to temp array
            int i;
            for (i = 0; i < maxDataKey; i++)
            {
                tempKeyList[i] = curNode->dataKey[i];
                tempPointerList[i] = (Address *)curNode->pointer[i];
            }

            // Get index where key can be inserted
            i = b;

            // // Check if key at index i has duplicates
            // if (i < maxDataKey && curNode->dataKey[i] == key)
            // {
            //     // If there is duplicates it means there is an existing linked list
            //     // Insert key into linked list
            //     insertListNode((ListNode *)curNode->pointer[i], address, key);
            //     return;
            // }

            // If there is no duplicate then insert new key
            // First move temp list backwards by 1 key
            for (int j = maxDataKey; j > i; j--)
            {

                tempKeyList[j] = tempKeyList[j - 1];
                tempPointerList[j] = tempPointerList[j - 1];
            }

            // // Create new linked list
            // TreeNode *nodeLL = new TreeNode(maxDataKey);
            // nodeLL->dataKey[0] = key;
            // nodeLL->isLeaf = false;
            // nodeLL->numOfKey = 1;
            // nodeLL->pointer[0] = &address;

            // Create a new ListNode for the new record
            ListNode *newListNode = new ListNode(&address);
            
            // if(i >= maxDataKey){
            //     newLeafNode->dataKey[0] = key;
            //     newLeafNode->pointer[0] = newListNode;
            //     newLeafNode->numOfKey = 1;
            // }
            // else
            // {
                
            // }

            // Insert new key into temp key and pointer list
            tempKeyList[i] = key;
            tempPointerList[i] = newListNode;

            //first node should have ceiling of ((maxdatakey+1)/2))
            int leftNodeNumOfKey = (int)ceil((maxDataKey+1)/2);
            int rightNodeNumOfKey = (int)floor((maxDataKey+1)/2);
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

            // // Set empty float for wrong keys
            // for (i = curNode->numOfKey; i < maxDataKey; i++)
            // {
            //     curNode->dataKey[i] = int();
            // }

            // // Set null pointer for wrong pointers
            // for (i = curNode->numOfKey + 1; i < maxDataKey + 1; i++)
            // {
            //     // Address nullAddress{nullptr, 0};
            //     // curNode->pointer[i] = nullAddress;
            //     curNode->pointer[i] = nullptr;
            // }

            // Address curNodeOriginalAddress{curNodeDiskAddress, 0};
            // index->writeToDisk(curNode, nodeSize, curNodeOriginalAddress);

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
                newParentRoot->isLeaf = false;
                newParentRoot->numOfKey = 1;

                // Write new parent root to disk and update the root disk address
                // Address newParentRootAddress = index->writeToDisk(newParentRoot, nodeSize);
                // addressOfRoot = newParentRootAddress.blockAddress;
                root = newParentRoot;

                root->isLeaf = false;

                numOfNode++;
            }

            // If curNode is not root, insert a new parent in the middle levels of the trees
            else
            {
                insertUpdateParent((TreeNode *)parent, (TreeNode *)newLeafNode, newLeafNode->dataKey[0]);
            }
        }
    }

    //numOfNode = index->getNumBlocksAllocated();
}

// Function to insert record into existing linked list
void BPTree::insertListNode(ListNode* head, Address address, int key)
{

    // when you enter this function, the linked list will already have at least 1 list node
    // because this function is only called when there is a duplicate key, meaning the previous key has initialised the first list node

    ListNode *newNode = new ListNode(&address);

    ListNode *tempNode = head;
    
    while (tempNode->next != nullptr) {
        tempNode = tempNode->next;
    }

    tempNode->next = newNode;



    // TreeNode *curNode = (TreeNode *)index->readFromDisk(curNodeAddress, nodeSize);

    // if (curNode->numOfKey < maxDataKey)
    // {

    //     // Move all keys back by one space
    //     for (int i = curNode->numOfKey; i > 0; i--)
    //     {
    //         curNode->dataKey[i] = curNode->dataKey[i - 1];
    //     }

    //     // Move all pointers back by one space
    //     for (int i = curNode->numOfKey + 1; i > 0; i--)
    //     {
    //         curNode->pointer[i] = curNode->pointer[i - 1];
    //     }

    //     // Insert new key at start index
    //     curNode->dataKey[0] = key;
    //     curNode->pointer[0] = address;
    //     curNode->numOfKey++;

    //     // Write back to disk
    //     curNodeAddress = index->writeToDisk((void *)curNode, nodeSize, curNodeAddress);
    //     return curNodeAddress;
    // }
    // // No space in the node, create a new linked list node
    // else
    // {

    //     // Make a new node
    //     TreeNode *nodeLL = new TreeNode(maxDataKey);
    //     nodeLL->isLeaf = false;
    //     nodeLL->dataKey[0] = key;
    //     nodeLL->numOfKey = 1;

    //     // Insert key
    //     nodeLL->pointer[0] = address;

    //     // Insert previous node disk address as next
    //     nodeLL->pointer[1] = curNodeAddress;

    //     // Write new linked list to disk
    //     Address nodeLLAddress = index->writeToDisk((void *)nodeLL, nodeSize);
    //     return nodeLLAddress;
    // }
}

// Function to update parent nodes
// Add new parent nodes if needed
// Update child nodes respectively
void BPTree::insertUpdateParent(TreeNode *curNode, TreeNode *childNode, int key)
{

    // // Get latest parent and child from disk
    // Address curNodeAddress{curNodeDiskAddress, 0};
    // TreeNode *curNode = (TreeNode *)index->readFromDisk(curNodeAddress, nodeSize);

    // Address childAddress{childDiskAddress, 0};
    // TreeNode *childNode = (TreeNode *)index->readFromDisk(childAddress, nodeSize);

    // Set root to current node if current node disk address equals root address
    if (curNode == root)
    {
        root = curNode;
    }

    // If parent (curNode) still has space, add child node as a pointer
    if (curNode->numOfKey < maxDataKey)
    {
        int a;
        while (a < curNode->numOfKey)
        {
            if (key == curNode->dataKey[a]) return;
            a++;
        }
        // Get index to insert new child node
        int i = 0;
        while (key > curNode->dataKey[i] && i < curNode->numOfKey)
        {
            i++;
        }

        // // Check for duplicates at i
        // // This is where datakey[i] has space, insert key at i
        // if (curNode->dataKey[i] == key)
        // {
        //     // If duplicate exists, it means linked list exists
        //     // Insert and update node to the linked list
        //     insertListNode((ListNode *)curNode->pointer[i], address, key);
        //     return;
        // }

        if(i < curNode->numOfKey){
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
        curNode->pointer[i+1] = childNode;
        curNode->numOfKey++;
    }

    // Else if parent node does not have space, split parent node and insert more parent nodes
    else
    {

        // Initialize next node
        TreeNode *newInternalNode = new TreeNode(maxDataKey);
        TreeNode *nextNode = (TreeNode *)curNode->pointer[curNode->numOfKey];
        
        // Extra pointer to keep track of new child pointer
        int tempKeyList[maxDataKey + 1];
        void *tempPointerList[maxDataKey + 2];

        // Copy all keys into temp key list
        for (int i = 0; i < maxDataKey; i++)
        {
            tempKeyList[i] = curNode->dataKey[i];
        }

        for (int i = 0; i < maxDataKey + 1; i++)
        {
            tempPointerList[i] = curNode->pointer[i];
        }

        // Find index to insert key
        int i = 0;
        while (key < tempKeyList[i] && i < maxDataKey)
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
        for (int j = maxDataKey + 1; j > i + 1; j--)
        {
            tempPointerList[j] = tempPointerList[j - 1];
        }

        // Insert new pointer to right of child's key
        tempPointerList[i + 1] = childNode;

        // // Split nodes into 2. floor(n/2) keys for left node
        // curNode->numOfKey = (maxDataKey + 1) / 2;


        int leftNodeNumOfKey = (int)ceil(maxDataKey/2);
        int rightNodeNumOfKey = (int)floor(maxDataKey/2);
        
        int leftNodeNumOfPointer = (int)(ceil((maxDataKey/2))) + 1;
        int rightNodeNumOfPointer = (int)(floor((maxDataKey/2))) + 1;


        // Update left keys into curNode from tempKeyList
        for (i = 0; i < leftNodeNumOfKey; i++)
        {
            curNode->dataKey[i] = tempKeyList[i];
        }

        // Update left pointer into curNode from tempPointerList
        for (i = 0; i < leftNodeNumOfPointer; i++){
            curNode->pointer[i] = tempPointerList[i];
        }

        // Update right keys into newInternalNode from tempkeylist

        // Next update new internal node
        // Keep track of i index since we are using remaining keys and pointers
        int initial_i = i;
        for (int j = 0; j < rightNodeNumOfKey; i++, j++)
        {
            newInternalNode->dataKey[j] = tempKeyList[i];
        }   

        // Update right pointer into newInternalNode from tempPointerList
        i = initial_i;
        for (int j = 0; j < rightNodeNumOfPointer; i++, j++)
        {
            newInternalNode->pointer[j] = tempPointerList[i];
        }



        // Update end pointer of newInternalNode to next node
        newInternalNode->pointer[newInternalNode->numOfKey] = nextNode;
        newInternalNode->numOfKey = rightNodeNumOfKey;
        newInternalNode->isLeaf = false;


        // Assign curNode pointer to new internal address
        curNode->pointer[curNode->numOfKey] = newInternalNode;


        // Update parentNode with these 2 new internal nodes



        // // Insert new keys into new parent node
        // for (int i = 0, j = curNode->numOfKey + 1; i < newParentNode->numOfKey; i++, j++)
        // {
        //     newParentNode->pointer[i] = tempPointerList[j];
        // }

        // // Set empty float for wrong keys
        // for (i = curNode->numOfKey; i < maxDataKey; i++)
        // {
        //     curNode->dataKey[i] = float();
        // }

        // // Set null pointer for wrong pointers
        // for (i = curNode->numOfKey + 1; i < maxDataKey + 1; i++)
        // {
        //     //Address nullAddress{nullptr, 0};
        //     curNode->pointer[i] = nullptr;
        // }

        // // Assign new child to parent
        // curNode->pointer[curNode->numOfKey] = childNode;

        numOfNode++;

        // // Write old parent and new parent to disk
        // index->writeToDisk(curNode, nodeSize, curNodeAddress);
        // Address newParentDiskAddress = index->writeToDisk(newParentNode, nodeSize);

        // if current node is root, we need to create new root
        if (curNode == root)
        {
            TreeNode *newParentRoot = new TreeNode(maxDataKey);

            // Loop through nodes to get left most leaf node from the right side of the tree
            TreeNode *tempNode = (TreeNode *)newInternalNode->pointer[0];
            while (tempNode->isLeaf == false){
                tempNode = (TreeNode *)tempNode->pointer[0];
            }

            // Update parent root to hold children
            newParentRoot->dataKey[0] = tempNode->dataKey[0]; // to update

            // Update parent root children with previous 2 nodes
            newParentRoot->pointer[0] = curNode;
            newParentRoot->pointer[1] = newInternalNode;

            // Update parent root variables
            newParentRoot->isLeaf = false;
            newParentRoot->numOfKey = 1;

            root = newParentRoot;

            // // save new parent root to disk
            // Address newRootDiskAddress = index->writeToDisk(root, nodeSize);

            // // Update root address
            // addressOfRoot = newRootDiskAddress.blockAddress;

            numOfNode++;
        }

        // If current node is not parent, need to split again
        // call this function recursively
        else
        {
            // Get parent and perform a recursive call
            TreeNode *parent = getParent(curNode, curNode->dataKey[0]);
            if(parent != nullptr){
                insertUpdateParent(parent, (TreeNode *)newInternalNode, tempKeyList[curNode->numOfKey]);
            }
        }
    }
}