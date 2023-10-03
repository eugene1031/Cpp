#include<iostream>

using namespace std;

int main(){
    int arr[10];
    int *p = arr;
    p[0] = p[1] = 1;
    cout << " Fibonacci number : " << endl;
    for (int i = 0; i < 10; i++)
    {
        if(i > 1){
        p[i] = p[i - 2] + p[i - 1];
        }
    }
    for(int fibo: arr){
        cout << fibo << endl;
    }
        return 0;
}