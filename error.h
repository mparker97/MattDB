#ifnder ERROR_H
#define ERROR_H

#define EINVAL	// Invalid Arguments
#define EDIVZ	// Divide by Zero
#define ENULPTR	// Null Pointer
#define EUNINIT	// Uninitialized
#define ERANGE	// Out of Range
#define ENOMEM	// Out of Memory
#define EEMPTY	// Empty
#define ESYNTAX // Syntax error
#define ENOSOL	// No solution
#define EPANIC	// System panic; should never happen

// DB spec
#define ENOTAB	// Table doesn't exist
#define ENOFLD	// Field doesn't exist

#define ERR_PTR(x) ((void*)(x))
#define PTR_ERR(x) ((int)(x))

#define PERR(x) (PTR_ERR(x) < 0) // TODO: FIX

#endif