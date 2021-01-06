#ifndef OP_TREE_H
#define OP_TREE_H
#include "common.h"
#include "parse_class.h"
#include "list.h"

/*
Ops:
	SELECT
	FROM
	WHERE
	ORDER_BY
	GROUP_BY
	INSERT_INTO
	UPDATE
	SET
Funcs:
	COUNT()
	SUM()
	FIRST()
	LAST()
	AVERAGE()
	STDDEV()
	UNIQUE()
	DUPLICATE()
	

Func = 
AggFunc = COUNT|SUM|FIRST|LAST|AVERAGE|STDDEV|UNIQUE|DUPLICATE

All can have balanced parenthesis (with whitespaces) around them

  (Space) = Regex([ \t\n]+)
Parens(x) = (x)

PosInt = Regex([0-9]+)
Int = [-]PosInt
Float = Int[.[PosInt]][f]
Bool = TRUE|FALSE
Item = Regex(".*")|Regex(\S+)
Name = Regex([a-z0-9_]+)
NameItem = "Regex([a-z0-9_ ])"
CondOp = =|<>|>|<|>=|<=
UBoolOp = NOT
BBoolOp = OR|AND|XOR|NOR|NAND|XNOR
UArithOp = -
BArithOp = +|-|*|/|^|%
UOp = UArithOp|UBoolOp
Bop = BArithOp|BBoolOp
OpItem = [UOp]Item
List(x) = x[,[ ]List(x)]
Expr = Func(List(Expr))|OpItem[ Bop Expr]
Cond = Expr CondOp Expr[, BBoolOp Cond]
Table = Item:Table
Field = Item:Field[.Table]
SelectItem = {Field|AggFunc(Field)}[ AS Item]

Select = Table | SELECT List(SelectItem) FROM Select[ WHERE List(Cond)][ GROUP BY List(Item)][ ORDER BY List(Item)[ ASC | DESC]]

*/

struct op_tree_node{
	A_LIST_UNION(struct op_tree_node, ch, _, _, ls); // list of op_tree_nodes
	int op_id; // id of the operation // TODO: parentheses?
};

/*
struct op_tree_node{
	struct l_list list; // last pointer is tagged and points to the parent
	size_t list_len;
	union{
		char* op;
		void* (*func)(struct arg_list); // TODO: might scrap above
		off_t val;
	};
	struct op_tree_node* child;
};

int op_tree_init(struct op_tree_node* n);
void op_tree_deinit(struct op_tree_node* n);

struct op_tree_node* op_tree_node_parent(struct op_tree_node* n){
	l_list_foreach(&n->list, struct op_tree_node, list, n){
		if (tagged_ptr(n->list)){
			return container_of(struct op_tree_node, list, untag_ptr(n->list));
		}
	}
	return ERR_PTR(-EPANIC); // should never happen
}

struct op_tree_node* op_tree_node_head(struct op_tree_node* n){
	struct op_tree_node* p = op_tree_node_parent(n);
	if (p != NULL){
		p = p->child;
	}
	return p;
}

void* op_tree_eval(struct op_tree_node* root, void* base){ // TODO: need base?
	struct op_tree_node* n;
	struct arg_list* args;
	void* ret;
	int i;
	n = root->child;
	if (n == NULL){
		return &root->val;
	}
	args = malloc(sizeof(struct arg_list) + root->list_len * sizeof(void*));
	if (args == NULL){
		printf("Malloc failed\n");
		return ERR_PTR(-ENOMEM); // TODO
	}
	i = 0;
	l_list_foreach(&n->list, struct op_tree_node, list, n){
		args->args[i] = op_tree_eval(n, base); // TODO: error returned?
		i++;
		if (tagged_ptr(n->list)){
			break;
		}
	}
	ret = root->func(args);
	free(args);
	return ret;
}

#endif
*/