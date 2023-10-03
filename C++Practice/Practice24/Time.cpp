#include <iostream>
#include <time.h>
using namespace std;
int Fibo(int N)
{
    if (N < 3)
    {
        return 1;
    }
    else
        return Fibo(N - 2) + Fibo(N - 1);
}
int main()
{
    clock_t start, finish;

    int N;
    cout << " Please enter N: \n";
    cin >> N;
    start = clock();
    for (int i = 1; i < N + 1; i++)
    {
        cout << Fibo(i) << " ";
    }
    cout << endl;
    finish = clock();
    double time = (finish - start) / (double)(CLOCKS_PER_SEC);
    cout << "The time cost: " << time << endl;
    return 0;
}