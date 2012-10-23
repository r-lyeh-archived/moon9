#include <iostream>

#include "callstack.hpp"

void f5() { std::cout << moon9::callstack().str(); }
void f4() { f5(); }
void f3() { f4(); }
void f2() { f3(); }
void f1() { f2(); }

int main( int argc, char **argv )
{
	f1();
	return 0;
}
