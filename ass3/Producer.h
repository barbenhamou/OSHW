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
        Producer(int id, int size);
        string buildString();
};
