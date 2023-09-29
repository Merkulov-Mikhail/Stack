#include "stack.h"
#include <stdio.h>

/*
STACK[0x8003F0] "stk" from main.cpp ( 35 ) main()
    called from stack.cpp( 31 ) StackPush() // Через вызов stackDump
    {
        size = 3
        capacity = 5
        data[0x003580]
        {
            *[0] = 10
            *[1] = 20
            *[2] = 30
             [3] = NAN
             [4] = NAN
        }
    }

    #define STACK_DUMP( stk ) StackDump( ( stk ), __FILE__, __LINE__, __FUNC__ );

*/

#define getPtr( stk, pos ) ptrGetter( ( stk ), ( pos ), sizeof( elem_t ) )

#define PERSON( pos )      destroyPart( pos, sizeof( egypt_t ) )

#define stackDump( stk, res )   _stackDump( ( stk ), #stk, __LINE__, __func__, __FILE__, res )

static uint64_t stackRealloc  ( stack* stk, uint64_t size );
static uint64_t stackOk       ( stack* stk );

static void destroyParts      (       void* ptr, size_t elSize, uint64_t nElems );
static void destroyPart       (       void* ptr, size_t size );
static int isPoisoned         ( const void* ptr, size_t size );

static void ptrSetter         (       stack* stk, uint64_t pos, elem_t value );
static elem_t* ptrGetter      ( const stack* stk, uint64_t pos, size_t size );

static hash_t countStructHash ( const stack* stk );
static hash_t countDataHash   ( const stack* stk );
static void recalculateHash   ( stack* stk );

static void _stackDump        ( const stack* stk, const char* stkName, int line, const char* funcName, const char* fileName, uint64_t res );

static void bytesPrint        ( void* ptr, size_t size );
// stackDump(); // Meta diagnostic ( this pizdec )


/*
No dump ):
*/


void stackCtor( stack* stk ){

    stk->size = 0;
    stk->capacity = START_CAPACITY;

    stk->data = ( elem_t* ) calloc( sizeof( elem_t ), stk->capacity );

    destroyParts( getPtr( stk, 0 ), sizeof( elem_t ), stk->capacity );

    PERSON( &( stk->slave   ) );
    PERSON( &( stk->pharaon ) );

    recalculateHash( stk );

    // everything is ok
}


static void destroyParts( void* ptr, size_t elSize, uint64_t nElems ){
    for ( uint64_t pos = 0; nElems; nElems--, pos++ ){
        destroyPart( ptr + pos * elSize, elSize );
    }
}


static void recalculateHash( stack* stk ){
    stk->structHash = countStructHash( stk );
    stk->dataHash   = countDataHash  ( stk );
}


static hash_t countStructHash( const stack* stk ){
    return hash( stk, sizeof( stack ) - sizeof( hash_t ) * 2 );
}

static hash_t countDataHash( const stack* stk ){
    return hash( stk->data,  sizeof( elem_t ) * stk->capacity );
}


void stackDtor( stack* stk ){

    for ( size_t pos = 0;  pos < stk->capacity; pos++ )
        destroyPart( ( void* ) getPtr( stk, pos ), sizeof( elem_t ) );

    free( stk->data );
    stk->size     = -1;
    stk->capacity = -2;
}


uint64_t stackPush( stack* stk, elem_t value ){
    // printf("%p", stk);
    uint64_t result = stackOk( stk );

    if ( result != STACK_ERRORS::OK )
        return result;

    ptrSetter( stk, stk->size, value );
    stk->size++;

    recalculateHash( stk );

    return STACK_ERRORS::OK;
}

uint64_t stackPop( stack* stk, elem_t* retValue ){

    uint64_t result = stackOk( stk );

    if ( stk->size == 0 )
        result |= STACK_ERRORS::STACK_OUT_OF_BOUNDS;

    if ( result != STACK_ERRORS::OK ){
        #ifdef DUMP
            stackDump( stk, result );
        #endif
        return result;
    }

    stk->size--;

    if ( retValue )
        *retValue = *( elem_t* ) ptrGetter( stk, stk->size, sizeof( elem_t ) );

    destroyPart(   ( void* ) ptrGetter( stk, stk->size, sizeof( elem_t ) ), sizeof( elem_t ) );

    recalculateHash( stk );

    return result;
}

static elem_t* ptrGetter( const stack* stk, uint64_t pos, size_t size ){
    return ( elem_t* )( stk->data + pos * size );
}

static void ptrSetter( stack* stk, uint64_t pos, elem_t value ){
    elem_t* ptr = getPtr( stk, pos );
    *ptr = value;
}

