#include "disk_storage.h"
#include "helper_types.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <unordered_map>

using namespace std;

int main(){
    // Set blockSize to be 200B
    int BLOCKSIZE = 200;

    // =============== Experiment 1 ======================= //
    // Create a 150MB disk
    DiskStorage disk(150000000, BLOCKSIZE);

    // Reset the no. of blocks accessed
    disk.resetNumBlocksAccessed();

    // Read in data
    std::cout << "Reading in data..." << endl;
    std::ifstream file("../data/data.tsv");

    // Check if file is open
    if (file.is_open()){
        std::string line;
        int recordNum = 0;

        while(std::getline(file, line)){
            Record record;
            stringstream linestream(line);
            string data;

            

        }
    }
}