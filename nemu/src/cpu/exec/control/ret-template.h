#include "cpu/exec/template-start.h"
#define instr ret

make_helper(ret) {
    cpu.eip = swaddr_read(REG(R_ESP), 4); 
    REG(R_ESP) += 4;                     
    print_asm("ret");
    return 0; 
}
#include "cpu/exec/template-end.h"