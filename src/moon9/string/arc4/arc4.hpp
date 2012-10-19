/* Simple functions to encrypt/decrypt and/or swap crypted strings.
 * - rlyeh
 */

#pragma once

#include <string>

namespace moon9
{
    namespace encrypt
    {

    }
    namespace decrypt
    {

    }
    namespace swap
    {
        std::string ARC4( const std::string &text, const std::string &passkey );
    }

    using swap::ARC4;
}
