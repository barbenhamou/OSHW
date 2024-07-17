#include "UnBoundedBuffer.h"

using namespace std;

UnBoundedBuffer::UnBoundedBuffer() {
    sem_init(&mutex, 0, 0);
}

void UnBoundedBuffer::insert(string s) {
    sem_wait(&mutex);
    //mutex.acquire();

    buffer.push(s);
    count++;

    sem_post(&mutex);
    //mutex.release();
}

string UnBoundedBuffer::remove() {
    sem_wait(&mutex);
    //mutex.acquire();

    string temp = "";

    if (!buffer.empty()) {
        temp = buffer.front();
        buffer.pop();
        count--;
    }

    sem_post(&mutex);
    //mutex.release();

    return temp;
}

int UnBoundedBuffer::getCount() {
    return this->count;
}
