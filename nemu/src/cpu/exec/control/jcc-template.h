#include "cpu/exec/template-start.h"

#define JCC_EXEC static void do_execute() {\
    if (COND) {\
        cpu.eip += op_src->val;\
    }\
    print_asm(str(instr) " %x", cpu.eip + 1 + DATA_BYTE);\
}\
make_instr_helper(si)


#define instr je
#define COND (cpu.eflags.ZF == 1)
JCC_EXEC
#undef instr
#undef COND

#define instr jne
#define COND (cpu.eflags.ZF == 0)
JCC_EXEC
#undef instr
#undef COND

#define instr jl
#define COND (cpu.eflags.SF != cpu.eflags.OF)
JCC_EXEC
#undef instr
#undef COND

#define instr jle
#define COND  (cpu.eflags.ZF || (cpu.eflags.SF != cpu.eflags.OF))
JCC_EXEC
#undef instr
#undef COND

#define instr jg
#define COND ((cpu.eflags.ZF == 0) && (cpu.eflags.SF == cpu.eflags.OF))
JCC_EXEC
#undef instr
#undef COND

#define instr jge
#define COND (cpu.eflags.SF == cpu.eflags.OF)
JCC_EXEC
#undef instr
#undef COND

#define instr jb
#define COND (cpu.eflags.CF == 1)
JCC_EXEC
#undef instr
#undef COND

#define instr ja
#define COND ((cpu.eflags.CF == 0) && (cpu.eflags.ZF == 0))
JCC_EXEC
#undef instr
#undef COND

#define instr jbe
#define COND ((cpu.eflags.CF == 1) || (cpu.eflags.ZF == 1))
JCC_EXEC
#undef instr
#undef COND

#define instr jae
#define COND (cpu.eflags.CF == 0)
JCC_EXEC
#undef instr
#undef COND


#define instr jc
#define COND (cpu.eflags.CF == 1)
JCC_EXEC
#undef instr
#undef COND

#define instr jnc
#define COND (cpu.eflags.CF == 0)
JCC_EXEC
#undef instr
#undef COND

#define instr jz
#define COND (cpu.eflags.ZF == 1)
JCC_EXEC
#undef instr
#undef COND

#define instr jnz
#define COND (cpu.eflags.ZF == 0)
JCC_EXEC
#undef instr
#undef COND

#define instr jna
#define COND ((cpu.eflags.CF == 1) || (cpu.eflags.ZF == 1))
JCC_EXEC
#undef instr
#undef COND

#define instr jnae
#define COND (cpu.eflags.CF == 1)
JCC_EXEC
#undef instr
#undef COND

#define instr jnb
#define COND (cpu.eflags.CF == 0)
JCC_EXEC
#undef instr
#undef COND

#define instr jnbe
#define COND ((cpu.eflags.CF == 0) && (cpu.eflags.ZF == 0))
JCC_EXEC
#undef instr
#undef COND

#define instr jng
#define COND ((cpu.eflags.ZF == 1) || (cpu.eflags.SF != cpu.eflags.OF))
JCC_EXEC
#undef instr
#undef COND

#define instr jnge
#define COND (cpu.eflags.SF != cpu.eflags.OF)
JCC_EXEC
#undef instr
#undef COND

#define instr jnl
#define COND (cpu.eflags.SF == cpu.eflags.OF)
JCC_EXEC
#undef instr
#undef COND

#define instr jnle
#define COND ((cpu.eflags.ZF == 0) && (cpu.eflags.SF == cpu.eflags.OF))
JCC_EXEC
#undef instr
#undef COND

#define instr jo
#define COND (cpu.eflags.OF == 1)
JCC_EXEC
#undef instr
#undef COND

#define instr jno
#define COND (cpu.eflags.OF == 0)
JCC_EXEC
#undef instr
#undef COND

#define instr jp
#define COND (cpu.eflags.PF == 1)
JCC_EXEC
#undef instr
#undef COND

#define instr jpe
#define COND (cpu.eflags.PF == 1)
JCC_EXEC
#undef instr
#undef COND

#define instr jpo
#define COND (cpu.eflags.PF == 0)
JCC_EXEC
#undef instr
#undef COND

#define instr jnp
#define COND (cpu.eflags.PF == 0)
JCC_EXEC
#undef instr
#undef COND

#define instr js
#define COND (cpu.eflags.SF == 1)
JCC_EXEC
#undef instr
#undef COND

#define instr jns
#define COND (cpu.eflags.SF == 0)
JCC_EXEC
#undef instr
#undef COND

#define instr jcxz
#define COND (cpu.ecx == 0)  
JCC_EXEC
#undef instr
#undef COND

#define instr jecxz
#define COND (cpu.ecx == 0)
JCC_EXEC
#undef instr
#undef COND



#include "cpu/exec/template-end.h"

