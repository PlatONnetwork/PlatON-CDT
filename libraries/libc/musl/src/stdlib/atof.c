#ifdef NO_ONTOLOGY_WASM
#include <stdlib.h>

double atof(const char *s)
{
	return strtod(s, 0);
}
#endif
