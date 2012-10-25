// simple halffloat class
// - rlyeh
// ref : based on code by http://stackoverflow.com/questions/6162651/half-precision-floating-point-in-java

#pragma once

class float16
{
    public:

        float16();
        float16( float _v );

        operator float() const;

        template<typename T> float16 &operator +=( const T &t ) { v = float2half(half2float(v)+t); return *this; }
        template<typename T> float16 &operator -=( const T &t ) { v = float2half(half2float(v)-t); return *this; }
        template<typename T> float16 &operator *=( const T &t ) { v = float2half(half2float(v)*t); return *this; }
        template<typename T> float16 &operator /=( const T &t ) { v = float2half(half2float(v)/t); return *this; }

    protected:

        unsigned short v;
};


// ignores the higher 16 bits
float half2float( unsigned short hbits );

// returns all higher 16 bits as 0 for all results
unsigned short float2half( float fval );
