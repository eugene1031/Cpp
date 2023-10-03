#include<iostream>

using namespace std;

int main(){
    int num;
    cout << "Please enter 1 integer \n";
    cin >> num;
    bool A = true;
    if (num % 10 == num / 10 % 10 || num % 10 == num / 100 % 10 || num % 10 == num / 1000)
    {
        A = false;
    }
    if (num / 10 % 10 == num / 100 % 10 || num / 10 % 10 == num / 1000)
    {
        A = false;
    }
    if (num / 100 % 10 == num / 1000)
    {
        A = false;
    }
    if(A == true){
        cout << "Right \n";
    }
    else{
        cout << "Wrong \n";
    }
    return 0;
}