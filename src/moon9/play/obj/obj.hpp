// simple generic object class whose members/properties can be added/removed/retrieved at runtime (usage: check sample files)
// accesing object members seems to be x3 times slower (on my machine) than accessing regular c++ object members
// - rlyeh ~~ listening to Sienna Root / We are them

#pragma once

#include <map>
#include <string>
#include <sstream>
#include <vector>

#ifdef MSGPACK_DEFINE
//#   include <moon9/string/string.hpp>
//#   include <moon9/io/compress.hpp>
//#   include <moon9/io/pack.hpp>
#   include <moon9/io/netstring.hpp>
#endif

std::string make_obj_tinyid( const char *name );

class obj;

template<typename T>
class objfield : public std::map< obj*, T >
{
    public:

    const std::string id;       // used when serializing
    const std::string name;     // useful when debugging
    const std::string comment;  // useful when debugging

    explicit
    objfield( const char *_name ) : id(make_obj_tinyid(_name)), name(_name)
    {}
    objfield( const char *_name, const char *_comment ) : id(make_obj_tinyid(_name)), name(_name), comment(_comment)
    {}
};

class obj
{
    static std::string mirror( const obj *ptr, bool inscribe )
    {
        // @todo threadsafe!

        static std::map< const obj *, std::string > *_set = 0;
        if( !_set )
		{
			static char placement[ sizeof(std::map< const obj *, std::string >) ]; 
			_set = (std::map< const obj *, std::string > *)placement; // no leak and no memory traced :P
			new (_set) std::map< const obj *, std::string >();        // memtraced recursion safe; we don't track placement-news
		}
		std::map< const obj *, std::string > &set = *_set;

        std::string res;

        auto it = set.find( ptr );
        bool found = ( it != set.end() );

        if( !inscribe )
        {
            if( found )
                set.erase( it );
        }
        else
        {
            if( !found )
            {
                res = ptr->save();
                set[ ptr ] = res;
            }
            else
            {
                res = obj( (*it).second ).diff( *ptr );
                (*it).second = ptr->save();
            }
        }

        return res;
    }

    public:

    obj()
    {}

    obj( const obj &other )
    {
        user_defined_clone( &other );
    }

    explicit
    obj( const std::string &bindata )
    {
        load( bindata );
    }

    ~obj()
    {
        mirror( this, false );
        user_defined_erase();
    }

    obj& operator=( const obj &other )
    {
        user_defined_erase();
        user_defined_clone( &other );
        return *this;
    }

    // access component

    template <typename T>
    inline typename T::mapped_type &operator[]( T &property ) // at()
    {
        return property[ this ]; //return ( property[ this ] = property[ this ] );
    }

    template <typename T>
    inline typename const T::mapped_type &operator[]( T &property ) const // at()
    {
        obj *self = const_cast<obj*>(this);
        return property[ self ]; //return ( property[ self ] = property[ self ] );
    }

    // check component

    template <typename T>
    bool has( T &property ) const
    {
        obj *self = const_cast<obj*>(this);
        return property.find( self ) != property.end();
    }

    template <typename T>
    const char *name( T &property ) const
    {
        obj *self = const_cast<obj*>(this);
        auto found = property.find( self );
        return found != property.end() ? property.name.c_str() : 0;
    }

    template <typename T>
    const char *comment( T &property ) const
    {
        obj *self = const_cast<obj*>(this);
        auto found = property.find( self );
        return found != property.end() ? property.comment.c_str() : 0;
    }

    // remove component

    template <typename T>
    void del( T &property )
    {
        ( property[ this ] = property[ this ] );
        property.erase( property.find( this ) );
    }

    // serialize object

    std::string str() const
    {
        bs = bistrings();

        user_defined_print();

        std::stringstream ss;
        ss << this << std::endl;
        for( auto &it : bs )
            ss << it.first << it.second;

        bs = bistrings();

        return ss.str();
    }

    // save/load interface

    std::string save() const
    {
        return obj().diff( *this );
    }

    void load( const std::string &bindata )
    {
        *this = obj();

        patch( bindata );
    }

#ifndef MSGPACK_DEFINE

    std::string diff( const obj &obj2 ) const
    {
        std::cerr << "<moon/play/obj.hpp> says: you missed to #include <msgpack.hpp> before including me" << std::endl;

        return std::string();
    }

