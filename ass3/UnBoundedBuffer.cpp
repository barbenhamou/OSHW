#include "UnBoundedBuffer.h"

using namespace std;

UnBoundedBuffer::UnBoundedBuffer() {
    sem_init(&mutex, 0, 1);
}

void UnBoundedBuffer::insert(string s) {
    sem_wait(&mutex);
    //mutex.acquire();

    //cout << "In Critical Insert\n";

    if (s.compare("")) {
        buffer.push(s);
        count++;
    }

    //cout << "Out Critical Insert\n";


    sem_post(&mutex);
    //mutex.release();
}

string UnBoundedBuffer::remove() {
    sem_wait(&mutex);
    //mutex.acquire();

    //cout << "In Critical Remove\n";


    string temp = "";

    if (!buffer.empty()) {
        temp = buffer.front();
        buffer.pop();
        count--;
    }

    //cout << "Out Critical Remove\n";

    sem_post(&mutex);
    //mutex.release();

    return temp;
}

int UnBoundedBuffer::getCount() {
    return this->count;
}

void UnBoundedBuffer::print() {
    sem_wait(&mutex);

    queue<string> temp;

    while (!this->buffer.empty()) {
        cout << this->buffer.front() << "\n";
        temp.push(this->buffer.front());
        this->buffer.pop();
    }

    while (!temp.empty()) {
        this->buffer.push(temp.front());
        temp.pop();
    }

    sem_post(&mutex);
}