#include<iostream>
#include<iomanip> //for setprecision

using namespace std;

int main(){
    double rad,degree;
    cout << "Please enter your radian\n";
    cin >> rad;
    degree = rad / M_PI * 180;
    cout << "Degree in C++: " << fixed << setprecision(3) << degree << "\n"; //for C++
    printf("Degree in C: % .3f", degree); //for C
    return 0;
}