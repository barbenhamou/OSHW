#ifndef BOUNDEDBUFFER_H
#define BOUNDEDBUFFER_H

#include <semaphore>
#include <string>
#include <vector>

using namespace std;

class BoundedBuffer {
    private:
        binary_semaphore mutex;
        counting_semaphore<> empty, full;
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