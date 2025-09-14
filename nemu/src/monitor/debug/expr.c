#include "nemu.h"
#include <stdlib.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ,NEQ, NUM,AND,OR,NOT,HEX,REG,DEREF,NEG
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
	{"==", EQ},				// equal
	{"!=",NEQ},
	{"&&",AND},
	{"||",OR},
	{"\\!",NOT}
	{"0[Xx][0-9]+",HEX},
	{"\\$[a-z]+",REG},	
	{"\\+", '+'},			// plus				
	{"\\-",'-'},
	{"\\*",'*'},
	{"\\/",'/'},
	{"[0-9]+",NUM},
	{"\\(",'('},
	{"\\)",')'}
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

					default: panic("please implement me");
				}
				break;                    
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}


static bool checkparentheses(int p, int q) {
    if (tokens[p].type != '(' || tokens[q].type != ')') return false;
    int bal = 0;
    for (int i = p; i <= q; i++) {
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
static int find_op(int p,int q){
	int pos=-1;
	int i;
	int pri=INT32_MAX;
	for (i=p;i<=q;i++){
		if (priority(tokens[i].type)==-1||
		    tokens[i].type==NOT||
		    tokens[i].type==NEG||
		    tokens[i].type==DEREF){
			continue;
		}
		int j=0;
		int k=p;
		for (k;k<i;k++){
			if (tokens[k].type=='(') j++;
			if (tokens[k].type==')') j--;
		}
		if (j!=0) continue;
		if (pri>=priority(tokens[i].type)) {
			pri=priority(tokens[i].type);
			pos=i;
		}
	}
	return pos;
}

uint32_t eval(int p,int q) {
  if (p > q) {
    panic("eval wrong");
  }
  else if (p == q) {
	if (tokens[p].type==NUM){
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
		else {
			panic("eval wrong");
			assert(0);
			return 0;
		}
	}
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
			case NOT:return !val;
			case NEG:return -val;
			case DEREF:return *(uint32_t *)val;
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
			default: assert(0);
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

	for(i = 0; i < nr_token; i++){
  		if(tokens[i].type == '*' && (i == 0 || tokens[i-1].type !=NUM)) {
    		tokens[i].type = DEREF;
  		}
		if (i==0&&tokens[i].type=='-')	{
			tokens[i].type=NEG;
		}
		else if ((tokens[i].type=='-')&&((tokens[i-1].type=='+')||(tokens[i-1].type=='-')||(tokens[i-1].type=='*')||(tokens[i-1].type=='/'))){
			tokens[i].type=NEG;
		}
	}
	uint32_t result;
	result=eval(0,nr_token-1);
	return result;
}



