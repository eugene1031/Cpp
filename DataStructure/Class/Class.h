#ifndef CLASS_H_INCLUDED
#define class_h_included
#include<iostream>
#include<stdlib.h>

using namespace std;

class Course
{
    friend void print(Course &c); //讓外部print function可以引用private的部分
    friend ostream &operator<<(ostream&, Course&);
    friend istream &operator>>(istream&, Course&);

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
    double* get_score_list();
    double average();
    // Course();                         //不帶引數的建構式
    // Course(string, int, double*);     //帶引數的建構式
    Course(string = "Unknown", int = 0); // 綜合建構式(初始狀態為未設引數的狀態)
    ~Course();                           // 解構式
    float operator[](int);
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
double* Course::get_score_list(){
    return scores;
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
Course::Course(string n, int num){
    lecturer = n;
    students = num;
    if(num!=0){
        scores = (double *) calloc(num, sizeof(double));
    }
}
Course::~Course(){
    cout << "Bye!" << endl;
    free(scores);
}

ostream& operator<<(ostream& os, Course& c){
    os << "Lecturer: " << c.lecturer << endl;
    os << "Number of students: " << c.students << endl;
    for (int i = 0; i < c.students; i++){
        os << i + 1 << "-th: " << c.scores[i] << endl;
    }
    os << "Avr:" << c.average() << endl;
    return os;
}
istream& operator>>(istream& is, Course& c){
    cout << "Please enter lecturer and students number: " << endl;
    is >> c.lecturer >> c.students;
    if (c.students > 0) {
        c.scores = (double *)calloc(c.students, sizeof(double));
    }
    cout << "Please enter enter " << c.students << " scores: " << endl;
    for (int i = 0; i < c.students; i++)
    {
        is >> *(c.scores+i);
    }
    return is;
}
float Course::operator[](int index){
    return scores[index];
}

#endif