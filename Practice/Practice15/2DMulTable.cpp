#include<iostream>
#include<iomanip>

using namespace std;

int main(){
    int mul[9][9];
    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++){
            mul[i][j] = (j + 1) * (i + 1);
            cout << setw(3) << mul[i][j];
            // cout << j << "*" << i << "=" << setw(3) << i * j << "\t";
        }
        cout << endl;
    }
    for (int i = 1; i < 10; i++){
        for (int j = 1; j < 10; j++){
            cout << j << "*" << i << " = " << setw(2) << mul[i - 1][j - 1] << " ";
        }
        cout << endl;
    }
        return 0;
}