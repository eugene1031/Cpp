#include<iostream>
#include<stack>

using namespace std;

stack<int> A;
stack<int> B;
stack<int> C;

void Print_Stack(stack<int> s)
{
    if(s.empty()){
        return;
    }
        int data = s.top();
        s.pop();
        Print_Stack(s);
        cout << data << " ";
        s.push(data);
}
void Print_All(){
        cout << "\nA: ";
        Print_Stack(A);
        cout << "\nB: ";
        Print_Stack(B);
        cout << "\nC: ";
        Print_Stack(C);
        cout << "\n-------";
}

void Hanoi(int num, stack<int>&from, stack<int>&to, stack<int>&others){
    if (num == 1){
        to.push(from.top());
        from.pop();
        Print_All();
        return;
    }
    else{
        Hanoi(num - 1, from, others, to);
        to.push(from.top());
        from.pop();
        Print_All();
        Hanoi(num - 1, others, to, from);
    }
}


int main(){
    int N;
    cout << "Please enter a number: ";
    cin >> N;
    for (int i = N; i > 0; i--){
        A.push(i);
    }

    Hanoi(N, A, C, B);

    return 0;
}