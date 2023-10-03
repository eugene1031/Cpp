#include <iostream>
#include <fstream>
using namespace std;

int main()
{
    ofstream file;
    file.open("MulTable.txt");
    if (file.is_open()) // 確認是否有開成功
    {
        for (int i = 1; i < 10; i++)
        {
            for (int j = 1; j < 10; j++)
            {
                file << i << "*" << j << "=" << i * j << "\t";
            }
            file << endl;
        }
        cout << " Finish writing data into MulTable.txt. \n";
    }
    else
        cout << " File not exist! " << endl;
    file.close();
    return 0;
}