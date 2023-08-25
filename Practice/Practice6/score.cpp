#include<iostream>

using namespace std;

int main(){
    float score;

    cout << "Please enter score\n";
    cin >> score;

if (90<=score&&score<=100){
    cout << "Your score is: A\n";
}
    else if(80<=score&&score<=89){
    cout << "Your score is: B\n";
}
 else if(70<=score&&score<=79){
    cout << "Your score is: C\n";
}
 else if(60<=score&&score<=69){
    cout << "Your score is: D\n";
}
 else if(0<=score&&score<=59){
    cout << "Your score is: F\n";
}
else{
    cout << " The score is unavailable.\n";
}

    return 0;
}