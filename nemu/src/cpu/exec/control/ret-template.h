#include "cpu/exec/template-start.h"
#define instr ret

make_helper(ret) {
    cpu.eip = swaddr_read(REG(R_ESP), 4); 
    REG(R_ESP) += 4;                   
    print_asm("ret");
    return 0; 
}

make_helper(ret_i) {
	uint16_t imm = instr_fetch(eip + 1, 2);
	cpu.eip = swaddr_read(cpu.esp, 4) - (2 + 1);
	cpu.esp += 4 + imm;

	print_asm("ret $0x%x", imm);

	return 2 + 1;
}
#include "cpu/exec/template-end.h"