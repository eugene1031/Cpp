#include<iostream>
#include<fstream>
using namespace std;
int main(){
    ofstream file;
    file.open("MulTable.csv");
    if(file.is_open()){
        for (int i = 1; i < 10; i++){
            for (int j = 1; j < 10; j++){
                file << i * j << ',';
            }
            file << endl;
        }
    }
    else
        cout << "Error!\n";
    return 0;
}