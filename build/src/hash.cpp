#include "hash.h"


uint64_t hash( const char* str ){

    if ( !str )
        return 0;

    uint64_t res = str[0];

    for ( int i = 2; str[i] && str[i - 1]; i += 2 ){
        int power = str[i] ^ str[i - 1];

        if ( power < ( str[i] & str[i - 1] ) )
            power = str[i] & str[i - 1];

        if ( power == 0 )
            power = 1;
        res = fastPow( res, power, BIG_NUMBER );
    }
    return res;
}


uint64_t fastPow( const uint64_t x, const  uint64_t power, const uint64_t mod ){

    if ( power == 0 )
        return 1;

    if ( x > mod )
        return fastPow( x % mod, power, mod );

    if ( power & 1 ) // power % 2 == 1
        return ( fastPow( x, power >> 1, mod ) ) * x % mod;

    uint64_t res = fastPow( x, power >> 1, mod );
    return ( res * res ) % mod;
}
