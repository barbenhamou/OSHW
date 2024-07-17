#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include <string>
#include "UnBoundedBuffer.h"

class ScreenManager {
    private:
        UnBoundedBuffer* screenBuffer;
    
    public:
        ScreenManager(UnBoundedBuffer* screenBuffer);
        void collectData();
}

#endif