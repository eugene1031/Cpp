#include <iostream>

using namespace std;

int main()
{
    int ans = 1234;
    int guess = 0000;
    int countA = 0;
    int countB = 0;
    while (ans != guess)
    {
        cout << "Guess a number(in 4)\n";
        cin >> guess;
        for (int i = 0; i < 4; i++)
        {
            int a = ans / (int)pow(10, i) % 10;
            for (int j = 0; j < 4; j++)
            {
                int g = guess / (int)pow(10, j) % 10;
                if (a == g)
                {
                    if (i == j)
                    {
                        countA++;
                    }
                    else{
                        countB++;
                    }
                }
            }
        }
        cout << countA << "A" << countB << "B\n";
        countA = 0;
        countB = 0;
    }
    while(ans == guess){
        cout << "You win!\n";
        break;
    }
    return 0;
}