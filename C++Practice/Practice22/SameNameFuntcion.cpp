#include<iostream>
using namespace std;
void print(int i){
    cout << " It is an int " << i << endl;
}
void print(float f){
    cout << " It is a float " << f << endl;
}
void print(int *p, int len){
    cout << " It is a int array \n ";
    for (int i = 0; i < len; i++){
        cout << p[i];
    }
    cout << endl;
}
int main(){
    int i = 1;
    float f = 0.5;
    int arr[5] = {1, 2, 3, 4, 5};
    print(i);
    print(f);
    print(arr, 5);

    return 0;
}