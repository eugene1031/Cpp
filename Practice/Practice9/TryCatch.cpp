#include<iostream>

using namespace std;

int main(){
    float a, b;
    cout << "Enter two float\n";
    try{
        cin >> a >> b;
        if (b == 0)
        {
            throw b;
        }
        else{
            cout << "a/b= " << a / b << endl;
        }
    }
    catch(...){
        cout << "Error!"<<endl;
    }
    return 0;
}