#include "bp_tree/bp_tree.h"
#include "storage/disk_storage.h"
#include "storage/helper_types.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <unordered_map>

using namespace std;

int main()
{
    // =============== Read in data and save on disk ======================= //
    // User can set the block size
    cout << "Welcome! First enter the Block Size (200 or 500 Bytes): ";
    int BLOCKSIZE = 200;
    // cin >> BLOCKSIZE;

    // Error handling by setting the Block Size to 200 by default
    if (BLOCKSIZE != 200 && BLOCKSIZE != 500)
    {
        cout << "\nInvalid Block Size, it will default to 200 Bytes" << endl;
        BLOCKSIZE = 200;
    }
    cout << "Success! Block Size: " << BLOCKSIZE << "B" << endl;

    // Create a 150MB disk
    DiskStorage disk(150000000, BLOCKSIZE);

    // Create a 350MB disk (changeeeeeee)
    DiskStorage index(350000000, BLOCKSIZE);

    // Creating the tree
    BPTree bptree = BPTree(BLOCKSIZE, &disk, &index);
    cout << "\nMax keys in node of B+ tree (n): " << bptree.getTotalDataKey() << endl;
    cout << "Height of the B+ tree: " << bptree.getHeight() << endl;

    // Reset the no. of blocks accessed
    disk.resetNumBlocksAccessed();

    // Read in data
    ifstream file("data/data.tsv");

    // Check if file is open
    if (file.is_open())
    {
        cout << "\nReading in data..." << endl;
        cout << "Building B+ Index simultaneously..." << endl;

        // Each line in the file will be read into this variable
        string line;

        // Counter
        int recordNum = 0;

        // Flag to check if it is the first line of the tsv
        bool firstLine = true;

        while (getline(file, line))
        {
            // Throw away the first line because it is just column names
            if (firstLine)
            {
                firstLine = false;
                continue;
            }

            // Data will be saved into a Record
            Record record;

            stringstream linestream(line);
            string data;

            // Saving the tconst value
            strcpy(record.tconst, line.substr(0, line.find("\t")).c_str());
            getline(linestream, data, '\t');

            // Saving averageRating and numVotes into the record
            linestream >> record.averageRating >> record.numVotes;

            // Write this record to the disk
            Address address = disk.writeToDisk(&record, sizeof(Record));

            // Building B+ tree index on numVotes by inserting the records sequentially
            bptree.insertKey(address, record.numVotes);

            recordNum++;
        }
        cout << "Number of records read: " << recordNum << endl;
        file.close();
    }

    // Experiment 1
    cout << "\n\n============================== Experiment 1 ==============================" << endl;
    cout << "Storing the data on the disk...                  " << endl;
    cout << "\nNumber of blocks used for disk storage       : " << disk.getNumBlocksAllocated() << endl;
    cout << "Size of database used by allocated blocks      : " << disk.getTotalDiskUsage() << "B or " << disk.getTotalDiskUsage() / pow(10, 6) << "MB" << endl;
    cout << "Actual size of database used by saved records  : " << disk.getDiskUsage() << "B or " << disk.getDiskUsage() / pow(10, 6) << "MB" << endl;
    cout << "__________________________________________________________________________" << endl;

    // Reset the no. of blocks accessed for the next experiment
    disk.resetNumBlocksAccessed();

    // Experiment 2
    cout << "\n\n============================== Experiment 2 ==============================" << endl;
    cout << "B+ Tree was constructed on numVotes. Parameters...       " << endl;
    cout << "\nParameter n of the B+ Tree   : " << bptree.getTotalDataKey() << endl;
    cout << "Number of nodes of the B+ tree : " << bptree.getNumOfNodes() << endl;
    cout << "Height of the B+ tree          : " << bptree.getHeight() << endl;
    cout << "__________________________________________________________________________" << endl;

    // Reset the no. of blocks accessed for the next experiment
    disk.resetNumBlocksAccessed();

    // Experiment 3
    cout << "\n\n============================== Experiment 3 ==============================" << endl;
    // Displaying the average of “averageRating’s” of the records that are returned
    cout << "Average of 'averageRating' attribute of records retrirved : " << bptree.getAverageOfAverageRatings() << endl;
    cout << "__________________________________________________________________________" << endl;

    // Reset the no. of blocks accessed for the next experiment
    disk.resetNumBlocksAccessed();

    // Experiment 4
    cout << "\n\n============================== Experiment 4 ==============================" << endl;
    cout << "__________________________________________________________________________" << endl;

    // Reset the no. of blocks accessed for the next experiment
    disk.resetNumBlocksAccessed();

    // Experiment 5
    cout << "\n\n============================== Experiment 5 ==============================" << endl;
    cout << "__________________________________________________________________________" << endl;

    return 0;
}