#include<iostream>

using namespace std;

int main(){

    double x, y, z,out;
    cout << "Enter three number.\n";
    cin >> x >> y>>z;
    cout << "Sum = " << 1.0/6*(x*x+35+2*x*x*x+x*y*y+3*z)<<"\n"; //小心1/6若沒加小數點會變0

    return 0;
}