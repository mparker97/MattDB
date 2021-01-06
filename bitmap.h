#ifndef BITMAP_H
#define BITMAP_H

#define BITMAP_T_TYPE unsigned char
#define BITMAP_T_SZ (sizeof(BITMAP_T_TYPE) * 8)

typedef BITMAP_T_TYPE[] bitmap_t;

#define new_bitmap_t(nm, i) BITMAP_T_TYPE nm[((i) + BITMAP_T_SZ - 1) / BITMAP_T_SZ]

inline int bitmap_index(bitmap_t bm, off_t idx){
	return (bm[idx / BITMAP_T_SZ] >> (idx % BITMAP_T_SZ)) & 1;
}

inline void bitmap_set(bitmap_t bm, off_t idx){
	bm[idx / BITMAP_T_SZ] |= (1 << (idx % BITMAP_T_SZ));
}

inline void bitmap_unset(bitmap_t bm, off_t idx){
	bm[idx / BITMAP_T_SZ] &= ~(1 << (idx % BITMAP_T_SZ));
}

// "new" functions - nonzero when the idx bit is changed

inline unsigned char bitmap_new_set(bitmap_t bm, off_t idx){
	unsigned char c = bm[idx / BITMAP_T_SZ];
	bitmap_set(bm, idx);
	return c ^ bm[idx / BITMAP_T_SZ];
}

inline unsigned char void bitmap_new_unset(bitmap_t bm, off_t idx){
	unsigned char c = bm[idx / BITMAP_T_SZ];
	bitmap_unset(bm, idx);
	return c ^ bm[idx / BITMAP_T_SZ];
}

#endif