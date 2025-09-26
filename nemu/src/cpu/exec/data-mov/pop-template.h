#include "cpu/exec/template-start.h"
#define instr pop


static void do_execute(){
    DATA_TYPE result=swaddr_read(REG(R_ESP),DATA_BYTE);
    OPERAND_W(op_src, result);
    REG(R_ESP)=DATA_BYTE+REG(R_ESP);
    print_asm_template1();
}

make_instr_helper(r);

#include "cpu/exec/template-end.h"