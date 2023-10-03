#include<iostream>

using namespace std;

int main(){
    int num;

    cout << "Enter a integer\n";
    cin >> num;
    if(num%15==0){
        cout << "C\n";
    }
    else if(num%3==0){
        cout << "A\n";
    }
    else if(num%5==0){
        cout << "B\n";
    }
    else{
        cout << "other\n";
    }
    return 0;
}