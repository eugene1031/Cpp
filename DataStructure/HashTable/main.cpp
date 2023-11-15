#include <iostream>
#include <vector>
#include <list>

using namespace std;

template <typename T1, typename T2>
struct Data
{
    T1 Key;
    T2 Value;
};

template <typename T1, typename T2>
class Unordered_Map
{
private:
    int len;
    vector<list<Data<T1, T2>>> data;
    int Hash_Func_Div(T1);
    int Hash_Func_Mul(T1);

public:
    T2 &operator[](T1);
    Unordered_Map(int = 128);
};
template <typename T1, typename T2>
Unordered_Map<T1, T2>::Unordered_Map(int m)
{
    len = m;
    data.resize(m);
}
template <typename T1, typename T2>
T2 &Unordered_Map<T1, T2>::operator[](T1 input)
{
    int index = Hash_Func_Mul(input);
    // data[index]: linked_list
    for (auto iter.data[index].begin(); iter != data[index].end(); iter++)
    {
        if ((*iter).Key == input)
        {
            return (*iter).Value;
        }
    }
    data[index].push_back(Data<T1, T2>{input, 0});
    return data[index].back().Value;
}

template <typename T1, typename T2>
int Unordered_Map<T1, T2>::Hash_Func_Div(T1 input)
{
    long long int sum = 0;
    for (char c : input)
    {
        sum += c;
    }
    return sum % len;
}
template <typename T1, typename T2>
int Unordered_Map<T1, T2>::Hash_Func_Mul(T1 input)
{
    long long int sum = 0;
    double c = (sqrt(5) - 1) / 2.0;
    int tmp = 1;
    for (char c : input)
    {
        sum += tmp * c;
        tmp *= 256;
    }
    double frac = c * sum - (int)(c * sum);
    int index = frac * len;
    return index;
}
int main()
{
    string str;
    cout << "Please enter a str:" << endl;
    cin >> str;
    int number;
    cout << "Please enter a balance:" << endl;
    cin >> number;
    Unordered_Map<string, int> balance;
    balance[str] = number;
    cout << balance[str];
    return 0;
}