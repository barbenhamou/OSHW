#include "CoEditor.h"

using namespace std;

CoEditor::CoEditor(UnBoundedBuffer* msgsBuffer, UnBoundedBuffer* screenBuffer) : msgsBuffer(msgsBuffer), screenBuffer(screenBuffer) {}

void CoEditor::edit() {

    string temp;

    while (1) {

        temp = msgsBuffer->remove();
        if (!temp.compare("DONE")) {
            this->screenBuffer->insert("DONE");
            break;
        }

        this_thread::sleep_for(chrono::milliseconds(100));

        screenBuffer->insert(temp);

    }

}