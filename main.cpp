#include "main.h"


int main(){
    stack F;
    stackCtor( &F );
    uint64_t a = stackPush( &F, 15 );

    printf( "%d %lld", F.size, a );
    return 0;
}
