#ifndef UNBOUNDEDBUFFER_H
#define UNBOUNDEDBUFFER_H

#include <semaphore>
#include <string>
#include <queue>

using namespace std;

class UnBoundedBuffer {
    private:
        binary_semaphore mutex;
        queue<string> buffer;
        int count = 0;
        
     public:
        UnBoundedBuffer();
        void insert(string s);
        string remove();
        int getCount();
};


#endif