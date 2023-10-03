#include<iostream>
using namespace std;
void cos(float a, float b, float c, float &cosa, float &cosb, float &cosc){
    cosa = (b * b + c * c - a * a) / (2 * b * c);
    cosb = (a * a + c * c - b * b) / (2 * a * c);
    cosc = (a * a + b * b - c * c) / (2 * a * b);
}
int main(){
    float a, b, c;
    float cosa, cosb, cosc;
    cout << " Please enter 3 length of triangle : \n";
    cin >> a >> b >> c;
    cos(a, b, c, cosa, cosb, cosc);
    cout << " cosA : " << cosa << endl;
    cout << " cosB : " << cosb << endl;
    cout << " cosC : " << cosc << endl;
    return 0;
}