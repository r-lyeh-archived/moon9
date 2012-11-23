// safe raii directory changer. MIT licensed
// - rlyeh

#pragma once

#include <string>

namespace moon9
{
    class chdir
    {
        public:

        explicit
        chdir( const std::string &path );
        ~chdir();

        protected:

        void set( const std::string &path ) const;
        std::string get() const;
        std::string base;

        private:

        chdir( const chdir &other );
        chdir &operator =( const chdir &other );
    };
}
