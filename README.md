# cz4031-database-system-principles
### Compiling and running code
First I compile the disk_storage class file from the main directory: <br />
```g++ -c storage/disk_storage.cpp -o storage/disk_storage.o``` <br /><br />
Then I go back to the main folder and compile main.cpp: <br />
```g++ -o main main.cpp storage/disk_storage.o``` <br /><br />
And I run main with: <br />
```./main``` <br /><br />
It seems damn weird and I think there should be a better way to do this? I don't recall doing it this way when I used c++ in year 2. Pls advise tq
