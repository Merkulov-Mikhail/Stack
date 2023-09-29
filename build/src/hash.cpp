#include "hash.h"


#define max( a, b ) ( ( a ) > ( b ) ? ( a ) : ( b ) )
#define min( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )


hash_t hash( const void* ptr, uint64_t size ){
    const char* str = ( const char* ) ptr;

    if ( !str )
        return 0;

    hash_t res = str[0];

    for ( uint64_t i = 1 ; i < size; i += 1 ){
        uint64_t power = ( str[i] ^ str[i - 1] ) + ( str[i] > str[i - 1] ? str[i] : str[i - 1] );

        power += implication( min( str[i], str[i - 1] ), max( str[i], str[i - 1] ) );

        if ( power == 0 )
            power = 1;

        res = fastPow( res, power, BIG_NUMBER ) + power;
    }
    return res;
}


int64_t fastPow( uint64_t x, uint64_t power, const uint64_t mod ){

    int64_t ans = 1;
    for(; power; power >>=1, x=x*x % mod) {
        if(power & 1) {
            ans *= x;
            if(ans > mod) ans %= mod;
        }
    }
    return ans;
}




int implication( char a, char b ){

    int res = 0;

    for ( int i = 8; i + 1; i-- )
        res = ( ( !( ( a >> i ) & 1 ) ) | ( ( b >> i ) & 1 ) ) & !( !( ( b >> i ) & 1 ) & !( ( a >> i ) & 1 ) );

    return res;
}


int checkHash( hash_t toCheck, hash_t right ){
    if ( ( toCheck ^ right ) == 0 )
        return 1;
    return 0;
}
