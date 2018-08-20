#include <iostream>
#include <vector>

#include "Blackboard/blackboard_local.h"

using namespace std;

int main()
{
    cout << "-------------" << endl;

    Blackboard bb( std::unique_ptr<BlackboardLocal>( new BlackboardLocal) );
    std::vector<int> values = { 1,3,7 };

    bb.set("pippo", 5);
    bb.set("hello","world");
    bb.set("vect", values );

    cout << "-------------" << endl;

    int num = 0;
    std::string world("empty");
    std::vector<int> vals;

    bb.get("pippo", num);
    bb.get("hello", world);
    bb.get("vect", vals);

    cout << "pippo: "<< num << endl;
    cout << "hello: "<< world << endl;
    cout << "vect: " << vals[0] << " " << vals[1] << " " << vals[2] << endl;

    SafeAny::Any v(5u);

     cout << "var: " << v.convert<float>()  << endl;

    return 0;
}
