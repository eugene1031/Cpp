#include<iostream>
#include<vector>
using namespace std;

int main(){
class Solution {
public:
    vector<int> spiralOrder(vector<vector<int> > & matrix) {
        int rows = matrix.size();
        int cols = matrix[0].size();
        vector<int> result;
        int left = 0, right = cols - 1, top = 0, bottom = rows - 1;

        while(left <= right && top <= bottom){
        for(int j = left; j <= right; j++){
            result.push_back(matrix[top][j]);
        }
        top++;

        for(int i = top; i <= bottom; i++){
            result.push_back(matrix[i][right]);
        }
        right--;

        if (top <= bottom) {
            for(int j = right; j >= left; j--){
                result.push_back(matrix[bottom][j]);
            }
            bottom--;
        }

        if (left <= right) {
            for(int i = bottom; i >= top; i--){
                result.push_back(matrix[i][left]);
            }
            left++;
        }
    }
        return result;
    }
};


    return 0;
}