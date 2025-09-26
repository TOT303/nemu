#include "cpu/exec/template-start.h"
#define instr ret

make_helper(ret) {
     printf("ret: before - esp=0x%x, eip=0x%x\n", REG(R_ESP), cpu.eip);
    cpu.eip = swaddr_read(REG(R_ESP), 4); 
    REG(R_ESP) += 4;  
    printf("ret: after - esp=0x%x, eip=0x%x\n", REG(R_ESP), cpu.eip);                   
    print_asm("ret");
    return 0; 
}
#include "cpu/exec/template-end.h"