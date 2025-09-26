#include "cpu/exec/template-start.h"
#define instr call
static void do_execute(){
    REG(R_ESP)-=DATA_BYTE;
    
    swaddr_write(REG(R_ESP),DATA_BYTE,cpu.eip+DATA_BYTE+1);
    cpu.eip=cpu.eip+op_src->val;
    print_asm_template1();
}

make_instr_helper(si);
#include "cpu/exec/template-end.h"