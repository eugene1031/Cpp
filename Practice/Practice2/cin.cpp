#include<iostream>

using namespace std;

int main(){

    string name;
    cout << "What's your name?\n";
    //cin >> name; //空格後的吃不到
    getline(cin, name); //for整行字串（可使用空格）
    cout << "Hello," << name << " nice to meet you in C++ class.\n";

    return 0;
}