#include <iostream>
#include "callback.hpp"

class Foo
{
public:
    Foo() {}
    double MemberFunction(int a, int b)            { return a+b; }
    double ConstMemberFunction(int a, int b) const { return a-b; }
    static double StaticFunction(int a, int b)     { return a*b; }
};

double GlobalFunction(int a, int b) { return a/(double)b; }

void Invoke(int a, int b, moon9::callback<double (int, int)> callback)
{
    if(callback)
    {
        std::cout << callback(a, b) << std::endl;
    }
    else
    {
        std::cout << "Nothing to call." << std::endl; 
    }
}

int main()
{
    Foo f;
    Invoke(10, 20, make_callback_method(&f, &Foo::MemberFunction));
    Invoke(10, 20, make_callback_method(&f, &Foo::ConstMemberFunction));
    Invoke(10, 20, make_callback_function(&Foo::StaticFunction));

    moon9::callback<double (int, int)> cb = make_callback_function(&GlobalFunction);
    Invoke(10, 20, cb);

    cb = moon9::callback_null();
    Invoke(10, 20, cb);

    /* Expected output:
        30.000000
        -10.000000
        200.000000
        0.500000
        Nothing to call.
    */

    return 0;
}
