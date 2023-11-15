#include<iostream>
#include<queue>

using namespace std;

int main(){
    queue<char> boys, girls;
    int rounds, num_boy, num_girl;

    cout << "Please enter rounds, number of boys and girls:" << endl;
    cin >> rounds >> num_boy >> num_girl;

    for (int i = 0; i < num_boy; i++){
        boys.push('A' + i);
    }
    for (int j = 0; j < num_girl; j++){
        girls.push('a' + j);
    }

    cout << "It's party time!" << endl;
    for (int i = 0; i < rounds; i++){
        cout << "Rounds #" << i + 1 << ": " << boys.front() << " <-> " << girls.front() << endl;
        boys.push(boys.front());
        girls.push(girls.front());
        boys.pop();
        girls.pop();
    }

        return 0;
}