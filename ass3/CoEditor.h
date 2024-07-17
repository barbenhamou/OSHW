#ifndef COEDITOR_H
#define COEDITOR_H

#include "UnBoundedBuffer.h"
#include <string>
#include <chrono>
#include <thread>

class CoEditor {
    // private:
    //     UnBoundedBuffer* msgsBuffer;
    //     UnBoundedBuffer* screenBuffer;
    public:
        CoEditor();
        void edit(UnBoundedBuffer* msgsBuffer, UnBoundedBuffer* screenBuffer);
};

#endif