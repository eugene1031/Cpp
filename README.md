# C++ Study Note
## VScode常用指令
自動排版：shift + option + f
關閉分頁：command + w
選多行：按住option
註解多行：command + k -> command + c
取消註解：command + k -> command + u
執行：control + option + n
選取所有相同變數：command + d
## #include<>
#include<iomanip> 
    : setw() ：設輸出寬度
      setprecision() : 設位數
      fixed << setprecision() : 設小數位數
## Switch

```
switch(int){
    
    case(int):    
    cout <<;
    break;      //有break才會停，不然會繼續往下看其他case
    
    case(int):
    case(int):  //這兩個case做同件事
    cout << ;
    break;
    
    default:    //else的概念
    cout << ;
    break;
    }
```
## Try,Catch
ex. a/b 當b = 0 時 -> Error
```
float a, b;
    cout << "Enter two float\n";
    try{
        cin >> a >> b;
        if (b == 0)
        {
            throw b;
        }
        else{
            cout << "a/b= " << a / b << endl;
        }
    }
    catch(...){
        cout << "Error!"<<endl;
    }
```
## 檢查數列中數字同異
```
int num;
    cout << "Please enter 1 integer(4 number) \n";
    cin >> num;
    bool flag = true;
    for (int i = 0; i < 4; i++){
        int A = num / (int) pow(10, i) % 10;
        for (int j = i + 1; j < 4; j++){
            int B = num / (int) pow(10, j) % 10; //pow(10, j)為double不能％（需轉型int)
            if(A == B){
                flag = false;
            }
        }
    }
    if(flag == true){
        cout << "Right! \n";
    }
    else{
        cout << "Wrong! \n";
    }
```
## For vs While
for : 通常用在已知執行次數、範圍時
while : 通常用在未知執行次數、範圍時
## ex. 1A2B
```
int ans = 1234;
    int guess = 0000;
    int countA = 0;
    int countB = 0;
    while (ans != guess)
    {
        cout << "Guess a number(in 4)\n";
        cin >> guess;
        for (int i = 0; i < 4; i++)
        {
            int a = ans / (int)pow(10, i) % 10;
            for (int j = 0; j < 4; j++)
            {
                int g = guess / (int)pow(10, j) % 10;
                if (a == g)
                {
                    if (i == j)
                    {
                        countA++;
                    }
                    else{
                        countB++;
                    }
                }
            }
        }
        cout << countA << "A" << countB << "B\n";
        countA = 0;
        countB = 0;
    }
    while(ans == guess){
        cout << "You win!\n";
        break;
    }
```
## Range-based for loop
for字串＆陣列
在某特定範圍內，利用for依序把東西取出
```
for(暫存變數：範圍){
    程式碼;
    }
```
ex.
```
int arr[3] = {1, 2, 3};
for(int a : arr){
    cout << a;
    }
```
## Pointer
`資料型態 *指標名稱`
1. 數值存給指標
ex. 
`float *p;    //float型別, 名稱為p的指標`
ex.
```
int v = 10;
int *p = &v; //把v的位址存在*p裡面
cout << p;   //p = &v
cout << &p;  //p的位址
cout << *p;  //*p = 10 = v的值
```
2. 陣列存給指標時，會把陣列第一個的位址存入指標
ex.
```
int arr[3];
int *p = arr;               //把arr[0]的位址存在*p裡面
for(int i = 0; i < 3; i++){
    p[i] = i + 1;           //這兩種方法都可以
    *(p + i) = i + 1;       //這兩種方法都可以
}
for(int a:arr){
    cout << a << " ";       //a = 1, 2, 3
    }
```
### 動態記憶體配置 (不熟)
1. #include <stdlib.h>
2. 用malloc()使作業系統挖出一塊空間 
    Pointer = (資料型態*)malloc(sizeof(資料型態) * 個數);
3. 結束後free(指標名稱)釋放記憶體

