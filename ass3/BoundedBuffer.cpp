#include "BoundedBuffer.h"

using namespace std;

BoundedBuffer::BoundedBuffer(int size) : buffer(size), buffer_size(size), next_in(0), next_out(0), count(0) {
        sem_init(&mutex, 0, 0);
        sem_init(&empty, 0, size);
        sem_init(&full, 0, 0);
    } 

void BoundedBuffer::insert(string s) {
    sem_wait(&empty);
    sem_wait(&mutex);
    //empty.acquire();
    //mutex.acquire();

    buffer[next_in] = s;
    ++count;
    next_in = (next_in + 1) % buffer_size;

    sem_post(&mutex);
    sem_post(&full);
    // mutex.release();
    // full.release();
}

string BoundedBuffer::remove() {
    if (sem_trywait(&full) != 0) {
        return ""; //indicates a problem
    }

    if (sem_trywait(&mutex) != 0) {
        sem_post(&full);
        return ""; //indicates a problem
    }

    string temp;
    temp = buffer[next_out];
    next_out = (next_out + 1) % buffer_size;

    sem_post(&mutex);
    sem_post(&empty);

    return temp;
}
