#include <cassert>
#include <iostream>
#include <vector>

#include "string.hpp"

#include "file.hpp"
#include "xml.hpp"


namespace moon9 {

xml::xml()
{}

xml::xml( const xml &t )
{
    str( t.str() );
}

xml::xml( const std::string &t )
{
    str( t );
}

xml &xml::operator=( const xml &t )
{
    doc.reset();
    parent = std::vector< pugi::xml_node >();
    latest = pugi::xml_node();

    str( t.str() );
    return *this;
}

// declarative construction

// push a level of depth, then insert node
xml &xml::push( const moon9::string &key, const moon9::string &text, const moon9::string &comment )
{
    if( latest.empty() || (!parent.size()) )
    {
        if( verbose )
            std::cout << "push new" << std::endl;

        latest = doc.append_child( pugi::node_element );
    }
    else
    {
        if( verbose )
            std::cout << "push" << std::endl;

        latest = parent.back().append_child( pugi::node_element );
    }

    update_node( latest, key, text, comment );

    parent.push_back( latest );

    return (*this);
}

// push a level of depth, then insert node
xml &xml::push( const pugi::xml_node &node )
{
    if( latest.empty() || (!parent.size()) )
    {
        if( verbose )
            std::cout << "push new" << std::endl;

        latest = doc.append_child( pugi::node_element );
    }
    else
    {
        if( verbose )
            std::cout << "push" << std::endl;

        latest = parent.back().append_child( pugi::node_element );
    }

    update_node( latest, node );

    parent.push_back( latest );

    return (*this);
}

// insert node at current depth
xml &xml::node( const moon9::string &key, const moon9::string &text, const moon9::string &comment )
{
    if( latest.empty() || (!parent.size()) )
    {
        if( verbose )
            std::cout << "node new" << std::endl;

        latest = doc.append_child( pugi::node_element );
    }
    else
    {
        if( verbose )
            std::cout << "node" << std::endl;

        latest = parent.back().append_child( pugi::node_element );
    }

    update_node( latest, key, text, comment );

    return (*this);
}

// insert node at current depth
xml &xml::node( const pugi::xml_node &node )
{
    if( latest.empty() || (!parent.size()) )
    {
        if( verbose )
            std::cout << "node new" << std::endl;

        latest = doc.append_child( pugi::node_element );
    }
    else
    {
        if( verbose )
            std::cout << "node" << std::endl;

        latest = parent.back().append_child( pugi::node_element );
    }

    update_node( latest, node );

    return (*this);
}

// set attribute for current node
xml &xml::set( const moon9::string &key, const moon9::string &value )
{
    if( verbose )
        std::cout << "set " << key << " = " << value << std::endl;

    latest.append_attribute( key.c_str() ).set_value( value.c_str() );

    return (*this);
}

// set attribute for current node
xml &xml::set( /*const*/ attribute &_attribute )
{
    if( verbose )
        std::cout << "set " << _attribute.name() << " = " << _attribute.value() << std::endl;

    latest.append_attribute( _attribute.name() ).set_value( _attribute.value() );

    return (*this);
}

// pop a level of depth back
xml &xml::back()
{
    if( verbose )
        std::cout << "back" << std::endl;

    if( parent.size() )
        parent.pop_back();

    if( parent.size() )
        latest = parent.back();
    else
        latest = pugi::xml_node();

    return (*this);
}

// to unify, replace & deprecate. allow xml arithmetic

xml xml::subtree( const std::string &XPath ) const
{
    std::ostringstream ss;

    try
    {
        pugi::xpath_node_set subset = doc.select_nodes( XPath.c_str() );
        //assertion( subset.size() > 0 );

        if( subset.size() )
            subset[0].node().print( ss );

        //subset.print( ss );
    }
    catch (...)
    {
        std::cout << moon9::string( "exception while xml.subtree( '\1' ) where xml = '\2'", XPath, str() ) << std::endl;
        assert( false );
    }

    xml doc;
    doc.str( ss.str() );
    return doc;
}

std::string xml::subtext( const std::string &XPath ) const
{
    std::ostringstream ss;

    try
    {
        pugi::xpath_node_set subset = doc.select_nodes( XPath.c_str() );
        //assertion( subset.size() > 0 );

        if( subset.size() )
            subset[0].node().print( ss );

        //subset.print( ss );
    }
    catch (...)
    {
        std::cout << moon9::string( "exception while xml.subtext( '\1' ) where xml = '\2'", XPath, str() ) << std::endl;
        assert( false );
    }

    return ss.str();
}

xml xml::tree( const std::string &XPath ) const
{
    std::ostringstream ss;

    try
    {
        pugi::xpath_node_set subset = doc.select_nodes( XPath.c_str() );
        //assertion( subset.size() > 0 );

        if( subset.size() )
        {
            iterator it = subset[0].node().begin();

            for( ; it != subset[0].node().end(); ++it )
                it->print( ss );
        }
        //subset.print( ss );
    }
    catch (...)
    {
        std::cout << moon9::string( "exception while xml.tree( '\1' ) where xml = '\2'", XPath, str() ) << std::endl;
        assert( false );
    }

    xml doc;
    doc.str( ss.str() );
    return doc;
}


std::string xml::text( const std::string &XPath ) const
{
    std::ostringstream ss;

    try
    {
        pugi::xpath_node_set subset = doc.select_nodes( XPath.c_str() );
        //assertion( subset.size() > 0 );

        if( subset.size() )
        {
            iterator it = subset[0].node().begin();

            for( ; it != subset[0].node().end(); ++it )
                it->print( ss );
        }
        //subset.print( ss );
    }
    catch (...)
    {
        std::cout << moon9::string( "exception while xml.text( '\1' ) where xml = '\2'", XPath, str() ) << std::endl;
        assert( false );
    }

    return ss.str();
}

// serialization

std::string xml::str() const
{
    std::ostringstream data;

    doc.save( data );

    if( verbose )
        std::cout << data.str() << std::endl;

    return data.str();
}

void xml::str( const std::string &data )
{
    pugi::xml_parse_result result = doc.load_buffer( data.c_str(), data.size() );
}

#if 0

template <typename T>
T xml::query( const std::string &XPath_fmt, ... ) const
{
    va_list args;
    va_start( args, XPath_fmt );
    moon9::string XPath = moon9::format::list( XPath_fmt.c_str(), args );
    va_end( args );

    try
    {
        std::string result = pugi::xpath_query( XPath.c_str() ).evaluate_string( doc );

        return moon9::string( result ).as<T>();
    }
    catch (const pugi::xpath_exception& e)
    {
        std::cout << "Select failed: " << e.what() << std::endl;
    }
    catch( ... )
    {
        std::cout << "exceptionnnn!" << std::endl;
    }

    return T();
}

template <typename T>
void xml::query( T &t, const std::string &XPath_fmt, ... ) const
{
    va_list args;
    va_start( args, XPath_fmt );
    moon9::string XPath = moon9::format::list( XPath_fmt.c_str(), args );
    va_end( args );

    t = query<T>( XPath.c_str() );
}

#endif

void xml::update_node( pugi::xml_node &node, const moon9::string &key, const moon9::string &text, const moon9::string &comment)
{
    node.set_name( key );

    if( comment != "" )
        //node.prepend_child( ... )
        node.parent().insert_child_before(pugi::node_comment, node).set_value( comment );

    if( text != "" )
        node.append_child(pugi::node_pcdata).set_value( text );
}

void xml::update_node( pugi::xml_node &node, const pugi::xml_node &content )
{
    node.set_name( content.name() );

    if( content.value() )
        node.append_child(pugi::node_pcdata).set_value( content.first_child().value() );

    // and comments? content.value() ?
}

xml::iterator xml::begin() const
{
    return doc.root().begin();
}

xml::iterator xml::end() const
{
    return doc.root().end();
}

xml::attribute_iterator xml::attribute_begin() const
{
    return doc.root().attributes_begin();
}

xml::attribute_iterator xml::attribute_end() const
{
    return doc.root().attributes_end();
}


} //namespace moon9
