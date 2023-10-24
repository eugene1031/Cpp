#include<iostream>
#include"Stack.h"

using namespace std;

int main(){
    Stack_Array<int> data;
    for(int i = 0; i < 5; i++){
        data.Push(i);
        data.Print_Stack();
    }
    for(int i = 0; i < 5; i++){
        cout << data.Top() << " ";
        data.Print_Stack();
        data.Pop();
    }

    return 0;
}