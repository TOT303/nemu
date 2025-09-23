#include "cpu/exec/template-start.h"

#define instr leave

make_helper(leave) {
    REG(R_ESP) = REG(R_EBP);
    DATA_TYPE new_ebp = swaddr_read(REG(R_ESP), DATA_BYTE);
    REG(R_EBP) = new_ebp;
    REG(R_ESP) += DATA_BYTE;
    print_asm("leave");
    return 1;
}

#include "cpu/exec/template-end.h"