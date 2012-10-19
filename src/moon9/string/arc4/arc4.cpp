/*
 * Contains portions of ARC4 algorithm VBA implementation
 * Copyright (c) 2000, Mike Shaffer. All rights reserved.
 */

#include <string>

#include "arc4.hpp"

namespace moon9
{
    namespace swap
    {
        std::string ARC4( const std::string &text, const std::string &passkey )
        {
            // Original VBA code by Mike Shaffer
            // http://www.4guysfromrolla.com/webtech/code/rc4.inc.html

            int sbox[256], key[256];

            size_t ilen = passkey.size();

            if( !ilen )
                return text;

            for( int a = 0; a < 256; a++ )
            {
                key[a] = passkey[a % ilen];
                sbox[a] = a;
            }

            for( int a = 0, b = 0; a < 256; a++ )
            {
                b = (b + sbox[a] + key[a]) % 256;
                std::swap(sbox[a], sbox[b]);
            }

            std::string output;
            output.resize( text.size() );

            for( int a = 0, i = 0, j = 0, iTextLen = text.size(), k; a < iTextLen; a++ )
            {
                i = (i + 1) % 256;
                j = (j + sbox[i]) % 256;
                std::swap(sbox[i], sbox[j]);
                k = sbox[(sbox[i] + sbox[j]) % 256];
                output[a] = text.at(a) ^ k;
            }

            return output;
        }
    }
}
