#include "CoEditor.h"

using namespace std;

CoEditor::CoEditor() {}

void CoEditor::edit(UnBoundedBuffer* msgsBuffer, UnBoundedBuffer* screenBuffer) {

    string temp;

    while (1) {

        temp = msgsBuffer->remove();
        if (!temp.compare("DONE")) {
            screenBuffer->insert("DONE");
            break;
        }

        this_thread::sleep_for(chrono::milliseconds(100));

        screenBuffer->insert(temp);

    }

}