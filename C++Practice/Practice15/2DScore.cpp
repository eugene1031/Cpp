#include <iostream>

using namespace std;

int main()
{
    float scores[2][3];
    float avr[2] = {0};
    for (int i = 0; i < 2; i++)
    {
        cout << "======班級" << i + 1 << endl;
        for (int j = 0; j < 3; j++)
        {
            cout << "學生" << j + 1 << endl;
            cin >> scores[i][j];
            avr[i] += scores[i][j];
        }
        avr[i] /= 3;
    }
  
    for (int i = 0; i < 2; i++){
        cout << "The average of class " << i + 1 << " is " << avr[i] << endl;
    }
        if (avr[0] > avr[1])
        {
            cout << "Class 1 is better" << endl;
        }
        else if(avr[0] == avr[1]){
            cout << " Both classes are good!" << endl;
        }
        else
            cout << "Class 2 is better" << endl;

    return 0;
}