#include "Producer.h"

using namespace std;

int Producer::NEWS_COUNTER = 0, Producer::SPORTS_COUNTER = 0, Producer::WEATHER_COUNTER = 0;

Producer::Producer(int id, int msgCount, BoundedBuffer* buffer) : id(id), msgCount(msgCount), buffer(buffer) {}

string Producer::buildString() {
    srand(time(0) + id);

    string builder = "Producer ", type ="";
    int rnd = rand() % 3, j = 0;
    switch (rnd) {
        case 0:
            type = "SPORTS";
            j = SPORTS_COUNTER;
            Producer::SPORTS_COUNTER++;
            break;

        case 1:
            type = "NEWS";
            j = NEWS_COUNTER;
            Producer::NEWS_COUNTER++;
            break;

        case 2:
            type = "WEATHER";
            j = WEATHER_COUNTER;
            Producer::WEATHER_COUNTER++;
            break;
    }

    builder = builder.append(to_string(id));
    builder = builder.append(" ");
    builder = builder.append(type);
    builder = builder.append(" ");
    builder = builder.append(to_string(j));
    return builder;
} 

void Producer::produce() {
    string temp = "";

    for (int i = 0; i < msgCount; ++i) {
        temp = buildString();
        buffer->insert(temp);
    }

    buffer->insert("DONE");
}
