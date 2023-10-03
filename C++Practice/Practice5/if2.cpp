#include<iostream>

using namespace std;

int main(){
    int rad;
    
    cout << "Please enter radius\n";
    cin >> rad;
    int Area = rad * rad * M_PI;
    if(rad>0){
        cout << Area << "\n";
    }
    else{
        cout << "You give a wrong radius\n";
    }

    return 0;
}