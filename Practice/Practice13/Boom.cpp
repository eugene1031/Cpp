#include <iostream>

using namespace std;

int main()
{
    int guess = -1;
    int Low = 0;
    int High = 100;
    int bomb = 60;
    while (guess != bomb)
    {
        cout << "Please guess a number from " << Low << " to " << High << "\n";
        cin >> guess;
        if (Low < guess && guess < High)
        {
            if (guess > bomb)
            {
                High = guess;
            }
            else
            {
                Low = guess;
            }
        }
        else
        {
            cout << "Out of range!\n";
        }
    }
    while (guess == bomb)
    {
        cout << "BOOM!\n";
        break;
    }
    return 0;
}