#include<iostream>
using namespace std;

typedef struct{
    string name;
    float English;
    float Math;
} Score;

float average(Score s){
    return (s.English + s.Math) / 2;
}

int main()
{
    Score a = {"Mick", 90, 100};
    cout << "The average of " << a.name << " : " << average(a) << endl;


    Score b;
    b.name = "Rallod";
    b.English = 30;
    b.Math = 20;
    cout << "The average of " << b.name << " : " << average(b) << endl;
    return 0;
}