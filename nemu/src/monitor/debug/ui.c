#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_help(char *args);

static int cmd_info(char *args) {
	if (*args=='r'){
		int i;
		for (i=0;i<8;i++){

			switch (i){
				case 0:
				printf("eax %x",cpu.gpr[0]._32);
				break;
				case 1:
				printf("ecx %x",cpu.gpr[1]._32);
				break;
				case 2:
				printf("edx %x",cpu.gpr[2]._32);
				break;
				case 3:
				printf("ebx %x",cpu.gpr[3]._32);
				break;
				case 4:
				printf("esp %x",cpu.gpr[4]._32);
				break;
				case 5:
				printf("ebp %x",cpu.gpr[5]._32);
				break;
				case 6:
				printf("esi %x",cpu.gpr[6]._32);
				break;
				case 7:
				printf("edi %x",cpu.gpr[7]._32);
				break;
			}
		printf("\n");
		}
	}
	
	return 0;
}
static int cmd_step(char *args){
	int N;
	sscanf(args,"%d",&N);
	cpu_exec(N);
	return 0;
}
static int cmd_x(char *args) {
    int count;
    uint32_t addr;
    int len = 4; // 默认 4 字节

    // 解析参数（支持可选的长度参数）
    if (sscanf(args, "%d %x %d", &count, &addr, &len) < 2) {
        printf("Usage: x <count> <addr> [len=1|2|4]\n");
        return -1;
    }

    // 检查长度合法性
    if (len != 1 && len != 2 && len != 4) {
        printf("Invalid length: %d. Use 1, 2, or 4.\n", len);
        return -1;
    }
	int i;
    // 逐个读取并打印内存值
    for (i = 0; i < count; i++) {
        uint32_t value = swaddr_read(addr + i * len, len);
        printf("0x%08x: 0x%0*x\n", addr + i * len, len * 2, value);
    }

    return 0;
}
static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },

	/* TODO: Add more commands */
	{"si", "step N instruction", cmd_step},
	{"info", "Print register state", cmd_info},
	{"x", "examine memory", cmd_x},


};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
