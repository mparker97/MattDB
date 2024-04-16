#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include "error.h"
#define PG_SZ 4096
#define BITS_PER_BYTE 8

typedef unsigned long off_t;
typedef long idx_t;
typedef uint64_t size_t;
typedef uint64_t id_t;

extern FILE* tables_f;
extern unsigned char* db_path;
extern int dp_path_len;

#define TABLE_NAME_SZ 64

#define offset_of(typ, memb) ((off_t)&((typ*)NULL)->(memb))
#define container_of(typ, memb, ptr) ((typ*)((char*)(ptr) - offset_of(typ, memb)))

/* f[fl][sz][il]:
	Find [f]irst/[l]ast [s]igned/[z]ero bit in an [i]nteger (32-bit) / [l]ong (64-bit).
*/

static inline int ffsi(uint32_t x){
	int s;
	if (x == 0){
		return -ENOSOL;
	}
	for (s = 0; (x & 1) != 0; x >>= 1, s++);
	return s;
}
#define ffzi(x) ffsi(~x)

static inline int flsi(uint32_t x){
	int s;
	if (x == 0){
		return -ENOSOL;
	}
	for (s = BITS_PER_BYTE * sizeof(uint32_t) - 1; (x & (BITS_PER_BYTE * sizeof(uint32_t) - 1)) != 0; x <<= 1, s--);
	return s;
}
#define flzi(x) flsi(~x)

static inline int ffsl(uint64_t x){
	int s;
	if (x == 0){
		return -ENOSOL;
	}
	for (s = 0; (x & 1) != 0; x >>= 1, s++);
	return s;
}
#define ffzl(x) ffsl(~x)

static inline int flsl(uint64_t x){
	int s;
	if (x == 0){
		return -ENOSOL;
	}
	for (s = BITS_PER_BYTE * sizeof(uint64_t) - 1; (x & (BITS_PER_BYTE * sizeof(uint64_t) - 1)) != 0; x <<= 1, s--);
	return s;
}
#define flzl(x) flsl(~x)

static ssize_t ffsbm(uint64_t* bm, ssize_t sz){
	ssize_t s, i;
	for (s = -1, i = 0; i < sz; i += BITS_PER_BYTE * sizeof(ssize_t), bm++){
		if (*bm != 0){
			s = i + ffsl(*bm);
			if (s > sz){
				s = -1;
			}
			break;
		}
	}
	return s;
}

static ssize_t ffzbm(uint64_t* bm, ssize_t sz){
	ssize_t s, i;
	for (s = -1, i = 0; i < sz; i += BITS_PER_BYTE * sizeof(ssize_t), bm++){
		if (*bm != ~(uint64_t)0){
			s = i + ffsz(*bm);
			if (s > sz){
				s = -1;
			}
			break;
		}
	}
	return s;
}

// roundup_pow2: Round 'x' up to the nearest multiple of 'p', where 'p' is a power of 2.
#define roundup_pow2(x, p) (((x) + (p) - 1) & ~((p) - 1))
// pg_roundup: Round 'x' up to the nearest page size multiple.
#define pg_roundup(x) roundup_pow2(x, PG_SZ)

// pow2_roundup: Round 'sz' up to the nearest power of two.
static inline size_t pow2_roundup(size_t sz){
	size_t s;
	int i = flsl(sz);
	if (i < 0){
		return 0;
	}
	s = 1 << i;
	if (s < sz){
		s <<= 1;
	}
	return s;
}
// pow2_rounddown: Round 'sz' down to the nearest power of two.
static inline size_t pow2_rounddown(size_t sz){
	int i = flsl(sz);
	if (i < 0){
		return 0;
	}
	return 1 << i;
}

#define tagged_ptr(x) ((x & (sizeof(void*) - 1)) != NULL)
#define tag_ptr(x) (x | 1ULL)
#define untag_ptr(x) (x & ~1ULL)

// freec: Free and clear pointer
#define freec(x) do{free(x); x = NULL;} while (0)

// closec: Close and clear pointer
#define closec(x) do{close(x); x = -1;} while (0)

// open file at "path/file" with flags
int open_pf_flags(char* path, char* file, int flags){
	char* cat;
	int s = strlen(path);
	int ret;
	cat = malloc(s + strlen(file) + 2);
	if (cat == NULL){
		fail_out(ENOMEM);
	}
	strcpy(cat, path);
	cat[s] = '/';
	strcpy(cat + s + 1, file);
	ret = open(cat, flags | O_RDWR);
	free(cat);
	return ret;
}

#define open_pf(a, b) open_pf_flags(a, b, 0)
#define creat_pf(a, b) open_pf_flags(a, b, O_CREAT)

#endif
