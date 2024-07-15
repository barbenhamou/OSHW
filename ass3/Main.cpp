#include <iostream>
#include <fstream>

using namespace std;
 
int main(int argc, char *argv[])
{
    if (argc < 1) {
        cout << "needs config.txt file";
        return 1;
    }

    ofstream ConfigFile(argv[1]);


    
    return 0;
}