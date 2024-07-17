#ifndef COEDITOR_H
#define COEDITOR_H

#include "UnBoundedBuffer.h"
#include <string>
#include <chrono>
#include <thread>

class CoEditor {
    private:
        UnBoundedBuffer* msgsBuffer;
        UnBoundedBuffer* screenBuffer;
    public:
        CoEditor(UnBoundedBuffer* msgsBuffer, UnBoundedBuffer* screenBuffer);
        void edit();
};

#endif