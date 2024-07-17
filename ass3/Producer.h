#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

#include "BoundedBuffer.h"

class Producer {
    public:
        static int SPORTS_COUNTER;
        static int NEWS_COUNTER;
        static int WEATHER_COUNTER;

    private:
        int id;
        int msgCount;
        BoundedBuffer* buffer;
    
    public:
        Producer(int id, int msgCount, BoundedBuffer* buffer);
        string buildString();
        void produce();
};
