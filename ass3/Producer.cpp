#include "Producer.h"

using namespace std;

int Producer::NEWS_COUNTER = 0, Producer::SPORTS_COUNTER = 0, Producer::WEATHER_COUNTER = 0;

Producer::Producer(int id, int size) {
    srand(time(0) + id);
    int rnd = rand() % 3;
    switch (rnd) {
        case 0:
            this->type = "SPORTS";
            this->j = SPORTS_COUNTER;
            Producer::SPORTS_COUNTER++;
            break;

        case 1:
            this->type = "NEWS";
            this->j = NEWS_COUNTER;
            Producer::NEWS_COUNTER++;
            break;

        case 2:
            this->type = "WEATHER";
            this->j = WEATHER_COUNTER;
            Producer::WEATHER_COUNTER++;
            break;
    }

    this->id = id;
    this->size = size;
}

string Producer::buildString() {
    string builder = "Producer ";
    builder = builder.append(to_string(this->id));
    builder = builder.append(" ");
    builder = builder.append(this->type);
    builder = builder.append(" ");
    builder = builder.append(to_string(this->j));
    return builder;
} 

int main() {
    Producer p(5, 5);
    cout << p.buildString();
    return 0;
}