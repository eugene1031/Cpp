#ifndef CIRCULAR_LINKED_LIST_H_INCLUDED
#define CIRCULAR_LINKED_LIST_H_INCLUDED
#include <iostream>
#include"Function.h"
using namespace std;

template <typename T>
struct Node
{
    T Data;
    Node *Prev;
    Node *Next;
};

template <typename T>
class Linked_List
{
private:
    Node<T> *Head;
    //Node<T> *Tail;
    int Len;

public:
    class Iterator
    {
    friend Linked_List;
    private:
        Node<T> *iter;

    public:
        Iterator(Node<T> * = nullptr);
        // void operator++(); //++Iterator
        // void operator--();
        // void operator++(int); // Iterator++
        // void operator--(int);
        // bool operator==(Iterator);
        // bool operator!=(Iterator);
        // void operator=(Iterator);
        // Iterator operator+(int);
        // Iterator operator-(int);
        // T &operator*();
    };
    Linked_List();
    // void Print_List();
    // int Search_List(T);
    // void Push_Front(T);
    // void Pop_Front();
    // void Pop_Back();
    // void Clear();
    // void Reverse();
    // void Insert(Iterator, const T&);
    // void Erase(Iterator);
    // void Remove(T);
    void Push_Back(T);
    void Rotate(int);
    // Iterator Begin();
    // Iterator End();
    // template <typename T2>
    // friend typename Linked_List<T2>::Iterator Find(Linked_List<T2>, T2);
};

// template <typename T>
// typename Linked_List<T>::Iterator Linked_List<T>::Begin()
// {
//     return Iterator(Head);
// }
// template <typename T>
// typename Linked_List<T>::Iterator Linked_List<T>::End()
// {
//     return Iterator(nullptr);
// };

// template <typename T>
// void Linked_List<T>::Insert(Iterator it, const T& value)
// {
//     // A B, B is provided by user = it
//     // A C B
//     // TODO: Cannot use to push front.
//     // TODO: Cannot use to push tail.

//     Node<T> *new_node = new Node<T>;
//     new_node->Data = value;
//     new_node->Prev = it.iter->Prev;
//     new_node->Next = it.iter;

//     it.iter->Prev->Next = new_node;
//     it.iter->Prev = new_node;
// };
// template <typename T>
// void Linked_List<T>::Erase(Iterator it)
// {
//     // A B C, B is provided by user = it
//     // A C 
//     // TODO: Cannot use to erase front.
//     // TODO: Cannot use to erase tail.

//     it.iter->Prev->Next = it.iter->Next;
//     it.iter->Next->Prev = it.iter->Prev;
//     delete it.iter;
// };
// template <typename T>
// void Linked_List<T>::Remove(T value)
// {
//     //iter = pointer;
// }

// template <typename T>
// typename Linked_List<T>::Iterator Find(Linked_List<T> List, T data)
// {
//     typename Linked_List<T>::Iterator null_iter(nullptr);
//     if (List.Head == nullptr)
//     {
//         return null_iter;
//     }

//     typename Linked_List<T>::Iterator current(List.Head);
//     typename Linked_List<T>::Iterator tail_iter(List.Tail);

//     while (current != null_iter)
//     {
//         if (*current == data)
//         {
//             return current;
//         }
//         current++;
//     }
//     return current; // Not found
// }

// template <typename T>
// Linked_List<T>::Iterator::Iterator(Node<T> *pointer)
// {
//     iter = pointer;
// };
// template <typename T>
// void Linked_List<T>::Iterator::operator++()
// { //++Iterator
//     if (iter != nullptr)
//     {
//         iter = iter->Next;
//     }
// }
// template <typename T>
// void Linked_List<T>::Iterator::operator--()
// {
//     if (iter != nullptr)
//     {
//         iter = iter->Prev;
//     }
// }
// template <typename T>
// void Linked_List<T>::Iterator::operator++(int)
// { // Iterator++
//     if (iter != nullptr)
//     {
//         iter = iter->Next;
//     }
// }
// template <typename T>
// void Linked_List<T>::Iterator::operator--(int)
// {
//     if (iter != nullptr)
//     {
//         iter = iter->Prev;
//     }
// }
// template <typename T>
// bool Linked_List<T>::Iterator::operator==(Iterator iter2)
// {
//     return iter == iter2.iter;
// }
// template <typename T>
// bool Linked_List<T>::Iterator::operator!=(Iterator iter2)
// {
//     return iter != iter2.iter;
// }
// template <typename T>
// void Linked_List<T>::Iterator::operator=(Iterator iter2)
// {
//     iter = iter2.iter;
// }
// template <typename T>
// typename Linked_List<T>::Iterator Linked_List<T>::Iterator::operator+(int offset)
// {
//     Iterator result(iter);
//     for (int i = 0; i < offset; i++)
//     {
//         if (result->iter == nullptr)
//         {
//             return result;
//         }
//         result->iter = result->iter->Next;
//     }
//     return result;
// }
// template <typename T>
// typename Linked_List<T>::Iterator Linked_List<T>::Iterator::operator-(int offset)
// {
//     Iterator result(iter);
//     for (int i = 0; i < offset; i++)
//     {
//         if (result->iter == nullptr)
//         {
//             return result;
//         }
//         result->iter = result->iter->Prev;
//     }
//     return result;
// }
// template <typename T>
// T &Linked_List<T>::Iterator::operator*()
// {
//     return iter->Data;
// }

