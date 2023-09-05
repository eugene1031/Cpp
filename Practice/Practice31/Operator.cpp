#include <iostream>
#include "Operator.h"
using namespace std;

ostream &operator<<(ostream &os, XY abc)
{
    os << "(x, y) = "
       << "(" << abc.x << ", " << abc.y << ")" << endl;
    return os;
}
XY operator+(XY a, XY b)
{
    XY c = {a.x + b.x, a.y + b.y};
    return c;
}