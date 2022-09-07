#ifndef HELPER_TYPES_H
#define HELPER_TYPES_H

// Address of a record
struct Address {
    void *blockAddress;
    int offset;
};

struct Record {
    // Alphanumeric unique id of the movie. All tconsts are 9 chars long, so set length=10 (including \0)
    char tconst[10];
    float averageRating;
    int numVotes;
};

#endif