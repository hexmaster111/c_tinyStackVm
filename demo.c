#include <unistd.h>
#include <stdio.h>
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "tinyvm.h"

#define PIN_COUNT 10
struct
{
    int pins[PIN_COUNT];
} g_demo_state;
void write_pin(int p, int s){g_demo_state.pins[p] = s;}
int read_pin(int p){return g_demo_state.pins[p];}


#define MAX_STACK 10
int g_stackMemory[MAX_STACK] = {0};

int main()
{

    tvm_test();

    TVMCONFIG conf = {
        .read_pin = read_pin,
        .write_pin = write_pin,
        .stack = g_stackMemory,
        .max_stack = MAX_STACK,
    };

    TVM vm = {0};

    TVM_OPP testProgram[] = {
        (TVM_OPP){OPP_READ, 1},
        (TVM_OPP){OPP_READ, 2},
        (TVM_OPP){OPP_READ, 0},
        (TVM_OPP){OPP_OR, 2},
        (TVM_OPP){OPP_AND, 2},
        (TVM_OPP){OPP_SET, 0}

    };

    //   TVM_OPP testProgram[] = {
    //         (TVM_OPP){OPP_READ, 1},
    //         (TVM_OPP){OPP_READ, 2},
    //         (TVM_OPP){OPP_READ, 3},
    //         (TVM_OPP){OPP_AND, 3},
    //         (TVM_OPP){OPP_SET, 4}

    //     };

    int len = sizeof(testProgram) / sizeof(testProgram[0]);

    tvm_init(&vm, &conf, testProgram, len);
    int first = 1;

    InitWindow(800, 600, "TVM");
    SetTargetFPS(60);

    bool run = false;
    double last_step = 0;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        if (run && GetTime() > last_step + 0.5)
        {
            tvm_step(&vm);
            last_step = GetTime();
        }

        if (GuiButton((Rectangle){10, 10, 32, 32}, "STEP"))
            tvm_step(&vm);

        TVM_OPP o = vm.program[vm.program_line];

        GuiCheckBox((Rectangle){10, 45, 18, 32}, "RUN", &run);

        int reg_view_x = 55, reg_view_y = 10;
        for (size_t i = 0; i < vm.cfg->max_stack; i++)
        {
            DrawText(TextFormat("%d", i), reg_view_x, reg_view_y + i * 24, 24, YELLOW);
        }

        for (size_t i = 0; i < vm.stack_ptr; i++)
        {
            DrawText(TextFormat("%d", vm.cfg->stack[i]),
                     reg_view_x + 24 + 5, reg_view_y + i * 24, 24, YELLOW);
        }

        DrawTextPro(GetFontDefault(),
                    "STACK",
                    (Vector2){reg_view_x + 24 + 5, reg_view_y + vm.cfg->max_stack * 24},
                    (Vector2){0, 0}, 90, 18, 1,
                    YELLOW);

        for (size_t i = 0; i < PIN_COUNT; i++)
        {
            if (GuiButton((Rectangle){reg_view_x + 50 + i * 34, 10, 32, 32},
                          TextFormat("%d", i)))
            {
                g_demo_state.pins[i] = !g_demo_state.pins[i];
            }
        }

        for (size_t i = 0; i < PIN_COUNT; i++)
        {
            if (g_demo_state.pins[i])
                DrawRectangleRec((Rectangle){reg_view_x + 50 + i * 34, 64, 32, 32}, YELLOW);
        }

        for (size_t i = 0; i < vm.program_len; i++)
        {
            DrawText(TextFormat("%s %d",
                         to_string(vm.program[i].kind),
                         vm.program[i].arg),
                     reg_view_x + 100, 128 + 18 * i, 18,
                     vm.program_line == i ? YELLOW : GREEN);
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}