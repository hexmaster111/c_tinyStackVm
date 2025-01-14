#include "tinyvm.h"
#include <assert.h>

#include <stdio.h> //debugging

const char *to_string(OOP_KIND k)
{
    if (k == OPP_AND)
        return "AND ";
    if (k == OPP_OR)
        return "OR  ";
    if (k == OPP_READ)
        return "READ";
    if (k == OPP_SET)
        return "SET ";
    return "invalid k!";
}

void tvm_push(TVMPTR vm, int val)
{
    vm->cfg->stack[vm->stack_ptr] = val;
    vm->stack_ptr = vm->stack_ptr + 1;
}

int tvm_pop(TVMPTR vm)
{
    vm->stack_ptr = vm->stack_ptr - 1;
    int i = vm->cfg->stack[vm->stack_ptr];
    return i;
}

void tvm_step(TVMPTR vm)
{
    TVM_OPP o = vm->program[vm->program_line];

    switch (o.kind)
    {
    case OPP_READ:
    {
        int pv = vm->cfg->read_pin(o.arg);
        tvm_push(vm, pv);
    }
    break;

    case OPP_SET:
    {
        int pv = tvm_pop(vm);
        vm->cfg->write_pin(o.arg, pv);
    }
    break;

    case OPP_OR:
    {
        int final_value = tvm_pop(vm);
        for (int i = 1; i < o.arg; i++)
        {
            int v = tvm_pop(vm);
            final_value = v | final_value;
        }
        tvm_push(vm, final_value);
    }
    break;

    case OPP_AND:
    {
        int final_value = tvm_pop(vm);
        for (int i = 1; i < o.arg; i++)
        {
            final_value = final_value & tvm_pop(vm);
        }
        tvm_push(vm, final_value);
    }
    break;

    default:
        break;
    }

    vm->program_line = vm->program_line + 1;
    if (vm->program_line >= vm->program_len)
    {
        vm->program_line = 0;
    }
}

void tvm_init(TVMPTR vm, TVMCONFIGPTR config, TVM_OPP *program, int len)
{
    assert(vm != 0 && ("vm != null"));
    assert(config != 0 && ("config != null"));

    vm->program = program;
    vm->program_len = len;
    vm->cfg = config;
    vm->program_line = 0;
}

#include <assert.h>
void tvm_test()
{
    int stack[5] = {0};
    TVM tst_vm = {0};
    TVMCONFIG con = {
        .max_stack = 5,
        .stack = stack,
    };
    tvm_init(&tst_vm, &con, NULL, 0);

    tvm_push(&tst_vm, 1);
    assert(tvm_pop(&tst_vm) == 1);

    tvm_push(&tst_vm, 4);
    tvm_push(&tst_vm, 2);
    assert(tvm_pop(&tst_vm) == 2);
    assert(tvm_pop(&tst_vm) == 4);

    tvm_push(&tst_vm, 5);
    tvm_push(&tst_vm, 6);
    tvm_push(&tst_vm, 7);
    assert(tvm_pop(&tst_vm) == 7);
    assert(tvm_pop(&tst_vm) == 6);
    assert(tvm_pop(&tst_vm) == 5);

    tvm_push(&tst_vm, 8);
    tvm_push(&tst_vm, 9);
    tvm_push(&tst_vm, 10);
    tvm_push(&tst_vm, 11);
    assert(tvm_pop(&tst_vm) == 11);
    assert(tvm_pop(&tst_vm) == 10);
    assert(tvm_pop(&tst_vm) == 9);
    assert(tvm_pop(&tst_vm) == 8);

    tvm_push(&tst_vm, 12);
    tvm_push(&tst_vm, 13);
    tvm_push(&tst_vm, 14);
    tvm_push(&tst_vm, 15);
    assert(tvm_pop(&tst_vm) == 15);
    assert(tvm_pop(&tst_vm) == 14);
    assert(tvm_pop(&tst_vm) == 13);
    assert(tvm_pop(&tst_vm) == 12);
}