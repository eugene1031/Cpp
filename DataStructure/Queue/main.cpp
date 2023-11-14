#include<iostream>

using namespace std;

template <typename T>
struct Node{
    T Data;
    Node *Next;
    Node(T data, Node *next) : Data(data), Next(next) {}    //不加沒辦法直接賦值
};

template <typename T>
class Queue{
    private:
        Node<T> *First;
        Node<T> *Rear;
    public:
        Queue();
        T Front();
        T Back();
        bool Empty();
        int Size();
        void Push(T);
        void Pop();
        void Print_Queue();
};
int main()
{
        Queue<int> data;
        for (int i = 1; i <= 10; i++){
            data.Push(i);
        }
        data.Print_Queue();
        cout << "Front: " << data.Front() << endl;
        cout << "Back: " << data.Back() << endl;
        data.Pop();
        data.Print_Queue();
        data.Push(11);
        data.Print_Queue();

        return 0;
}

template <typename T>
Queue<T>::Queue()
{
    First = Rear = nullptr;
}

template <typename T>
T Queue<T>::Front()
{
    if(Empty()){
        cout << " Error: This queue is empty!" << endl;
        return 0;
    }
    return First->Data;
}

template <typename T>
T Queue<T>::Back()
{
    if(Empty()){
        cout << " Error: This queue is empty!" << endl;
        return 0;
    }
    return Rear->Data;
}

template <typename T>
bool Queue<T>::Empty()
{
    return First == nullptr;
}

template <typename T>
int Queue<T>::Size()
{
    if(Empty()) return 0;
    Node<T> *current = First;
    int len = 1;
    while (current != Rear)
    {
        cout << current->Data << " ";
        current = current->Next;
        len++;
    }
    return len;
}

template <typename T>
void Queue<T>::Push(T value)
{
    if(Empty()){
        First = Rear = new Node<T>(value, nullptr);
        return;
    }
    Node<T> *new_node = new Node<T>(value, nullptr);
    Rear->Next = new_node;
    Rear = new_node;
}

template <typename T>
void Queue<T>::Pop()
{
    if(Empty()) return;
    Node<T> *tmp = First;
    First = First->Next;
    delete tmp;
    if(First == nullptr)
        Rear = nullptr;
}

template <typename T>
void Queue<T>::Print_Queue()
{
    if(Empty()){
        cout << " This queue is empty!" << endl;
    }
    else{
        Node<T> *current = First;
        cout << "Queue: ";
        while(current != Rear){
            cout << current->Data << " ";
            current = current->Next;
        }
        cout << current->Data << endl;
    }
}
