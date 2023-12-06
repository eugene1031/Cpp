#include <iostream>
using namespace std;

void printDiamond(int n) {
    int space = n - 1;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < space; j++) {
            cout << " ";
        }
        for (int j = 0; j <= i; j++) {
            cout << "* ";
        }
        cout << endl;
        space--;
    }
    space = 0;
    for (int i = n; i > 0; i--) {
        for (int j = 0; j < space; j++) {
            cout << " ";
        }
        for (int j = 0; j < i; j++) {
            cout << "* ";
        }
        cout << endl;
        space++;
    }
}

int main() {
    int n;
    cout << "Enter the number of rows for the diamond: ";
    cin >> n;

    printDiamond(n);

    return 0;
}
