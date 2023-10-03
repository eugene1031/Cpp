#ifndef oprerator_H_included
#define oprerator_H_included
#include <iostream>
using namespace std;

typedef struct
{
    float x;
    float y;
} XY;
ostream &operator<<(ostream &, XY);
XY operator+(XY, XY);

#endif