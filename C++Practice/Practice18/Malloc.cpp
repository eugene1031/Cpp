#include<iostream>
#include<stdlib.h>

using namespace std;

int main(){
    int num;
    float *score;
    cout << "Please input the number of students.\n";
    cin >> num;
    score = (float * )malloc(sizeof(float) * num);
    for (int i = 0; i < num; i++){
        cout << "Please enter the score of #" << i + 1 << endl;
        cin >> score[i];
    }
    for (int i = 0; i < num; i++){
        cout << "#" << i + 1 << " " << score[i] << endl;
    }
        return 0;
}