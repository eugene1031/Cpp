#include <iostream>
#include "Vector.h"
using namespace std;

int main(){

    Vector<int> v(5);
    v[0] = 1;
    v.At(1) = 3;
    v[2] = 99;
    v[4] = 101;

    cout << v.Front() << endl;
    cout << v.At(1) << endl;
    cout << v[2] << endl;
    cout << v[3] << endl;
    cout << v.Back() << endl;
    for (int i = 0; i < v.Size(); i++){
        cout << v[i] << " ";
    }
    cout << endl << endl;
    // v.Pop_Back();
    v.Push_Back(1000);
    // v.Insert(1, 100);
    //v.Erase(1, 3);
    //v.Clear();
    for (int i = 0; i < v.Size(); i++)
    {
        cout << v[i] << " ";
    }
        return 0;
}