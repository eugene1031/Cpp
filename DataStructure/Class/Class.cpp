#include<iostream>
#include"Class.h"
using namespace std;

int main()
{
    Course A;
    // double scores[3] = { 80, 70, 60 };
    // A.set_lecturer(" C.YL ");
    // A.set_student_number(3);
    // A.set_score_list(scores);
    //print(A);
    
    cin >> A;
    cout << A;
    for (int i = 0; i < A.get_student_number(); i++)
    {
        cout << A[i] << endl;
    }
    return 0;
}