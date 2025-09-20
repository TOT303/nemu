#include "cpu/exec/template-start.h"
#define instr call
static void do_execute(){
    REG(R_ESP)-=DATA_BYTE;
    swaddr_write(REG(R_ESP),DATA_BYTE,op_src->val);
    cpu.eip=cpu.eip+op_src->val;
}

make_instr_helper(si);
#include "cpu/exec/template-end.h"