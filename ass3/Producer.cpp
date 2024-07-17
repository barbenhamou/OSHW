#include "Producer.h"

using namespace std;

Producer::Producer(int id, int msgCount) 
: id(id), msgCount(msgCount), NEWS_COUNTER(0), SPORTS_COUNTER(0), WEATHER_COUNTER(0) {}

string Producer::buildString() {
    srand(time(0) + this->id);

    string builder = "Producer ", type ="";
    int rnd = rand() % 3, j = 0;
    switch (rnd) {
        case 0:
            type = "SPORTS";
            j = this->SPORTS_COUNTER;
            this->SPORTS_COUNTER++;
            break;

        case 1:
            type = "NEWS";
            j = this->NEWS_COUNTER;
            this->NEWS_COUNTER++;
            break;

        case 2:
            type = "WEATHER";
            j = this->WEATHER_COUNTER;
            this->WEATHER_COUNTER++;
            break;
    }

    builder = builder.append(to_string(this->id));
    builder = builder.append(" ");
    builder = builder.append(type);
    builder = builder.append(" ");
    builder = builder.append(to_string(j));
    return builder;
} 

void Producer::produce(BoundedBuffer* buffer) {
    string temp = "";

    for (int i = 0; i < msgCount; ++i) {
        temp = buildString();
        buffer->insert(temp);
    }

    buffer->insert("DONE");

    cout << to_string(this->id) << " Done\n";
}

int Producer::getId() {
    return id;
}
