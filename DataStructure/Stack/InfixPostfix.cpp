#include<iostream>
#include<stack>

using namespace std;

int main(){
    string str = "((a+(b*c))+d)";
    stack<char> sign;

    for (char c : str)
    {
        if(c == '('){
            continue;
        }
        else if(c >= 'a' && c <= 'z'){
            cout << c;
        }
        else if(c == ')'){
            cout << sign.top();
            sign.pop();
        }
        else{
            sign.push(c);
        }
    }
    return 0;
}