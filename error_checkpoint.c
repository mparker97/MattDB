#include <setjmp.h>
#include "errors.h"

int checkpoint_stack = 0;
jmp_buf checkpoints[MAX_CHECKPOINTS];
