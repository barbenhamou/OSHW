#include <semaphore>
#include <string>
#include <queue>

using namespace std;

class UnBoundedBuffer {
    private:
        binary_semaphore mutex;
        queue<string> buffer;
    
    public:
        UnBoundedBuffer() : mutex(1) {}

        void insert(string s) {
            mutex.acquire();

            buffer.push(s);

            mutex.release();
        }

        string remove() {
            mutex.acquire();

            string temp = "";

            if (!buffer.empty()) {
                temp = buffer.front();
                buffer.pop();
            }

            mutex.release();

            return temp;
        }
};
