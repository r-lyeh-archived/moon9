#include <iostream>
#include <string>

#include "markov.hpp"

int main( int argc, char **argv )
{
    {
        // Tolkien's orcs
        std::string list =
            "Azog, "
            "Balcmeg, "
            "Boldog, "
            "Bolg, "
            "Golfimbul, "
            "Gorbag, "
            "Gorgol, "
            "Grishnakh, " // Grishnákh,
            "Lagduf, "
            "Lug, "
            "Lugdush, "
            "Mauhur, " // Mauhúr,
            "Muzgash, "
            "Orcobal, "
            "Othrod, "
            "Radbug, "
            "Shagrat, "
            "Snaga, "
            "Ufthak, "
            "Ugluk" // Uglúk,
        ;

        moon9::markov mkv( list, " ,;\r\n\t" );

        std::cout << "Orc-ish names: ";

        for( int i = 0; i < 10; ++i )
            std::cout << mkv.next() << ", ";

        std::cout << std::endl;
    }

    {
        // Hindu mythology
        std::string list = "ADITI, ADITYA, AGNI, ANANTA, ANIL, ANIRUDDHA, ARJUNA, ARUNA, ARUNDHATI, BALA, BALADEVA, BHARATA,"
        " BHASKARA, BRAHMA, BRIJESHA, CHANDRA, DAMAYANTI, DAMODARA, DEVARAJA, DEVI, DILIPA, DIPAKA, DRAUPADI, DRUPADA, DURGA,"
        " GANESHA, GAURI, GIRISHA, GOPALA, GOPINATHA, GOTAMA, GOVINDA, HARI, HARISHA, INDIRA, INDRA, INDRAJIT, INDRANI, "
        " JAGANNATHA, JAYA, JAYANTI, KALI, KALYANI, KAMA, KAMALA, KANTI, KAPILA, KARNA, KRISHNA, KUMARA, KUMARI, LAKSHMANA, "
        " LAKSHMI, LALITA, MADHAVA, MADHAVI, MAHESHA, MANI, MANU, MAYA, MINA, MOHANA, MOHINI, MUKESHA, MURALI, NALA, NANDA, "
        " NARAYANA, PADMA, PADMAVATI, PANKAJA, PARTHA, PARVATI, PITAMBARA, PRABHU, PRAMODA, PRITHA, PRIYA, PURUSHOTTAMA, RADHA, "
        " RAGHU, RAJANI, RAMA, RAMACHANDRA, RAMESHA, RATI, RAVI, REVA, RUKMINI, SACHIN, SANDHYA, SANJAYA, SARASWATI, SATI, "
        " SAVITR, SAVITRI, SHAILAJA, SHAKTI, SHANKARA, SHANTA, SHANTANU, SHIVA, SHIVALI, SHRI, SHRIPATI, SHYAMA, SITA, SRI, "
        " SUMATI, SUNDARA, SUNITA, SURESHA, SURYA, SUSHILA, TARA, UMA, USHA, USHAS, VALLI, VASANTA, VASU, VIDYA, VIJAYA, "
        " VIKRAMA, VISHNU, YAMA, YAMI";

        moon9::markov mkv( list, " ,;\r\n\t" );

        std::cout << "Hindu-ish names: ";

        for( int i = 0; i < 10; ++i )
            std::cout << mkv.next() << ", ";

        std::cout << std::endl;
    }

    return 0;
}

