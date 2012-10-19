/* Trie data structure using STL. Based on code by Vivek Narayanan
 * - rlyeh
 */

#pragma once

#include <map>
#include <vector>
#include <string>

class trie
{
    public:

        trie();

        bool find( const std::string &word ) const;
        void insert( const std::string &word );
        std::vector<std::string> get_prefixes() const;
        std::vector<std::string> autocomplete( const std::string &prefix ) const;

    protected:

        trie( const std::string &_value );

        std::map<char, trie> children;
        std::string value;
        bool flag;
};

