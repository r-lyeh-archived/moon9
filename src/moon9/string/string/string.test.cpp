#include <cassert>

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
    double pi = 3.1415926535897932384626433832795;
    const double cpi = 3.1415926535897932384626433832795;
    std::cout << moon9::string( pi ) << std::endl;
    std::cout << moon9::string( cpi ) << std::endl;



    assert( moon9::string(3.1415926535897932384626433832795) != "3.1415926535897932384626433832795" );

    assert( moon9::string().strip() == moon9::string() );
    assert( moon9::string("").strip() == moon9::string() );
    assert( moon9::string("abc").strip() == "abc" );
    assert( moon9::string("abc ").strip() == "abc" );
    assert( moon9::string(" abc").strip() == "abc" );
    assert( moon9::string(" abc ").strip() == "abc" );
    assert( moon9::string("a b c").strip() == "a b c" );
    assert( moon9::string(" a b c").strip() == "a b c" );
    assert( moon9::string("a b c ").strip() == "a b c" );
    assert( moon9::string(" a b c ").strip() == "a b c" );

    assert( moon9::string("abc").lstrip() == "abc" );
    assert( moon9::string("abc ").lstrip() == "abc " );
    assert( moon9::string(" abc").lstrip() == "abc" );
    assert( moon9::string(" abc ").lstrip() == "abc " );
    assert( moon9::string("a b c").lstrip() == "a b c" );
    assert( moon9::string(" a b c").lstrip() == "a b c" );
    assert( moon9::string("a b c ").lstrip() == "a b c " );
    assert( moon9::string(" a b c ").lstrip() == "a b c " );

    assert( moon9::string("abc").rstrip() == "abc" );
    assert( moon9::string("abc ").rstrip() == "abc" );
    assert( moon9::string(" abc").rstrip() == " abc" );
    assert( moon9::string(" abc ").rstrip() == " abc" );
    assert( moon9::string("a b c").rstrip() == "a b c" );
    assert( moon9::string(" a b c").rstrip() == " a b c" );
    assert( moon9::string("a b c ").rstrip() == "a b c" );
    assert( moon9::string(" a b c ").rstrip() == " a b c" );

    //ptr to method
    if( 1 )
    {
        ctest_stream2 cts2;
        std::cout << moon9::string( &ctest_stream2::test ) << std::endl;
    }

    assert( !moon9::string("false").as<int>() );
    assert(  moon9::string( "true").as<int>() );

    int a = moon9::string();
    int b = int( moon9::string() );

    assert( !a );
    assert( !b );

    //TEST
    std::string test_stream1a = ctest_stream1();
    //moon9::string test_stream1b = ctest_stream1();

    assert( test_stream1a == "im b" );
    //assert( test_stream1b == "im b" );

    const char *str_constchar  = "hello world";

    moon9::string str_explicit("hello world");

    moon9::string str_formatted  = moon9::string("hello \1", "world");
    moon9::string str_stdstring  = std::string("hello world");
    moon9::string str_copyctor1  = moon9::string( "hello world" );
    moon9::string str_copyctor2  = moon9::string( str_stdstring );

    moon9::string str_assignop;  str_assignop = str_explicit;

    assert( str_explicit  == "hello world" );

    assert( str_formatted == "hello world" );
    assert( str_stdstring == "hello world" );
    assert( str_copyctor1 == "hello world" );
    assert( str_copyctor2 == "hello world" );

    assert( str_assignop  == "hello world" );

    assert( moon9::string(         ).as<bool>() == false );
    assert( moon9::string(       0 ).as<bool>() == false );
    assert( moon9::string(       1 ).as<bool>() ==  true );
    assert( moon9::string(       2 ).as<bool>() ==  true );
    assert( moon9::string(     "0" ).as<bool>() == false );
    assert( moon9::string(     "1" ).as<bool>() ==  true );
    assert( moon9::string(     "2" ).as<bool>() ==  true );
    assert( moon9::string(   false ).as<bool>() == false );
    assert( moon9::string(    true ).as<bool>() ==  true );
    assert( moon9::string( "false" ).as<bool>() == false );
    assert( moon9::string(  "true" ).as<bool>() ==  true );

    assert( moon9::string(       ).as<char>() ==   '\0' );
    assert( moon9::string( false ).as<char>() ==   '\0' );
    assert( moon9::string(  true ).as<char>() == '\x01' );
    assert( moon9::string(     0 ).as<char>() ==   '\0' );
    assert( moon9::string(     1 ).as<char>() == '\x01' );
    assert( moon9::string(    33 ).as<char>() ==    '!' );

    //short
    //long

    assert( moon9::string(         ).as<int>() ==  0 );
    assert( moon9::string(   false ).as<int>() ==  0 );
    assert( moon9::string(    true ).as<int>() ==  1 );
    assert( moon9::string( "false" ).as<int>() ==  0 );
    assert( moon9::string(  "true" ).as<int>() ==  1 );
    assert( moon9::string(       0 ).as<int>() ==  0 );
    assert( moon9::string(       1 ).as<int>() ==  1 );
    assert( moon9::string(      -1 ).as<int>() == -1 );

    assert( moon9::string(         ).as<unsigned>() ==             0 );
    assert( moon9::string(   false ).as<unsigned>() ==             0 );
    assert( moon9::string(    true ).as<unsigned>() ==             1 );
    assert( moon9::string( "false" ).as<unsigned>() ==             0 );
    assert( moon9::string(  "true" ).as<unsigned>() ==             1 );
    assert( moon9::string(      -1 ).as<unsigned>() == (unsigned)(-1) );

    assert( moon9::string(         ).as<size_t>() ==           0  );
    assert( moon9::string(   false ).as<size_t>() ==           0  );
    assert( moon9::string(    true ).as<size_t>() ==           1  );
    assert( moon9::string( "false" ).as<size_t>() ==           0  );
    assert( moon9::string(  "true" ).as<size_t>() ==           1  );
    assert( moon9::string(      -1 ).as<size_t>() == (size_t)(-1) );

    // add limits float, double
    assert( moon9::string(         ).as<float>() == 0.f );
    assert( moon9::string(   false ).as<float>() == 0.f );
    assert( moon9::string(    true ).as<float>() == 1.f );
    assert( moon9::string( "false" ).as<float>() == 0.f );
    assert( moon9::string(  "true" ).as<float>() == 1.f );
    assert( moon9::string(     3.f ).as<float>() == 3.f );

    assert( moon9::string(         ).as<double>() == 0.0 );
    assert( moon9::string(   false ).as<double>() == 0.0 );
    assert( moon9::string(    true ).as<double>() == 1.0 );
    assert( moon9::string( "false" ).as<double>() == 0.f );
    assert( moon9::string(  "true" ).as<double>() == 1.f );
    assert( moon9::string(     3.0 ).as<double>() == 3.0 );

