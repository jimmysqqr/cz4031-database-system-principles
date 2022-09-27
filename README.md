# CZ4031 - Database System Principles

### Compiling and Running the code
First compile the disk_storage class file from the main diretory: <br />
```g++ -c storage/disk_storage.cpp -o storage/disk_storage.o``` <br /><br />
Then compile main.cpp: <br />
```g++ -o main main.cpp storage/disk_storage.o``` <br /><br />
Finally, run main with: <br />
```./main``` <br /><br />

It seems damn weird and I think there should be a better way to do this? I don't recall doing it this way when I used c++ in year 2. Pls advise tq

We gots to make a cmake file.
