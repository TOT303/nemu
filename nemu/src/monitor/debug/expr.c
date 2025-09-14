#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ, NUM
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
	{"==", EQ}				// equal	
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
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
					case (NOTYPE):
					break;
					case (EQ):
					tokens->type[i]='=';break;
					case ('+'):
					tokens->type[i]='+';break;
					case ('-'):
					tokens->type[i]='-';break;
					case ('*'):
					tokens->type[i]='*';break;
					case ('/'):
					tokens->type[i]='/';break;
					case (NUM):
					tokens->type[i]=NUM;
					tokens->str[i]=e[i];break;
					case ('('):
					tokens->type[i]='(';break;
					case (')'):
					tokens->type[i]=')';break;
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
bool checkparentheses(p, q) {
	if (tokens->type[p]!='('||tokens->type[q]!=')'){
		return false;
	}
	int i=p;
	int j=q;
	bool find_l=0;
	bool find_r=0;
	for (i;i<q-1;i++){
		if (tokens->type[i]=='('){
			find_l=1;
		}
		for (j;j>i;j--){
			if (tokens->type[j]==')'){
				find_r=1;
				break;
			}
			
		}
		if (find_l^find_r==0){
			find_l=find_r=0;
			continue;
		}
		else {
			return false;
			}
	}
	return true;
}
int find_op(p,q){

}

uint32_t eval(p, q) {
  if (p > q) {
    panic("eval wrong");
  }
  else if (p == q) {
    /* Single token.
       For now this token should be a number.
       Return the value of the number. */
    return stoi(tokens->str[p]);
  }
  else if (checkparentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
       If that is the case, just throw away the parentheses. */
    return eval(p + 1, q - 1);
  }
  else {
    op = tokens->type[p+1];
    val1 = eval(p, op - 1);
    val2 = eval(op + 1, q);
    switch (op_type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      default: assert(0);
    }
  }
}
uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	for(i = e; i < nr_token; i++){
  		if(tokens[i].type == '*' && (i == 0 || tokens[i-1].type certaintype)) {
    		tokens[i].type = DEREF;
    		return eval(?, ?);
  		}	
	}
	panic("please implement me");
	return 0;
}