#if 0

    std::string test_original = "lalalalalalalalallalalalallallalalalalalalalallalalalalalalalalalallalalalalalalalallalalalallallalalalalalalalallalalalalalalalalalallalalalalalalalallalalalallallalalalalalalalallalalalalalalalalalallalalalalalalalallalalalallallalalalalalalalallalalalalalalalalalallalalalalalalalallalalalallallalalalalalalalallalalalalalalalalalallalalalalalalalallalalalallallalalalalalalalallalalalalalalalalalallalalalalalalalallalalalallallalalalalalalalallalalalalalalalalalalallalalalalalalala ohohohoh yes!!!!";
    moon9::string test_zipping = moon9::string( test_original ).z();
    std::string test_zipped = test_zipping;
    moon9::string test_unzipping = moon9::string( test_zipped ).unz();
    std::string test_unzipped = test_unzipping;

    assert(  test_original == test_unzipped );
    assert(  test_zipped.size() < test_unzipped.size() );

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

        assert( original_unz == original );
    }

#endif

#if 0

    {   // encoding
        moon9::string str1 = "abc";
        moon9::string str2 = "hello world! how are you? ive got $1 :) roses are red abcdefghijklmnopqrstuvwxyz12309219$=)!(\"'-/\\┬À=!)\"$(%)";
        moon9::string str3 = "login&mario@bm.es";

        assert( str1 == moon9::string( str1.encode91() ).decode91() );

        using namespace moon9;
        assert( str2 == decode91(encode91(str2)) );
        std::cout << str2 << std::endl << ' vs ' << encode91(str2) << ' vs ' << std::endl << decode91(encode91(str2)) << std::endl;

        using namespace moon9;
        assert( str2 == decode91(encode91(str2)) );

        std::cout << __LINE__ << ' ' << str3 << std::endl;
        std::cout << __LINE__ << ' ' <<  encode91(str3) << std::endl;
        std::cout << __LINE__ << ' ' <<  decode91(encode91(str3)) << std::endl;
        const std::string encoded = encode91(str3);
        std::cout << __LINE__ << ' ' <<  std::string(encode91(str3)) << std::endl;
        std::cout << __LINE__ << ' ' <<  moon9::string(encoded.substr(0)).decode91() << std::endl;
        assert( str3 == decode91(encode91(str3)) );
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
    assert( moon9::string("%25hello%25%25world%25").replace("%25","") == "helloworld" );
    //same replacement
    assert( moon9::string("%25hello%25%25world%25").replace("%25","%25") == "%25hello%25%25world%25" );
    //longer replacement
    assert( moon9::string("%25hello%25%25world%25").replace("%25","%255") == "%255hello%255%255world%255" );
    //shorter replacement
    assert( moon9::string("%25hello%25%25world%25").replace("%25","%2") == "%2hello%2%2world%2" );

    assert( moon9::string().size() == 0 );
    assert( moon9::string("").size() == 0 );
    assert( moon9::string() == "" );
    assert( moon9::string("") == "" );

    // moon9::string<<T and std::cout<<moon9::string support

/*
    assert( ( moon9::string() << false << std::endl ) == "false\n" );
    assert( ( moon9::string() << '1' << std::endl ) == "1\n" );
    assert( ( moon9::string() << "2" << std::endl ) == "2\n" );
    assert( ( moon9::string() << 3 << std::endl ) == "3\n" );
    assert( ( moon9::string() << 4.f << std::endl ) == "4.f\n" );
    assert( ( moon9::string() << 5.0 << std::endl ) == "5.0\n" );
    assert( ( moon9::string() << std::string("6") << std::endl ) == "6\n" );
    assert( ( moon9::string() << moon9::string("7") << std::endl ) == "7\n" );
*/

    assert( moon9::string("Hi!").at(-6) == 'H' );
    assert( moon9::string("Hi!").at(-5) == 'i' );
    assert( moon9::string("Hi!").at(-4) == '!' );
    assert( moon9::string("Hi!").at(-3) == 'H' );
    assert( moon9::string("Hi!").at(-2) == 'i' );
    assert( moon9::string("Hi!").at(-1) == '!' );
    assert( moon9::string("Hi!").at( 0) == 'H' );
    assert( moon9::string("Hi!").at( 1) == 'i' );
    assert( moon9::string("Hi!").at( 2) == '!' );
    assert( moon9::string("Hi!").at( 3) == 'H' );
    assert( moon9::string("Hi!").at( 4) == 'i' );
    assert( moon9::string("Hi!").at( 5) == '!' );

    assert( moon9::string().at(-1) == '\0' );
    assert( moon9::string().at( 0) == '\0' );
    assert( moon9::string().at( 1) == '\0' );

#define EVAL(expr) do { \
    double ret = moon9::string(#expr).eval().as<double>(); \
    std::cout << #expr << '=' << ret << std::endl; assert( expr == ret ); } while(0)

    EVAL(1+1);
    EVAL(3+(2*4));
    EVAL(45*2);
    EVAL(3.1459);



    std::cout << "All ok." << std::endl;

    return 0;
}

