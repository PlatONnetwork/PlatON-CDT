#include "stdio_impl.h"
#ifdef NO_ONTOLOGY_WASM
#include <sys/ioctl.h>
#endif

size_t __stdout_write(FILE *f, const unsigned char *buf, size_t len)
{
#ifdef NO_ONTOLOGY_WASM
	struct winsize wsz;
	f->write = __stdio_write;
	if (!(f->flags & F_SVB) && __syscall(SYS_ioctl, f->fd, TIOCGWINSZ, &wsz))
		f->lbf = -1;
#endif
	return __stdio_write(f, buf, len);
}
