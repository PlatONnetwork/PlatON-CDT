#ifdef NO_ONTOLOGY_WASM
#include <time.h>
#include "syscall.h"

time_t time(time_t *t)
{
	struct timespec ts;
	__clock_gettime(CLOCK_REALTIME, &ts);
	if (t) *t = ts.tv_sec;
	return ts.tv_sec;
}
#else
#include <time.h>
const char __utc[] = "UTC";

time_t time(time_t *t)
{
	time_t ut = timestamp();
	if (t) *t = ut;
	return ut;
}
#endif
