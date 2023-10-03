#include<iostream>
#include"Operator.h"
#include"Operator.cpp"
using namespace std;

int main()
{
    XY a = {1.8, 2.0};
    XY b = {3.5, 4.1};
    cout << a << endl;
    cout << b << endl;
    cout << a + b;
    return 0;
}