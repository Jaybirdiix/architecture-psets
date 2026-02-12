//========================================================================
// resource.h : Machine dependent resource utilities
//========================================================================

#ifndef _MAVEN_RESOURCE_H
#define _MAVEN_RESOURCE_H

#include <sys/resource.h>

// Make sure we can use the same lib in both C and C++ programs
#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------
// getrusage
//------------------------------------------------------------------------
// Implementation of the common getrusage function which reports the
// reosurce utilization of a process. The currently implementation is
// quite limited but is sufficient for the bare maven system. The rusage
// struct and RUSAGE_ macros are currently defined in
// xcc/src/newlibg/libc/include/sys/resource.h

int getrusage( int who, struct rusage* ru );

#ifdef __cplusplus
}
#endif
#endif /* _MAVEN_RESOURCE_H */
