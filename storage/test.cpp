#include <iostream>
#include "helper_types.h"

using namespace std;

class ListNode 
{
private:
    
public:
    ListNode *next;
    int *recordAddress;
    // Constructor for a ListNode
    ListNode(int *recordAddress)
    {
        this->recordAddress = recordAddress; 
        this->next = nullptr;
    }
};


int main() {
    int x = 69, y = 420;
    ListNode *head = nullptr, *temp = nullptr;
    
    for(int i=1; i<10; i++) {
        int *t = new int(i);
        if(head == nullptr) {
            head = new ListNode(t);
            temp = head;
        }
        else {
            temp->next = new ListNode(t);
            temp = temp->next;
        }
    }

    temp = head;
    while(temp) {
        cout << *(temp->recordAddress) << " ";
        temp = temp->next;
    }
}


