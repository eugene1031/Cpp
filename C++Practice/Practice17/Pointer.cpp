#include<iostream>

using namespace std;

int main(){
    
    float v = 2.5;
    float *p = &v;
    //p = &v;
    cout << " Initialize float to 2.5\n";
    cout << "*p = " << *p << endl;

    v = 3.5;
    cout << " Change float to 3.5\n";
    cout << "*p = " << *p << endl;


    return 0;
}