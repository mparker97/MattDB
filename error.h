/*
This is a checkpointing system.
The source file "error_checkpoint.c" establishes an array of MAX_CHECKPOINTS checkpoints.
Checkpoints are made with the fail_checkpoint() function:
	0 is returned when the checkpoint is first made.
	When an error occurs, use the fail_out() macro, passing the relevant error constant.
		The control flow then jumps to the most recent fail_checkpoint() call, returning this time the supplied error constant.
The most recent checkpoint is taken down with the fail_uncheckpoint() function.
	When a checkpointed section of code is completed successfully, use this to close off the checkpoint.
	fail_uncheckpoint() should be called after returning to a checkpoint via fail_out(), too.
*/

#ifndef ERROR_H
#define ERROR_H
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
//#include "globals.h"

#define ERR_PTR(x) ((void*)(x))
#define PTR_ERR(x) ((int)(x))

#define PERR(x) (PTR_ERR(x) < 0) // TODO: FIX

#define ERROR_NAME_LEN 7 // length of error names (without \0)
enum{
	EINVAL=1,	// Invalid Arguments
	EDIVZ,		// Divide by Zero
	ENULPTR,	// Null Pointer
	EUNINIT,	// Uninitialized
	ERANGE,		// Out of Range
	ENOMEM,		// Out of Memory
	EEMPTY,		// Empty
	ESYNTAX,	// Syntax error
	ENOSOL,		// No solution
	EPANIC,		// System panic; should never happen
	ECREAT,		// Creat failed
	// DB specific
	DBEBASE // DB specific start
	ENOTAB=DBEBASE, // Table doesn't exist
	ENOFLD,		// Field doesn't exist
	EPRKEY,		// Primary key
	ENOTSMD,	// Tables metadata file missing
	ENOTMD,		// Table metadata file missing
	ENOTFIL,	// Table file missing
	ENODB,		// Database not chosen
	
	NUME // End
};

const static unsigned char global_errors[NUME][ERROR_NAME_LEN + 1] = {
	[EINVAL ] = "EINVAL ",
	[EDIVZ  ] = "EDIVZ  ",
	[ENULPTR] = "ENULPTR",
	[EUNINIT] = "EUNINIT",
	[ERANGE ] = "ERANGE ",
	[ENOMEM ] = "ENOMEM ",
	[EEMPTY ] = "EEMPTY ",
	[ESYNTAX] = "ESYNTAX",
	[ENOSOL ] = "ENOSOL ",
	[EPANIC ] = "EPANIC ",
	[ECREAT ] = "ECREAT ",
	// DB specific
	[ENOTAB ] = "ENOTAB ",
	[ENOFLD ] = "ENOFLD ",	
	[EPRKEY ] = "EPRKEY ",	
	[ENOTSMD] = "ENOTSMD",
	[ENOTMD ] = "ENOTMD ",	
	[ENOTFIL] = "ENOTFIL",
	[ENODB  ] = "ENODB  "
	
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

static void print_error(const unsigned char* m, const char* file, int line){
	fprintf(stderr, "FAIL_OUT ENCOUNTERED ERROR %s (%s:%d)\n", m, file, line);
}

#define MAX_CHECKPOINTS 10
extern int checkpoint_stack;
extern jmp_buf checkpoints[MAX_CHECKPOINTS];
static int fail_checkpoint(){
	if (++checkpoint_stack == MAX_CHECKPOINTS){
		fprintf(stderr, "Checkpoint stack full\n");
		exit(1);
	}
	return setjmp(checkpoints[checkpoint_stack]);
}

static inline void fail_uncheckpoint(){
	checkpoint_stack--;
}

#ifdef _DEBUG
#define do_fail_out(e, m) do {print_error(m, __FILE__, __LINE__); longjmp(checkpoints[checkpoint_stack], e);} while (0)
#else
#define do_fail_out(e, m) longjmp(checkpoints[checkpoint_stack], e)
#endif

#define fail_out(e) do_fail_out(e, global_errors[e])

#pragma GCC diagnostic pop

#endif
