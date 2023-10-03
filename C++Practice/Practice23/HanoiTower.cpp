#include <iostream>
using namespace std;
int Hanoi(int N)
{
    if (N == 1)
    {
        return 1;
    }
    else
        return 2 * Hanoi(N - 1) + 1;
}
int main()
{
    int N;
    cout << " Please input N: ";
    cin >> N;
    cout << " You will need " << Hanoi(N) << " steps \n";
    return 0;
}