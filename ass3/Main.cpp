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

const int numProducers = 3;
    
BoundedBuffer* producerBuffers = (BoundedBuffer*)malloc(numProducers * sizeof(BoundedBuffer));
Producer* producers = (Producer*)malloc(numProducers * sizeof(Producer));

UnBoundedBuffer sportsQueue;
UnBoundedBuffer newsQueue;
UnBoundedBuffer weatherQueue;
UnBoundedBuffer screenBuffer;

void* producerThreadFunction(void *arg) {
    Producer* producer = (Producer*)arg;
    producer->produce(&producerBuffers[producer->getId()]);
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
 
int main() {
    int producerIds[3] = {0, 1, 2};
    int msgsCount[3] = {3, 112, 987};
    int queueSizes[3] = {5, 5, 30};

    vector<pthread_t> producerHandles(3);

    pthread_t dispatcherHandle;

    pthread_t sportsEditorHandle, newsEditorHandle, weatherEditorHandle;

    pthread_t screenManagerHandle;

    for (int i = 0; i < 3; ++i) {
        producerBuffers[i] = BoundedBuffer(queueSizes[i]);
        producers[i] = Producer(producerIds[i], msgsCount[i]);
    }

    Dispatcher dispatcher(3);
    CoEditor sportsEditor;
    CoEditor newsEditor;
    CoEditor weatherEditor;
    ScreenManager screenManager;

    for (int i = 0; i < 3; ++i) {
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