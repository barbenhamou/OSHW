#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

class Producer {
    public:
        static int SPORTS_COUNTER;
        static int NEWS_COUNTER;
        static int WEATHER_COUNTER;

    private:
        int id;
        string type;
        int j;
        int size;
    
    public:
        Producer(int id, int size) {
            srand(time(0));
            int rnd = rand() % 3;
            switch (rnd) {
                case 0:
                    type = "SPORTS";
                    j = SPORTS_COUNTER;
                    SPORTS_COUNTER++;
                    break;

                case 1:
                    type = "NEWS";
                    j = NEWS_COUNTER;
                    NEWS_COUNTER++;
                    break;

                case 2:
                    type = "WEATHER";
                    j = WEATHER_COUNTER;
                    WEATHER_COUNTER++;
                    break;
            }

            Producer::id = id;
            Producer::size = size;
        }

        string buildString() {
            string builder = "Producer ";
            builder = builder.append(to_string(id));
            builder = builder.append(" ");
            builder = builder.append(type);
            builder = builder.append(" ");
            builder = builder.append(to_string(j));
            return builder;
        } 
};

int Producer::NEWS_COUNTER = 0, Producer::SPORTS_COUNTER = 0, Producer::WEATHER_COUNTER = 0;

int main() {
    Producer p(5, 5);
    cout << p.buildString();
    return 0;
}