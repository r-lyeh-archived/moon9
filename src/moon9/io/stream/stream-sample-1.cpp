// simple html logger sample
// - rlyeh

#include <cstdio>

#include <iostream>
#include <string>

#include "stream.hpp"

void my_custom_logger( bool open, bool feed, bool close, const std::string &line )
{
    static FILE *fp;
    static std::string cache;

    if( open )
    {
        fp = std::fopen("log.html", "wb");

        const char *jscript =
        "<script>format = function() {"
        "document.body.innerHTML = '<pre>' + document.body.innerHTML.replace(/>/g,'&gt;').replace(/</g,'&lt;').replace(/\"/g,'&quot;') + '</pre>';"
        "}</script>";

        std::fprintf( fp, "%s", "<!-- generated file -->\n" );
        std::fprintf( fp, "<html><head>%s</head><body onload='replace_all()'>\n<pre>", jscript );
    }
    else
    if( close )
    {
        std::fprintf( fp, "%s", ( cache + "\n</pre></body></html>\n<!-- closing log file... -->\n" ).c_str() );
        std::fclose( fp );

        cache = std::string();
    }
    else
    if( feed )
    {
        std::fprintf( fp, "[*] %s\n", cache.c_str() );

        cache = std::string();
    }
    else
    {
        cache += line;
    }
}

int main( int argc, char **argv )
{
    std::cout << "next lines are going to be redirected to a 'log.html' file ..." << std::endl;

    attach( std::cout, my_custom_logger );

    std::cout << "this is line #1, redirected from standard std::cout stream to a custom HTML file logger" << std::endl;
    std::cout << "this is line #2, redirected from standard std::cout stream to a custom HTML file logger" << std::endl;

    detach( std::cout );

    std::cout << "... back from redirecting. Please check 'log.html' file." << std::endl;

    return 0;
}
