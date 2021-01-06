#ifndef COMPARATOR_H
#define COMPARATOR_H

#define COMPARATOR_MIN 0
#define COMPARATOR_INT 0
#define COMPARATOR_NINT 1
#define COMPARATOR_UINT 2
#define COMPARATOR_NUINT 3
#define COMPARATOR_LONG 4
#define COMPARATOR_NLONG 5
#define COMPARATOR_ULONG 6
#define COMPARATOR_NULONG 7
#define COMPARATOR_FLOAT 8
#define COMPARATOR_NFLOAT 9
#define COMPARATOR_DOUBLE 10
#define COMPARATOR_NDOUBLE 11
#define COMPARATOR_CHAR 12
#define COMPARATOR_NCHAR 13
#define COMPARATOR_PTR 14
#define COMPARATOR_NPTR 15
#define COMPARATOR_STR 16
#define COMPARATOR_NSTR 17
#define COMPARATOR_MAX 17

struct comparator;

/*
comparator_init: Initialize the comparator whose comparison function is specified by func.
The values compared from two input pointers are the pointers at off bytes ahead.
*/
int comparator_init(struct comparator* c, int func, off_t off);

/*
comparator_comp: Compare values a and b using comparator c.
The integer returned has the same "sign" as *a - *b according to the comparator
*/
int comparator_comp(struct comparator* c, void* a, void* b);

#endif