#include<iostream>

using namespace std;

int main(){
    int N;
    cout << "Enter a integer\n";
    cin >> N;
    for (int i = 1; i < N + 1; i++){
        if(N % i == 0){
            cout << "Factor: " << i << endl;
        }
    }

        return 0;
}