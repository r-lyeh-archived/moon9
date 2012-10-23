#include <cassert>

#include <iostream>
#include <string>

#include "hash.hpp"

int main(int argc, char* argv[])
{
    std::string key = "abcdefghijklmnopqrstuvwxyz1234567890";

    std::cout << "General Purpose Hash Function Algorithms Test" << std::endl;

    std::cout << " 1. RS-Hash Function Value:    " << moon9::hash::RS( key ).str()    << std::endl;
    std::cout << " 2. JS-Hash Function Value:    " << moon9::hash::JS( key ).str()    << std::endl;
    std::cout << " 3. PJW-Hash Function Value:   " << moon9::hash::PJW( key ).str()   << std::endl;
    std::cout << " 4. ELF-Hash Function Value:   " << moon9::hash::ELF( key ).str()   << std::endl;
    std::cout << " 5. BKDR-Hash Function Value:  " << moon9::hash::BKDR( key ).str()  << std::endl;
    std::cout << " 6. SDBM-Hash Function Value:  " << moon9::hash::SDBM( key ).str()  << std::endl;
    std::cout << " 7. DJB-Hash Function Value:   " << moon9::hash::DJB( key ).str()   << std::endl;
    std::cout << " 8. FNV-Hash Function Value:   " << moon9::hash::FNV( key ).str()   << std::endl;
    std::cout << " 9. BP-Hash Function Value:    " << moon9::hash::BP( key ).str()    << std::endl;
    std::cout << "10. AP-Hash Function Value:    " << moon9::hash::AP( key ).str()    << std::endl;
    std::cout << "11. CRC32-Hash Function Value: " << moon9::hash::CRC32( key ).str() << std::endl;
    std::cout << "12. MH2-Hash Function Value:   " << moon9::hash::MH2( key ).str()   << std::endl;
    std::cout << "13. BJ1-Hash Function Value:   " << moon9::hash::BJ1( key ).str()   << std::endl;
    std::cout << "14. CRC-Hash Function Value:   " << moon9::hash::GCRC( key ).str()  << std::endl;
    std::cout << "15. SHA1-Hash Function Value:  " << moon9::hash::SHA1( key ).str()  << std::endl;

    // unit tests

    key = std::string("0");

    std::cout << "Empty hash tests" << std::endl;

    std::cout << " 1. RS-Hash Function Value:    " << moon9::hash::RS( key ).str()    << std::endl;
    std::cout << " 2. JS-Hash Function Value:    " << moon9::hash::JS( key ).str()    << std::endl;
    std::cout << " 3. PJW-Hash Function Value:   " << moon9::hash::PJW( key ).str()   << std::endl;
    std::cout << " 4. ELF-Hash Function Value:   " << moon9::hash::ELF( key ).str()   << std::endl;
    std::cout << " 5. BKDR-Hash Function Value:  " << moon9::hash::BKDR( key ).str()  << std::endl;
    std::cout << " 6. SDBM-Hash Function Value:  " << moon9::hash::SDBM( key ).str()  << std::endl;
    std::cout << " 7. DJB-Hash Function Value:   " << moon9::hash::DJB( key ).str()   << std::endl;
    std::cout << " 8. FNV-Hash Function Value:   " << moon9::hash::FNV( key ).str()   << std::endl;
    std::cout << " 9. BP-Hash Function Value:    " << moon9::hash::BP( key ).str()    << std::endl;
    std::cout << "10. AP-Hash Function Value:    " << moon9::hash::AP( key ).str()    << std::endl;
    std::cout << "11. CRC32-Hash Function Value: " << moon9::hash::CRC32( key ).str() << std::endl;
    std::cout << "12. MH2-Hash Function Value:   " << moon9::hash::MH2( key ).str()   << std::endl;
    std::cout << "13. BJ1-Hash Function Value:   " << moon9::hash::BJ1( key ).str()   << std::endl;
    std::cout << "14. CRC-Hash Function Value:   " << moon9::hash::GCRC( key ).str()  << std::endl;
    std::cout << "15. SHA1-Hash Function Value:  " << moon9::hash::SHA1( key ).str()  << std::endl;

    std::cout << ":(" << std::endl;

    //assert( moon9::hash::SHA1( std::string() ).str() == "d41d8cd98f00b204e9800998ecf8427e" );

    //std::cout << "All ok." << std::endl;

    return 0;
}

