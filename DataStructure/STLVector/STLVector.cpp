#include<iostream>
#include<vector>

using namespace std;

int main(){
    vector<int> vec;
    int input;

    // while(true){
    //     cout << "Please enter an integer:" << endl;
    //     cin >> input;
    //     if(input == 0){
    //         break;
    //     }
    //     vec.push_back(input);
    // }
    // cout << "Length: " << vec.size() << endl;

    // for (vector<int>::iterator iter = vec.begin(); iter != vec.end(); iter++){
    //     cout << *iter << " ";
    // }
    // cout << endl;

    vector< vector<int> > data_1(9);    //宣告一個vector data_1 裡面有9個vector
    vector<int> data_2[9];              //宣告一個陣列 data_2 裡面有9個vector

    for (int i = 0; i < data_1.size(); i++){
        data_1[i].resize(9);
        data_2[i].resize(9);
    }

    for (int i = 0; i < data_1.size(); i++){
        for (int j = 0; j < data_1[0].size(); j++){
            data_1[i][j] = (i + 1) * (j + 1);
        }
    }

    for (int i = 0; i < data_1.size(); i++){
        for (int j = 0; j < data_1[0].size(); j++){
            cout << data_1[i][j] << " ";
        }
        cout << endl;
    }

        return 0;
}