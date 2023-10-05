#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

template <typename T>
class Vector
{
private:
    T *Pointer;
    int Len;
    int Capacity;

public:
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
    void Insert(int, T);
    void Erase(int);
    void Erase(int, int);
    void Clear();
    void Reserve(int);
    void Resize(int);
};
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
    Len--;
    T *tmp = (T *)malloc(sizeof(T) * Len);
    for (int i = 0; i < Len; i++)
    {
        tmp[i] = Pointer[i];
    }
    free(Pointer);
    Pointer = tmp;
}

template <typename T>
void Vector<T>::Insert(int Index, T data)
{
    if (Index >= Size())
        return;
    if (Index < 0)
        return;
    Len++;
    T *tmp = (T *)malloc(sizeof(T) * Len);
    for (int i = 0; i < Len; i++)
    {
        if (i < Index)
        {
            tmp[i] = Pointer[i];
        }
        else if (i == Index)
        {
            tmp[i] = data;
        }
        else
        {
            tmp[i] = Pointer[i - 1];
        }
    }
    free(Pointer);
    Pointer = tmp;
}

template <typename T>
void Vector<T>::Erase(int Index)
{
    if (Empty())
        return;
    if (Index >= Size())
        return;
    Len--;
    T *tmp = (T *)malloc(sizeof(T) * Len);
    for (int i = 0; i < Len; i++)
    {
        if (i < Index)
        {
            tmp[i] = Pointer[i];
        }
        else if (i > Index)
        {
            tmp[i] = Pointer[i + 1];
        }
    }
    free(Pointer);
    Pointer = tmp;
}

template <typename T>
void Vector<T>::Erase(int Start, int End)
{
    // if(Empty()){
    // return;
    // }
    // if(End >= Size())
    // return;
    // if(End <= Start)
    // return;
    // if (Start < 0)
    // return;
    Len -= End - Start; // Start = 3, End = 6 -> Delete: 3, 4, 5
    T *tmp = (T *)malloc(sizeof(T) * Len);
    for (int i = 0; i < Len; i++)
    {
        if (i < Start)
        {
            tmp[i] = Pointer[i];
        }
        else
        {
            tmp[i] = Pointer[i + (End - Start)];
        }
    }
    free(Pointer);
    Pointer = tmp;
}

template <typename T>
void Vector<T>::Clear()
{
    free(Pointer);
    Len = 0;
    Capacity = 0;
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