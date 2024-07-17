#include "ScreenManager.h"

using namespace std;

ScreenManager::ScreenManager() {}

void ScreenManager::collectData(UnBoundedBuffer* screenBuffer) {
    int doneCount = 0;
    
    string temp;

    while (doneCount < 3) {
        temp = screenBuffer->remove();
        if (temp.compare("DONE")) {
            doneCount++;
            continue;
        }

        cout << temp << "\n";
    }

    cout << "DONE";
}