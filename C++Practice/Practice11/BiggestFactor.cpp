#include<iostream>

using namespace std;

int main(){
    int n;
    cout << "Please enter n \n";
    cin >> n;
    for (int i = n - 1; i > 0; i--){
        if(n % i == 0){
            cout << "The biggest factor is :" << i << endl;
            break;
        }
    }

        return 0;
}