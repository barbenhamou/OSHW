#include "UnBoundedBuffer.h"
#include <iostream>
#include <chrono>
#include <fstream>
#include <pthread.h>
#include "BoundedBuffer.h"
#include "Producer.h"
#include "Dispatcher.h"
#include "CoEditor.h"
#include "ScreenManager.h"

using namespace std;

BoundedBuffer* producerBuffers;
Producer* producers;

UnBoundedBuffer sportsQueue;
UnBoundedBuffer newsQueue;
UnBoundedBuffer weatherQueue;
UnBoundedBuffer screenBuffer;


vector<vector<int>> analyzeFile(const char* filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        if (file.bad()) {
            cout << "bad file\n";
        }


        cout << strerror(errno) << "\n";

        return {}; // Return an empty vector on failure
    }

    int id, msgs, size;
    vector<vector<int>> ret;
    string line;

    while (getline(file, line)) {
        if (line.find("Co-Editor") != string::npos) {
            continue;
        }

        if (line.find("PRODUCER") != string::npos) {
            line.erase(0, line.find(" ") + 1);
            id = stoi(line);

            getline(file, line);
            msgs = stoi(line);

            getline(file, line);
            line.erase(0, line.find(" = ") + 3);
            size = stoi(line);

            ret.push_back({id, msgs, size});
        }
    }

    return ret;
}

void* producerThreadFunction(void *arg) {
    Producer* producer = (Producer*)arg;
    producer->produce(&producerBuffers[producer->getId() - 1]);
    return nullptr;
}

void* dispatcherThreadFunction(void *arg) {
    Dispatcher* dispatcher = (Dispatcher*)arg;
    dispatcher->dispatch(producerBuffers, &sportsQueue, &newsQueue, &weatherQueue);
    return nullptr;
}

void* sportsCoEditorThreadFunction(void *arg) {
    CoEditor* editor = (CoEditor*)arg;
    editor->edit(&sportsQueue, &screenBuffer);
    return nullptr;
}

void* newsCoEditorThreadFunction(void *arg) {
    CoEditor* editor = (CoEditor*)arg;
    editor->edit(&newsQueue, &screenBuffer);
    return nullptr;
}


void* weatherCoEditorThreadFunction(void *arg) {
    CoEditor* editor = (CoEditor*)arg;
    editor->edit(&weatherQueue, &screenBuffer);
    return nullptr;
}

void* screenManagerThreadFunction(void *arg) {
    ScreenManager* screenManager = (ScreenManager*)arg;
    screenManager->collectData(&screenBuffer);
    return nullptr;
}
 
int main(int argc, char* argv[]) {

    ifstream file(argv[1]);
    if (!file.is_open()) {
        if (file.bad()) {
            cout << "bad file\n";
        }


        cout << strerror(errno) << "\n";

        return {}; // Return an empty vector on failure
    }

    int id, msgs, size;
    vector<vector<int>> ret;
    string line;

    while (getline(file, line)) {
        if (line.find("Co-Editor") != string::npos) {
            continue;
        }

        if (line.find("PRODUCER") != string::npos) {
            line.erase(0, line.find(" ") + 1);
            id = stoi(line);

            getline(file, line);
            msgs = stoi(line);

            getline(file, line);
            line.erase(0, line.find(" = ") + 3);
            size = stoi(line);

            ret.push_back({id, msgs, size});
        }
    }
    
    producerBuffers = (BoundedBuffer*)malloc(ret.size() * sizeof(BoundedBuffer));
    producers = (Producer*)malloc(ret.size() * sizeof(Producer));

    vector<pthread_t> producerHandles(ret.size());

    pthread_t dispatcherHandle;

    pthread_t sportsEditorHandle, newsEditorHandle, weatherEditorHandle;

    pthread_t screenManagerHandle;

    int id1, size1, count1;

    for (int i = 0; i < ret.size(); ++i) {
        id1 = ret[i][0];
        size1 = ret[i][2];
        count1 = ret[i][1];
        producerBuffers[i] = BoundedBuffer(size1);
        producers[i] = Producer(id1, count1);
    }


    Dispatcher dispatcher(ret.size());
    CoEditor sportsEditor;
    CoEditor newsEditor;
    CoEditor weatherEditor;
    ScreenManager screenManager;

    for (int i = 0; i < ret.size(); ++i) {
        pthread_create(&producerHandles[i], nullptr, producerThreadFunction, &producers[i]);
        this_thread::sleep_for(chrono::milliseconds(1));

    }

    pthread_create(&dispatcherHandle, nullptr, dispatcherThreadFunction, &dispatcher);


    pthread_create(&sportsEditorHandle, nullptr, sportsCoEditorThreadFunction, &sportsEditor);
    pthread_create(&newsEditorHandle, nullptr, newsCoEditorThreadFunction, &newsEditor);
    pthread_create(&weatherEditorHandle, nullptr, weatherCoEditorThreadFunction, &weatherEditor);


    pthread_create(&screenManagerHandle, nullptr, screenManagerThreadFunction, &screenManager);

    for (auto& thread : producerHandles) {
        pthread_join(thread, nullptr);
    }

    pthread_join(dispatcherHandle, nullptr);

    pthread_join(sportsEditorHandle, nullptr);
    pthread_join(newsEditorHandle, nullptr);
    pthread_join(weatherEditorHandle, nullptr);
    pthread_join(screenManagerHandle, nullptr);
    
    return 0;
}