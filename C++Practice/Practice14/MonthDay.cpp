#include<iostream>

using namespace std;

int main(){
    int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    cout << "Please enter the month and day\n";
    int month, day;
    int count = 0;
    cin >> month >> day;
    for (int i = 0; i < month - 1; i++){
        count += days[i];
    }
        cout << "The order of " << month << "/" << day << " is " << count + day << endl;
    return 0;
}