#ifndef UNBOUNDEDBUFFER_H
#define UNBOUNDEDBUFFER_H

#include <semaphore.h>
#include <string>
#include <queue>
#include <iostream>

using namespace std;

class UnBoundedBuffer {
    private:
        sem_t mutex;
        queue<string> buffer;
        int count = 0;
        
     public:
        UnBoundedBuffer();
        void insert(string s);
        string remove();
        int getCount();
        //void print();
};


#endif