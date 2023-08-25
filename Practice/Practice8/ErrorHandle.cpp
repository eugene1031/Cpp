#include<iostream>

using namespace std;

int main(){
    int w, h,Area;
    cout << "Please enter width and height:\n";
    
    if (cin >> w >> h)
    {   
        Area = w * h;
        cout << "Area: "<< Area <<"\n";
    }
    else{
        cout << "Error\n";
    }
    return 0;
}