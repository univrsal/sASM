
#include "asm.h"

int main(int argc, char** args)
{
    if (argc > 1) {
        sasm_asm* asm_lang = sasm_asm_load(args[1]);
    }
    return 0;
}