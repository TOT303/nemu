#include "cpu/exec/helper.h"
#define DATA_BYTE 1        
#include "cmp-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "cmp-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "cmp-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */
make_helper(cmp_i2rm_b);
make_helper(cmp_si2rm_b);
make_helper(cmp_r2rm_b);
make_helper(cmp_rm2r_b);
make_helper(cmp_i2a_b);

make_helper_v(cmp_i2rm)
make_helper_v(cmp_si2rm)
make_helper_v(cmp_r2rm)
make_helper_v(cmp_rm2r)
make_helper_v(cmp_i2a)

