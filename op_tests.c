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
	int op_id; // id of the operation // TODO: parentheses?
};

// returns a new spot for a child node to be constructed
struct op_tree_node* op_tree_insert(struct op_tree_node* par){
	// insert ch to par's list
	return a_list_add(&par->ls);
}

void op_tree_delete(struct op_tree_node* par){
	
}




/*
PCA__BASE + 1 through PCO__BASE - 1 imply an arbitrary type
	Types like these encountered look ahead to the next element(s) to determine those type(s), which should be present afterwords.
	Ex: The 'List' type (PCS_LIST) is a list of an arbitrary type x:
		List(x) = x[,List(x)]
	In PC notation, a segment List(x) is written as {PCS_LIST, PCA__BASE + 1}, the latter implying an arbitrary type x.
	When PCS_LIST is encountered in a PC, when peeking down to its definition, an extra arbitrary type token (PCA__BASE + 1) notating how
		many arugments to take (amount above PCA__BASE) is found at the beginning of the sequence, meaning that PCS_LIST in this case
		should have another PC id following it to determine this arbitrary type.
	This behavior can be nested. For example, a list of lists of ints would be called as {PCS_LIST, PCS_LIST, PCS_INT}

As an example, the list PCS is broken down here:
	Imagine, for example, the parent phrase segment this came from is {List, Int}.
	(PCS_LIST) = {PCA__BASE + 1, PCA__BASE + 1, PCO_LB, ',', PCS_LIST, PCA__BASE + 1, PCO_RB, PCO_END}
		PCA__BASE + 1 // This implies there must be (PCA__BASE + 1) - PCA__BASE = 1 argument supplied from the parent when using this PCS
			When going down and resolving 'List' in the example parent phrase, this parent phrase is
				looked up and the next item is taken as the required argument, in this case 'Int'.
		PCA__BASE + 1 // This is substituted with the argument found from the parent in the previous step.
			// In the example, this would be resolved to 'Int'.
		PCO_LB // A left bracket, implying the start of an optional (greedy) phrase.
		',' // A comma
		PCS_LIST // A list PCS, to be resolved through recursion.
		PCA__BASE + 1 // As before, substituted with the argument found from the parent in the previous step.
			// In the example, this is yet again 'Int'. This argument is then used in the same manner as 'Int' in the parent when resolving
				The immediate previous PCS_LIST in this sequence through recursion, thus reiterating the process to the parent's grandchild.
		PCO_RB // A right bracket, implying the close of the previously opened optional phrase.
		PCO_END // Terminates the PCS.
	In a more readable format, this is List(x) = x[,List(x)]
			

List(x) = x[, List(x)]
NList(x, UInt) = x, NList(x, n - 1)

*/

/*
A = B C
B = D[ C]
*/

typedef unsigned short parse_class_t;

/*
struct arg_idx{
	unsigned short stack_idx; // index of the stack frame where this argument can be resolved
	unsigned short stack_off; // offset into the array of that stack frame's PCS where this argument is
};
*/

struct pcs_arg_idx{
	parse_class_t pcs; // the PCS in this stack frame
	int stack_off;
	//arg_idx* args; // the arg_idx list obtained through pc_get_arg_idx for this PCS item
	int* args; // the arg_idx list obtained through pc_get_arg_idx for this PCS item
	// For a PCS with arguments (quantity notated at the beginning), arg_idx gives where to resolve them
};

struct parser{
	struct a_list stack; // a_list of pcs_arg_idx structs to represent the stack
};

#define PCA__BASE 256 // Arguments

enum{
	// 256-511 // PCAs
	// PCOs // Operators
	PCO__BASE = 512,
	PCO_END = PCO__BASE,
	PCO_OR,
	PCO_LB,
	PCO_RB,
	// PCRs // Regexes
	PCR__BASE,
	PCR_SPACE = PCR__BASE,
	PCR_INT,
	PCR_UINT,
	PCR_FLOAT,
	// PCSs // Sequences
	PCS__BASE,
	PCS_INT = PCS__BASE,
	PCS_UINT,
	PCS_NUM,
	PCS_LIST,
	PCS_BOP,
	PCS_EXPR,
	
};

