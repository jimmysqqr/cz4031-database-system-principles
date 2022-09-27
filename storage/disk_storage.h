#ifndef DISK_STORAGE_H
#define DISK_STORAGE_H

#include "helper_types.h"

#include <vector>
#include <unordered_map>
#include <tuple>

class DiskStorage
{
private:
    // =============== Instance Variables ================ //
    std::size_t maxDiskCapacity; // Max size of main memory which is allocated to be used as disk storage
    std::size_t blockSize;       // Self explanatory
    std::size_t diskUsage;       // Size of the disk used up by records in storage
    std::size_t totalDiskUsage;  // Total size of the disk used up by allocated blocks
    std::size_t blockUsage;      // Size used up within the current block
    int numBlocksAllocated;      // Number of blocks allocated
    int numBlocksAccessed;       // Number of blocks accessed
    void *disk;                  // Pointer to the disk
    void *currentBlock;          // Pointer to the current block we are inserting to

public:
    // =============== Constructor ======================= //
    // This is the fraction of main memory that is allocated to be used as disk storage
    // maxDiskCapacity: Max size of main memory which is allocated to be used as disk storage
    // blockSize: Self explanatory
    DiskStorage(std::size_t maxDiskCapacity, std::size_t blockSize);

    // =============== Setters & Getters ================ //
    std::size_t getMaxDiskCapacity() const
    {
        return maxDiskCapacity;
    };

    std::size_t getBlockSize() const
    {
        return blockSize;
    };

    std::size_t getDiskUsage() const
    {
        return diskUsage;
    };

    std::size_t getTotalDiskUsage() const
    {
        return totalDiskUsage;
    };

    std::size_t getBlockUsage() const
    {
        return blockUsage;
    };

    int getNumBlocksAllocated() const
    {
        return numBlocksAllocated;
    };

    int getNumBlocksAccessed() const
    {
        return numBlocksAccessed;
    };

    // This function resets the number of blocks accessed, and returns the previous value
    int resetNumBlocksAccessed()
    {
        int oldValue = numBlocksAccessed;
        numBlocksAccessed = 0;
        return oldValue;
    };

    // =============== Class Methods ==================== //
    // Allocates a block from the disk. This method is used by the function allocateRecord
    bool allocateBlock();

    // Allocates a record to the disk. Allocates a new block if record exceeds the current free block.
    Address allocateRecord(std::size_t recordSize);

    // Deallocates a record from the disk
    bool deallocateRecord(Address address, std::size_t recordSize);

    // Read data from disk
    void *readFromDisk(Address address, std::size_t size);

    // Write data to disk with a source address
    Address writeToDisk(void *srcAddress, std::size_t size);

    // Update data on disk with a source address
    Address writeToDisk(void *srcAddress, std::size_t size, Address destAddress);

    // =============== Destructor ==================== //
    ~DiskStorage();
};
#endif