#include<iostream>
using namespace std;
enum class status{
isPositive,
isNegative,
isZero
};

int main(){
int N;
cout << "Please enter a number:";
cin >> N;
status s;
if(N > 0){
    s = status::isPositive;
}
else if(N < 0){
    s = status::isNegative;
}
else{
    s = status::isZero;
}

switch(s)
{
case status::isPositive:
cout << "Positive!\n";
    break;
case status::isNegative:
cout << "Negative!\n";
    break;
case status::isZero:
cout << "Zero!\n";
    break;
}
    return 0;
}