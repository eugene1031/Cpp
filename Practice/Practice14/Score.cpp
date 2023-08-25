#include<iostream>

using namespace std;

int main(){
    int scores[5];
    int sum = 0;

    cout << "Please enter 5 scores\n";
    for (int i = 0; i < 5; i++){
        cin >> scores[i];
        sum += scores[i];
    }
    float avr = sum / 5;

    float squarsum = 0;
    for (int i = 0; i < 5; i++){
        squarsum += pow(scores[i] - avr, 2);
    }
    float SD = pow(squarsum / 5, 0.5);

    cout << "Average: " << avr << endl;
    cout << "SD: " << SD << endl;
    
    for (int i = 0; i < 5; i++){
        if(scores[i] < avr){
            cout << "Number " << i + 1 << " is below the average!\n";
        }
    }
        return 0;
}