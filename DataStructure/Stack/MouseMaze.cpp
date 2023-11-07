#include<iostream>
#include<stack>
#include<vector>

using namespace std;

int main(){
    // Wall = 1, Path = 0, Answer = 2
    int MAZE[10][12] =  {
			{1,1,1,1,1,1,1,1,1,1,1,1},
			{1,0,0,0,0,0,0,0,0,1,1,1},
			{1,1,1,0,1,1,1,1,0,0,1,1},
			{1,1,1,0,1,0,0,1,1,1,1,1},
			{1,1,1,0,1,0,1,1,0,0,0,1},
			{1,1,0,0,0,0,1,1,1,0,1,1},
			{1,1,0,1,1,1,1,0,0,0,0,1},
			{1,1,0,1,1,1,1,0,1,1,0,1},
			{1,0,0,0,0,0,0,0,1,1,0,1},
			{1,1,1,1,1,1,1,1,1,1,1,1},
	};
    
    //存走過的路徑
    vector<vector<bool> > visited(10, vector<bool>(12, false));

    //存對的路徑
    stack<pair<int, int> > path;

    path.push(make_pair(1, 1));
    visited[1][1] = true;

    while(!path.empty()){
        pair<int, int> current = path.top();
        int x = current.first;
        int y = current.second;
        if(x == 8 && y == 10){
            break;
        }
        //下
        if(visited[x + 1][y] == false && MAZE[x + 1][y] == 0){
            path.push(make_pair(x + 1, y));
            visited[x + 1][y] = true;
        }
        //上
        else if(visited[x - 1][y] == false && MAZE[x - 1][y] == 0){
            path.push(make_pair(x - 1, y));
            visited[x - 1][y] = true;
        }
        //右
        else if(visited[x][y + 1] == false && MAZE[x][y + 1] == 0){
            path.push(make_pair(x, y + 1));
            visited[x][y + 1] = true;
        }
        //左
        else if(visited[x][y - 1] == false && MAZE[x][y - 1] == 0){
            path.push(make_pair(x, y - 1));
            visited[x][y - 1] = true;
        }
        else{
            path.pop();
        }
    }
    
    //路徑塗上2
    while(!path.empty()){
        pair<int, int> current = path.top();
        int x = current.first;
        int y = current.second;
        MAZE[x][y] = 2;
        path.pop();
    }

    //印出
    for (int i = 0; i < 10; i++){
        for (int j = 0; j < 12; j++){
            cout << MAZE[i][j] << " ";
        }
        cout << endl;
    }

        return 0;
}