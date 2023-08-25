#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>

using namespace std;
using namespace Eigen;

int main(){
    vector<int> items{1, 2, 3};
    MatrixXcd A(2,2);
    A << 1,0,1,1;
    cout << "Hello world" << "\n";
    for(int i = 0; i<3; i++){
    cout << "item" << i << "=" << items[i] << endl;
    }
    cout << "A:" << "\n" << A << "\n";
    return 0;
}  