static void destroyPart( void* ptr, size_t size ){
    /*
    Документации на английском не будет
    Устанавливаем значение 0xDEADDEAD в 8 подряд идущих байт пока можем.
    Т.е. всего будет size // 8 вставок 0xDEADDEAD, счётчик cnt_8byte будет равен size // 8.

    Если осталось 4 байта, вставляем туда 0xDEAD и умножаем счётчик на 2
    Если осталось 2 байта или 1 байт, вставляем туда значение счётчика, увеличиваем счётчик на один

    Таким образом, достаточно легко обнаружить отравление данных

    Пример
    ------------
    size = 31, в памяти будет лежать следующее
    DEADDEAD DEADDEAD DEADDEAD DEAD 6 7
    ------------
    size = 17
    DEADDEAD DEADDEAD 2
    ------------
    size = 35
    DEADDEAD DEADDEAD DEADDEAD DEADDEAD 4 5
    ------------
    size = 16
    DEADDEAD DEADDEAD
    ------------
    size = 7
    DEAD 0 1

    */
    size_t front_pos = 0;
    int cnt_8byte = 0;

    while ( size ){
        if ( size >= 8 ){
            * ( uint64_t * ) ( ptr + front_pos ) = ( uint64_t ) 0xDEADDEAD;

            front_pos += 8;
            size -= 8;

            cnt_8byte += 1;
        }

        else if ( size >= 4 ){
            * ( uint32_t * ) ( ptr + front_pos ) = ( uint32_t ) 0xDEAD;

            front_pos += 4;
            size -= 4;

            cnt_8byte *= 2;
        }

        else if ( size >= 2 ){
            * ( uint16_t * ) ( ptr + front_pos ) = ( uint16_t ) cnt_8byte;

            front_pos += 2;
            size -= 2;

            cnt_8byte++;
        }

        else{
            * ( uint8_t  * ) ( ptr + front_pos ) = ( uint8_t  ) cnt_8byte;
            size -= 1;
        }
    }
}


static int isPoisoned( const void* ptr, size_t size ){

    size_t front_pos = 0;
    int cnt_8byte    = 0;

    while ( size ){
        if ( size >= 8 ){
            if ( 0xDEADDEAD ^ ( * ( uint64_t * ) ( ptr + front_pos ) ) )
                return 0;

            front_pos += 8;
            size -= 8;

            cnt_8byte += 1;
        }

        else if ( size >= 4 ){
            if ( 0xDEAD     ^ ( * ( uint32_t * ) ( ptr + front_pos ) ) )
                    return 0;

            front_pos += 4;
            size -= 4;

            cnt_8byte *= 2;
        }

        else if ( size >= 2 ){
            if ( cnt_8byte  ^  ( * ( uint16_t * ) ( ptr + front_pos ) ) )
                return 0;

            front_pos += 2;
            size -= 2;

            cnt_8byte++;
        }

        else{
            if ( cnt_8byte  ^  ( * ( uint8_t * ) ( ptr + front_pos ) ) )
                return 0;
            size -= 1;
        }
    }
    return 1;
}


static uint64_t stackRealloc( stack* stk, uint64_t size ){
    void* ptr = realloc( stk->data, size );
    if ( ptr ){

        uint64_t cap = size / sizeof( elem_t );

        stk->data = ptr;

        if ( cap > stk->capacity )
            destroyParts( getPtr( stk, stk->capacity), sizeof( elem_t ), cap - stk->capacity );

        else
            destroyParts( getPtr( stk, cap          ), sizeof( elem_t ), stk->capacity - cap );

        stk->capacity = cap;
        return STACK_ERRORS::OK;
    }

    return STACK_ERRORS::STACK_OUT_OF_MEMORY;

}


static uint64_t stackOk( stack* stk ){

    uint64_t res = 0;
    if ( !stk ){ res |= STACK_NULL; return res;}

    if ( !isPoisoned( &( stk->pharaon ), sizeof( egypt_t ) ) ) res |= EGYPT_SYSTEM_DOWN;
    if ( !isPoisoned( &( stk->slave ),   sizeof( egypt_t ) ) ) res |= EGYPT_SYSTEM_DOWN;

    if ( !( res & EGYPT_SYSTEM_DOWN ) ){ // if canarys are dead, we definitly have an UB

        if ( !checkHash( stk->structHash, countStructHash( stk ) ) ) res |= HASH_STRUCT_ERROR;

        if ( !( res & HASH_STRUCT_ERROR )){ // if Struct is modified, we do not use data in it
            if ( !checkHash( stk->dataHash,   countDataHash( stk ) ) )   res |= HASH_DATA_ERROR;

            if ( stk->capacity < stk->size ) res |= STACK_OUT_OF_BOUNDS;
            if ( !stk->data )                res |= STACK_DATA_NULL;
        }
    }

    if ( !res ){

        if ( stk->capacity == stk->size ){
            uint64_t result = stackRealloc( stk, stk->capacity * MULTYPLIER * sizeof( elem_t ) );

            if ( result )
                res |= STACK_OUT_OF_MEMORY;
        }

        if ( stk->size * MULTYPLIER * 2 < stk->capacity && stk->capacity > START_CAPACITY ){
            uint64_t result = stackRealloc( stk, stk->capacity / MULTYPLIER * sizeof( elem_t ) );

            if ( result )
                res |= STACK_OUT_OF_MEMORY;
        }
    }

    #ifdef DUMP
        if ( res )
            stackDump( stk, res );
    #endif

    return res;
}

