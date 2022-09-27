# CZ4031 - Database System Principles

### Compiling and Running the project

1. Compile the disk_storage class file from the main diretory: <br />
   `g++ -c storage/disk_storage.cpp -o storage/disk_storage.o` <br /><br />
2. Compile the bp_tree class file from the main diretory: <br />
   `g++ -c bp_tree/bp_tree.cpp -o bp_tree/bp_tree.o` <br /><br />
3. Link the object files with main.cpp to produce an executable file: <br />
   `g++ -o main main.cpp storage/disk_storage.o bp_tree/bp_tree.o` <br /><br />
4. Finally, run main.exe with: <br />
   `./main` <br /><br />

We gots to make a cmake file.
