#include <iostream>
#include <vector>

using namespace std;

int main()
{
    int row, col;

    cout << "輸入列與行數： " << endl;
    cin >> row >> col;
    vector<int> v[row];
    for (int i = 0; i < row; i++)
    {
        v[i].resize(col);
    }
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            cin >> v[i][j];
        }
    }
    vector<int> row_max(row);
    int sum = 0;
    for (int i = 0; i < row; i++)
    {
        int max = 0;
        for (int j = 0; j < col; j++)
        {
            if (v[i][j] > max)
            {
                max = v[i][j];
                row_max[i] = max;
            }
        }
        sum += row_max[i];
    }
    cout << "Sum: " << sum << endl;

    int counts = 0;
    int dev = 0;
    for (int i = 0; i < row; i++)
    {
        dev = sum % row_max[i];
        if (dev == 0)
        {
            cout << row_max[i] << " ";
            counts++;
        }
    }
    if (counts == 0)
    {
        cout << "-1" << endl;
    }
    return 0;
}