<<<<<<< HEAD
#include "disk_storage.h"
#include "helper_types.h"

#include <iostream>
#include <vector>
#include <tuple>
#include <cstring>

// This class is used as the fraction of main memory that is allocated to be used as disk storage
// Refer to the header file for more details about the instance variables and class methods

// =============== Constructor ======================= //
DiskStorage::DiskStorage(std::size_t maxDiskCapacity, std::size_t blockSize){
    this->maxDiskCapacity = maxDiskCapacity;
    this->blockSize = blockSize;
    this->diskUsage = 0;
    this->totalDiskUsage = 0;
    this->blockUsage = 0;
    this->numBlocksAllocated = 0;
    this->numBlocksAccessed = 0;

    // Reserve part of the main memory for the "disk". operator new is basically similar to malloc
    this->disk = operator new(maxDiskCapacity);
    // Reserve memory by setting everything to null
    std::memset(disk, '\0', maxDiskCapacity);

    this->currentBlock = nullptr;

};

// =============== Class Methods ======================= //
// Allocates a block from the disk. Returns false if unsuccessful
bool DiskStorage::allocateBlock(){
    // Check if disk capacity will be exceeded if a new block is allocated
    if (totalDiskUsage + blockSize > maxDiskCapacity){
        std::cout << "Error: Disk is full, cannot allocate a new block: " << totalDiskUsage << "/" << maxDiskCapacity << '\n'; 
        return false;
    }

    // Update current block pointer
    currentBlock = (char *)disk + numBlocksAllocated*blockSize;

    // Reset the usage of the current block
    blockUsage = 0;

    // Increment number of blocks allocated
    numBlocksAllocated += 1;

    // Increase total disk usage
    totalDiskUsage += blockSize;

    return true;
}

// Allocates a record to one or more blocks. Returns the address where the new record should be inserted
Address DiskStorage::allocateRecord(std::size_t recordSize){
    // Check if record size is too large to fit into a block
    if (recordSize > blockSize){
        std::cout << "Error: Record size is too large for block. Record size: " << recordSize << ", Block size: " << blockSize << '\n';
        throw std::logic_error("Record size is too large for block");
    }

    // If no blocks have been allocated on disk yet, or record can't fit into the current block
    if (numBlocksAllocated==0 || (blockUsage + recordSize > blockSize)){
        // Allocate a new block
        if (!allocateBlock()){
            throw std::logic_error("Failed to allocate a new block");
        }
    }

    // Return the address where the new record should be inserted, in the form of {blockAddress, offset}
    Address recordAddress = {currentBlock, blockUsage};

    // Update the block usage, which is how much of the current block has already been used
    blockUsage += recordSize;

    // Update disk usage
    diskUsage += recordSize;

    return recordAddress;
};

// Deallocates a record from disk. Returns false if deallocation is unsuccessful
bool DiskStorage::deallocateRecord(Address address, std::size_t recordSize){
    try {
        // Get the address of the record which is to be deleted
        void *addressOfRecord = (char *)address.blockAddress + address.offset;

        // Delete the record by replacing it with null
        std::memset(addressOfRecord, '\0', recordSize);

        // Update the disk usage
        diskUsage -= recordSize;

        // Now I need to check if the block is completely empty after the record deletion
        // Create a temp block full of nulls
        unsigned char temp[blockSize];
        memset(temp, '\0', blockSize);

        // Compare the temp block with the actual block
        if (memcmp(temp, address.blockAddress, blockSize)==0){
            // The actual block is empty, hence update the necessary instance variables
            totalDiskUsage -= blockSize;
            numBlocksAllocated --;
        }
        return true;
    } catch (...) {
        std::cout << "Error: Unable to deallocate record at this block address: " << address.blockAddress << ", offset: " << address.offset << '\n';
        return false;
    };
};

// Reads data from a block on the disk
void *DiskStorage::readFromDisk(Address address, std::size_t size){
    // Malloc some other part of main memory
    void *mainMemAddress = operator new(size);

    // Simulate retrieving from disk to main memory
    std::memcpy(mainMemAddress, (char *)address.blockAddress + address.offset, size);

    // Update number of blocks accessed
    numBlocksAccessed++;

    return mainMemAddress; 
};

// Writes something to the disk. Returns the disk address
Address DiskStorage::writeToDisk(void *srcAddress, std::size_t size){
    // Allocate space on disk and get the destination address
    Address destAddress = allocateRecord(size);

    // Copy from src to dest
    std::memcpy((char *)destAddress.blockAddress + destAddress.offset, srcAddress, size);

    // Increment number of blocks accessed
    numBlocksAccessed++;

    return destAddress;
}

// Update data on disk if it has already been saved before
Address DiskStorage::writeToDisk(void *srcAddress, std::size_t size, Address destAddress){
    // Copy from src to dest
    std::memcpy((char *)destAddress.blockAddress + destAddress.offset, srcAddress, size);

    // Increment number of blocks accessed
    numBlocksAccessed++;

    return destAddress;
}

// Destructor
DiskStorage::~DiskStorage(){};
=======
#include "disk_storage.h"
#include "helper_types.h"

