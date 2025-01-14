#ifndef tinyvm_h
#define tinyvm_h


void tvm_test();

typedef enum OOP_KIND
{
    OPP_READ,
    OPP_SET,
    OPP_OR,
    OPP_AND
} OOP_KIND;

const char *to_string(OOP_KIND k);

typedef struct TVM_OPP
{
    OOP_KIND kind;
    int arg;
} TVM_OPP;

typedef void (*WritePinFnPtr)(int pin, int state);
typedef int (*ReadPinFnPtr)(int pin);

typedef struct
{
    WritePinFnPtr write_pin;
    ReadPinFnPtr read_pin;

    int *stack;
    int max_stack;
} TVMCONFIG;
typedef TVMCONFIG *TVMCONFIGPTR;

typedef struct
{
    TVMCONFIGPTR cfg;

    int program_line;
    TVM_OPP *program;
    int program_len;
    int stack_ptr;
} TVM;
typedef TVM *TVMPTR;

void tvm_step(TVMPTR vm);
void tvm_init(TVMPTR vm, TVMCONFIGPTR config, TVM_OPP *program, int len);
#endif // tinyvm_h
