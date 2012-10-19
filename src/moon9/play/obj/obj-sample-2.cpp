#include <map>
#include <string>
#include <sstream>
#include <iostream>

#include "obj.hpp"

// our obj components

objfield<      size_t > counter("counter");
objfield< std::string > id("id");

// our obj initialization

void obj::user_defined_clone( const obj *other )
{
    copy_field( id, other );
    copy_field( counter, other );
}

void obj::user_defined_erase()
{
    erase_field( id );
    erase_field( counter );
}

void obj::user_defined_print() const
{
    print_field( id );
    print_field( counter );
}

void obj::user_defined_diff( const obj *other ) const {}
void obj::user_defined_patch() {}

// our sample

#include <chrono>
#include <vector>
#include <iomanip>

int main( int argc, char **argv )
{
    {
        // construct an object

        std::cout << "Sizeof(empty obj)=" << sizeof(obj) << std::endl;

        obj obj1;
        obj1[ id ] = "obj1";
        obj1[ counter ] = 0;

        std::cout << "Sizeof(valid obj)=" << sizeof(obj1) << std::endl;

        // benchmark its access time

        {
            std::cout << "Benchmarking... ";
            std::chrono::microseconds seconds1, seconds2;
            {
                auto t_start = std::chrono::high_resolution_clock::now();
                for( size_t i = 0; i < 200000000; ++i )
                    obj1[counter]++;
                seconds1 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - t_start);
            }
            size_t &ref = obj1[counter];
            {
                auto t_start = std::chrono::high_resolution_clock::now();
                for( size_t i = 0; i < 200000000; ++i )
                    ref++;
                seconds2 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - t_start);
            }
            double relative_speed = double( seconds1.count() ) / seconds2.count();
            std::cout << "relative access is x" << std::fixed << std::setprecision(2) << relative_speed << " times " << ( seconds1 >= seconds2 ? "slower" : "faster" ) << " than direct access" << std::endl;
        }

        // debug obj

        std::cout << obj1.str() << std::endl;
    }

    return 0;
}