#include <iostream>
#include <vector>
#include <tuple>
#include <cstring>

// This class is used as the fraction of main memory that is allocated to be used as disk storage
// Refer to the header file for more details about the instance variables and class methods

// =============== Constructor ======================= //
DiskStorage::DiskStorage(std::size_t maxDiskCapacity, std::size_t blockSize)
{
    this->maxDiskCapacity = maxDiskCapacity;
    this->blockSize = blockSize;
    this->diskUsage = 0;
    this->totalDiskUsage = 0;
    this->blockUsage = 0;
    this->numBlocksAllocated = 0;
    this->numBlocksAccessed = 0;

    // Reserve part of the main memory for the "disk". operator new is basically similar to malloc
    this->disk = operator new(maxDiskCapacity);
    // Reserve memory by setting everything to null
    std::memset(disk, '\0', maxDiskCapacity);

    this->currentBlock = nullptr;
};

// =============== Class Methods ======================= //
// Allocates a block from the disk. Returns false if unsuccessful
bool DiskStorage::allocateBlock()
{
    // Check if disk capacity will be exceeded if a new block is allocated
    if (totalDiskUsage + blockSize > maxDiskCapacity)
    {
        std::cout << "Error: Disk is full, cannot allocate a new block: " << totalDiskUsage << "/" << maxDiskCapacity << '\n';
        return false;
    }

    // Update current block pointer
    currentBlock = (char *)disk + numBlocksAllocated * blockSize;

    // Reset the usage of the current block
    blockUsage = 0;

    // Increment number of blocks allocated
    numBlocksAllocated += 1;

    // Increase total disk usage
    totalDiskUsage += blockSize;

    return true;
}

// Allocates a record to one or more blocks. Returns the address where the new record should be inserted
Address DiskStorage::allocateRecord(std::size_t recordSize)
{
    // Check if record size is too large to fit into a block
    if (recordSize > blockSize)
    {
        std::cout << "Error: Record size is too large for block. Record size: " << recordSize << ", Block size: " << blockSize << '\n';
        throw std::logic_error("Record size is too large for block");
    }

    // If no blocks have been allocated on disk yet, or record can't fit into the current block
    if (numBlocksAllocated == 0 || (blockUsage + recordSize > blockSize))
    {
        // Allocate a new block
        if (!allocateBlock())
        {
            throw std::logic_error("Failed to allocate a new block");
        }
    }

    // Return the address where the new record should be inserted, in the form of {blockAddress, offset}
    Address recordAddress = {currentBlock, blockUsage};

    // Update the block usage, which is how much of the current block has already been used
    blockUsage += recordSize;

    // Update disk usage
    diskUsage += recordSize;

    return recordAddress;
};

// Deallocates a record from disk. Returns false if deallocation is unsuccessful
bool DiskStorage::deallocateRecord(Address address, std::size_t recordSize)
{
    try
    {
        // Get the address of the record which is to be deleted
        void *addressOfRecord = (char *)address.blockAddress + address.offset;

        // Delete the record by replacing it with null
        std::memset(addressOfRecord, '\0', recordSize);

        // Update the disk usage
        diskUsage -= recordSize;

        // Now I need to check if the block is completely empty after the record deletion
        // Create a temp block full of nulls
        unsigned char temp[blockSize];
        memset(temp, '\0', blockSize);

        // Compare the temp block with the actual block
        if (memcmp(temp, address.blockAddress, blockSize) == 0)
        {
            // The actual block is empty, hence update the necessary instance variables
            totalDiskUsage -= blockSize;
            numBlocksAllocated--;
        }
        return true;
    }
    catch (...)
    {
        std::cout << "Error: Unable to deallocate record at this block address: " << address.blockAddress << ", offset: " << address.offset << '\n';
        return false;
    };
};

// Reads data from a block on the disk
void *DiskStorage::readFromDisk(Address address, std::size_t size)
{
    // Malloc some other part of main memory
    void *mainMemAddress = operator new(size);

    // Simulate retrieving from disk to main memory
    std::memcpy(mainMemAddress, (char *)address.blockAddress + address.offset, size);

    // Update number of blocks accessed
    numBlocksAccessed++;

    return mainMemAddress;
};

// Writes something to the disk. Returns the disk address
Address DiskStorage::writeToDisk(void *srcAddress, std::size_t size)
{
    // Allocate space on disk and get the destination address
    Address destAddress = allocateRecord(size);

    // Copy from src to dest
    std::memcpy((char *)destAddress.blockAddress + destAddress.offset, srcAddress, size);

    // Increment number of blocks accessed
    numBlocksAccessed++;

    return destAddress;
}

// Update data on disk if it has already been saved before
Address DiskStorage::writeToDisk(void *srcAddress, std::size_t size, Address destAddress)
{
    // Copy from src to dest
    std::memcpy((char *)destAddress.blockAddress + destAddress.offset, srcAddress, size);

    // Increment number of blocks accessed
    numBlocksAccessed++;

    return destAddress;
}

// Destructor
DiskStorage::~DiskStorage(){};
>>>>>>> origin/main
