//========================================================================
// types.h : Machine dependent type declarations
//========================================================================
// This header file will overwrite the default newlib one. I copied the
// default one from xcc/src/newlib/libc/include/machine/types.h and
// adjusted the typedefs accordingly for maven. Specifically I made the
// clock return type unsigned long long instead of just unsigned long.

#ifndef _MAVEN_TYPES_H
#define _MAVEN_TYPES_H

// Make sure we can use the same lib in both C and C++ programs
#ifdef __cplusplus
extern "C" {
#endif

// Used with clock() function

#define _CLOCK_T_ unsigned long long

// Used with time() function

#define _TIME_T_ long

// Not sure what these are for but they are referenced in 
// xcc/src/newlibg/libc/include/sys/types.h

#define _CLOCKID_T_ unsigned long
#define _TIMER_T_   unsigned long

#ifdef __cplusplus
}
#endif
#endif /* MAVEN_TYPES_H */
