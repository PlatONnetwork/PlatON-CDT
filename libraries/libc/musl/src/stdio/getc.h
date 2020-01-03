#include "stdio_impl.h"
#include "pthread_impl.h"

#ifdef __GNUC__
__attribute__((__noinline__))
#endif
static int locking_getc(FILE *f)
{
#ifdef NO_ONTOLOGY_WASM
	if (a_cas(&f->lock, 0, MAYBE_WAITERS-1)) __lockfile(f);
	int c = getc_unlocked(f);
	if (a_swap(&f->lock, 0) & MAYBE_WAITERS)
		__wake(&f->lock, 1, 1);
	return c;
#else
	int c = getc_unlocked(f);
	return c;
#endif
}

static inline int do_getc(FILE *f)
{
#ifdef NO_ONTOLOGY_WASM
	int l = f->lock;
	if (l < 0 || l && (l & ~MAYBE_WAITERS) == __pthread_self()->tid)
		return getc_unlocked(f);
#endif
	return locking_getc(f);
}
