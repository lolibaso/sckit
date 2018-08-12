#include <queue>
#include <iostream>
#include <stdlib.h>

using namespace std;

int main(int argc, char* argv[])
{
    
    priority_queue<int>  pq;

    pq.push(1);
    pq.push(4);
    pq.push(8);
    pq.push(2);
    pq.push(3);
    pq.push(5);
    pq.push(7);
    pq.push(0);
    pq.push(6);

    cout<< "size: " << pq.size()<<endl;

    cout << pq.top() << endl;
    pq.pop();
    cout << pq.top() << endl;
    pq.pop();
    cout << pq.top() << endl;

    int m = atoi(argv[1]);
    cout << "input: " << m << endl;

    for(int i=0; i<pq.size(); i++) 
    {
        if(m > pq.top()) 
        {
            pq.pop();
            pq.push(m);
        }
    }

    cout << pq.top() << endl;
    pq.push(9);
    pq.pop();
    cout << pq.top() << endl;
    pq.pop();
    cout << pq.top() << endl;
    pq.pop();
    cout << pq.top() << endl;

   

    return 0;
}
