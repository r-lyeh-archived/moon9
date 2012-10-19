// Generates random names based on the statistical weight of letter sequences in a collection of sample names
// Original code by http://www.siliconcommandergames.com/MarkovNameGenerator.htm
// - rlyeh

#pragma once

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <random> // mt

#include "string.hpp"

namespace moon9
{
    class markov
    {
        public:

        markov();

        markov( const moon9::strings &sampleNames, size_t minLength = 4, size_t order = 1 );

        void setup( const moon9::strings &sampleNames, size_t minLength = 4, size_t order = 1 );

        //Get the next random name
        std::string next();

        //Reset the used names
        void forget();

    private:

        //private members
        std::map< std::string, std::string > _chains;
        moon9::strings _samples;
        moon9::strings _used;

        size_t _order;
        size_t _minLength;

        //Get a random letter from the chain
        char get_letter( std::string token );

        // mersenne twister random generator
        std::mt19937 gen;
        int rand( int from, int to );
    };
}

