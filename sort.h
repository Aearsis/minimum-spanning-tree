#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef TYPE
#error Please define TYPE before including sort.
#endif

#ifndef GET
#error Please define GET before including sort
#endif

/**
 * Example:
 *  #define TYPE struct type
 *  TYPE {
 *    ... some other data ...
 *    int32_t sort_by;
 *  }
 *  #define GET(x) x.sort_by
 */

static inline void
radixsort(TYPE* array, size_t size)
{
	TYPE* swap = malloc(sizeof(TYPE) * size);
	TYPE* end = array + size;
	TYPE* send = swap + size;

	size_t a1 [257], a2 [257], a3[257], a4[257];
	memset(a1, 0, sizeof(size_t) * 257);
	memset(a2, 0, sizeof(size_t) * 257);
	memset(a3, 0, sizeof(size_t) * 257);
	memset(a4, 0, sizeof(size_t) * 257);

	for (TYPE* it = array; it != end; ++it) {
		uint32_t x = GET(*it);
		a4[(x & 0xff) + 1]++; x >>= 8;
		a3[(x & 0xff) + 1]++; x >>= 8;
		a2[(x & 0xff) + 1]++; x >>= 8;
		a1[(x & 0xff) + 1]++;
	}

	for (int i = 1; i < 257; i++) {
		a1[i] += a1[i - 1];
		a2[i] += a2[i - 1];
		a3[i] += a3[i - 1];
		a4[i] += a4[i - 1];
	}

	for (TYPE* it = array; it != end; ++it) {
		int x = GET(*it) & 0xff;
		swap[a4[x]++] = *it;
	}

	for (TYPE* it = swap; it != send; ++it) {
		int x = (GET(*it) >> 8) & 0xff;
		array[a3[x]++] = *it;
	}

	for (TYPE* it = array; it != end; ++it) {
		int x = (GET(*it) >> 16) & 0xff;
		swap[a2[x]++] = *it;
	}

	for (TYPE* it = swap; it != send; ++it) {
		int x = (GET(*it) >> 24) & 0xff;
		array[a1[x]++] = *it;
	}
}
