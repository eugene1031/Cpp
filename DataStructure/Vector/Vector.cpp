#include <iostream>
#include "Vector.h"
using namespace std;

int main(){

    Vector<int> v;
    v[0] = 1;
    v.At(1) = 3;
    v[2] = 99;
    v[4] = 101;

    Vector<int>::Iterator iter = v.Begin();
    
    // iter = Find(v, 2);

    // if(iter != v.End()){
    //         cout << *iter << endl;
    // }
    // else{        
    //     cout << "Not found!" << endl;
    // }
    // Vector<int>::Iterator iter(&v[0]);
    // for (int i = 0; i < 5; i++){
    //     cout << *iter << " ";
    //     iter++;
    // }
    // cout << endl;
    // Vector<int>::Iterator iter2 = iter;

    // if(iter == iter2){
    //     cout << "Same" << endl;
    // }
    // else{
    //     cout << "Different" << endl;
    // }
    // cout << v.Front() << endl;
    // cout << v.At(1) << endl;
    // cout << v[2] << endl;
    // cout << v[3] << endl;
    // cout << v.Back() << endl;
    // for (int i = 0; i < v.Size(); i++){
    //     cout << v[i] << " ";
    // }
    // cout << endl << endl;
    // v.Pop_Back();
    // v.Push_Back(1000);
    v.Insert(iter + 5, 100);
    // v.Erase(iter + 2);
    //v.Erase(1, 3);
    // v.Clear();
    for (int i = 0; i < v.Size(); i++)
    {
        cout << v[i] << " ";
    }
    return 0;
}