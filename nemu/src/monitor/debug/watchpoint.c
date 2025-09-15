#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp(){
	WP* spare=NULL;
	if (free_==NULL) assert(0);
	else {
		wp->next = head;
		head = wp;
		spare=free_;
		free_=free_->next;
	}

	return WP*

}
void free_wp(WP* wp){
	if (wp->NO==0||wp->NO==1) head=NULL;
	else {
		head=wp_pool[wp->NO-2].next;
	}
	free_=wp;
}

