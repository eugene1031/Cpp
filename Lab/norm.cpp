#include <iostream>
#include <vector>
#include <complex>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <complex>
#include <vector>
#include <cstdlib>

using namespace std;
using namespace Eigen;

int main()
{
    MatrixXcd A(4,2);
    A << complex<double>(0, 0), complex<double>(1.41421, 0),
        complex<double>(-1.41421, 0), complex<double>(0, 0),
        complex<double>(0, 0), complex<double>(0, 0),
        complex<double>(0, 0), complex<double>(0, 0);
    cout << A << "\n";
    cout << A.squaredNorm();

    return 0;
}