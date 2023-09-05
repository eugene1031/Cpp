#include<iostream>
using namespace std;
typedef struct{
    float x;
    float y;
} XY;

ostream& operator<<(ostream& os, XY abc){
    os << "(x, y) = " << "(" << abc.x << ", " << abc.y << ")" << endl;
    return os;
}
XY operator+(XY a, XY b){
    XY c = {a.x + b.x, a.y + b.y};
    return c;
}

int main()
{
    XY a = {1.0, 2.0};
    XY b = {3.5, 4.1};
    cout << a << endl;
    cout << b << endl;
    cout << a + b;
    return 0;
}