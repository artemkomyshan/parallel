#include <iostream>
#include "utility/sequence.hpp"

using namespace std;

int main()
{
    start([]
    {
        cout << "first void job \n";
        return 6;
    }).next([](int i)
    {
        cout << "first int job " << i << "\n";
        return 7;
    }).next([](int i)
    {
        cout << "second job " << i << "\n";
    }).next([]
    {
        cout << "second void job\n";
    });

    cout << "Hello World!" << endl;
    return 0;
}
