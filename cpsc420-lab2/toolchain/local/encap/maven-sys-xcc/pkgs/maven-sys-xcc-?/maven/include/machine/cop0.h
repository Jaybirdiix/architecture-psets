//========================================================================
// cop0.h : Helper functions to read/write cop0 registers
//========================================================================

#ifndef _MAVEN_COP0_H
#define _MAVEN_COP0_H

#include <machine/syscfg.h>

// Make sure we can use the same lib in both C and C++ programs
#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------
// maven_get_cop0_count()
//------------------------------------------------------------------------
// Return the 32b cycle count

static inline unsigned int maven_get_cop0_count()
{
  unsigned int ret;
  __asm__ ( "mfc0 %0, $%1"
            : "=r"(ret) : "i"(MAVEN_SYSCFG_REGDEF_COP0_COUNT) );
  return ret;
}

//------------------------------------------------------------------------
// maven_get_cop0_core_id()
//------------------------------------------------------------------------
// Return the core id for the current core

static inline unsigned int maven_get_cop0_core_id()
{
  unsigned int ret;
  __asm__ ( "mfc0 %0, $%1"
            : "=r"(ret) : "i"(MAVEN_SYSCFG_REGDEF_COP0_CORE_ID) );
  return ret;
}

//------------------------------------------------------------------------
// maven_set_cop0_stats_enable( bool value )
//------------------------------------------------------------------------
// Sets the statistics enable control register

static inline void maven_set_cop0_stats_enable( bool value )
{
  __asm__ ( "mtc0 %0, $%1"
            :: "r"(value), "i"(MAVEN_SYSCFG_REGDEF_COP0_STATS_EN) );
}

#ifdef __cplusplus
}
#endif
#endif /* _MAVEN_COP0_H */