// Regex patterns
const unsigned char* const PCR[] = {
	[PCR_SPACE - PCR__BASE] = "[ \t\n]*",
	[PCR_INT - PCR__BASE] = "-?[0-9]+",
	[PCR_UINT - PCR__BASE] = "[0-9]+"
	[PCR_FLOAT - PCR__BASE] = "-?[0-9]*\.[0-9]*f?" // TODO: this matches ".", problem?
};

#define PCR__index(x) PCR[(x) - PCR__BASE]

// "Types" to be found while parsing
const parse_class_t* const PCS[] = {
	[PCS_INT - PCS__BASE] = {PCR_INT, PCO_END}, // regex("-?[0-9]+")
	[PCS_UINT - PCS__BASE] = {PCR_UINT, PCO_END}, // regex("[0-9]+")
	[PCS_NUM - PCS__BASE] = {PCR_INT, PCO_OR, PCR_UINT, PCO_END}, // INT | UINT
	[PCS_LIST - PCS__BASE] = {PCA__BASE + 1, PCA__BASE + 1, PCO_LB, ',', PCS_LIST, PCA__BASE + 1, PCO_RB, PCO_END}, // x[, List(x)]
	[PCS_BOP - PCS__BASE] = {'+', PCO_OR, '-', PCO_OR, '*', PCO_OR, '/', PCO_OR, '%', PCO_OR, '^', PCO_END}, // +|-|*|/|%|^
	[PCS_EXPR - PCS__BASE] = {PCS_NUM, PCO_LB, PCS_BOP, PCS_EXPR, PCO_RB, PCO_END}, // NUM[ BOP EXPR]
	[PCS_FLOAT - PCS__BASE] = {PCR_FLOAT, PCA_END} // regex("-?[0-9]*\.[0-9]*f?")
};

#define PCS__index(x) PCS[(x) - PCS__BASE]

/*
int pc_num_args(parse_class_t idx){
	parse_class_t* pc;
	new_bitmap_t(bm, 256);
	int ret = 0;
	for (pc = PCS[pc], *pc != _PC_END; pc++){
		if (pc_is_arg(*pc)){
			if (bitmap_new_set(bm, *pc - PCA_BASE)){
				ret++;
			}
		}
	}
	return ret;
}
*/

bool pc_parse_seq(unsigned char** ps, parse_class_t* par, parse_class_t** ppc);

// Returns an array that specifies at which index past the parent pc each argument is found
// Use len = NULL
int* pc_get_arg_idx(struct parser* p, parse_class_t* pc, int* len){
	int* ret = NULL;
	int num_args;
	int l;
	num_args = PCS__index(*pc)[0] - PCA__BASE; // The number of arguments to fetch from the parent, as given by the first item in the PCS
	if (0 < num_args && num_args < PCO__BASE - PCA__BASE){ // if there are arguments
		if (!len){ // create array if first caller (not in recursion)
			ret = malloc(num_args * sizeof(int)); // array for position indices of each argument
			// assume success for now, but TODO
			ret[i] = 1;
		}
		// There may be phrases with a more complex nested argument structure, so that the arguments aren't lined up one after the other
			// For example, if the first argument itself has an argument, that pushes the second argument over a position:
				// f(g(x), y) --> f g x y
				//                  ^0  ^1 is pushed to position 3
		for (int i = 1; i < num_args; i++){
			l = 1;
			pc_get_arg_idx(p, pc + i, &l); // get total argument length
			if (!len){
				ret[i] = ret[i - 1] + l; // fill next spot in index array // TODO
			}
			else{
				len += l; // build up total argument count
			}
		}
	}
	return ret;
}

void parser_init(struct parser* p){
	a_list_init(&p->stack, A_LIST_DEFAULT_INIT_SZ, sizeof(struct pcs_arg_idx));
}

// pc is a pointer to the parent (an item in a PCS about to be recursed into; it's supplied arguments are immediately following it)
void parser_push(struct parser* p, parse_class_t* pc, int stack_frame, int stack_off){
	struct pcs_arg_idx* pai = a_list_add(&p->stack);
	if (pai){
		pai->pcs = PCS__index(*pc);
		pai->stack_off = stack_off;
		pai->arg_idx = pc_get_arg_idx(p, pc, NULL);
	}
	else{
		// TODO: a_list_add fail
	}
}

