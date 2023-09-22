#ifndef STACK_H

    #define STACK_H
    #define DEBUG


    #include <cstddef>
    #include <cstdint>
    #include <stdlib.h>

    const int MULTYPLIER = 2;
    const int START_CAPACITY = 2;

    typedef int elem_t;
    typedef uint64_t egypt_t;
    typedef uint64_t stackSize_t;

    struct stack{
        egypt_t     slave;
        void*     data;
        stackSize_t capacity;
        stackSize_t size;
        egypt_t     pharaon;
    };

    enum STACK_ERRORS{
        OK                  = 0,
        STACK_NULL          = 1 << 0,
        STACK_OUT_OF_BOUNDS = 1 << 1,
        STACK_DATA_NULL     = 1 << 2,
        STACK_OUT_OF_MEMORY = 1 << 3,
        EGYPT_SYSTEM_DOWN   = 1 << 4
    };


    void stackCtor( stack* stk );
    void stackDtor( stack* stk );
    uint64_t stackPush( stack* stk, elem_t value );
    uint64_t stackPop( stack* stk, elem_t* retValue );


#endif
