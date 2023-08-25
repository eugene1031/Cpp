#include<iostream>

using namespace std;

int main(){
    int num[5];
    cout << "Please enter 5 number : ";
    for (int i = 0; i < 5; i++){
        cin >> num[i];
    }
    for (int i = 0; i < 4; i++){
        for (int j = i + 1; j < 5; j++){
            if(num[i] > num[j]){
                int tmp;
                tmp = num[i];
                num[i] = num[j];
                num[j] = tmp;
            }
        }
    }
    cout << " The ordered array is : ";
    for (int i = 0; i < 5; i++)
    {
        cout << num[i] << " ";
    }
      return 0;
}