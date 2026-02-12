//========================================================================
// syscfg.h : Maven system configuration macros
//========================================================================
// This is the key file where we place all maven system configuration
// macros. All macros in this file should begin with a MAVEN_SYSCFG_
// prefix so that users can immediately know where such macros are
// defined. This file should be replicated in the simulator directories
// and in libgloss here: maven/sys/xcc/src/libgloss/maven. The libgloss
// makefile is reposible for installing this file into
// ${sysroot}/include/machine so that other maven software (proxy
// kernel, libraries, applications) can include it like this:
//
//  #include <machine/syscfg.h>
//

#ifndef MAVEN_SYSCFG_H
#define MAVEN_SYSCFG_H

//------------------------------------------------------------------------
// System Calls
//------------------------------------------------------------------------
// We are using arbitrary system calls which do not map to any
// particular operating system. Standard system calls found on most
// linux/unix machines should use smaller numbers while extra system
// calls specific to maven and our infrastructure should start from
// 0x4000. No one should ever refer to the system call number directly
// so we can renumber system calls whenever we like.

#define MAVEN_SYSCFG_SYSCALL_EXIT                           1
#define MAVEN_SYSCFG_SYSCALL_READ                           2
#define MAVEN_SYSCFG_SYSCALL_WRITE                          3
#define MAVEN_SYSCFG_SYSCALL_OPEN                           4
#define MAVEN_SYSCFG_SYSCALL_CLOSE                          5
#define MAVEN_SYSCFG_SYSCALL_LINK                           6
#define MAVEN_SYSCFG_SYSCALL_UNLINK                         7
#define MAVEN_SYSCFG_SYSCALL_LSEEK                          8 
#define MAVEN_SYSCFG_SYSCALL_FSTAT                          9
#define MAVEN_SYSCFG_SYSCALL_STAT                          10
                                                         
// Extra maven specific system calls                     
                                                         
#define MAVEN_SYSCFG_SYSCALL_NUMCORES                    4000
#define MAVEN_SYSCFG_SYSCALL_SENDAM                      4001
                                                         
#define MAVEN_SYSCFG_SYSCALL_BTHREAD_ONCE                4002
#define MAVEN_SYSCFG_SYSCALL_BTHREAD_KEY_CREATE          4003
#define MAVEN_SYSCFG_SYSCALL_BTHREAD_KEY_DELETE          4004
#define MAVEN_SYSCFG_SYSCALL_BTHREAD_KEY_SETSPECIFIC     4005
#define MAVEN_SYSCFG_SYSCALL_BTHREAD_KEY_GETSPECIFIC     4006

#define MAVEN_SYSCFG_SYSCALL_YIELD                       4007

//------------------------------------------------------------------------
// Coprocessor 0 Control Registers
//------------------------------------------------------------------------

#define MAVEN_SYSCFG_REGDEF_COP0_COUNT      9
#define MAVEN_SYSCFG_REGDEF_COP0_COUNT_LO   9
#define MAVEN_SYSCFG_REGDEF_COP0_COUNT_HI  25

#define MAVEN_SYSCFG_REGDEF_COP0_CORE_ID   17
#define MAVEN_SYSCFG_REGDEF_COP0_TID_MASK  18
#define MAVEN_SYSCFG_REGDEF_COP0_TID_STOP  19
#define MAVEN_SYSCFG_REGDEF_COP0_STATS_EN  21

//------------------------------------------------------------------------
// Maven minimum and maximum vector length
//------------------------------------------------------------------------

#define MAVEN_SYSCFG_VLEN_MIN  4
#define MAVEN_SYSCFG_VLEN_MAX 32

//------------------------------------------------------------------------
// Timing
//------------------------------------------------------------------------

// How many processor cycles per second. Assume maven runs at 1 GHz.
#define MAVEN_SYSCFG_CYCLES_PER_SEC 1000000000

//------------------------------------------------------------------------
// Threads
//------------------------------------------------------------------------

// Size of the various stacks
#define MAVEN_SYSCFG_USER_THREAD_STACK_SIZE   0x00010000
#define MAVEN_SYSCFG_KERNEL_THREAD_STACK_SIZE 0x00010000
#define MAVEN_SYSCFG_VP_THREAD_STACK_SIZE     0x00001000

//------------------------------------------------------------------------
// Memory layout
//------------------------------------------------------------------------

#define MAVEN_SYSCFG_MEMORY_SIZE  0x10000000
#define MAVEN_SYSCFG_USER_START   0x00000000
#define MAVEN_SYSCFG_USER_SIZE    0x0C000000
#define MAVEN_SYSCFG_KERNEL_START 0x0C000000
#define MAVEN_SYSCFG_KERNEL_SIZE  0x04000000
#define MAVEN_SYSCFG_ARGS_SIZE    0x00010000

#define MAVEN_SYSCFG_MAX_PROCS 64 // maximum number of processors in the system
#define MAVEN_SYSCFG_MAX_KEYS  64 // maximum number of unique keys per thread

#endif /* MAVEN_SYSCFG_H */
