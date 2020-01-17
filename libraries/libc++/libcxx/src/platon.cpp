#pragma once

/**
 * Define specific things for the onto system
 */

extern "C" {
   void  platon_abort( unsigned int test, const char* msg  );
   void __cxa_pure_virtual() { platon_abort(false, "[error:] pure virtual method called"); }
}
