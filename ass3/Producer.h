#pragma once

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

#include "BoundedBuffer.h"

class Producer {
    private:
        int SPORTS_COUNTER;
        int NEWS_COUNTER;
        int WEATHER_COUNTER;
        int id;
        int msgCount;
        //BoundedBuffer* buffer;
    
    public:
        Producer(int id, int msgCount);
        string buildString();
        void produce(BoundedBuffer* buffer);
        int getId();
};
