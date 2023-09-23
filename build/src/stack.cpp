#include "stack.h"
#include <stdio.h>


#define getPtr( stk, pos ) ptrGetter( ( stk ), ( pos ), sizeof( elem_t ) )

#define PERSON( pos )      destroyPart( pos, sizeof( egypt_t ) )


static uint64_t stackRealloc  ( stack* stk, uint64_t size );
static uint64_t stackOk       ( stack* stk );

static void destroyPart       (       void* ptr, size_t size );
static int isPoisoned         ( const void* ptr, size_t size );

static void ptrSetter         (       stack* stk, uint64_t pos, elem_t value );
static elem_t* ptrGetter      ( const stack* stk, uint64_t pos, size_t size );

static hash_t countStructHash ( const stack* stk );
static hash_t countDataHash   ( const stack* stk );
static void recalculateHash   ( stack* stk );
// stackDump(); // Meta diagnostic ( this pizdec )
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

/*
No dump ):
*/


void stackCtor( stack* stk ){

    stk->size = 0;
    stk->capacity = START_CAPACITY;

    stk->data = ( elem_t* ) calloc( sizeof( elem_t ), stk->capacity );

    for ( stackSize_t pos = 0; pos < stk->capacity; pos++  )
        destroyPart( getPtr( stk, pos ), sizeof( elem_t ) );

    PERSON( &( stk->slave   ) );
    PERSON( &( stk->pharaon ) );

    recalculateHash(stk);

    // everything is ok
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

    uint64_t result = stackOk( stk );

    if ( result != STACK_ERRORS::OK )
        return result;

    ptrSetter( stk, stk->size, value );
    stk->size++;

    recalculateHash( stk );

    return value;
}

uint64_t stackPop( stack* stk, elem_t* retValue ){

    uint64_t result = stackOk( stk );

    if ( result != STACK_ERRORS::OK )
        return result;

    stk->size--;
    *retValue = * ( elem_t* ) ptrGetter( stk, stk->size, sizeof( elem_t ) );

    destroyPart( ( void* ) ptrGetter( stk, stk->size, sizeof( elem_t ) ), sizeof( elem_t ) );

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
        stk->data = ptr;
        stk->capacity = size / sizeof( elem_t );

        return STACK_ERRORS::OK;
    }

    stk->data = ptr;
    return STACK_ERRORS::STACK_OUT_OF_MEMORY;

}


static uint64_t stackOk( stack* stk ){
    uint64_t res = 0;

    if ( !stk ) res |= STACK_NULL;

    if ( stk->capacity < stk->size ) res |= STACK_OUT_OF_BOUNDS;
    if ( !stk->data )                res |= STACK_DATA_NULL;

    if ( !isPoisoned( ( void* ) &( stk->pharaon ), sizeof( egypt_t ) ) ) res |= EGYPT_SYSTEM_DOWN;
    if ( !isPoisoned( ( void* ) &( stk->slave ),   sizeof( egypt_t ) ) ) res |= EGYPT_SYSTEM_DOWN;

    if ( !checkHash( stk->dataHash,   countDataHash( stk ) ) )   res |= HASH_DATA_ERROR;

    if ( !checkHash( stk->structHash, countStructHash( stk ) ) ) res |= HASH_STRUCT_ERROR;

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

    return res;
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
