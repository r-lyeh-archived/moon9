#include <moon9/test/test.hpp>

#include "string.hpp"

struct ctest_stream1
{
    operator std::string()
    {
        return "im b";
    }
};

struct ctest_stream2
{
    ctest_stream2()
    {}

    ctest_stream2( const std::string &t )
    {
        std::cout << t << std::endl;
    }

    void test()
    {}
};


int main( int argc, char **argv )
{
    test3( moon9::string(3.1415926535897932384626433832795), ==, "3.1415926535897931" );

    test3( moon9::string().strip(), ==, moon9::string() );
    test3( moon9::string("").strip(), ==, moon9::string() );
    test3( moon9::string("abc").strip(), ==, "abc" );
    test3( moon9::string("abc ").strip(), ==, "abc" );
    test3( moon9::string(" abc").strip(), ==, "abc" );
    test3( moon9::string(" abc ").strip(), ==, "abc" );
    test3( moon9::string("a b c").strip(), ==, "a b c" );
    test3( moon9::string(" a b c").strip(), ==, "a b c" );
    test3( moon9::string("a b c ").strip(), ==, "a b c" );
    test3( moon9::string(" a b c ").strip(), ==, "a b c" );

    test3( moon9::string("abc").lstrip(), ==, "abc" );
    test3( moon9::string("abc ").lstrip(), ==, "abc " );
    test3( moon9::string(" abc").lstrip(), ==, "abc" );
    test3( moon9::string(" abc ").lstrip(), ==, "abc " );
    test3( moon9::string("a b c").lstrip(), ==, "a b c" );
    test3( moon9::string(" a b c").lstrip(), ==, "a b c" );
    test3( moon9::string("a b c ").lstrip(), ==, "a b c " );
    test3( moon9::string(" a b c ").lstrip(), ==, "a b c " );

    test3( moon9::string("abc").rstrip(), ==, "abc" );
    test3( moon9::string("abc ").rstrip(), ==, "abc" );
    test3( moon9::string(" abc").rstrip(), ==, " abc" );
    test3( moon9::string(" abc ").rstrip(), ==, " abc" );
    test3( moon9::string("a b c").rstrip(), ==, "a b c" );
    test3( moon9::string(" a b c").rstrip(), ==, " a b c" );
    test3( moon9::string("a b c ").rstrip(), ==, "a b c" );
    test3( moon9::string(" a b c ").rstrip(), ==, " a b c" );

    //ptr to method
    if( 1 )
    {
        ctest_stream2 cts2;
        std::cout << moon9::string( &ctest_stream2::test ) << std::endl;
    }

    test1( !moon9::string("false").as<int>() );
    test1(  moon9::string( "true").as<int>() );

    int a = moon9::string();
    int b = int( moon9::string() );

    test1( !a );
    test1( !b );

    //TEST
    std::string test_stream1a = ctest_stream1();
    //moon9::string test_stream1b = ctest_stream1();

    test3( test_stream1a, ==, "im b" );
    //test3( test_stream1b, ==, "im b" );

    const char *str_constchar  = "hello world";

    moon9::string str_explicit("hello world");

    moon9::string str_formatted  = moon9::string("hello \1", "world");
    moon9::string str_stdstring  = std::string("hello world");
    moon9::string str_copyctor1  = moon9::string( "hello world" );
    moon9::string str_copyctor2  = moon9::string( str_stdstring );

    moon9::string str_assignop;  str_assignop = str_explicit;

    test3( str_explicit , ==, "hello world" );

    test3( str_formatted, ==, "hello world" );
    test3( str_stdstring, ==, "hello world" );
    test3( str_copyctor1, ==, "hello world" );
    test3( str_copyctor2, ==, "hello world" );

    test3( str_assignop , ==, "hello world" );

    test3( moon9::string( 'a' ), ==, 'a' );
    test3( moon9::string( "hi" ), ==, "hi" );
    test3( moon9::string( true ), ==, true );
    test3( moon9::string( 16384 ), ==, 16384 );
    test3( moon9::string( 3.14159 ), ==, 3.14159 );
    test3( moon9::string( 3.14159f ), ==, 3.14159f );

    test3( moon9::string(), ==, 0 );
    test3( moon9::string(), ==, 0.f );
    test3( moon9::string(), ==, 0.0 );
    test3( moon9::string(), ==, '\0' );
    test3( moon9::string(), ==, "" );
    test3( moon9::string(), ==, false );

    test3( moon9::string( 'a' ), ==, 'a' );
    test3( moon9::string( 'a' ), ==, "a" );
//    test3( moon9::string( 'a' ), ==, 97 );
    test3( moon9::string( "a" ), ==, 'a' );
    test3( moon9::string( "a" ), ==, "a" );
//    test3( moon9::string( "a" ), ==, 97 );
//    test3( moon9::string(  97 ), ==, 'a' );
//    test3( moon9::string(  97 ), ==, "a" );
    test3( moon9::string(  97 ), ==, 97 );
    test3( moon9::string(  97 ).as<int>(), ==, 97 );
    test3( moon9::string(  97 ).as<char>(), ==, 'a' );

    test3( moon9::string(         ).as<bool>(), ==, false );
    test3( moon9::string(       0 ).as<bool>(), ==, false );
    test3( moon9::string(       1 ).as<bool>(), ==,  true );
    test3( moon9::string(       2 ).as<bool>(), ==,  true );
    test3( moon9::string(     "0" ).as<bool>(), ==, false );
    test3( moon9::string(     "1" ).as<bool>(), ==,  true );
    test3( moon9::string(     "2" ).as<bool>(), ==,  true );
    test3( moon9::string(   false ).as<bool>(), ==, false );
    test3( moon9::string(    true ).as<bool>(), ==,  true );
    test3( moon9::string( "false" ).as<bool>(), ==, false );
    test3( moon9::string(  "true" ).as<bool>(), ==,  true );

    test3( moon9::string(   'a' ).as<char>(), ==,    'a' );
    test3( moon9::string(       ).as<char>(), ==,   '\0' );
    test3( moon9::string(     0 ).as<char>(), ==,    '0' );
    test3( moon9::string(     1 ).as<char>(), ==,    '1' );
    test3( moon9::string(    33 ).as<char>(), ==,    '!' );
    test3( moon9::string( false ).as<char>(), ==,   '\0' );
    test3( moon9::string(  true ).as<char>(), ==, '\x01' );

    //short
    //long

    test3( moon9::string(         ).as<int>(), ==,  0 );
    test3( moon9::string(   false ).as<int>(), ==,  0 );
    test3( moon9::string(    true ).as<int>(), ==,  1 );
    test3( moon9::string( "false" ).as<int>(), ==,  0 );
    test3( moon9::string(  "true" ).as<int>(), ==,  1 );
    test3( moon9::string(       0 ).as<int>(), ==,  0 );
    test3( moon9::string(       1 ).as<int>(), ==,  1 );
    test3( moon9::string(      -1 ).as<int>(), ==, -1 );

    test3( moon9::string(         ).as<unsigned>(), ==,             0 );
    test3( moon9::string(   false ).as<unsigned>(), ==,             0 );
    test3( moon9::string(    true ).as<unsigned>(), ==,             1 );
    test3( moon9::string( "false" ).as<unsigned>(), ==,             0 );
    test3( moon9::string(  "true" ).as<unsigned>(), ==,             1 );
    test3( moon9::string(      -1 ).as<unsigned>(), ==, (unsigned)(-1) );

    test3( moon9::string(         ).as<size_t>(), ==,           0  );
    test3( moon9::string(   false ).as<size_t>(), ==,           0  );
    test3( moon9::string(    true ).as<size_t>(), ==,           1  );
    test3( moon9::string( "false" ).as<size_t>(), ==,           0  );
    test3( moon9::string(  "true" ).as<size_t>(), ==,           1  );
    test3( moon9::string(      -1 ).as<size_t>(), ==, (size_t)(-1) );

    // add limits float, double
    test3( moon9::string(         ).as<float>(), ==, 0.f );
    test3( moon9::string(   false ).as<float>(), ==, 0.f );
    test3( moon9::string(    true ).as<float>(), ==, 1.f );
    test3( moon9::string( "false" ).as<float>(), ==, 0.f );
    test3( moon9::string(  "true" ).as<float>(), ==, 1.f );
    test3( moon9::string(     3.f ).as<float>(), ==, 3.f );

    test3( moon9::string(         ).as<double>(), ==, 0.0 );
    test3( moon9::string(   false ).as<double>(), ==, 0.0 );
    test3( moon9::string(    true ).as<double>(), ==, 1.0 );
    test3( moon9::string( "false" ).as<double>(), ==, 0.f );
    test3( moon9::string(  "true" ).as<double>(), ==, 1.f );
    test3( moon9::string(     3.0 ).as<double>(), ==, 3.0 );

#if 0

    std::string test_original = "lalalalalalalalallalalalallallalalalalalalalallalalalalalalalalalallalalalalalalalallalalalallallalalalalalalalallalalalalalalalalalallalalalalalalalallalalalallallalalalalalalalallalalalalalalalalalallalalalalalalalallalalalallallalalalalalalalallalalalalalalalalalallalalalalalalalallalalalallallalalalalalalalallalalalalalalalalalallalalalalalalalallalalalallallalalalalalalalallalalalalalalalalalallalalalalalalalallalalalallallalalalalalalalallalalalalalalalalalalallalalalalalalala ohohohoh yes!!!!";
    moon9::string test_zipping = moon9::string( test_original ).z();
    std::string test_zipped = test_zipping;
    moon9::string test_unzipping = moon9::string( test_zipped ).unz();
    std::string test_unzipped = test_unzipping;

    test3(  test_original, ==, test_unzipped );
    test3(  test_zipped.size() < test_unzipped.size() );

    {
        std::string original = "tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot""tobeornottobeortobeornot";

        // original -> 624
        // fastlz -> 29
        // lzfx -> 33
        // lzjb -> 118

        std::cout << moon9::string( original ).hexdump();

        std::cout << moon9::string( original ).z().hexdump();

        moon9::string compressed = moon9::string( original ).z();

        std::cout << compressed.hexdump();

        std::cout << original.size() << " bytes vs " << compressed.size() << " bytes" << std::endl;

        std::string original_unz = compressed.unz().as<std::string>();

        std::cout << original_unz << std::endl;

        test3( original_unz, ==, original );
    }

#endif

#if 0

    {   // encoding
        moon9::string str1 = "abc";
        moon9::string str2 = "hello world! how are you? ive got $1 :) roses are red abcdefghijklmnopqrstuvwxyz12309219$=)!(\"'-/\\┬À=!)\"$(%)";
        moon9::string str3 = "login&mario@bm.es";

        test3( str1, ==, moon9::string( str1.encode91() ).decode91() );

        using namespace moon9;
        test3( str2, ==, decode91(encode91(str2)) );
        std::cout << str2 << std::endl << ' vs ' << encode91(str2) << ' vs ' << std::endl << decode91(encode91(str2)) << std::endl;

        using namespace moon9;
        test3( str2, ==, decode91(encode91(str2)) );

        std::cout << __LINE__ << ' ' << str3 << std::endl;
        std::cout << __LINE__ << ' ' <<  encode91(str3) << std::endl;
        std::cout << __LINE__ << ' ' <<  decode91(encode91(str3)) << std::endl;
        const std::string encoded = encode91(str3);
        std::cout << __LINE__ << ' ' <<  std::string(encode91(str3)) << std::endl;
        std::cout << __LINE__ << ' ' <<  moon9::string(encoded.substr(0)).decode91() << std::endl;
        test3( str3, ==, decode91(encode91(str3)) );
    }

    {
        std::cout << moon9::string( "jr2f7=~/LRb6[Eyi1ol-c" ).decode91() << std::endl;

        moon9::string str( "jr2f7=~/LRb6[Eyi1ol-c" );
        std::cout << str.decode91() << std::endl;
    }

    {
        moon9::string str3 = "login&mario@bm.es";
        const std::string encoded1(  moon9::string(str3).encode91() );
        const std::string encoded2 = moon9::string(str3).encode91();
              std::string encoded3(  moon9::string(str3).encode91() );
              std::string encoded4 = moon9::string(str3).encode91();
        std::string encoded5 = str3.substr(0);
        std::string encoded6 = std::string(str3.substr(0));
        std::string encoded7 = moon9::string(str3.substr(0));

        std::cout << moon9::echo( encoded1, encoded2, encoded3, encoded4, encoded5, encoded6, encoded7 );
    }

#endif

    {
        //todo: different escapes here
    }


    //del replacement
    test3( moon9::string("%25hello%25%25world%25").replace("%25",""), ==, "helloworld" );
    //same replacement
    test3( moon9::string("%25hello%25%25world%25").replace("%25","%25"), ==, "%25hello%25%25world%25" );
    //longer replacement
    test3( moon9::string("%25hello%25%25world%25").replace("%25","%255"), ==, "%255hello%255%255world%255" );
    //shorter replacement
    test3( moon9::string("%25hello%25%25world%25").replace("%25","%2"), ==, "%2hello%2%2world%2" );

    test3( moon9::string().size(), ==, 0 );
    test3( moon9::string("").size(), ==, 0 );
    test3( moon9::string(), ==, "" );
    test3( moon9::string(""), ==, "" );

    // moon9::string<<T and std::cout<<moon9::string support

/*
    test3( ( moon9::string() << false << std::endl ), ==, "false\n" );
    test3( ( moon9::string() << '1' << std::endl ), ==, "1\n" );
    test3( ( moon9::string() << "2" << std::endl ), ==, "2\n" );
    test3( ( moon9::string() << 3 << std::endl ), ==, "3\n" );
    test3( ( moon9::string() << 4.f << std::endl ), ==, "4.f\n" );
    test3( ( moon9::string() << 5.0 << std::endl ), ==, "5.0\n" );
    test3( ( moon9::string() << std::string("6") << std::endl ), ==, "6\n" );
    test3( ( moon9::string() << moon9::string("7") << std::endl ), ==, "7\n" );
*/

    test3( moon9::string("Hi!").at(-6), ==, 'H' );
    test3( moon9::string("Hi!").at(-5), ==, 'i' );
    test3( moon9::string("Hi!").at(-4), ==, '!' );
    test3( moon9::string("Hi!").at(-3), ==, 'H' );
    test3( moon9::string("Hi!").at(-2), ==, 'i' );
    test3( moon9::string("Hi!").at(-1), ==, '!' );
    test3( moon9::string("Hi!").at( 0), ==, 'H' );
    test3( moon9::string("Hi!").at( 1), ==, 'i' );
    test3( moon9::string("Hi!").at( 2), ==, '!' );
    test3( moon9::string("Hi!").at( 3), ==, 'H' );
    test3( moon9::string("Hi!").at( 4), ==, 'i' );
    test3( moon9::string("Hi!").at( 5), ==, '!' );

    test3( moon9::string().at(-1), ==, '\0' );
    test3( moon9::string().at( 0), ==, '\0' );
    test3( moon9::string().at( 1), ==, '\0' );

#define EVAL(expr) do { \
    double ret = moon9::string(#expr).eval().as<double>(); \
    test3( expr, ==, ret ); } while(0)

    EVAL(1+1);
    EVAL(3+(2*4));
    EVAL(45*2);
    EVAL(3.1459);



    std::cout << "All ok." << std::endl;

    return 0;
}

