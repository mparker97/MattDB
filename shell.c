#include <stdlin.h>
#include "common.h"
#include "list.h"
#include "op_tree.h"

#define LINE_BUF_SZ 4096

char line_buf[LINE_BUF_SZ];
int op_stack = 0;

struct op_tree_node* op_tree_root;

inline bool op_strtok_delim(char c){
	return c == ' ' || c == ',' || c == '\t' || c == '\n';
}

/*char* op_strtok(char* str){
	static char* i;
	char* s;
	bool quote = false;
	if (str != NULL){
		s = str;
	}
	else{
		s = i;
	}
	for (; op_strtok_delim(*s); s++);
	if (*s == 0){
		return NULL;
	}
	for (i = s; *i != 0; i++){ // loop through chars
		switch (*i){
			case '"':
				if (quote){ // ending quote
					if (i[-1] != '\\'){ // not an escaped quote
						quote = false;
					}
				}
				else{ // beginning quote
					quote = true;
				}
				continue;
			case '(': // beginning parenthesis
				if (!quote){ // not in quote
					op_stack++;
				}
				break;
			case ')': // ending parenthesis
				if (!quote){ // not in quote
					op_stack--;
					if (op_stack < 0){
						// syntax error
					}
				}
				break;
			default:
				if (quote || !op_strtok_delim(*i)){ // open quote or not end of token
					continue;
				}
		}
		// end of token
		*i = 0;
		i++;
		break;	
	}
	if (quote || op_stack != 0){ // open quote or parenthesis
		// syntax error
	}
	return s;
}

void func(){
	char* c;
	char* op_ptr = op_strtok(line_buf);
	for (; op_ptr != NULL; op_ptr = op_strtok(NULL)){
		if (*op_ptr != 0){ // TODO: empty node for op_stack changes?
			
		}
	}
}*/

struct str_seg{
	char* start;
	char* stop;
}

char* c, *arg = NULL;
int n = strlen(input);
struct a_list al;
struct str_seg* new;
a_list_init(&al, sizeof(struct str_seg), 4);
for (c = input; c < input + n; c++){
	if (*c == '\"'){
		if (arg){
			if (c[-1] != '\\'){
				new = a_list_add(&al);
				if (!new){
					// error
				}
				new->start = arg;
				new->stop = c - 1;
				arg = NULL;
			}
		}
		else{
			arg = c + 1;
		}
	}
}

void parse_recurse(struct op_tree_node* n, char* addr, char* end){
	struct op_tree_node* new;
	char* c;
	for (c = addr; c < end; c++){
		if (*c == '('){
			if ((new = op_tree_add_node_at(n, c)) == NULL){
				// error
			}
			parse_recurse(new, c + 1, end);
		}
		else if (*c == ')'){
			return;
		}
		else{
			strtok(c, " \t");
			
		}
	}
}

a_list_deinit(al);
