#include <iostream>
#include <fstream>              //需include file stream

using namespace std;

int main()
{
    string str;
    cout << " Please enter your string: \n";
    getline(cin, str);
    
    ////////寫檔//////////
    ofstream file;
    file.open("example.txt");
    if (file.is_open())        //確認是否有開成功
    {
        file << str;
        cout << " Finish writing data into example.txt. \n";
    }
    else
        cout << " File not exist! " << endl;
    file.close();

    ////////讀檔//////////
    cout << "The content on the example.txt is ......\n";
    ifstream a;
    a.open("example.txt");
    if (a.is_open())         //確認是否有開成功
    {
        string abc;
        while (!a.eof())    //確認是否有讀到最結尾
        {
            getline(a, abc);
            cout << abc << endl;
        }
    }
    else
        cout << " File not exist! " << endl;
    a.close();
    cout << "Finishing reading data from example.txt. \n";

    return 0;
}