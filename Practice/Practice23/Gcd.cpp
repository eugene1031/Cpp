#include <iostream>
using namespace std;
int gcd(int a, int b)
{
    int c = a % b;
    if (c != 0)
    {
        a = b;
        b = c;
        return gcd(a, b);
    }
    else
        return b;
}
int main()
{
    int a, b;
    cout << " Please enter 2 integer: \n";
    cin >> a >> b;
    cout << gcd(a, b);
    return 0;
}