#include <iostream>
#include <stdlib.h>
using namespace std;
void average(float *score, int num)
{
    float avr = 0;
    for (int i = 0; i < num; i++)
    {
        avr += score[i];
    }
    avr /= num;
    cout << " Average of number is : " << avr << endl;
}
int main()
{
    int num;
    float *score;
    cout << " Please enter the number of scores : \n";
    cin >> num;
    cout << " Please enter the scores of number : \n";
    score = (float *)malloc(sizeof(float) * num);
    for (int i = 0; i < num; i++)
    {
        cin >> score[i];
    }
    average(score, num);
    
    return 0;
}