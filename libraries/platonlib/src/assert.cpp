#include <stdint.h>
#include <string.h>
#include <stdlib.h>


extern "C" {
void platon_revert();
void platon_debug(uint8_t *dst, size_t len);
void  platon_assert( uint32_t test, const char* msg ) {
	if (!test) {
        uint8_t * dest = (uint8_t *)msg;
		::platon_debug(dest, strlen(msg));
		platon_revert();
	}
}
}