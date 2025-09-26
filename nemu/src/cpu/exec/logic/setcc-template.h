#include "cpu/exec/template-start.h"

#define SETCC_EXEC \
    static void do_execute() { \
        int cond = COND; \
        OPERAND_W(op_src, cond); \
        print_asm(str(instr) " %s", op_src->str); \
    }\
make_instr_helper(rm)

#define instr seto      
#define COND (cpu.eflags.OF == 1)              
SETCC_EXEC
#undef instr
#undef COND

#define instr setno     
#define COND (cpu.eflags.OF == 0)              
SETCC_EXEC
#undef instr
#undef COND

#define instr setb      
#define COND (cpu.eflags.CF == 1)              
SETCC_EXEC
#undef instr
#undef COND

#define instr setc      
#define COND (cpu.eflags.CF == 1)              
SETCC_EXEC
#undef instr
#undef COND

#define instr setnae    
#define COND (cpu.eflags.CF == 1)              
SETCC_EXEC
#undef instr
#undef COND

#define instr setae     
#define COND (cpu.eflags.CF == 0)              
SETCC_EXEC
#undef instr
#undef COND

#define instr setnb     
#define COND (cpu.eflags.CF == 0)              
SETCC_EXEC
#undef instr
#undef COND
#define instr setnc     
#define COND (cpu.eflags.CF == 0)              
SETCC_EXEC
#undef instr
#undef COND
#define instr setz      
#define COND (cpu.eflags.ZF == 1)              
SETCC_EXEC
#undef instr
#undef COND
#define instr sete      
#define COND (cpu.eflags.ZF == 1)              
SETCC_EXEC
#undef instr
#undef COND
#define instr setnz     
#define COND (cpu.eflags.ZF == 0)              
SETCC_EXEC
#undef instr
#undef COND
#define instr setne     
#define COND (cpu.eflags.ZF == 0)              
SETCC_EXEC
#undef instr
#undef COND
#define instr setbe     
#define COND ((cpu.eflags.CF == 1) || (cpu.eflags.ZF == 1))  
SETCC_EXEC
#undef instr
#undef COND
#define instr setna     
#define COND ((cpu.eflags.CF == 1) || (cpu.eflags.ZF == 1))  
SETCC_EXEC
#undef instr
#undef COND
#define instr seta      
#define COND ((cpu.eflags.CF == 0) && (cpu.eflags.ZF == 0))  
SETCC_EXEC
#undef instr
#undef COND
#define instr setnbe    
#define COND ((cpu.eflags.CF == 0) && (cpu.eflags.ZF == 0))  
SETCC_EXEC
#undef instr
#undef COND
#define instr sets      
#define COND (cpu.eflags.SF == 1)              
SETCC_EXEC
#undef instr
#undef COND
#define instr setns     
#define COND (cpu.eflags.SF == 0)              
SETCC_EXEC
#undef instr
#undef COND
#define instr setp      
#define COND (cpu.eflags.PF == 1)              
SETCC_EXEC
#undef instr
#undef COND
#define instr setpe     
#define COND (cpu.eflags.PF == 1)              
SETCC_EXEC
#undef instr
#undef COND
#define instr setnp     
#define COND (cpu.eflags.PF == 0)              
SETCC_EXEC
#undef instr
#undef COND

#include "cpu/exec/template-end.h"