void parser_pop(struct parser* p){
	struct pcs_arg_idx* pai = a_list_del_last(&p->stack);
}

void parser_deinit(struct parser* p){
	a_list_deinit(&p->stack);
}

parse_class_t* pc_advance_pc(parse_class_t* pc, bool stop_at_or){
	int bracket_level = 0;
	for (;;) {
		pc++;
		switch(*pc){
			case PCO_OR:
				if (stop_at_or && bracket_level == 0){
					goto end;
				}
				break;
			case PCO_LB:
				bracket_level++;
				break;
			case PCO_RB:
				bracket_level--;
				if (bracket_level == -1){
					goto end;
				}
				break;
			case PCO_END:
				goto end;
		}
	}
end:
	return pc;
}

// Parses a regex expression in *'s' identified in PCR by 'idx'
bool pc_parse_regex(unsigned char** s, parse_class_t idx){ // idx must be a PCR
	regex_t reg;
	regmatch_t rm;
	bool ret = FALSE;
	if (!regex_comp(&reg, PCR[idx - PCR__BASE], REG_EXTENDED)){
		if (!regexec(&reg, *s, 0, &rm, 0)){
			*s += rm.rm_eo;
			ret = TRUE;
		}
		regex_free(&reg);
	}
	return ret;
}

//bool pc_parse_seq_new_op(unsigned char** ps, parse_class_t* par, parse_class_t** ppc, struct

// Parses the string pointed to by 's' according to the PCS 'par'
unsigned char* pc_parse_type(unsigned char* s, parse_class_t* par){ // par must point to a PCS
	int* arg_lens = NULL; // list of (positions + lengths) past par of the arbitrary arguments
	int* sum;
	bool ret;
	arg_lens = pc_get_arg_lens(par, sum, TRUE);
	/*struct op_tree_node* new_op = malloc(sizeof(struct op_tree_node));
	if (!new_op){
		return FALSE;
	}
	op->*/
	ret = pc_parse_seq(s, par, PCS[*par - PCS__BASE]);
	
	free(arg_offs);
	return ret;
}

// TODO: op_trees; consecutive literals form an operator
/*
ppc: A pointer to 
*/
unsigned char* pc_parse_seq(struct parser* p, unsigned char* s, int stack_frame, int stack_off){//, struct op_tree_node* op){
	// pc is a pointer to the item of a PCS that is the parent of this call
		// For example, if the overall phrase is f(g(x)) and this call is parsing g(x), pc would point to the 'g' item in the 'f' PCS array
	parse_class_t* pc = &PCS__index(a_list_index(p->stack, stack_frame)->pcs)[stack_off];
	unsigned char* ret = NULL;
	parser_push(p, pc, stack_frame, stack_off); // TODO: move?
	for (;;){ // if *s == 0, then it will either pass from PCO_END/PCO_RB or fail something and be handled by !ret
		// trim spaces
		if (!pc_parse_regex(&s, PCS_SPACE)){ // should never fail, really (except for malloc failure)
			goto fail;
		}
		
		if (*pc < PCA__BASE){ // literal
			if (*s != *pc){ // compare char in string to lower byte of parse_class_t variable
				ret = NULL;
			}
			s++;
		}
		else if (*pc < PCO__BASE){ // argument
			ret = pc_parse_arg(p, s, pc);
			//ret = pc_parse_type(s, par + arg_idx[*pc - PCA_BASE]); // TODO: what if this is an argument in the parent?
		}
		else if (*pc < PCR__BASE){ // operation
			switch(*pc){
				case PCO_END:
				case PCO_RB:
					goto pass;
				case PCO_OR:
					pc = pc_advance_pc(pc, FALSE); // short circuit this OR statement and continue
					break;
				case PCO_LB:
					ret = pc_parse_seq(p, s, par); // another call here so I can rewind if it fails
					break;
				default:
					goto fail;
			}
		}
		else if (*pc < PCS__BASE){ // regex
			s = pc_parse_regex(s, *pc);
		}
		else{ // sequence
			ret = pc_parse_type(&s, pc);
		}
		
		if (!ret){
			pc = pc_advance_pc(pc, TRUE);
			if (*pc == PCO_OR){ // OR condition found; rewind s and try this one
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
