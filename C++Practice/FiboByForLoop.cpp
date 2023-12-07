#include<iostream>
using namespace std;
int Fib(int n){
    int a = 0, b = 1, c;
    if(n < 2){
        return n;
    }
    for (int i = 0; i < n; i++)
    {
        c = a + b;
        a = b;
        b = c;
    }
    return a;
}
int main(){
    for (int i = 0; i < 10; i++){
        cout << Fib(i) << " ";
    }
    return 0;
}