// Generates random names based on the statistical weight of letter sequences in a collection of sample names
// Original code by http://www.siliconcommandergames.com/MarkovNameGenerator.htm
// - rlyeh

#include <algorithm>
#include <map>
#include <random>
#include <string>
#include <vector>

#include "markov.hpp"
#include "string.hpp"

namespace moon9
{
    markov::markov()
    {}

    markov::markov( const moon9::strings &sampleNames, size_t minLength, size_t order )
    {
        setup( sampleNames, minLength, order );
    }

    void markov::setup( const moon9::strings &sampleNames, size_t minLength, size_t order )
    {
        // clean up
        *this = markov();

        // reset mersenne twister
        std::random_device rd;
        gen = std::mt19937( rd() );

        //fix parameter values
        if( order < 1 )
            order = 1;
        if( minLength < 1 )
            minLength = 1;

        _order = order;
        _minLength = minLength;

        for( moon9::strings::const_iterator it = sampleNames.begin(), end = sampleNames.end(); it != end; ++it )
        {
            std::string upper = it->strip().uppercase();
            if( upper.size() >= order + 1 )
                _samples.push_back(upper);
        }

        //Build chains
        for( moon9::strings::const_iterator it = _samples.begin(), end = _samples.end(); it != end; ++it )
        {
            const moon9::string &word = *it;

            for( size_t letter = 0; letter < word.size() - order; letter++ )
            {
                std::string token = word.substr( letter, order );
                ( _chains[token] = _chains[token] ).push_back( word[ letter + order ] );
            }
        }
    }

    //Get the next random name
    std::string markov::next()
    {
        //get a random token somewhere in middle of sample word
        std::string s;

        do
        {
            size_t n = rand( 0, _samples.size() - 1 );
            size_t nameLength = _samples[n].size();
            s = _samples[n].substr( rand( 0, _samples[n].size() - _order - 1 ), _order );
            while( s.size() < nameLength )
            {
                std::string token = s.substr( s.size() - _order, _order );
                char c = get_letter( token );
                if (c != '?')
                    s += get_letter( token );
                else
                    break;
            }

            if( std::find( s.begin(), s.end(), ' ' ) == s.end() )
            {
                s = moon9::string( s.substr(0, 1) + s.substr(1) ).lowercase();
            }
            else
            {
                moon9::strings tokens = moon9::string(s).tokenize(" ");
                s = std::string();
                for( size_t t = 0; t < tokens.size(); t++ )
                {
                    if( !tokens[t].size() )
                        continue;
                    if( tokens[t].size() == 1 )
                        tokens[t] = tokens[t].uppercase();
                    else
                        tokens[t] = moon9::string( tokens[t].substr(0, 1) + tokens[t].substr(1) ).lowercase();
                    if( s.size() )
                        s += ' ';
                    s += tokens[t];
                }
            }
        }
        while ( std::find( _used.begin(), _used.end(), s ) != _used.end() || s.size() < _minLength );

        // We filter name to avoid false positives ('shit', 'orange', 'mama' ...)
        //
        // if( google(s).num_results > 0 )
        // s = next();
        _used.push_back(s);

        return s;
    }

    //Reset the used names
    void markov::forget()
    {
        _used.clear();
    }

    //Get a random letter from the chain
    char markov::get_letter( std::string token )
    {
        if( _chains.find(token) == _chains.end() )
            return '?';
        std::string letters = _chains[token];
        size_t n = rand( 0, letters.size() - 1 );
        return letters[n];
    }

    int markov::rand( int from, int to )
    {
        return std::uniform_int_distribution<int>(from, to)(gen);
    }
}

