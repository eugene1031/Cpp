#include <iostream>

using namespace std;

int main()
{
    string str;
    cout << "Please enter a string: ";
    getline(cin, str);
    cout << "The result is: ";
    for (char a : str)
    {
        if (a >= 'A' && a <= 'Z')
        {
            a += 32;
            cout << a;
        }
        else if (a >= 'a' && a <= 'z')
        {
            a -= 32;
            cout << a;
        }
    }
    cout << endl;
    return 0;
}