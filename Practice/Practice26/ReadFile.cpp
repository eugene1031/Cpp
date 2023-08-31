#include<iostream>
#include<fstream>
using namespace std;

int main(){
    string filename;
    cout << " Please anter filename: " << endl;
    cin >> filename;

    ifstream file;
    file.open(filename);
    if(file.is_open()){
        string str;
        while(!file.eof()){
            getline(file, str);
            cout << str << endl;
        }
    }
    else
        cout << " File not exist! " << endl;
    file.close();
    return 0;
}