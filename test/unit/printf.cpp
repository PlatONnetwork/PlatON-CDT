#include "platon_unit_test.hpp"
#include <stdio.h>
#include <stdint.h>

extern "C" {
    __attribute__((used))
    void platon_debug( const char* cstr, uint32_t len ) {
        return;
    }
}

UNIT_BEGIN
int result = printf("platon_test");
UNIT_ASSERT(result >= 0)
result = printf("test number %d", 1234);
UNIT_ASSERT(result >= 0)
UNIT_SUCCESS
UNIT_END