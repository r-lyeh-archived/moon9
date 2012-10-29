// Generates random names based on the statistical weight of letter sequences in a collection of sample names
// Original code by http://www.siliconcommandergames.com/MarkovNameGenerator.htm
// - rlyeh

#include <cassert>

#include <algorithm>
#include <map>
#include <random>
#include <string>
#include <vector>
#include <iostream>

#include "markov.hpp"
#include "markov.inl"

namespace moon9
{
    markov::markov()
    {}

    markov::markov( const std::string &list_of_names, size_t minLength, size_t order )
    {
        setup( list_of_names, minLength, order );
    }

    void markov::setup( const std::string &list_of_names, size_t minLength, size_t order )
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

        std::vector<std::string> sampleNames = tokenize( list_of_names, " ,;\r\n\t" );

        size_t smallest = ~0;
        size_t longest = 0;

        for( std::vector<std::string>::const_iterator it = sampleNames.begin(), end = sampleNames.end(); it != end; ++it )
        {
            const std::string &entry = ( *it );
            //if( entry.size() >= order + 1 )
            {
                _samples.push_back( entry );
                longest = ( entry.size() > longest ? entry.size() : longest );
                smallest = ( entry.size() < smallest ? entry.size() : smallest );
            }
        }

        //if( !_samples.size() ) std::cerr << "Uh oh!" << std::endl;

        // adjust minlength in case provided samples are smaller than required output length
        if( longest < _minLength )
            _minLength = longest;

        if( _minLength > smallest )
            _minLength = smallest;

        //std::cout << longest << ',' << smallest << std::endl;

        //Build chains
        for( std::vector<std::string>::const_iterator it = _samples.begin(), end = _samples.end(); it != end; ++it )
        {
            const std::string &word = *it;

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
        if( !_samples.size() )
            return std::string();

        //get a random token somewhere in middle of sample word
        std::string s;

        do
        {
            size_t n = rand( 0, _samples.size() - 1 );
            size_t nameLength = _samples[n].size();
			if( nameLength > _order )
	           s = _samples[n].substr( rand( 0, nameLength - _order - 1 ), _order );
			else
			   s = _samples[n];
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
                s = s.substr(0, 1) + s.substr(1);
            }
            else
            {
                std::vector<std::string> tokens = tokenize( s, " " );
                s = std::string();
                for( size_t t = 0; t < tokens.size(); t++ )
                {
                    if( !tokens[t].size() )
                        continue;
                    if( tokens[t].size() == 1 )
                        tokens[t] = tokens[t];
                    else
                        tokens[t] = tokens[t].substr(0, 1) + tokens[t].substr(1);
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

