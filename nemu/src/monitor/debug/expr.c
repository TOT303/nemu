
#include "nemu.h"
#include <stdlib.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <elf.h>
extern Elf32_Sym* get_symtab();
extern char* get_strtab();
extern int get_nr();

Elf32_Sym *symtab;
char *strtab;
int nr_symtab_entry;


enum {
	NOTYPE = 256, EQ,NEQ, NUM,AND,OR,NOT,HEX,REG,DEREF,NEG,SYMBOL
	/* TODO: Add more token types */
	
};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},        // spaces
	{"==", EQ},				
	{"!=",NEQ},
	{"&&",AND},
	{"\\|\\|",OR},
	{"\\!",NOT},
	{"0[xX][0-9a-fA-F]+", HEX},
	{"\\$[a-z]+",REG},	
	{"\\+", '+'},				
	{"\\-",'-'},
	{"\\*",'*'},
	{"\\/",'/'},
	{"[0-9]+",NUM},
	{"\\(",'('},
	{"\\)",')'},
	{"\\w+",SYMBOL}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static uint32_t find_symbol_addr(char *str) {
    Elf32_Sym *symtab = get_symtab();
    char *strtab = get_strtab();
    int nr_symtab_entry = get_nr();
    int i;
    for (i = 0; i < nr_symtab_entry; i++) {
       
        if (symtab[i].st_name == 0) {
            continue;
        }
      
        if (ELF32_ST_TYPE(symtab[i].st_info) == STT_OBJECT || ELF32_ST_TYPE(symtab[i].st_info) == STT_FUNC) {
            char *sym_name = &strtab[symtab[i].st_name];
            if (strcmp(str, sym_name) == 0) {
                return symtab[i].st_value;
            }
        }
    }
    return 0;
}

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int   substr_len   = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
					i, rules[i].regex, position, substr_len, substr_len, substr_start);

				char  tmp[32];
				if (substr_len >= sizeof(tmp)) substr_len = sizeof(tmp) - 1;
				strncpy(tmp, substr_start, substr_len);
				tmp[substr_len] = '\0';

				position += substr_len;          
				uint32_t addr;
				switch (rules[i].token_type) {
					case NOTYPE: break;          
					case EQ:
					case NEQ:
					case '(':
					case ')':
					case NUM:
					case '+':
					case '-':
					case '*':
					case '/':
					case AND:
					case OR:
					case NOT:
					case HEX:
					case REG:                    
						tokens[nr_token].type = rules[i].token_type;
						strcpy(tokens[nr_token].str, tmp);
						nr_token++;
						break;
					case SYMBOL:
						addr = find_symbol_addr(tmp);
						if (addr == 0) panic("invalid symbol");
						snprintf(tokens[nr_token].str, sizeof(tokens[nr_token].str), "%u", addr);
						tokens[nr_token].type = SYMBOL; 
						nr_token++;
						break;
				}
				break;                    
			}
		}

		if(i == NR_REGEX) {
			return false;
		}
	}

	return true; 
}


static bool checkparentheses(int p, int q) {
    if (tokens[p].type != '(' || tokens[q].type != ')') return false;
    int bal = 0;
	int i;
    for (i = p; i <= q; i++) {
        if (tokens[i].type == '(') bal++;
        if (tokens[i].type == ')') bal--;
        if (bal == 0 && i < q) return false;  
    }
    return bal == 0;
}


static int priority(int type) {
    switch (type) {
		case AND: case OR: return 0;
		case EQ: case NEQ :  return 1;
		case '+': case '-': return 2;
		case '*': case '/': return 3;
		case NOT: case DEREF: case NEG: return 4;
		default:            return -1; 
    }
}
static int find_op(int p, int q)
{
    int pos = -1;
    int pri = INT32_MAX;
    int i;
	for (i=p;i<=q;i++){
		if (priority(tokens[i].type)==-1){
			continue;
		}
		int count=0;
		int j;
		for (j=p;j<i;j++){
			if (tokens[j].type=='(') count++;
			if (tokens[j].type==')') count--;
		}
		if (count) continue;

		if (pri>=priority(tokens[i].type)){
			pri=priority(tokens[i].type);
			pos=i;
		}
	}
	
    return pos;
}
uint32_t eval(int p,int q) {
	
  if (p > q) {
    panic("p>q,eval wrong");
  }
  else if (p == q) {
	if (tokens[p].type==NUM||tokens[p].type==SYMBOL){
		return (uint32_t)strtol(tokens[p].str,NULL,10);
	}
	else if (tokens[p].type==HEX){
		return (uint32_t)strtoul(tokens[p].str, NULL, 16);
	}
	else if (tokens[p].type==REG){
		if (strcmp(tokens[p].str, "$eax") == 0) return cpu.eax;
		else if (strcmp(tokens[p].str, "$ecx") == 0) return cpu.ecx;
		else if (strcmp(tokens[p].str, "$edx") == 0) return cpu.edx;
		else if (strcmp(tokens[p].str, "$ebx") == 0) return cpu.ebx;
		else if (strcmp(tokens[p].str, "$esp") == 0) return cpu.esp;
		else if (strcmp(tokens[p].str, "$ebp") == 0) return cpu.ebp;
		else if (strcmp(tokens[p].str, "$esi") == 0) return cpu.esi;
		else if (strcmp(tokens[p].str, "$edi") == 0) return cpu.edi;
		else if (strcmp(tokens[p].str, "$eip") == 0) return cpu.eip;
		else {
			panic("reg eval wrong");
			assert(0);
			return 0;
		}
	}
	panic("cannot access value");
    assert(0);
  }
  else if (checkparentheses(p, q) == true) {
    return eval(p + 1, q - 1);
  }
  else {
	int op = find_op(p,q);
	if (tokens[op].type==NOT||tokens[op].type==NEG||tokens[op].type==DEREF){
		uint32_t val=eval(op+1,q);
		switch (tokens[op].type){
			case NOT: return !val;
			case NEG: return -val;
			case DEREF: return swaddr_read(val, 4);
			default: assert(0);
		}
	}
	else {
		uint32_t val1 = eval(p, op - 1);
		uint32_t val2 = eval(op + 1, q);
		switch (tokens[op].type) {
			case '+': return val1 + val2;
			case '-': return val1 - val2;
			case '*': return val1 * val2;
			case '/': return val1 / val2;
			case EQ: return val1 ==val2;
			case NEQ: return val1!=val2;
			case AND: return val1&&val2;
			case OR:return val1||val2;

			default: panic("op eval wrong");
		}
	}
  }
  
}



uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert cotokensdes to evaluate the expression. */
	int i;
	for(i = 0; i < nr_token; i++){
  		if(tokens[i].type == '*' && (i == 0 || ((tokens[i-1].type !=NUM)&&(tokens[i-1].type!=')')))) {
    		tokens[i].type = DEREF;
  		}
		if (tokens[i].type == '-' &&
			(i == 0 || tokens[i-1].type == '+' || tokens[i-1].type == '-' ||
			tokens[i-1].type == '*' || tokens[i-1].type == '/' ||
			tokens[i-1].type == EQ || tokens[i-1].type == NEQ ||
			tokens[i-1].type == AND || tokens[i-1].type == OR ||
			tokens[i-1].type == NOT || tokens[i-1].type == '(' ||
			tokens[i-1].type == DEREF || tokens[i-1].type == NEG)) {
				
			tokens[i].type = NEG;
		}
	}
	uint32_t result;
	result=eval(0,nr_token-1);
	return result;
}



