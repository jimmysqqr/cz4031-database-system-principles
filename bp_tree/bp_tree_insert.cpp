#include "bp_tree.h"
#include "helper_types.h"

#include <vector>
#include <cstring>
#include <iostream>


// Function to insert data into nodes
void BPTree::insertKey(Address address, float key)
{
    if(addressOfRoot === nullptr)
    {


        TreeNode *nodeLL = new TreeNode(totalDataKey);
        nodeLL->dataKey[0] = key;
        nodeLL->isLeaf = false;
        nodeLL->numOfKey = 1;
        nodeLL->pointer[0] = address; // Insert disk address of the key

        // Allocate nodeLL and root address
        Address nodeLLRootAddress = index->writeToDisk((void *)nodeLL, nodeSize);
        
        
        root = new TreeNode(totalDataKey);
        root->dataKey[0] = key;
        root->isLeaf = true; // Both root and leaf
        root->numOfKey = 1;
        root->pointer[0] = nodeLLRootAddress; // Assign record disk to pointer

        // Write root node to disk 
        // Get disk address after writing to disk and assign it to addressOfRoot 
        addressOfRoot = index->writeToDisk(root,nodeSize).blockAddress;
    }
    // Else if root exists, traverse the nodes to find proper place to insert key
    else
    {
        TreeNode *curNode = root; // Inialize current node
        TreeNode *parent; // Keep track of parent node for updating
        void *parentDiskAddress = addressOfRoot; // Keep track of parent's disk address for updating
        void *curNodeDiskAddress = addressOfRoot; // Store current node's disk address for updating

        // Go through all nodes till you reach a leaf node
        while(curNode->isLeaf == false){

            // Set parent node for going back and assigning child node later and set parent disk address
            parent = curNode;
            parentDiskAddress = curNodeDiskAddress;


            for(int i = 0; i < curNode->numOfKey; i++){
                
                // If key is smaller than current key, go to left node pointer
                if(key < curNode->dataKey[i]){

                    // Read from disk and load it to main memory
                    TreeNode *mainMemoryNode = (TreeNode *)index->readFromDisk(curNode->pointer[i], nodeSize);

                    // Update curNodeDiskAddress to current pointer address for updates later
                    curNodeDiskAddress = curNode->pointer[i].blockAddress;

                    // Move to new node in main memory
                    curNode = (TreeNode *)mainMemoryNode;
                    break;
                }
                
                // if key is larger than all keys in the node, point to last pointer of last key 
                if (i == curNode->numOfKey-1 && key > curNode->dataKey[i])
                {
                    // Read from disk and load it to main memory
                    TreeNode *mainMemoryNode = (TreeNode *)index->readFromDisk(curNode->pointer[i+1], nodeSize);

                    // Update curNodeDiskAddress to pointer address for updates later
                    curNodeDiskAddress = curNode->pointer[i+1].blockAddress;
                    
                    // Move to new node in main memory
                    curNode = (TreeNode *)mainMemoryNode;
                    break;
                }
                
            }
        }


        // Reached a leaf node
        // If leaf node have space to insert a key, find out the place and insert record
        if(curNode->numOfKey < totalDataKey){
            int i = 0;

            // While its not the last key and key is larger than current key, keep moving to next key
            while (key > curNode->dataKey[i] && i < curNode->numOfKey)
            {
                i++;
            }
            
            // Check for duplicates at i 
            // This is where datakey[i] has space, insert key at i
            if(curNode->dataKey[i] == key){

                // If duplicate exists, it means linked list exists
                // Insert and update node to the linked list
                curNode->pointer[i] = insertLL(curNode->pointer[i], address, key);
            }

            // If no duplicates
            else
            {
                // Set last node pointer to a new pointer
                Address nextNode = curNode->pointer[curNode->numOfKey];

                // Move key backwards, move pointers also
                for(int j = curNode->numOfKey; j > i; j--){
                    // Preserve index order
                    curNode->dataKey[j] = curNode->dataKey[j - 1];
                    curNode->pointer[j] = curNode->pointer[j - 1];
                }



                // Create a new linked list
                TreeNode *nodeLL = new TreeNode(totalDataKey);
                nodeLL->dataKey[0] = key;
                nodeLL->isLeaf = false;
                nodeLL->numOfKey = 1;
                nodeLL->pointer[0] = address;

                // Write new linked list node to disk
                Address nodeLLAddress = index->writeToDisk((void *)nodeLL, nodeSize);

                // Update variables
                curNode->dataKey[i] = key;
                curNode->pointer[i] = nodeLLAddress;
                curNode->numOfKey++;

                // Update next node
                curNode->pointer[curNode->numOfKey] = nextNode;

                
                Address curNodeOriginalAddress{curNodeDiskAddress,0};
                index->writeToDisk(curNode, nodeSize, curNodeOriginalAddress);
                
            }
        }
        // If there is no space to insert new key, split the node into two and update the parent if required
        else
        {

            // Create new leaf node to insert keys and pointers
            TreeNode* newLeafNode = new TreeNode(totalDataKey);

            // Create temporary list for keys and pointers
            float tempKeyList[totalDataKey + 1];
            float tempPointerList[totalDataKey + 1];

            // Initialize next node
            Address nextNode = curNode->pointer[curNode->numOfKey];
                
            // Copy all keys and pointers to temp array
            int i;
            for(i=0; i < totalDataKey; i++){
                tempKeyList[i] = curNode->dataKey[i];
                tempPointerList = curNode->pointer[i];
            }

            // Get index where key can be inserted
            i = 0;
            while(key > tempKeyList[i] && i < totalDataKey){
                i++;
            }
            
            // Check if key at index i has duplicates
            if(i < totalDataKey && curNode->dataKey[i] == key){
                
                // If there is duplicates it means there is an existing linked list
                // Insert key into linked list
                curNode->pointer[i] = insertLL(curNode->pointer[i], address, key);
                return;
            }


            // If there is no duplicate then insert new key
            // First move temp list backwards by 1 key
            for(int j=totalDataKey; j > i; j--){

                tempKeyList[j] = tempKeyList[j-1];
                tempPointerList[j] = tempPointerList[j-1];
            }

            // Create new linked list
            TreeNode *nodeLL = new TreeNode(totalDataKey);
            nodeLL->dataKey[0] = key;
            nodeLL->isLeaf = false;
            nodeLL->numOfKey = 1;
            nodeLL->pointer[0] = address;

            // Write new linked list node to disk
            Address nodeLLAddress = index->writeToDisk((void *)nodeLL,nodeSize);

            // Insert new key into temp key and pointer list
            tempKeyList[i] = key;
            tempPointerList[i] = nodeLLAddress;

            // New node is a leaf node
            newLeafNode->isLeaf = true;


            // Set last pointer of new leaf node to point to previous last pointer of existing node (curNode)
            // curNode was full, so last pointer would use numOfKey 
            newLeafNode->pointer[newLeafNode->numOfKey] = nextNode;
            
            
            // Update other pointers, keys and parent nodes

            // First update current node's keys and pointers
            for(i=0; i < curNode->numOfKey; i++){
                curNode->dataKey[i] = tempKeyList[i];
                curNode->pointer[i] = tempPointerList[i];
            }

            // Next update new leaf node
            // Keep track of i index since we are using remaining keys and pointers
            for(int j=0; j < newLeafNode->numOfKey; i++, j++){
                newLeafNode->dataKey[j] = tempKeyList[i];
                newLeafNode->pointer[j] = tempPointerList[i];
            }

            // Write the leaf nodes to disk
            Address newLeafAddress = index->writeToDisk(newLeafNode,nodeSize);

            // Assign curNode pointer to new leaf address
            curNode->pointer[curNode->numOfKey] = newLeafAddress;

            // Set empty float for wrong keys
            for(i = curNode->numOfKey; i < totalDataKey; i++){
                curNode->dataKey[i] = float();
            }

            // Set null pointer for wrong pointers
            for(i = curNode->numOfKey+1; i < totalDataKey+1; i++){
                Address nullAddress{nullptr, 0};
                curNode->pointer[i] = nullAddress;
            }

            Address curNodeOriginalAddress{curNodeDiskAddress, 0};
            index->writeToDisk(curNode, nodeSize, curNodeOriginalAddress);





            // If curNode is at root level, then we need to make a new parent root
            if(curNode == root){
                TreeNode *newParentRoot = new TreeNode(totalDataKey);

                // Set new parent root's key to be left bound of the right child (new leaf node)
                newParentRoot->dataKey[0] = newLeafNode->dataKey[0];

                // Point new parent root's children as current node and new node
                Address curNodeDisk{curNodeDiskAddress, 0};
                newParentRoot->pointer[0] = curNodeDisk;
                newParentRoot->pointer[1] = newLeafAddress;

                // Update new parent root's variables
                newParentRoot->isLeaf = false;
                newParentRoot->numOfKey = 1;

                // Write new parent root to disk and update the root disk address
                Address newParentRootAddress = index->writeToDisk(newParentRoot,nodeSize);
                addressOfRoot = newParentRootAddress.blockAddress;
                root = newParentRoot;
            }
            // If curNode is not root, insert a new parent in the middle levels of the trees
            else 
            {

            }
        }
    }

    numOfNode = index->getNumBlocksAllocated();
}

