// Generates random names based on the statistical weight of letter sequences in a collection of sample names
// Original code by http://www.siliconcommandergames.com/MarkovNameGenerator.htm
// - rlyeh

#pragma once

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <random> // mt

namespace moon9
{
    class markov
    {
        public:

        markov();

        explicit
        markov( const std::string &list_of_names, const std::string &separators, size_t minLength = 4, size_t order = 1 );

        void setup( const std::string &list_of_names, const std::string &separators, size_t minLength = 4, size_t order = 1 );

        //Get the next random name
        std::string next();
        std::string operator()() { return next(); }

        //Reset the used names
        void forget();

    private:

        //private members
        std::map< std::string, std::string > _chains;
        std::vector< std::string > _samples;
        std::vector< std::string > _used;

        size_t _order;
        size_t _minLength;

        //Get a random letter from the chain
        char get_letter( std::string token );

        // mersenne twister random generator
        std::mt19937 gen;
        int rand( int from, int to );
    };
}

