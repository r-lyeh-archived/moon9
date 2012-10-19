/* simple arab->roman converstion function: untested!
 * [ref] http://www.cyber-dojo.com/dojo/create
 * - rlyeh
 */

#include <map>
#include <string>

namespace moon9
{
    std::string roman( int i )
    {
        static std::map< int, std::string > table;

        if( !table.size() )
        {
            table[1] =    "I"; table[10] =    "X"; table[100] =    "C"; table[1000] =    "M";
            table[2] =   "II"; table[20] =   "XX"; table[200] =   "CC"; table[2000] =   "MM";
            table[3] =  "III"; table[30] =  "XXX"; table[300] =  "CCC"; table[3000] =  "MMM";
            table[4] =   "IV"; table[40] =   "XL"; table[400] =   "CD"; table[4000] = "MMMM";
            table[5] =    "V"; table[50] =    "L"; table[500] =    "D";
            table[6] =   "VI"; table[60] =   "LX"; table[600] =   "DC";
            table[7] =  "VII"; table[70] =  "LXX"; table[700] =  "DCC";
            table[8] = "VIII"; table[80] = "LXXX"; table[800] = "DCCC";
            table[9] =   "IX"; table[90] =   "XC"; table[900] =   "CM";
        }

        std::string out;

        for( int base = 1; i > 0; base *= 10, i /= 10 )
        {
            int mod = i % 10;

            if( mod > 0 )
                out = table[mod * base] + out;
        }

        return out;
    }
}
