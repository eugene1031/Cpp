#include<iostream>

using namespace std;

int main(){
    int num;
    cout << "Please enter 1 integer(4 number) \n";
    cin >> num;
    bool flag = true;
    for (int i = 0; i < 4; i++){
        int A = num / (int) pow(10, i) % 10;
        for (int j = i + 1; j < 4; j++){
            int B = num / (int) pow(10, j) % 10;
            if(A == B){
                flag = false;
            }
        }
    }
    if(flag == true){
        cout << "Right! \n";
    }
    else{
        cout << "Wrong! \n";
    }
        return 0;
}