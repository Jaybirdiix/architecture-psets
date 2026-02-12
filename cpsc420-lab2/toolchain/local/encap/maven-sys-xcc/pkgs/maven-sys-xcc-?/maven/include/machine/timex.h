//========================================================================
// timex.h : Machine dependent non-standard time utilities
//========================================================================

#ifndef _MAVEN_TIMEX_H
#define _MAVEN_TIMEX_H 

#include <machine/syscfg.h>

// Make sure we can use the same lib in both C and C++ programs
#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------
// cycles_t
//------------------------------------------------------------------------
// We use a 64b type to try and avoid overflow issues.

typedef long long cycles_t;

//------------------------------------------------------------------------
// get_cycles_per_sec
//------------------------------------------------------------------------
// Return the number of cycles per second.

static inline cycles_t get_cycles_per_sec()
{
  return (cycles_t) MAVEN_SYSCFG_CYCLES_PER_SEC;
}

//------------------------------------------------------------------------
// get_cycles
//------------------------------------------------------------------------
// This function returns the number of cycles since the program started
// executing. Use get_cycles_per_sec() to convert cycles into time.

cycles_t get_cycles();

#ifdef __cplusplus
}
#endif
#endif /* _MAVEN_TIMEX_H */
