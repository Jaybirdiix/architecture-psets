//========================================================================
// time.h : Machine dependent time utilities
//========================================================================

#ifndef _MAVEN_TIME_H
#define _MAVEN_TIME_H

// Make sure we can use the same lib in both C and C++ programs
#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------
// CLOCKS_PER_SEC
//------------------------------------------------------------------------
// I think both CLOCKS_PER_SEC and CLK_PER_SEC should be defined. I also
// think the new standard requires these macros to be set to 100000
// which means that the best resolution possible with the clock function
// is microseconds. Usually the resultion is much, much worse than that
// on the order of milliseconds (related to the operating system's
// scheduling clock). On maven we will actually provide microsecond
// resolution. For finer granularity use get_cycles() provided in
// <machine/timex.h>.

#define _CLOCKS_PER_SEC_ 1000000
#define CLK_PER_SEC _CLOCKS_PER_SEC_

#ifdef __cplusplus
}
#endif
#endif /* _MAVEN_TIME_H */
