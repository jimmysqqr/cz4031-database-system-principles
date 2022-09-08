# cz4031-database-system-principles
### Compiling and running code
First I compile the disk_storage class file from the storage directory:
g++ -c disk_storage.cpp -o disk_storage.o
Then I go back to the main folder and compile main.cpp:
g++ -o main main.cpp storage/disk_storage.o
And I run main with:
./main
It seems damn weird and I think there should be a better way to do this? Pls advise
