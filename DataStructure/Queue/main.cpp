#include<iostream>

using namespace std;

template <typename T>
struct Node{
    T Data;
    Node *Next;
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

    return 0;
}

template <typename T>
Queue<T>::Queue()
{
    First = Rear = nullptr;
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
