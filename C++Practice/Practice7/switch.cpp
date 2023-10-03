#include<iostream>

using namespace std;

int main(){
float score;
cout << "Please enter score\n";
cin >> score;
int div = score / 10;
if(0<=score&&score<=100){
switch (div)
{
    case(10):
    case(9):
        cout << "Your score is: A\n";
        break;
    case(8):
        cout << "Your score is: B\n";
        break;
    case(7):
        cout << "Your score is: C\n";
        break;
    case(6):
        cout << "Your score is: D\n";
        break;
    default:
        cout << "Your score is: F\n";
        break;
    
       
    }
}
else{
 cout << "Unavailable\n";
}
return 0;
}