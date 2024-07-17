#include "Dispatcher.h"

Dispatcher::Dispatcher(int numProducers) 
: numProducers(numProducers) {
    //this->producerBuffers = producerBuffers;
    // this->sportsBuffer = sportsBuffer;
    // this->newsBuffer = newsBuffer;
    // this->weatherBuffer = weatherBuffer;
}


void Dispatcher::dispatch(BoundedBuffer* producerBuffers, UnBoundedBuffer *sportsBuffer, UnBoundedBuffer *newsBuffer, UnBoundedBuffer *weatherBuffer) {
    int amountDone = 0;
    string ret;
    while (amountDone < this->numProducers) {
        for (int j = 0; j < this->numProducers; ++j) {

            //producerBuffers[j].print();

            string ret = producerBuffers[j].remove();

            if (!ret.compare("")) {
                continue;
            }

            if (!ret.compare("DONE")) {
                amountDone++;
                break;  // Exit the inner loop to move to the next producer
            }

            if (ret.find("SPORTS") != string::npos) {
                sportsBuffer->insert(ret);
            } else if (ret.find("NEWS") != string::npos) {
                newsBuffer->insert(ret);
            } else if (ret.find("WEATHER") != string::npos) {
                weatherBuffer->insert(ret);
            }
        }
    }

    //cout << "Dispatcher done\n";

    if (amountDone == this->numProducers) {
        sportsBuffer->insert("DONE");
        newsBuffer->insert("DONE");
        weatherBuffer->insert("DONE");
    }

    // sportsBuffer->print();
    // newsBuffer->print();
    // weatherBuffer->print();
}
