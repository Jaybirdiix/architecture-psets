//========================================================================
// syscall.h : Syscall numbers and helper macros
//========================================================================
// The system call numbers are brought in via syscfg.h. In addition we
// provide a set of helper macros which simplify making system calls
// with zero to three arguments.
//
// The current implementation of these macros assumes the following mips
// syscall instruction interface:
//
//  - a0 : syscall argument 0
//  - a1 : syscall argument 1
//  - a2 : syscall argument 2
//  - a3 : error flag (0 = success, 1 = failure)
//  - v0 : syscall id, then return value from system call
//
// The first argument should be just the name of the system call in all
// capital letters. It should correspond to the suffix of
// MAVEN_SYSCFG_SYSCALL so for example, to make the close system call we
// would use something like this:
//
//  int close( int file ) 
//  {
//    int result, error_flag;
//    MAVEN_SYSCALL_ARG1( CLOSE, result, error_flag, file );
//   
//    if ( error_flag == 1 ) {
//      errno = result;
//      return -1;
//    }
//    
//    return 0;
//  }
//

#include <machine/syscfg.h>

//------------------------------------------------------------------------
// MAVEN_SYSCALL_ARG0
//------------------------------------------------------------------------

#define MAVEN_SYSCALL_ARG0( sys_, res_, eflag_ )                        \
  register __typeof__ (res_)   res_   ## _ asm ("v0");                  \
  register __typeof__ (eflag_) eflag_ ## _ asm ("a3");                  \
  __asm__ volatile                                                      \
  ( "li $v0, %2; syscall"                                               \
    : "=r"(res_ ## _), "=r"(eflag_ ## _)                                \
    : "i"(MAVEN_SYSCFG_SYSCALL_ ## sys_) );                             \
  res_   = res_ ## _;                                                   \
  eflag_ = eflag_ ## _;                                                 \

//------------------------------------------------------------------------
// MAVEN_SYSCALL_ARG1
//------------------------------------------------------------------------

#define MAVEN_SYSCALL_ARG1( sys_, res_, eflag_, a0_ )                   \
  register __typeof__ (res_)   res_   ## _ asm ("v0");                  \
  register __typeof__ (a0_)    a0_    ## _ asm ("a0") = a0_;            \
  register __typeof__ (eflag_) eflag_ ## _ asm ("a3");                  \
  __asm__ volatile                                                      \
  ( "li $v0, %3; syscall"                                               \
    : "=r"(res_ ## _), "=r"(eflag_ ## _)                                \
    : "r"(a0_ ## _),                                                    \
      "i"(MAVEN_SYSCFG_SYSCALL_ ## sys_) );                             \
  res_   = res_ ## _;                                                   \
  eflag_ = eflag_ ## _;                                                 \

//------------------------------------------------------------------------
// MAVEN_SYSCALL_ARG2
//------------------------------------------------------------------------

#define MAVEN_SYSCALL_ARG2( sys_, res_, eflag_, a0_, a1_ )              \
  register __typeof__ (res_)   res_   ## _ asm ("v0");                  \
  register __typeof__ (a0_)    a0_    ## _ asm ("a0") = a0_;            \
  register __typeof__ (a1_)    a1_    ## _ asm ("a1") = a1_;            \
  register __typeof__ (eflag_) eflag_ ## _ asm ("a3");                  \
  __asm__ volatile                                                      \
  ( "li $v0, %4; syscall"                                               \
    : "=r"(res_ ## _), "=r"(eflag_ ## _)                                \
    : "r"(a0_ ## _), "r"(a1_ ## _),                                     \
      "i"(MAVEN_SYSCFG_SYSCALL_ ## sys_) );                             \
  res_   = res_ ## _;                                                   \
  eflag_ = eflag_ ## _;                                                 \

//------------------------------------------------------------------------
// MAVEN_SYSCALL_ARG3
//------------------------------------------------------------------------

#define MAVEN_SYSCALL_ARG3( sys_, res_, eflag_, a0_, a1_, a2_ )         \
  register __typeof__ (res_)   res_   ## _ asm ("v0");                  \
  register __typeof__ (a0_)    a0_    ## _ asm ("a0") = a0_;            \
  register __typeof__ (a1_)    a1_    ## _ asm ("a1") = a1_;            \
  register __typeof__ (a2_)    a2_    ## _ asm ("a2") = a2_;            \
  register __typeof__ (eflag_) eflag_ ## _ asm ("a3");                  \
  __asm__ volatile                                                      \
  ( "li $v0, %5; syscall"                                               \
    : "=r"(res_ ## _), "=r"(eflag_ ## _)                                \
    : "r"(a0_ ## _), "r"(a1_ ## _), "r"(a2_ ## _),                      \
      "i"(MAVEN_SYSCFG_SYSCALL_ ## sys_) );                             \
  res_   = res_ ## _;                                                   \
  eflag_ = eflag_ ## _;                                                 \

