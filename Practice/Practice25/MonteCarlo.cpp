#include <iostream>
#include<stdlib.h>
#include <time.h>
using namespace std;

int main()
{
    srand(time(NULL));
    int in = 0, out = 0;
    for (int i = 0; i < 10000000; i++)
    {
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;
        if(x * x + y * y > 1){
            out++;
        }
        else if(x * x + y * y < 1)
            in++;
    }

    double pi = 4.0 * in / (in + out);
    cout << "Pi = " << pi << endl;
    return 0;
}