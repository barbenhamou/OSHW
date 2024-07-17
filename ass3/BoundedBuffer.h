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
        int count = 0;
        int next_in = 0;
        int next_out = 0;
        int buffer_size;
        
    
     public:
        BoundedBuffer(int size);
        void insert(string s);
        string remove();
};


#endif