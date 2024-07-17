#ifndef BOUNDEDBUFFER_H
#define BOUNDEDBUFFER_H

#include <semaphore.h>
#include <string>
#include <fcntl.h>
#include <iostream>
#include <vector>

using namespace std;

class BoundedBuffer {
    private:
        sem_t mutex;
        sem_t empty, full;
        vector<string> buffer;
        int count;
        int next_in;
        int next_out;
        int buffer_size;
        
    
     public:
        BoundedBuffer(int size);
        void insert(string s);
        string remove();
};


#endif