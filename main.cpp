#include "main.h"
#include <string.h>


int main(){
    stack F;
    stackCtor( &F );
    printf( "%lld\n", hash( &F, sizeof( stack ) ) );
    printf( "%lld\n", hash( &F, sizeof( stack ) ) );
    printf( "%lld\n", hash( &F, sizeof( stack ) ) );
    printf( "%lld\n", hash( &F, sizeof( stack ) ) );
    printf( "%lld\n", hash( &F, sizeof( stack ) ) );
    for ( int i = 0; i < 100; i++ ){
        uint64_t res = stackPush( &F, i );
        printf( "PUSHED: %d\n", i );
        printf( "%lld\n", hash( &F, sizeof( stack ) ) );
        if ( res )
            printf( "%d\n", res );
    }
    printf( "size: %d\n", F.size );
    F.size = 0;
    printf( "size: %d\n", F.size );
    stackPush( &F, 15 );
    printf( "here" );
    return 0;
}
