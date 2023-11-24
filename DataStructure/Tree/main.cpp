#include<iostream>

using namespace std;
template<typename T>
struct Node{
    int index;
    T data;
    Node<T> *left;
    Node<T> *right;

    Node(int idx, T val, Node<T>* leftPtr = nullptr, Node<T>* rightPtr = nullptr)
        : index(idx), data(val), left(leftPtr), right(rightPtr) {}
};
template <typename T>
class Binary_Tree
{
public:
    Node<T> *root;
    Binary_Tree();
    void Print(Node<T>);
    bool Insert(int, T);
    Node<T> *Search(int);
    bool Delete(int);
};

template <typename T>
Binary_Tree<T>::Binary_Tree()
{
    root = nullptr;
}

template <typename T>
void Binary_Tree<T>::Print(Node<T> node)
{
    cout << "Index: " << node.index << endl;
    cout << "Data: " << node.data << endl;
}

template <typename T>
bool Binary_Tree<T>::Insert(int target, T value)
{
    if(root == nullptr){
        root = new Node<T>{target, value, nullptr, nullptr};
        return true;
    }
    Node<T> *current = root;
    while(true){
        if(current->index == target){
            return false;
        }
        else if(target < current->index){
            //Go to left
            if(current->left == nullptr){
                current->left = new Node<T>{target, value, nullptr, nullptr};
                return true;
            }
            current = current->left;
        }
        else{
            //Go to right
            if(current->right == nullptr){
                current->right = new Node<T>{target, value, nullptr, nullptr};
                return true;
            }
            current = current->right;
        }
    }
}

template <typename T>
Node<T> *Binary_Tree<T>::Search(int target)
{
    if(root == nullptr){
        return nullptr;
    }
    Node<T> *current = root;
    while(true){
        if(current->index == target){
            return current;
        }
        else if(target < current->index){
            //Go to left
            if(current->left == nullptr){
                return nullptr;
            }
            current = current->left;
        }
        else{
            //Go to right
            if(current->right == nullptr){
                return nullptr;
            }
            current = current->right;
        }
    }
}

template <typename T>
bool Binary_Tree<T>::Delete(int)
{
    return false;
}

int main()
{
    Binary_Tree<string> tree;
    tree.Insert(1, "Mick");
    tree.Insert(2, "Rallod");
    tree.Insert(3, "Daphene");

    Node<string>* found = tree.Search(2);
    tree.Print(*found);

    return 0;
}