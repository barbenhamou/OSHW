#include "BoundedBuffer.h"

using namespace std;

BoundedBuffer::BoundedBuffer(int size)
    : buffer(size), buffer_size(size), mutex(1), empty(size), full(0) {}

void BoundedBuffer::insert(string s) {
    empty.acquire();
    mutex.acquire();

    buffer[next_in] = s;
    ++count;
    next_in = (next_in + 1) % buffer_size;

    mutex.release();
    full.release();
}

string BoundedBuffer::remove() {
    if (!full.try_acquire()) {
        return ""; //indicates a problem
    }

    if (!mutex.try_acquire()) {
        full.release();
        return ""; //indicates a problem
    }

    string temp;
    temp = buffer[next_out];
    next_out = (next_out + 1) % buffer_size;

    mutex.release();
    empty.release();

    return temp;
}

int main() {
    return 0;
}