template <typename T>
Linked_List<T>::Linked_List()
{
    Head = nullptr;
    Len = 0;
    // Tail = nullptr;
}

// template <typename T>
// void Linked_List<T>::Print_List()
// {
//     if (Head == nullptr)
//     {
//         cout << "This list is empty!" << endl;
//         return;
//     }
//     Node<T> *current = Head;
//     cout << "Data:";
//     while (current->Next != 0)
//     {
//         cout << current->Data << " ";
//         current = current->Next;
//     }
//     cout << current->Data << endl;
// }

// template <typename T>
// int Linked_List<T>::Search_List(T target)
// {
//     if (Head == nullptr)
//     {
//         return -1;
//     }
//     int index = 0;
//     Node<T> *current = Head;
//     while (current->Next != 0)
//     {
//         if (current->Data == target)
//         {
//             return index;
//         }
//         current = current->Next;
//         index++;
//     }
//     if (current->Data == target)
//     { // 最後一筆
//         return index;
//     }
//     else
//     {
//         return -1;
//     }
//     return 0;
// }

// template <typename T>
// void Linked_List<T>::Push_Front(T value)
// {
//     if (Head == nullptr) // Empty
//     {
//         Node<T> *new_node = new Node<T>(); //{value, Head};  //{值, Next指標}(作法等同於下面兩行)
//         new_node->Data = value;
//         new_node->Prev = nullptr;
//         new_node->Next = nullptr;
//         Head = new_node;
//         Tail = new_node;
//     }
//     else
//     {
//         Head->Prev = new Node<T>();
//         Head->Prev->Data = value;
//         Head->Prev->Prev = nullptr;
//         Head->Prev->Next = Head;
//         Head = Head->Prev;
//     }
// }

// template <typename T>
// void Linked_List<T>::Pop_Front()
// {
//     if (Head == nullptr)
//     {
//         return;
//     }
//     if (Head == Tail) // 只有一筆資料
//     {
//         delete Head;
//         Head = Tail = nullptr;
//         return;
//     }
//     Head = Head->Next;
//     delete Head->Prev;
//     Head->Prev = nullptr;
// }

template <typename T>
void Linked_List<T>::Push_Back(T value)
{
    if (Head == nullptr) // Empty
    {
        Head = new Node<T>(); //{value, Head};  //{值, Next指標}(作法等同於下面兩行)
        Head->Data = value;
        Head->Prev = new_node;
        Head->Next = new_node;
    }
    else
    {
        // Tail : Head->Prev
        Node<T> *new_node = new Node<T>(); //{value, Head};  //{值, Next指標}(作法等同於下面兩行)
        new_node->Data = value;
        new_node->Prev = Head->Prev;
        Head->Prev->Next = new_node;
        Head->Prev = new_node;

        Tail->Next = new Node<T>();
        Tail->Next->Data = value;
        Tail->Next->Prev = Tail;
        Tail->Next->Next = nullptr;
        //Tail = Tail->Next;
    }
    Len++;
}

// template <typename T>
// void Linked_List<T>::Pop_Back()
// {
//     if (Head == nullptr) // 沒有資料
//     {
//         return;
//     }
//     if (Head == Tail) // 只有一筆資料
//     {
//         delete Tail;
//         Head = Tail = nullptr;
//         return;
//     }
//     Tail = Tail->Prev;
//     delete Tail->Next;
//     Tail->Next = nullptr;
// }

// template <typename T>
// void Linked_List<T>::Clear()
// {
//     Node<T> *current;
//     while (Head != nullptr)
//     {
//         current = Head;
//         Head = Head->Next;
//         delete current;
//     }
//     Tail = nullptr;
// }

// template <typename T>
// void Linked_List<T>::Reverse()
// {
//     if (Head == nullptr)
//     {
//         return;
//     }
//     if (Head == Tail)
//     {
//         return;
//     }
//     Node<T> *previous = nullptr;
//     Node<T> *current = Head;
//     Node<T> *preceding = Head->Next;
//     Tail = Head;

//     while (preceding != nullptr)
//     {
//         current->Next = previous; // 原本指向右邊改成指向左邊
//         current->Next = preceding;
//         previous = current; // 往右移
//         current = preceding;
//         preceding = preceding->Next;
//     }
//     current->Next = previous;
//     Head = current;
// }

#endif

template <typename T>
void Linked_List<T>::Rotate(int offset)
{
    //offset = offset % size()
    if(Head == nullptr){
        return;
    }
    offset = offset % Len;
    for (int i = 0; i < offset; i++){
        Head = Head->Next;
    }
}
