#include <iostream>
#include "Age.h"
using namespace std;

int youngage = 18;
int oldage = 65;

void distinguishAge()
{
    status age;
    int N;
    cout << "Please enter your age:" << endl;
    cin >> N;

    if (N <= youngage)
        age = isYoung;
    else if (N < oldage)
        age = isMiddle;
    else
        age = isold;

    switch (age)
    {
    case isYoung:
        cout << "Young" << endl;
        break;
    case isMiddle:
        cout << "Middle" << endl;
        break;
    case isold:
        cout << "Old" << endl;
        break;
    }
}