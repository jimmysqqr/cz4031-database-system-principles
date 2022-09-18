#include "storage/disk_storage.h"
#include "storage/helper_types.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <math.h>

using namespace std;

int main() {
    // =============== Read in data and save on disk ======================= //
    // Set blockSize to be 200B
    int BLOCKSIZE = 200;

    // Create a 150MB disk
    DiskStorage disk(150000000, BLOCKSIZE);

    // Reset the no. of blocks accessed
    disk.resetNumBlocksAccessed();

    // Read in data
    std::ifstream file("data/data.tsv");

    // Check if file is open
    if (file.is_open()){
        std::cout << "Reading in data..." << endl;
        // Each line in the file will be read into this variable
        std::string line;

        // Counter
        int recordNum = 0;

        // Flag to check if it is the first line of the tsv
        bool firstLine = true;

        while(std::getline(file, line)){
            // Throw away the first line because it is just column names
            if (firstLine) {
                firstLine = false;
                continue;
            }
            
            // Data will be saved into a Record
            Record record;

            stringstream linestream(line);
            string data;

            // Saving the tconst value
            strcpy(record.tconst, line.substr(0, line.find("\t")).c_str());
            std::getline(linestream, data, '\t');

            // Saving averageRating and numVotes into the record
            linestream >> record.averageRating >> record.numVotes;

            // Write this record to the disk
            Address address = disk.writeToDisk(&record, sizeof(Record));

            recordNum++;
        }
        cout << "Number of records: " << recordNum << endl;
        file.close();
    }

    // =============== Experiment 1 ======================= //
    // Completed the disk storage part for experiment 1
    std::cout << "Number of blocks used for disk storage: " << disk.getNumBlocksAllocated() << endl;
    std::cout << "Size of database used by allocated blocks: " << disk.getTotalDiskUsage() << "B, " << disk.getTotalDiskUsage()/pow(10, 6) << "MB"<< endl;
    std::cout << "Actual size of database used up by saved records: "<< disk.getDiskUsage() << "B, " << disk.getDiskUsage()/pow(10, 6) << "MB"<< endl;

    // TODO: Add in the blocks used by B+ tree and size of B+ tree
}