#include <regex.h>
#include "bitmap.h"
#include "bool.h"

struct a_list{
	void* list; // ptr to the list
	size_t sz; // absolute number of elements
	size_t elm_sz; // size of each element
};

int a_list_init(struct a_list* ls, size_t init_sz, size_t elm_sz);
void a_list_deinit(struct a_list* ls);
void* a_list_add(struct a_list* list);
int a_list_del(struct a_list* list, idx_t i);
int a_list_del_last(struct a_list* list);

#define a_list_index(ls, i) ((void*)((char*)((ls)->list) + (ls)->elm_sz * (i)))
#define a_list_foreachi(ls, it, i) for ((it) = a_list_index(ls, (i) = 0), (i) < (ls)->sz, it = a_list_index(ls, ++(i))

#define A_LIST_UNION(t, n0, n1, n2, n3) \
	union{ \
		struct{ \
			t* n0; \
			size_t n1; \
			size_t n2; \
		}; \
		struct a_list n3; \
	}

struct op_tree_node{
	A_LIST_UNION(struct op_tree_node, ch, _, _, ls); // list of op_tree_nodes
	int op_id;
};

// returns a new spot for a child node to be constructed
struct op_tree_node* op_tree_insert(struct op_tree_node* par, struct op_tree_node* ch){
	// insert ch to par's list
	return a_list_add(&par->ls);
}




/*
_PCA__BASE + 1 through _PCO__BASE - 1 imply an arbitrary type
	Types like these encountered look ahead to the next element(s) to determine those type(s), which should be present afterwords
	Ex: The 'List' type (_PCS_LIST) is a list of an arbitrary type x:
		List(x) = x[,List(x)]
	In PC notation, a segment List(x) is written as {_PCS_LIST, _PCA__BASE + 1}, the latter implying an arbitrary type x
	When _PCS_LIST is encountered in a PC, when peeking down to its definition, an extra arbitrary type token (_PCA__BASE + 1) notating how
		many arugments to take (amount above _PCA__BASE) is found at the beginning of the sequence, meaning that _PCS_LIST in this case
		should have another PC id following it to determine this arbitrary type.
	This behavior can be nested. For example, a list of lists of ints would be called as {_PCS_LIST, _PCS_LIST, _PCS_INT}

Val = Regex([0-9]+)
Bop = +|-|*|/|%|^
Expr = Val|Expr Bop Expr
	// Expr has preference for Val as it is listed first
*/

/*
A = B C
B = D[ C]
*/

typedef unsigned short parse_class_t;

#define _PCA__BASE 255 // Arguments

enum{
	// PCOs // Operators
	_PCO__BASE = 512,
	_PCO_END = _PCA__BASE,
	_PCO_OR,
	_PCO_LB,
	_PCO_RB,
	// PCRs // Regexes
	_PCR__BASE,
	_PCR_SPACE = _PCR__BASE,
	_PCR_INT,
	_PCR_UINT,
	_PCR_FLOAT,
	// PCSs // Sequences
	_PCS__BASE,
	_PCS_INT = _PCS__BASE,
	_PCS_UINT,
	_PCS_NUM,
	_PCS_LIST,
	_PCS_BOP,
	_PCS_EXPR
};

const unsigned char* const _PCR[] = {
	[_PCR_SPACE - _PCR__BASE] = "[ \t\n]*",
	[_PCR_INT - _PCR__BASE] = "-?[0-9]+",
	[_PCR_UINT - _PCR__BASE] = "[0-9]+"
	[_PCR_FLOAT - _PCR__BASE] = "-?[0-9]*\.[0-9]*f?" // TODO: this matches ".", problem?
};

const parse_class_t* const _PCS[] = {
	[_PCS_INT - _PCS__BASE] = {_PCR_INT, _PCO_END},
	[_PCS_UINT - _PCS__BASE] = {_PCR_UINT, _PCO_END},
	[_PCS_NUM - _PCS__BASE] = {_PCR_INT, _PCO_OR, _PCR_UINT, _PCO_END},
	[_PCS_LIST - _PCS__BASE] = {_PCA__BASE + 1, _PCA__BASE + 1, _PCO_LB, ',', _PCS_LIST, _PCA__BASE + 1, _PCO_RB, _PCO_END},
	[_PCS_BOP - _PCS__BASE] = {'+', _PCO_OR, '-', _PCO_OR, '*', _PCO_OR, '/', _PCO_OR, '%', _PCO_OR, '^', _PCO_END},
	[_PCS_EXPR - _PCS__BASE] = {_PCS_NUM, _PCO_LB, _PCS_BOP, _PCS_EXPR, _PCO_RB, _PCO_END},
	[_PCS_FLOAT - _PCS__BASE] = {_PCR_FLOAT, _PCA_END}
};

/*
int pc_num_args(parse_class_t idx){
	parse_class_t* pc;
	new_bitmap_t(bm, 256);
	int ret = 0;
	for (pc = _PCS[pc], *pc != _PC_END; pc++){
		if (pc_is_arg(*pc)){
			if (bitmap_new_set(bm, *pc - _PCA_BASE)){
				ret++;
			}
		}
	}
	return ret;
}
*/

bool pc_parse_seq(unsigned char** ps, parse_class_t* par, parse_class_t** ppc);

