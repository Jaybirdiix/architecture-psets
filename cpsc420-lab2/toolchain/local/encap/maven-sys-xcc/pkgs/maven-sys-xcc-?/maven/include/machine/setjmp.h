//========================================================================
// setjmp.h : Machine dependent declarations for setjmp/longjmp
//========================================================================
// This header file will overwrite the default newlib one. This header
// is included by the primary setjmp.h and it is supposed to define the
// jump buffer appropriately. The jump buffer is an array with one
// element for each callee saved register.
//
// The default version of this file is in
// xcc/src/newlib/libc/include/machine/setjmp.h and it includes support
// for the sigsetjmp and siglongjmp functions. Since we don't currently
// support processes and signals we don't need these functions.

#ifndef _MAVEN_SETJMP_H
#define _MAVEN_SETJMP_H

// Make sure we can use the same lib in both C and C++ programs
#ifdef __cplusplus
extern "C" {
#endif

#define _JBTYPE int
#define _JBLEN  23

typedef _JBTYPE jmp_buf[_JBLEN];

#ifdef __cplusplus
}
#endif
#endif /* MAVEN_SETJUMP_H */
