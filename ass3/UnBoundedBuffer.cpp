#include "UnBoundedBuffer.h"

using namespace std;

UnBoundedBuffer::UnBoundedBuffer() : mutex(1), count(0) {}

void UnBoundedBuffer::insert(string s) {
    mutex.acquire();

    buffer.push(s);
    count++;

    mutex.release();
}

string UnBoundedBuffer::remove() {
    mutex.acquire();

    string temp = "";

    if (!buffer.empty()) {
        temp = buffer.front();
        buffer.pop();
        count--;
    }

    mutex.release();

    return temp;
}

int UnBoundedBuffer::getCount() {
    return this->count;
}