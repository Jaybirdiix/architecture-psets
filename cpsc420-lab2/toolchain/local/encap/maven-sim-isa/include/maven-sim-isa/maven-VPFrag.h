#ifndef __MAVENVPFRAG_H
#define __MAVENVPFRAG_H

#include "types.h"
#include "syscfg.h"
#include "stdio.h"

struct MavenVPFrag
{
  uint32_t pc;
  uint32_t mask;

  //----------------------------------------------------------------------
  // stats counters
  //----------------------------------------------------------------------
  int size;

  // cycle number this fragment was formed
  uint32_t  start_cycle; 

  // number of branches this fragment has encountered but survived
  int numbranches;

  // the total number of times this fragment has split
  int numsplits;

  MavenVPFrag (uint32_t pc, uint32_t mask, int size)
    // initialize stats counters
    : numbranches(0),
      numsplits(0)
  {
    this->pc   = pc;
    this->mask = mask;
    this->size = size;
  };

  MavenVPFrag()
  : pc(-1),
    mask(0),
    size(0),
    numbranches(0),
    numsplits(0)
   {};

  void reset_stats()
  {
    this->numbranches = 0;
    this->numsplits   = 0;
  }
  
  void copy_stats(const MavenVPFrag& frag)
  {
    this->numbranches = frag.numbranches;
    this->numsplits   = frag.numsplits;
  }

};

#endif // __MAVENVPFRAG
