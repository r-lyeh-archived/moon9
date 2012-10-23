#include "xml.hpp"
#include "pak.hpp"

#include "pack.hpp"
#include "netstring.hpp"

namespace pack
{
    std::string zip( const bistrings &p )
    {
       // @todo: allow content.size() == 0 @ pak.cpp -> xzip :s
       moon9::pak pak;
        for( auto &it : p )
        {
            moon9::pakfile pf;
            pf["filename"] = it.first;
            pf["content"] = it.second;
            pak.push_back( pf );
        }
        return pak.bin();
    }

    std::string json( const bistrings &p )
    {
        #if 0

            cJSON *root,*fmt,*img,*thm,*fld;char *out;int i;        /* declare a few. */

            /* Here we construct some JSON standards, from the JSON site. */

            /* Our "Video" datatype: */
            root=cJSON_CreateObject();
            cJSON_AddItemToObject(root, "name", cJSON_CreateString("Jack (\"Bee\") Nimble"));
            cJSON_AddItemToObject(root, "format", fmt=cJSON_CreateObject());
            cJSON_AddStringToObject(fmt,"type",             "rect");
            cJSON_AddNumberToObject(fmt,"width",            1920);
            cJSON_AddNumberToObject(fmt,"height",           1080);
            cJSON_AddFalseToObject (fmt,"interlace");
            cJSON_AddNumberToObject(fmt,"frame rate",       24);

            out=cJSON_Print(root);  cJSON_Delete(root);     printf("%s\n",out);     free(out);

        #else

        std::string json;

        for( auto &it : p )
        {
            json += "{ \"key\": \"" + it.first + "\", \"val\": \"" + it.second + "\" },";

            //@todo. remove last comma :P
        }

        return std::string("[") + json + "]";

        #endif
    }

    std::string xml( const bistrings &p )
    {
        moon9::xml xml;

        xml.push("root");

        for( auto &it : p )
        {
            xml.node( "i" );
            xml.set( "key", it.first );
            xml.set( "val", it.second );
        }

        return xml.str();
    }
}

namespace unpack
{
    bistrings zip( const std::string &s )
    {
        bistrings bs;

        moon9::pak pak;
        pak.bin( s );

        for( auto &it : pak )
        {
            moon9::pakfile &pf = (it);
            bistring b;

            b.first = pf["filename"];
            b.second = pf["content"];

            bs.push_back( b );
        }

        return bs;
    }

    bistrings json( const std::string &s ); // @todo :P

    bistrings xml( const std::string &s )
    {
        bistrings bs;

        moon9::xml xml;
        xml.str( s );

        for( size_t it = 1, end = xml.query<size_t>("count(/root/i)") ; it <= end ; ++it )
        {
            bistring b;

            b.first  = xml.query<std::string>( moon9::string( "string(/root/i[\1]/@key)", it ) );
            b.second = xml.query<std::string>( moon9::string( "string(/root/i[\1]/@val)", it ) );

            bs.push_back( b );
        }

        return bs;
    }
}