// Function to insert record into existing linked list
Address BPTree::insertLL(Address curNodeAddress, Address address, float key)
{
    
    TreeNode *curNode = (TreeNode *)index->readFromDisk(curNodeAddress, nodeSize);

    if(curNode->numOfKey < totalDataKey){
        

        // Move all keys back by one space
        for(int i = curNode->numOfKey; i > 0; i--){
            curNode->dataKey[i] = curNode->dataKey[i-1];
        }

        // Move all pointers back by one space 
        for(int i = curNode->numOfKey + 1; i > 0; i-- ){
            curNode->pointer[i] = curNode->pointer[i-1];
        }

        // Insert new key at start index
        curNode->dataKey[0] = key;
        curNode->pointer[0] = address;
        curNode->numOfKey++;

        // Write back to disk
        curNodeAddress = index->writeToDisk((void *)curNode, nodeSize, curNodeAddress);
        return curNodeAddress;
    }
    // No space in the node, create a new linked list node
    else 
    {

        //Make a new node
        TreeNode *nodeLL = new TreeNode(totalDataKey);
        nodeLL->isLeaf = false;
        nodeLL->dataKey[0] = key;
        nodeLL->numOfKey = 1;

        // Insert key
        nodeLL->pointer[0] = address;

        // Insert previous node disk address as next
        nodeLL->pointer[1] = curNode;

        // Write new linked list to disk
        Address nodeLLAddress = index->writeToDisk((void *)nodeLL, nodeSize);
        return nodeLLAddress;
    }
}