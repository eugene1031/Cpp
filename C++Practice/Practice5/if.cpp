#include<iostream>

using namespace std;

int main(){
    
    cout << "Please enter a number\n";
    int num;
    cin >> num;
    int cal = num % 2;

if (cal==0){
    cout << "Even\n";
}
else
{
    cout << "Odd\n";
}
    return 0;
}