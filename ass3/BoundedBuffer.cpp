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
        BoundedBuffer(int size) : buffer(size) {
            buffer_size(size);
            mutex(1);
            empty(size);
            full(0);
        }

        void insert(string s) {
            empty.acquire()
            mutex.acquire();

            buffer[next_in] = s;
            ++count;
            next_in = (next_in + 1) % buffer_size;

            mutex.release();
            full.release();
        }

        string remove() {
            full.acquire();
            mutex.acquire();

            string temp;
            temp = buffer[next_out];
            next_out = (next_out + 1) % buffer_size;

            mutex.release();
            empty.release();

            return temp;
        }
};
