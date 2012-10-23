#include "xml.hpp"
#include "pak.hpp"

#include "pack.hpp"
#include "netstring.hpp"

namespace pack
{
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
}

namespace unpack
{
    bistrings json( const std::string &s ); // @todo :P
}

