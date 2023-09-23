#include "main.h"


int main(){
    stack F;
    stackCtor( &F );
    printf("go");
    uint64_t a = stackPush( &F, 15 );
    printf("go");
    printf("%lld\n", hash( &F, sizeof( stack )));
    printf("%lld\n", hash( &F, sizeof( stack )));
    printf("%lld\n", hash( &F, sizeof( stack )));
    printf("%lld\n", hash( &F, sizeof( stack )));
    printf("%lld\n", hash( &F, sizeof( stack )));
    printf( "%d %lld\n", F.size, a );
    for ( int i = 0; i < 100; i++){
        uint64_t res = stackPush(&F, i);
        printf("PUSHED: %d\n", i);
        if (res)
            printf("%d\n", res);
    }
    for ( int i = 0; i < 200; i++ ){
        elem_t l = 0;
        uint64_t res = stackPop(&F, &l);
        printf("%lld %lld \n", l, F.size);
        if (res)
            printf("%d\n", res);
    }
    return 0;
}
