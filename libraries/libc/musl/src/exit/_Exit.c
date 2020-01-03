#ifdef NO_ONTOLOGY_WASM
#include <stdlib.h>
#include "syscall.h"

_Noreturn void _Exit(int ec)
{
	__syscall(SYS_exit_group, ec);
	for (;;) __syscall(SYS_exit, ec);
}
#else
_Noreturn void abort(void)
{
}

_Noreturn void _Exit(int ec)
{
   abort();
}
#endif
