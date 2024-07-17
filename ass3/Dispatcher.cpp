#include "Dispatcher.h"

Dispatcher::Dispatcher(int numProducers, vector<BoundedBuffer> producerBuffers, UnBoundedBuffer *sportsBuffer, UnBoundedBuffer *newsBuffer, UnBoundedBuffer *weatherBuffer) 
: numProducers(numProducers), producerBuffers(producerBuffers), sportsBuffer(sportsBuffer), newsBuffer(newsBuffer), weatherBuffer(weatherBuffer) {}

void Dispatcher::dispatch() {
    int amountDone = 0;
    string ret;
    while (amountDone < this->numProducers) {
        for (auto buffer : producerBuffers) {
            ret = buffer.remove();

            if (!ret.compare("")) {
                continue;
            }

            if (!ret.compare("DONE")) {
                amountDone++;
                continue;
            }

            if (ret.find("SPORTS")) {
                sportsBuffer->insert(ret);
            } else if (ret.find("NEWS")) {
                newsBuffer->insert(ret);
            } else if (ret.find("WEATHER")) {
                weatherBuffer->insert(ret);
            }

        }
    }

    sportsBuffer->insert("DONE");
    newsBuffer->insert("DONE");
    weatherBuffer->insert("DONE");
}