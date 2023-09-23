#include "hash.h"


#define max( a, b ) ( ( a ) > ( b ) ? ( a ) : ( b ) )
#define min( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )


hash_t hash( const void* ptr, uint64_t size ){

    const char* str = ( const char* ) ptr;

    if ( !str )
        return 0;

    hash_t res = str[0];

    for ( uint64_t i = 1 ; i < size; i += 1 ){
        uint64_t power = (str[i] ^ str[i - 1]) + ( str[i] > str[i - 1] ? str[i] : str[i - 1] );

        power += implication( min( str[i], str[i - 1] ), max( str[i], str[i - 1] ) );

        if ( power == 0 )
            power = 1;

        power = fastPow( res, power, BIG_NUMBER );
        res   = fastPow( res, power, BIG_NUMBER ) + power;
    }
    return res;
}


uint64_t fastPow( const uint64_t x, const  uint64_t power, const uint64_t mod ){

    if ( power == 0 )
        return 1;

    if ( x > mod )
        return fastPow( x % mod, power, mod );

    if ( power & 1 )
        return ( fastPow( x, power >> 1, mod ) ) * x % mod;

    uint64_t res = fastPow( x, power >> 1, mod );
    return ( res * res ) % mod;
}


int implication( char a, char b ){
    int res = 0;

    for ( int i = 8; i + 1; i-- )
        res = ((!( (a >> i) & 1 ) ) | ( (b >> i) & 1 )) & !( !((b >> i ) & 1) & !((a >> i) & 1));

    return res;
}


int checkHash( hash_t toCheck, hash_t right){
    if ( (toCheck ^ right) == 0 )
        return 1;
    return 0;
}