### 多重指標
![](https://hackmd.io/_uploads/Bk6mXvvT2.png)

1. 
* v  = 10
* p1 = 0x01
* p2 = 0x02
* p3 = 0x03
2. 
* &v  = 0x01
* &p1 = 0x02
* &p2 = 0x03
* &p3 = 0x04
3.
* *p1   = 10
* *p2   = 0x01
* **p2  = 10
* *p3   = 0x02
* **p3  = 0x01
* ***p3 = 10
    
### 參考
    
`資料型態 &參考名稱 = 被參考變數`
1. 宣告時於資料型態後加&
2. 參考一定要被初始化
    
ex. 
```
float f = 1.5;    
float *p = &f;    //p  = f的位址
float &f2 = f;    //f2 = 1.5
float *&p2 = p;   //p2 = p = f的位址
```

### 字串長度計算
```
char *ptr = str
while(*ptr != str)
ptr++;
return ptr - str;
```

## 函式

```
輸出資料型態 函式名稱( 輸入資料型態 引數1, 輸入資料型態 引數2, ... ){
    程式碼
    
    return 回傳值;
    }
```
ex1.
```
int sum(int N){
    for (int i = N - 1; i > -1; i--){
        N += i;
    }
    return N;
}  
```

ex2.
    void : 不需return值  
```
void hello(void){
    cout << " Hi " << endl; 
    }
```
### Array
若要在函數中放入陣列，需使用指標+陣列長度
ex.
```
void average(float *score, int num)
{
    float avr = 0;
    for (int i = 0; i < num; i++)
    {
        avr += score[i];
    }
    avr /= num;
    cout << " Average of number is : " << avr << endl;
}
```

### Pass by value vs Pass by reference
由於函式一次只能回傳一筆資料，因此當需要回傳大於一筆資料時，使用pass by reference(內外參數連動)
ex.
```
void cos(float a, float b, float c, float &cosa, float &cosb, float &cosc){  //pass by reference要加&
    cosa = (b * b + c * c - a * a) / (2 * b * c);
    cosb = (a * a + c * c - b * b) / (2 * a * c);
    cosc = (a * a + b * b - c * c) / (2 * a * b);
}
int main(){
    float a, b, c;
    float cosa, cosb, cosc;
    cout << " Please enter 3 length of triangle : \n";
    cin >> a >> b >> c;
    cos(a, b, c, d, e, f);
    cout << " cosA : " << cosa << endl;
    cout << " cosB : " << cosb << endl;
    cout << " cosC : " << cosc << endl;
    return 0;
}
```
### Recursion
#### Gcd
![](https://hackmd.io/_uploads/rJ3UbFja3.png)
```
int gcd(int a, int b)
{
    int c = a % b;
    if (c != 0)
    {
        a = b;
        b = c;
        return gcd(a, b);
    }
    else
        return b;
}
```
#### Hanoi Tower
```
int Hanoi(int N)
{
    if (N == 1)
    {
        return 1;
    }
    else
        return 2 * Hanoi(N - 1) + 1;
}
```
## include
### 執行時間計算
time_t的單位為秒（但只能為整數）
clock_t的單位為CPU時間（tick）
```
#include<time.h>

clock_t start, finish; //由於想知道小數部分，所以用clock_t
start = clock();       //開始當下時間
///待測程式碼///
finish = clock();      //結束當下時間
double time = (finish - start) / (double)(CLOCKS_PER_SEC);
```
### 產生隨機數
```
#include<stdlib.h>
#include<time.h>  //若沒有include時間當作隨機的種子，每次隨機出來的數會是固定的
srand(time(NULL));
cout << rand();
```
### Monte Carlo推Pi
![](https://hackmd.io/_uploads/SJZLnhnT2.png)
```
srand(time(NULL));
    int in = 0, out = 0;
    for (int i = 0; i < 10000000; i++)
    {
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;
        if(x * x + y * y > 1){
            out++;
        }
        else if(x * x + y * y < 1)
            in++;
    }

    double pi = 4.0 * in / (in + out);
    cout << "Pi = " << pi << endl;
```

## 檔案輸入＆輸出
#include<fstream>
### 輸入
```
string str;
ifstream 任意名;
任意名.open("example.txt");
確認是否有開成功 //(任意名.is_open())
確認是否有讀到最結尾
任意名 >> str;
cout << str << endl;
用close()關檔 //任意名.close()
```
    
### 輸出
```
ofstream 任意名;
任意名.open("example.txt");
確認是否有開成功
任意名 << "要放入txt的內容";
利用close()關檔
```
ex.
```
#include <iostream>
#include <fstream>              //需include file stream

using namespace std;

int main()
{
    string str;
    cout << " Please enter your string: \n";
    getline(cin, str);
    
    ////////寫檔//////////
    ofstream file;
    file.open("example.txt");
    if (file.is_open())        //確認是否有開成功
    {
        file << str;
        cout << " Finish writing data into example.txt. \n";
    }
    else
        cout << " File not exist! " << endl;
    file.close();

    ////////讀檔//////////
    cout << "The content on the example.txt is ......\n";
    ifstream a;
    a.open("example.txt");
    if (a.is_open())         //確認是否有開成功
    {
        string abc;
        while (!a.eof())    //確認是否有讀到最結尾
        {
            getline(a, abc);
            cout << abc << endl;
        }
    }
    else
        cout << " File not exist! " << endl;
    a.close();
    cout << "Finishing reading data from example.txt. \n";

    return 0;
}
```
### CSV檔處理
CSV = Comma-Seperated Values
excel檔的行跟行之間由逗號分隔
輸入時：
`getline(filename, string, ','); //第三個引數代表讀到哪個字元停止`
![](https://hackmd.io/_uploads/BJhCagRan.png)
由於斷行符號無法看到，因此在讀取時分為三個步驟
第一步為一行一行讀入
第二步為把字串轉成字串串流
第三步讀到逗號就停，逐步取出資料

輸出：
ex.
```
#include<iostream>
#include<fstream>
using namespace std;
int main(){
    ofstream file;
    file.open("MulTable.csv");
    if(file.is_open()){
        for (int i = 1; i < 10; i++){
            for (int j = 1; j < 10; j++){
                file << i * j << ',';
            }
            file << endl;
        }
    }
    else
        cout << "Error!\n";
    return 0;
}
```
## 列舉
```
enum class status{
    isNormal,     //0
    isSuspension, //1
    isDropout,    //2
    isRelegation  //3
    };
```
## 結構
```
struct triangle{
    float a;
    float b;
    float c;
    };
struct triangle t = {1.0, 2.0, 3.0};
cout << "a:" << t.a << endl;
cout << "b:" << t.b << endl;
cout << "c:" << t.c << endl;
```

把要回傳的變數打包在一個Structure -> 可以一次回傳多個變數
```
struct triangle return_triangle(){
    struct triangle t = {1.0, 2.0, 3.0};
    return t;
    }
```
### typedef
：把資料型態重新命名
```
typedef 原本資料型態的名稱 新名稱
```
ex. 把struct triangle重新命名成triangle
```
struct triangle t; -> typedef struct triangle triangle;
```
ex. 最常用
```
typedef struct{
    float a;
    float b;
    float c;
    }triangle;
triangle p;
```

ex. 算平均成績
```
#include<iostream>
using namespace std;

typedef struct{
    string name;
    float English;
    float Math;
} Score;

float average(Score s){
    return (s.English + s.Math) / 2;
}

int main()
{
    Score a = {"Mick", 90, 100};
    cout << "The average of " << a.name << " : " << average(a) << endl;
}
```
## 重載運算子
ex. 想把struct直接cout出來
```
ostream& operator<<(ostream& os, Score s){
    os << "Name: " << s.name << endl;
    os << "mathScore: " << s.Math << endl;
    os << "englishScore: " << s.English << endl;
    return os;
    }
```
ex. 想比較兩個結構
```
bool operator>(Score s1, Score s2){
    if(average(s1) > average(s2))
    return true;
    else
    return false;
    }
bool operator<(Score s1, Score s2){
    if(average(s1) < average(s2))
    return true;
    else
    return false;
    }
    
int main(){
if(s1 > s2){
    cout << s1.name << " is better. " << endl;
else
    cout << s2.name << " is better. " << endl;
    }
    return 0;
}
```
ex. (x, y)相加
```
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
```
## 標頭檔建立與使用
    
1. .h檔放宣告（不需放參數）
```
#ifndef oprerator_H_included    //怕重複include的保護機制
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
```
    
2. .cpp檔放定義
```
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
```
    
3. main.cpp放主程式
```
#include<iostream>
#include"Operator.h"
#include"Operator.cpp"
using namespace std;

int main()
{
    XY a = {1.8, 2.0};
    XY b = {3.5, 4.1};
    cout << a << endl;
    cout << b << endl;
    cout << a + b;
    return 0;
}
```
### extern
在不同cpp間傳遞變數，include後就可以使用該變數