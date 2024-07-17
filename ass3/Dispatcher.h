#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "UnBoundedBuffer.h"
#include "BoundedBuffer.h"
#include <string.h>

class Dispatcher {
    private:
        //vector<BoundedBuffer> producerBuffers;
        UnBoundedBuffer *sportsBuffer, *newsBuffer, *weatherBuffer;
        int numProducers;
    public:
        Dispatcher(int numProducers, UnBoundedBuffer *sportsBuffer, UnBoundedBuffer *newsBuffer, UnBoundedBuffer *weatherBuffer);
        void dispatch(vector<BoundedBuffer> producerBuffers);
};

#endif