// Sample

#include <iostream>

#include "serial.hpp"

class object
{
    float x,y;

    public:

    object() : x(10),y(20)
    {}

    moon9::serial &serialize( moon9::serial &inout )
    {
        // serialize own private data
        inout.protected_data("x", x);
        inout.protected_data("y", y);

        return inout;
    }
};

class Car : public object
{
    public:

    float speed;

    Car() : speed(0.f)
    {}

    moon9::serial &serialize( moon9::serial &inout )
    {
        // parent serializes first
        object::serialize( inout );

        // serialize own private data
        // [...]

        // serialize public data
        inout.public_data("speed", speed);

        return inout;
    }
};

int main( int argc, char **argv )
{
    Car car;
    car.speed += 12.f;

    std::cout << "current C class:" << std::endl << std::endl;
    moon9::serial::print(car);

    std::cout << "saving C class:" << std::endl << std::endl;
    std::string savegame = moon9::serial::save(car);

    car.speed = 123990;
    std::cout << "altered C class:" << std::endl << std::endl;
    moon9::serial::print(car);

    moon9::serial::load(car, savegame);
    std::cout << "restored C class:" << std::endl << std::endl;
    moon9::serial::print(car);

    return 0;
}