int* pc_get_arg_lens(parse_class_t* pc, int* sum, bool do_malloc){
	int* ret = NULL;
	int num_args = *_PCS[*pc - _PCS__BASE];
	int pos = 0;
	if (_PCA__BASE < num_args && num_args < _PCO__BASE){
		if (do_malloc){
			ret = malloc((num_args - _PCA__BASE + 1) * sizeof(int));
			// assume success for now
		}
		ret[0] = 0;
		for (int i = 1; i < num_args + 1; i++){
			pc_get_arg_lens(pc + i, sum, FALSE); // get number of arguments
			pos += *sum + 1;
			if (do_malloc){
				ret[i] = pos;
			}
		}
	}
	*sum = pos;
	return ret;
}

parse_class_t* pc_advance_pc(parse_class_t* pc, bool stop_at_or){
	int bracket_level = 0;
	for (;;) {
		pc++;
		switch(*pc){
			case _PCO_OR:
				if (stop_at_or && bracket_level == 0){
					goto end;
				}
				break;
			case _PCO_LB:
				bracket_level++;
				break;
			case _PCO_RB:
				bracket_level--;
				if (bracket_level == -1){
					goto end;
				}
				break;
			case _PCO_END:
				goto end;
		}
	}
end:
	return pc;
}

bool pc_parse_regex(unsigned char** s, parse_class_t idx){ // idx must be a PCR
	regex_t reg;
	regmatch_t rm;
	bool ret = FALSE;
	if (!regex_comp(&reg, _PCR[idx - _PCR__BASE], REG_EXTENDED)){
		if (!regexec(&reg, *s, 0, &rm, 0)){
			*s += rm.rm_eo;
			ret = TRUE;
		}
		regex_free(&reg);
	}
	return ret;
}

bool pc_parse_type(unsigned char** s, parse_class_t* par){ // par must point to a PCS
	int* arg_lens = NULL; // list of (positions + lengths) past par of the arbitrary arguments
	int* sum;
	bool ret;
	arg_lens = pc_get_arg_lens(par, sum, TRUE);
	ret = pc_parse_seq(s, par, _PCS[*par - _PCS__BASE]);
	
	free(arg_offs);
	return ret;
}

bool pc_parse_seq(unsigned char** ps, parse_class_t* par, parse_class_t** ppc){
	parse_class_t* pc = *ppc;
	unsigned char* s = *ps;
	bool ret = TRUE;
	for (;;){ // if *s == 0, then it will either pass from _PCO_END/_PCO_RB or fail something and be handled by !ret
		if (!pc_parse_regex(&s, _PCS_SPACE)){ // should never fail, really (except for malloc failure)
			goto fail;
		}
		
		if (*pc < _PCA__BASE){ // literal
			if (*s != *pc){ // compare char in string to lower byte of parse_class_t variable
				ret = FALSE;
			}
			s++;
		}
		else if (*pc < _PCO__BASE){ // argument
			ret = pc_parse_type(&s, par + arg_lens[*pc - _PCO_BASE - 1]); // TODO: is this guaranteed to be a PCS?
		}
		else if (*pc < _PCR__BASE){ // operation
			switch(*pc){
				case _PCO_END:
				case _PCO_RB:
					goto pass;
				case _PCO_OR:
					pc = pc_advance_pc(pc, FALSE); // short circuit this OR statement and continue
					break;
				case _PCO_LB:
					ret = pc_parse_seq(&s, par, &pc);
					break;
				default:
					goto fail;
			}
		}
		else if (*pc < _PCS__BASE){ // regex
			s = pc_parse_regex(s, *pc);
		}
		else{ // sequence
			ret = pc_parse_type(&s, pc);
		}
		
		if (!ret){
			pc = pc_advance_pc(pc, TRUE);
			if (*pc == _PCO_OR){ // OR condition found; rewind s and try this one
				s = *ps;
			}
			else{
				goto fail;
			}
		}
		pc++;
	}
	goto pass;
fail:
	ret = FALSE;
	s = *ps;
pass:
	*ppc = pc;
	*ps = s;
	return ret;
}

void pc_parse_func(unsigned char* s){
	// try Expr first
}





#define A_LIST_INIT_SZ 4

int a_list_init(struct a_list* ls, size_t init_sz, size_t elm_sz){
	ls->elm_sz = elm_sz;
	ls->cap = 0;
	if (init_sz == 0)
		ls->sz = A_LIST_INIT_SZ;
	else
		ls->sz = pow2_roundup(init_sz);
	if ((ls->list = malloc(ls->sz * elm_sz)) == NULL){
		return -ENOMEM;
	}
	return 0;
}

void a_list_deinit(struct a_list* ls){
	void* it;
	freec(ls->list);
}

void* a_list_add(struct a_list* list){
	if (!(list->sz & (list->sz - 1))){
		realloc(list->list, (list->sz << 1) * list->elm_sz);
		if (list->list == NULL){
			return ERR_PTR(-ENOMEM);
		}
	}
	list->sz++;
	return (void*)(a_list_index(list, list->sz - 1));
}

int a_list_del(struct a_list* list, idx_t i){
	if (list->sz <= 0){
		return -E_EMPTY;
	}
	memcpy(a_list_index(list, i), a_list_index(list, list->sz - 1), list->elm_sz);
	list->sz--;
	return 0;
}

int a_list_del_last(struct a_list* list){
	if (list->sz <= 0){
		return -E_EMPTY;
	}
	//memset(a_list_index(list, list->cap - 1), 0, list->elm_sz);
	list->sz--;
	return 0;
}

int a_list_trim(struct a_list* list){
	realloc(list->list, list->sz * list->elm_sz);
	return 0;
}
