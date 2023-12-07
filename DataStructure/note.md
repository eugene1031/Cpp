# Data Structure
## Big O
f(n) ∈ O(g(n)) -> f(n) ≤ cg(n)
存在c > 0、n0，使得所有的n > n0滿足f(n) ≤ cg(n)
->lim(n->∞)(f(n)/g(n)) ≤ c
->只要f(n)/g(n)的極限值不是∞，則f(n) ∈ O(g(n))


![](https://hackmd.io/_uploads/H18TMkqCn.png)

![](https://hackmd.io/_uploads/H1DzQ1qRh.png)

## Class
ex.
```
#include<iostream>
using namespace std;

class Course{
    friend void print(Course &c); //讓外部print function可以引用private的部分

    private:
        string lecturer;
        int students;
        double* scores;

    public:
        void set_lecturer(string);
        string get_lecturer();
        void set_student_number(int);
        int get_student_number();
        void set_score_list(double*);
        double get_score_list();
        double average();
        //Course();                         //不帶引數的建構式
        //Course(string, int, double*);     //帶引數的建構式
        Course(string = "Unknown", int = 0, double* = 0); //綜合建構式(初始狀態為未設引數的狀態)
        ~Course();  //解構式
};

void print(Course& c){
    cout << "Lecturer: " << c.lecturer << endl;
    cout << "Number of students: " << c.students << endl;
    for (int i = 0; i < c.students; i++){
        cout << i + 1 << "-th: " << c.scores[i] << endl;
    }
    cout << "Avr:" << c.average() << endl;
}
void Course::set_lecturer(string n){
    lecturer = n;
}
string Course::get_lecturer(){
    return lecturer;
}
void Course::set_student_number(int n){
    students = n;
}
int Course::get_student_number(){
    return students;
}
void Course::set_score_list(double* n){
    for (int i; i < students; i++){
        scores = n;
    }
}
double Course::get_score_list(){
    for (int i; i < students; i++){
        return scores[i];
    }
}
double Course::average(){
    double m = 0;
    for (int i = 0; i < students; i++)
    {
        m += scores[i];
    }
    m /= students;
    return m;
}
// Course::Course(){    //不帶引數建構式
//     lecturer = "unknown";
//     students = 0;
//     scores = 0;
// }
Course::Course(string n, int num, double* scores_list){
    lecturer = n;
    students = num;
    scores = scores_list;
}
Course::~Course(){
    cout << "Bye!" << endl;
}
int main()
{
    Course A;
    double scores[3] = { 80, 70, 60 };
    A.set_lecturer(" C.YL ");
    A.set_student_number(3);
    A.set_score_list(scores);
    print(A);
    return 0;
}
```

## 函式模板
ex. T代號可視輸入情況代表不同的資料型別
```
template <class T>
T bool isBigger(T a, T b){
    return a > b;
}
```
### 檔案內容
宣告、模板放在.h
定義放在.cpp
標頭檔的位置在資料夾內要用" "
* #include "file.h" (資料夾內)
* #include <file.h> (Compiler的路徑內)
使用ifndef避免重複宣告

## Array
![](https://hackmd.io/_uploads/SJJnQYqxT.png)
![](https://hackmd.io/_uploads/SyeeEt5xa.png)
## Vector
![](https://hackmd.io/_uploads/rJVE4OHZp.png)
### auto
```
for(auto iter = v.begin(); iter != v.end(); iter++){
    cout << *iter << " " << endl;
}
```
### insert
![](https://hackmd.io/_uploads/r1OMH_HZ6.png)

### erase(前閉後開)
![](https://hackmd.io/_uploads/ByhYHdrb6.png)

### erase-remove
![](https://hackmd.io/_uploads/rJ17ksSWp.png)

### 二維vector
![](https://hackmd.io/_uploads/B141giH-a.png)
```
vector< vector<int> > data_1(9);    //宣告一個vector data_1 裡面有9個vector
vector<int> data_2[9];              //宣告一個陣列 data_2 裡面有9個vector
```
### Leetcode
![](https://hackmd.io/_uploads/rJ1sLIK-p.png)

hint: 設邊界
```
class Solution {
public:
    vector<int> spiralOrder(vector<vector<int>>& matrix) {
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
```
## 鏈結串列
![](https://hackmd.io/_uploads/S1VddKmM6.png)

![](https://hackmd.io/_uploads/HyzaQd9Wp.png)
![](https://hackmd.io/_uploads/SJHlVu5ba.png)
### reverse()
![](https://hackmd.io/_uploads/BJYSfn9-a.png)

```
template <typename T>
void Linked_List<T>::Reverse()
{
    if(First == nullptr){
        return;
    }
    if(First->Next == nullptr){
        return;
    }
    Node<T> *previous = nullptr;
    Node<T> *current = First;
    Node<T> *preceding = First->Next;

    while(preceding != nullptr){
        current->Next = previous;
        previous = current;
        current = preceding;
        preceding = preceding->Next;
    }
    current->Next = previous;
    First = current;
}
```
### Linked List種類
1. 單向
2. 雙向
![](https://hackmd.io/_uploads/SkafSIT-a.png)

3. 環狀
![](https://hackmd.io/_uploads/SJItHI6Za.png)

* Rotate作法： (改變Head指到的節點)
```
template <typename T>
void Linked_List<T>::Rotate(int offset)
{
    //offset = offset % size()
    if(Head == nullptr){
        return;
    }
    offset = offset % Len;
    for (int i = 0; i < offset; i++){
        Head = Head->Next;
    }
}
```

![](https://hackmd.io/_uploads/BJa7Yka-6.png)
## Stack & Queue
### Stack 

* last-in-first-out
* 插入刪除在同側（洗盤子拿盤子）

用途：
![](https://hackmd.io/_uploads/SyY0aC4GT.png)

#### ex. 老鼠走迷宮
![Pasted Graphic.png](https://hackmd.io/_uploads/B1ceiL8Xp.png)
```
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
```

#### 中序(Infix)轉後序(Postfix)
![image.png](https://hackmd.io/_uploads/Hkk1c7DXp.png)
```
#include<iostream>
#include<stack>

using namespace std;

int main(){
    string str = "((a+(b*c))+d)";
    stack<char> sign;

    for (char c : str)
    {
        if(c == '('){
            continue;
        }
        else if(c >= 'a' && c <= 'z'){
            cout << c;
        }
        else if(c == ')'){
            cout << sign.top();
            sign.pop();
        }
        else{
            sign.push(c);
        }
    }
    return 0;
}
```





### Queue

* first-in-first-out
* 插入刪除在不同側（排隊）
![image.png](https://hackmd.io/_uploads/r1Sby5O76.png)
![image.png](https://hackmd.io/_uploads/SJCY15_ma.png)
![image](https://hackmd.io/_uploads/SyvJCLeN6.png)
![image](https://hackmd.io/_uploads/BygybC8eV6.png)


## Hashtable

![image](https://hackmd.io/_uploads/Hkw9bvxEp.png)

### unordered_map
```
unordered_map<Key, Value> Name = {};
```
ex.
```
unordered_map<string, int> umap = {
    {"Tom", 1},
    {"Ann", 4},
    {"Jack", 2}
};
```
#### 插入元素
```
umap[key] = value
```
ex.
```
umap["John"] = 3;
```

#### 取得key的元素
ex.
```
cout << "id: " << umap["John"] << "\n";
```
#### 移除元素
ex.
```
umap.erase("Ann");
```
#### 迴圈尋訪unordered_map
1. method1
```
for (const auto& n : umap) {
    std::cout << "name: " << n.first << ", id: " << n.second << "\n";
}
```
2. method2 (iterator)
```
for (auto it = umap.begin(); it != umap.end(); it++) {
    std::cout << "name: " << (*it).first << ", id: " << (*it).second << "\n";
}
```
#### 清空
```
umap.clear();
```

## Tree
### 條件：
1. 只有一個根節點
2. 每個node只能有一個父節點
3. 不能形成環
4. 子樹無次序之分

![image](https://hackmd.io/_uploads/BJARqGjN6.png)

### 常用名詞
Degree(分歧度)：有幾個子節點
Leaf node(external node) : 沒有子節點的node
Non-Leaf node(internal node) : 有子節點的node
Sibling nodes : 有相同的父節點的節點們
Ancestor nodes : node往Parent方向走會經過的node
Descendant nodes : node往Child方向走會經過的node
Level : 根節點為Level 1
Height : 離最遠Descendant node間的edge數(初始定義不一定為0)
Depth : node與根節點的edge數

### 樹的紀錄法
![image](https://hackmd.io/_uploads/H1fGxmiVa.png)
1. 括號法 :
A(B(DE(G)F)C)

2. Linked List
![image](https://hackmd.io/_uploads/SJ87f7sV6.png)


![image](https://hackmd.io/_uploads/S1lPVwo4T.png)


![image](https://hackmd.io/_uploads/rJl9VDiVp.png)


### 二元樹定義
degree of tree = 2
每個父節點最多只能連到兩個子節點
左右子樹有次序之分

![image](https://hackmd.io/_uploads/Hy3v5Di4T.png)


### 二元樹表示法
![image](https://hackmd.io/_uploads/SkQdTviV6.png)


#### 陣列表示法
![image](https://hackmd.io/_uploads/r1vByOoEp.png)

![image](https://hackmd.io/_uploads/B1cUJdiVa.png)


#### 鏈結串列表示法
![image](https://hackmd.io/_uploads/SyCq-uiEp.png)
![image](https://hackmd.io/_uploads/HJ5hZujEp.png)


### 二元樹建立
![image](https://hackmd.io/_uploads/BJcvM_oVa.png)
![image](https://hackmd.io/_uploads/SyNqfuiEa.png)

### 二元樹尋訪
1. 前序(Pre-order)
中->左->右

2. 中序(In-order)
左->中->右
排出的順序會依大小排列

3. 後序(Post-order)
左->右->中

ex1
![image](https://hackmd.io/_uploads/r1GXJ8hN6.png)
ex2
![image](https://hackmd.io/_uploads/BymKgI346.png)
![image](https://hackmd.io/_uploads/S1M3xLhN6.png)

#### Predecessor
照大小排序，該節點的前一個節點
1. 左子樹不為空：左子樹的Right most
2. 左子樹為空：
    1. 找該節點的Ancestor Node
    2. 且該節點在Ancestor Node的右子樹

#### Successor
照大小排序，該節點的後一個節點
1. 右子樹不為空：右子樹的Left most
2. 右子樹為空：
    1. 找該節點的Ancestor Node
    2. 且該節點在Ancestor Node的左子樹

### BST delete node
分成3個cases
1. Leaf node : 直接刪除
2. One child : 刪除後，child補上位置
3. Two children : 找右子樹中的最小節點取代欲刪除的節點
```
/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
public:
    TreeNode* Left_Most(TreeNode* node){
        while(node->left != nullptr){
            node = node->left;
        }
        return node;
    }

    TreeNode* deleteNode(TreeNode* root, int key) {
        if(root == nullptr){
            return root;
        }
        //找key的位置
        else if(root->val < key){
            root->right = deleteNode(root->right, key);
        }
        else if(root->val > key){
            root->left = deleteNode(root->left, key);
        }
        else{
            //Case 1 Leaf node
            if(root->left == nullptr && root->right == nullptr){
                return nullptr;
            }
            
            //Case 2 One child
            else if(!root->left || !root->right){
                return root->left ? root->left : root->right;
            }
            
            //Case 3 Two children
            TreeNode* successor = Left_Most(root->right);
            root->val = successor->val;
            root->right = deleteNode(root->right, successor->val);
        }
        return root;
    }
};
```
### Diameter of Binary Tree
```
/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
public:
    int ans = 0;
    int height(TreeNode* root){
        if(root == nullptr){
            return 0;
        }
        int lheight = height(root->left);
        int rheight = height(root->right);
        ans = max(ans, lheight + rheight);
        return 1 + max(lheight, rheight);
    }
    int diameterOfBinaryTree(TreeNode* root) {
        height(root);
        return ans;
    }   
};
```

### 唯一的二元樹
下列狀況可以決定唯一的二元樹
1. 中序與前序
![image](https://hackmd.io/_uploads/r1MWJHeB6.png)
2. 中序與後序

## 二元堆疊(Binary Heap)
![image](https://hackmd.io/_uploads/HyK--SgH6.png)

## 霍夫曼編碼
![image](https://hackmd.io/_uploads/HkcJbLxSp.png)






