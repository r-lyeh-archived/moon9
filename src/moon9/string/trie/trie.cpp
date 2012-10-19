#include "trie.hpp"

trie::trie() : flag(false)
{}

trie::trie( const std::string &_value ) : value(_value), flag(false)
{}

bool trie::find( const std::string &word ) const
{
    trie *node = const_cast<trie *>( this );
    for( size_t i = 0; i < word.size(); ++i )
    {
        const char &c = word[i];
        if( node->children.find( c ) == node->children.end() )
            return false;
        else
            node = &node->children[c];
    }
    return true;
}

void trie::insert( const std::string &word )
{
    trie *node = this;
    for( size_t i = 0; i < word.size(); ++i )
    {
        const char &c = word[i];
        if( node->children.find( c ) == node->children.end() )
            node->children[ c ] = trie( node->value + c );
        node = &node->children[c];
    }
    node->flag = true;
}

std::vector<std::string> trie::get_prefixes() const
{
    std::vector<std::string> results;

    if( flag )
        results.push_back( value );

    if( children.size() )
    {
        for( std::map<char, trie>::const_iterator it = children.begin(); it != children.end(); ++it )
        {
            std::vector<std::string> nodes = it->second.get_prefixes();
            for( std::vector<std::string>::const_iterator node = nodes.begin(); node != nodes.end(); node++ )
                results.push_back( *node );
        }
    }
    return results;
}

std::vector<std::string> trie::autocomplete( const std::string &prefix ) const
{
    trie *node = const_cast<trie *>( this );
    std::vector<std::string> results;

    for( size_t i = 0; i < prefix.size(); ++i )
    {
        const char &c = prefix[ i ];
        if( node->children.find( c ) == node->children.end() )
            return results;
        else
            node = &node->children[ c ];
    }
    return node->get_prefixes();
}
