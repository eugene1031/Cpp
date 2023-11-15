#include<iostream>
#include<queue>

using namespace std;

int main(){
    int N;
    cout << "Please enter N:";
    cin >> N;

    queue<int> data;
    int temp1, temp2;

    cout << "Pascal Triangle:" << endl;
    data.push(1);
    cout << 1 << endl;

    //row 2 -> N
    for (int i = 2; i <= N; i++){
        temp1 = 0;
        for (int j = 1; j <= i - 1; j++){
            temp2 = data.front();
            data.pop();
            cout << temp1 + temp2 << " ";
            data.push(temp1 + temp2);
            temp1 = temp2;
        }
        data.push(1);
        cout << 1 << endl;
    }

        return 0;
}