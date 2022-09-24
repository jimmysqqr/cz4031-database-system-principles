#include "bp_tree.h"
#include "helper_types.h"

#include <vector>
#include <cstring>
#include <iostream>


// Function to insert data into nodes
void BPTree::insert(Address address, float key)
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
    }
}

// Function to insert record into existing linked list
Address BPTree::insertLL(Address curNodeAddress, Address address, float key){
    
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