static void bytesPrint( void* ptr, size_t size ){
    size_t pos = 0;
    printf( "0x" );
    while ( size ){

        if ( size >= 8 ){
            printf( "%.8x", *( uint64_t* ) ( ptr + pos ) );
            pos  += 8;
            size -= 8;
        }

        else if ( size >= 4 ){
            printf( "%.4x", *( uint32_t* ) ( ptr + pos ) );
            pos  += 4;
            size -= 4;
        }

        else if ( size >= 2 ){
            printf( "%.2x", *( uint16_t* ) ( ptr + pos ) );
            pos  += 2;
            size -= 2;
        }

        else{
            printf( "%.1x", *( uint8_t* ) ( ptr + pos ) );
            pos  += 1;
            size -= 1;
        }
    }
}

static void _stackDump        ( const stack* stk, const char* stkName, int line, const char* funcName, const char* fileName, uint64_t res ){
    printf( "\n\n-----------------------"
             "\nREASONS FOR THIS OUTPUT:\n" );

    if ( res & STACK_ERRORS::EGYPT_SYSTEM_DOWN ){
        printf( "\tSLAVE OR PHARAON ARE NOT PEOPLE\n" );
        return;
    }

    if ( res & STACK_ERRORS::HASH_DATA_ERROR ){
        printf( "\tDATA WAS MODIFIED\n" );
    }

    if ( res & STACK_ERRORS::HASH_STRUCT_ERROR ){
        printf( "\tSTRUCT MODIFIED\n" );
        return;
    }

    if ( res & STACK_ERRORS::STACK_DATA_NULL ){
        printf( "\tDATA IS NULLED\n" );
        return;
    }

    if ( res & STACK_ERRORS::STACK_NULL ){
        printf( "\tSTACK IS NULLED\n" );
        return;
    }

    if ( res & STACK_ERRORS::STACK_OUT_OF_BOUNDS ){
        printf( "\tBAD INDEXING, OUT oF BOUNDS\n" );
    }

    if ( res & STACK_ERRORS::STACK_OUT_OF_MEMORY ){
        printf( "\nOUT OF MEMORY\n" );
    }

    if ( res & STACK_ERRORS::STACK_UNDERFLOW ){
        printf( "\tPOP WITH SIZE=0\n" );
    }

    printf( "-----------------------\n\n\n" );

    printf( "STACK[0x%p] \"%s\" from %s ( %d ) %s\n{\n", &stk, stkName, fileName, line, funcName );

    if ( isPoisoned( &stk->slave, sizeof(egypt_t) ) )
        printf( "\tLcanary \t = 0x%8.x (OK)\n", stk->slave );
    else
        printf( "\tLcanary \t = 0x%8.x (ERROR)\n", stk->slave );

    printf( "\tsize    \t = %lld\n", stk->size );
    printf( "\tcapacity\t = %lld\n", stk->capacity );

    if ( isPoisoned( &stk->pharaon, sizeof(egypt_t) ) )
        printf( "\tRcanary \t = 0x%8.x (OK)\n", stk->pharaon );
    else
        printf( "\tRcanary \t = 0x%8.x (ERROR)\n", stk->pharaon );

    printf( "\tstructHash\t  = %lld\n", stk->structHash );
    printf( "\tdataHash  \t  = %lld\n", stk->dataHash  );

    printf( "\tdata[0x%p]\n",           stk->data );
    printf( "\t{\n" );

    for ( stackSize_t pos = 0; pos < stk->capacity; pos++ ){

        int cellStatus = isPoisoned( stk->data + pos * sizeof( elem_t ), sizeof( elem_t ) );

        if ( cellStatus )
            printf( "\t\t [%lld]\t = ", pos );
        else
            printf( "\t\t*[%lld]\t = ", pos );

        bytesPrint( stk->data + pos * sizeof( elem_t ), sizeof( elem_t ) );
        if ( cellStatus )
            printf(" (POISON)");
        else
            printf(" (OK)");

        printf( "\n" );
    }

    printf( "\t}\n" );
    printf( "}\n" );
}




/*
[5] [1] [2] [3] [4] [ ]

capacity = 6
size = 1

st = Stack

st.push()
st.size()
value = st.pop() // 2
value = st.pop() // 10

st.push( 5 )
st.push( 10 )
st.push( 2 )


*/
