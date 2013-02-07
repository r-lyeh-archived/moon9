/* Simple test framework. No dependencies. MIT licensed.
 * - rlyeh, 2012
 */

#include <iostream>
#include <sstream>
#include <string>

namespace tests
{
    size_t passed = 0, failed = 0, executed = 0;

    void default_warning_cb( const std::string &message )
    {}

    void default_report_cb( const std::string &report )
    {
        std::cerr << report << std::endl;
    }

    void (*warning_cb)( const std::string &message ) = default_warning_cb;
    void (*report_cb)( const std::string &report ) = default_report_cb;

    std::string &errors()
    {
        static struct on_shutdown
        {
            std::string errors;

            on_shutdown()
            {}

            ~on_shutdown()
            {
                if( !report_cb )
                    return;

                if( !executed )
                    return;

                std::stringstream ss;

                if( failed > 0 )
                {
                    ss << "Oops! " << failed << '/' << executed << " tests failed! Details following:" << std::endl;
                    ss << std::endl;
                    ss << errors;
                }
                else
                {
                    ss << "Ok! All " << executed << " tests succeeded!" << std::endl;
                }

                (*report_cb)( ss.str() );
            }
        } _;

        return _.errors;
    }
}
