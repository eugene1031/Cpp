#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED
#include <stdlib.h>
#include <iostream>
using namespace std;

template <typename T>
class Stack_Array
{
private:
    int Capacity;
    int Top_Index;
    T *Pointer;
    void Double_Capacity();

public:
    Stack_Array(int = 0);
    bool Empty();
    int Size();
    T Top();
    void Push(T);
    void Pop();
    void Print_Stack();
};

#endif

template <typename T>
void Stack_Array<T>::Double_Capacity()
{
    if (Capacity == 0)
    {
        Capacity = 1;
        Pointer = (T *)malloc(sizeof(T) * Capacity);
    }
    else
    {
        Capacity *= 2;
        T *tmp = Pointer;
        Pointer = (T *)malloc(sizeof(T) * Capacity);

        for (int i = 0; i <= Top_Index; i++)
        {
            Pointer[i] = tmp[i];
        }
        free(tmp);
    }
}

template <typename T>
Stack_Array<T>::Stack_Array(int len)
{
    Capacity = len;
    if (len > 0)
    {
        Pointer = (T *)malloc(sizeof(T) * len);
    }
    else
    {
        Pointer = nullptr;
    }
    Top_Index = -1;
}

template <typename T>
bool Stack_Array<T>::Empty()
{
    return (Top_Index == -1);
}

template <typename T>
int Stack_Array<T>::Size()
{
    return Top_Index + 1;
}

template <typename T>
T Stack_Array<T>::Top()
{
    if (Empty())
    {
        cout << "Error! This stack is empty!" << endl;
        return 0;
    }
    else
    {
        return Pointer[Top_Index];
    }
}

template <typename T>
void Stack_Array<T>::Push(T data)
{
    if (Top_Index == Capacity - 1)
    {
        Double_Capacity();
    }
    Top_Index++;
    Pointer[Top_Index] = data;
}

template <typename T>
void Stack_Array<T>::Pop()
{
    if (Empty())
    {
        return;
    }
    Top_Index--;
}

template <typename T>
void Stack_Array<T>::Print_Stack()
{
    cout << "Data: ";
    for (int i = 0; i <= Top_Index; i++)
    {
        cout << Pointer[i] << " ";
    }
    cout << endl;
}
