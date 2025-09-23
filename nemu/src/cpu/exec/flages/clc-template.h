#include "cpu/exec/template-start.h"
#define instr clc
make_helper(clc){
    cpu.eflags.CF=0;
    print_asm("clc");
    return 1;   
}

#include "cpu/exec/template-end.h"