    void patch( const std::string &bindata )
    {
        std::cerr << "<moon/play/obj.hpp> says: you missed to #include <msgpack.hpp> before including me" << std::endl;
    }

#else

    std::string diff( const obj &obj2 ) const
    {
        bs = bistrings();

        user_defined_diff( &obj2 );

        std::string result = //moon9::z
        ( pack::netstring( bs ) );

        bs = bistrings();

        return result;
    }

    void patch( const std::string &bindata )
    {
        bs = unpack::netstring( //moon9::unz
            ( bindata ) );

        while( bs.size() )
        {
            user_defined_patch();
            bs.pop_back();
        }
    }

#endif

    std::string diff() const
    {
        return mirror( this, true );
    }

    // implementation details

    protected:

    // context registration helpers (check sample files)

    typedef std::pair< std::string, std::string > bistring;
    typedef std::vector< bistring > bistrings;
    mutable bistrings bs;           // sizeof(obj) overhead! :s

    // register and serialize components
    // note: these are function prototypes! user must provide a body for each function (check sample files)

    void user_defined_clone( const obj *dst );
    void user_defined_erase();
    void user_defined_print() const;

    void user_defined_diff( const obj *other ) const;
    void user_defined_patch();

#ifdef MSGPACK_DEFINE

#endif

    template <typename T>
    void print_field( T &property, const obj *unused = 0 ) const
    {
        obj *self = const_cast<obj*>(this);
        auto found = property.find(self);

        if( found != property.end() )
        {
            std::stringstream ss1, ss2;

            if( property.comment.size() )
                ss1 << "\t/* " << property.comment << " */\n";

            ss2 << "\t." << property.name << "=" << found->second << "\n";

            bistring b;
            b.first = ss1.str();
            b.second = ss2.str();
            bs.push_back( b );
        }
    }

    template <typename T>
    void copy_field( T &property, const obj *other )
    {
        obj *that = const_cast<obj*>(other);
        auto found = property.find(that);

        if( found != property.end() )
            ( property[ this ] = property[ this ] ) = found->second;
    }

    template <typename T>
    void erase_field( T &property, const obj *unused = 0 )
    {
        auto found = property.find(this);

        if( found != property.end() )
            property.erase( found );
    }

#ifdef MSGPACK_DEFINE

    template <typename T>
    void diff_field( T &property, const obj *other ) const
    {
        const obj &obj1 = *this;
        const obj &obj2 = *other;

        if( obj1.has(property) && !obj2.has(property) )
        {
            bistring p( std::string("!") + property.id, "0" );
            bs.push_back( p );
        }
        else
        if( ( !obj1.has(property) && obj2.has(property) ) || ( obj1.has(property) && obj2.has(property) && !(obj1[property] == obj2[property]) ) )
        {
            std::string out;

            // serialize
            const T::mapped_type &value = obj2[property];
            msgpack::sbuffer m;
            msgpack::pack( m, value );

            out.resize( m.size() );
            memcpy( &out[0], m.data(), m.size() );

            // store
            bistring p( property.id, out );
            bs.push_back( p );
        }
    }

    template <typename T>
    void patch_field( T &property, const obj *unused = 0 )
    {
        const obj &obj1 = *this;

        bistring &b = bs.back();

        if( b.first == std::string("!") + property.id )
        {
            (*this).del(property);
        }
        else
        if( b.first == property.id )
        {
            std::string &sbuf = b.second;

            // deserialize it.
            msgpack::unpacked msg;
            msgpack::unpack( &msg, sbuf.data(), sbuf.size() );

            // convert it into statically typed object.
            msgpack::object mobj = msg.get();
            T::mapped_type &value = (*this)[property];
            mobj.convert( &value );
        }
    }

#endif

#if 0

    // c++11 alt

    // termination version
    void copy( obj *no )
    {}

    template<typename T, typename... ARGS>
    void copy( obj *other, T &property, ARGS &...others )
    {
        obj *that = const_cast<obj*>(other);
        auto found = property.find(that);

        if( found != property.end() )
            add( property, found->second );

        copy( no, others... ); // note: first argument does not appear here
    }

#endif

};
