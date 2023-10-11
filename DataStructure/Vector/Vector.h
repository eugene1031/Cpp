#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED
#include <cstdlib>


template <typename T>
class Vector
{
private:
    T *Pointer;
    int Len;
    int Capacity;

public:
    class Iterator{
        private:
            T* iter;
        public:
            Iterator(T* = nullptr);
            void operator++();  //++Iterator
            void operator--();  
            void operator++(int);   //Iterator++
            void operator--(int);
            bool operator==(Iterator);
            bool operator!=(Iterator);
            bool operator>(Iterator);
            bool operator<(Iterator);
            bool operator>=(Iterator);
            bool operator<=(Iterator);
            void operator=(Iterator);
            int operator-(Iterator);
            Iterator operator+(int);
            Iterator operator-(int);
            T& operator* ();
    };
    Vector(int = 0);
    ~Vector();
    T Front();
    T Back();
    T &At(int);         // 如不加參考，int為右值，右值不能放在等號左側 -> 無法賦值
    T &operator[](int); // 如不加參考，int為右值，右值不能放在等號左側 -> 無法賦值
    int Size();
    bool Empty();
    void Push_Back(T);
    void Pop_Back();
    void Insert(Iterator, T);
    void Erase(Iterator);
    void Erase(Iterator, Iterator);
    void Clear();
    void Reserve(int);
    void Resize(int);
    Iterator Begin();
    Iterator End();
};

template<typename T>
typename Vector<T>::Iterator Find(Vector<T> v, T data){
    for (auto iter = v.Begin(); iter != v.End(); iter++){
       if(*iter == data){
           return iter;
       }
       return v.End();
    }
}
template<typename T>
typename Vector<T>::Iterator Remove(Vector<T> v, T target){
    int counts = 0;
    for (auto iter = v.Begin(); iter != v.End(); iter++)
    {
       if(*iter == target){
           continue;
       }
       *(v.Begin() + counts) = *iter;
       counts++;
    }
    for (auto iter = (v.Begin() + counts); iter != v.End(); iter++)
    {
       *iter = target;
    }
    return v.Begin() + counts;
}

template<typename T>
Vector<T>::Iterator::Iterator(T *pointer){
    iter = pointer;
};
template<typename T>
void Vector<T>::Iterator::operator++(){  //++Iterator
    iter++;
}
template<typename T>
void Vector<T>::Iterator:: operator--(){
    iter--;
}
template<typename T> 
void Vector<T>::Iterator:: operator++(int){  //Iterator++
    iter++;
}
template<typename T>
void Vector<T>::Iterator:: operator--(int){
    iter--;
}
template<typename T>
bool Vector<T>::Iterator:: operator==(Iterator iter2){
    return iter == iter2.iter;
}
template<typename T>
bool Vector<T>::Iterator:: operator!=(Iterator iter2){
    return iter != iter2.iter;
}
template<typename T>
bool Vector<T>::Iterator:: operator>(Iterator iter2){
    return iter > iter2.iter;
}
template<typename T>
bool Vector<T>::Iterator:: operator<(Iterator iter2){
    return iter < iter2.iter;
}
template<typename T>
bool Vector<T>::Iterator::operator>=(Iterator iter2){
    return iter >= iter2.iter;
}
template<typename T>
bool Vector<T>::Iterator::operator<=(Iterator iter2){
    return iter <= iter2.iter;
}
template<typename T>
void Vector<T>::Iterator:: operator=(Iterator iter2){
    iter = iter2.iter;
}
template<typename T>
T &Vector<T>::Iterator::operator*(){
    return *iter;
}
template <typename T>
typename Vector<T>::Iterator Vector<T>::Begin()
{
    Iterator result(Pointer);
    return result;
}
template <typename T>
typename Vector<T>::Iterator Vector<T>::End()
{
    Iterator result(Pointer + Len);
    return result;
}
template <typename T>
Vector<T>::Vector(int length)
{
    if (length == 0)
    {
        Pointer = nullptr;
        Len = 0;
        Capacity = 0;
    }
    else
    {
        Len = length;
        Capacity = length;
        Pointer = (T *)calloc(length, sizeof(T));
    }
}
template <typename T>
Vector<T>::~Vector()
{
    free(Pointer);
}
template <typename T>
T Vector<T>::Front()
{
    return *Pointer;
}
template <typename T>
T Vector<T>::Back()
{
    return *(Pointer + Len - 1);
}

template <typename T>
T &Vector<T>::At(int Num)
{
    return Pointer[Num];
}

template <typename T>
T &Vector<T>::operator[](int Index)
{
    return Pointer[Index];
}

template <typename T>
int Vector<T>::Size()
{
    return Len;
}

template <typename T>
bool Vector<T>::Empty()
{
    if (Len == 0)
    {
        return true;
    }
}

template <typename T>
void Vector<T>::Push_Back(T data)
{
    if (Len == Capacity)
    {
        if (Capacity == 0)
        {
            Reserve(1);
        }
        else
        {
            Reserve(Capacity * 2);
        }
    }
        Pointer[Len] = data;
        Len++;
}

template <typename T>
void Vector<T>::Pop_Back()
{
    if(Empty()){
        return;
    }
    Len--;
}

template <typename T>
void Vector<T>::Insert(Iterator iter2, T data)
{
    if (iter2 >= End())
        return;
    if (iter2 < Begin())
        return;
    if(Len == Capacity){
        if(Capacity == 0){
            Reserve(1);
        }
        else{
            Reserve(Capacity * 2);
        }
    }
    auto tmp = iter2;
    for (iter2 = End(); iter2 >= tmp; iter2--)
    {
        *(iter2 + 1) = *(iter2);
    }
    *(tmp) = data;
    Len++;
}

template <typename T>
void Vector<T>::Erase(Iterator iter2)
{
    if (Empty())
        return ;
    if (iter2 >= End())
        return ;
    if(iter2 < Begin())
        return ;
    for (iter2++; iter2 < End(); iter2++)
    {
        *(iter2 - 1) = *(iter2);
    }
        Len--;
}

template <typename T>
void Vector<T>::Erase(Iterator start, Iterator finish)
{
    if(Empty()){
    return ;
    }
    if(finish >= End())
    return ;
    if(finish <= start)
    return ;
    if (start < End())
    return ;

    auto tmp = finish;
    for (; tmp < End(); tmp++)
    {
    *(tmp - (finish - start)) = *tmp;
    }
    Len -= (finish - start); // Start = 3, End = 6 -> Delete: 3, 4, 5
}
template <typename T>
int Vector<T>::Iterator::operator-(Iterator iter2){
    return (iter - iter2.iter);
}
template <typename T>
typename Vector<T>::Iterator Vector<T>::Iterator::operator+(int offset){
    Iterator result(iter + offset);
    return result;
}
template <typename T>
typename Vector<T>::Iterator Vector<T>::Iterator::operator-(int offset){
    Iterator result(iter - offset);
    return result;
}
template <typename T>
void Vector<T>::Clear()
{
    free(Pointer);
    Len = 0;
    Capacity = 0;
    Pointer = nullptr;
}

template <typename T>
void Vector<T>::Reserve(int N)
{
    if (N < Len)
    {
        return;
    }
    Capacity = N;
    T *tmp = (T *)malloc(sizeof(T) * Capacity);
    for (int i = 0; i < Len; i++)
    {
        tmp[i] = Pointer[i];
    }
    free(Pointer);
    Pointer = tmp;
}

template <typename T>
void Vector<T>::Resize(int N)
{
    if (N < 0)
    {
        return;
    }
    if (N <= Capacity)
    {
        Len = N;
    }
}


#endif