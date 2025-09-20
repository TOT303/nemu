#include "cpu/exec/template-start.h"
#define instr ret
static void do_execute(){
    cpu.eip=swaddr_read(REG(R_ESP),DATA_BYTE);
    REG(R_ESP)+=DATA_BYTE;
    
    print_asm("ret"); 
}

make_helper(ret) {
    op_src->type = OP_TYPE_IMM;
    op_src->imm  = swaddr_read(REG(R_ESP), 4);
    op_src->val  = op_src->imm;
    do_execute();
    return 1;
}
#include "cpu/exec/template-end.h"