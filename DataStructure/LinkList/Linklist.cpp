#include<iostream>
//#include"Linklist.h"
#include"Circular_Linked_List.h"


using namespace std;

int main(){
    Linked_List<int> List;
    // List.Push_Front(1);
    // List.Push_Front(2);
    // List.Push_Front(3);
    // List.Print_List();
    // List.Pop_Front();
    // List.Pop_Front();
    // List.Print_List();
    //List.Clear();
    //List.Reverse();
    //List.Print_List();
    // List.Pop_Front();
    // List.Pop_Front();
    // List.Print_List();

    List.Push_Back(1);
    List.Push_Back(2);
    List.Push_Back(3);
    List.Push_Back(4);
    List.Push_Back(5);
    List.Push_Back(6);
    List.Push_Back(7);

    // for (auto iter = List.Begin(); iter != List.End(); iter++){
    //     cout << *iter << " ";
    // }
    // Linked_List<int>::Iterator iter1 = Find(List, 3);
    // Linked_List<int>::Iterator iter2 = Find(List, 7);
    
    // List.Insert(Find(List, 4), 100);
    // //List.Erase(Find(List, 4));

    // for (; iter1 != iter2; iter1++)
    // {
    //     cout << *iter1 << " ";
    // }

        // List.Print_List();
        // List.Pop_Back();
        // List.Pop_Back();
        // List.Print_List();
        // List.Pop_Back();
        // List.Pop_Back();
        // List.Print_List();

        return 0;
}