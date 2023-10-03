#include<iostream>
using namespace std;
int sum(int N){
    for (int i = N - 1; i > -1; i--){
        N += i;
    }
    return N;
}
int sum2(int a){
    int result = 0;
    for (int i = 1; i < a + 1; i++){
        result += i;
    }
    return result;
}
int main(){
    cout << " Please enter a number \n";
    int num;
    cin >> num;
    cout << sum(num) << endl;
    cout << sum2(num) << endl;
    return 0;
}