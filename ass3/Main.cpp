#include <iostream>
#include <fstream>
#include <pthread.h>
#include "BoundedBuffer.h"
#include "UnBoundedBuffer.h"
#include "Producer.h"
#include "Dispatcher.h"
#include "CoEditor.h"
#include "ScreenManager.h"

using namespace std;

void* producerThreadFunction(void *arg) {
    Producer* producer = (Producer*)arg;
    producer->produce();
    return nullptr;
}

void* dispatcherThreadFunction(void *arg) {
    Dispatcher* dispatcher = (Dispatcher*)arg;
    dispatcher->dispatch();
    return nullptr;
}

void* coEditorThreadFunction(void *arg) {
    CoEditor* editor = (CoEditor*)arg;
    editor->edit();
    return nullptr;
}

void* screenManagerThreadFunction(void *arg) {
    ScreenManager* screenManager = (ScreenManager*)arg;
    screenManager->collectData();
    return nullptr;
}
 
int main(int argc, char *argv[])
{
    int producerIds[3] = {1, 2, 3};
    int msgsCount[3] = {30, 25, 16};
    int queueSizes[3] = {5, 3, 30};

    vector<pthread_t> producerHandles(3);

    pthread_t dispatcherHandle;

    pthread_t sportsEditorHandle, newsEditorHandle, weatherEditorHandle;

    pthread_t screenManagerHandle;

    
    vector<BoundedBuffer> producerBuffers{0};
    vector<Producer> producers{0};

    UnBoundedBuffer sportsQueue;
    UnBoundedBuffer newsQueue;
    UnBoundedBuffer weatherQueue;
    UnBoundedBuffer screenBuffer;

    for (int i = 0; i < 3; ++i) {
        producerBuffers.emplace_back(queueSizes[i]);
        producers.emplace_back(producerIds[i], msgsCount[i], &producerBuffers[i]);
    }

    Dispatcher dispatcher(3 ,producerBuffers, &sportsQueue, &newsQueue, &weatherQueue);
    CoEditor sportsEditor(&sportsQueue, &screenBuffer);
    CoEditor newsEditor(&newsQueue, &screenBuffer);
    CoEditor weatherEditor(&weatherQueue, &screenBuffer);
    ScreenManager screenManager(&screenBuffer);

    for (int i = 0; i < 3; ++i) {
        pthread_create(&producerHandles[i], nullptr, producerThreadFunction, &producers[i]);
    }

    pthread_create(&dispatcherHandle, nullptr, dispatcherThreadFunction, &dispatcher);
    pthread_create(&sportsEditorHandle, nullptr, coEditorThreadFunction, &sportsEditor);
    pthread_create(&newsEditorHandle, nullptr, coEditorThreadFunction, &newsEditor);
    pthread_create(&weatherEditorHandle, nullptr, coEditorThreadFunction, &weatherEditor);
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