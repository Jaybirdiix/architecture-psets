#ifndef __MAVENPVFBSTACK_PLUS_H
#define __MAVENPVFBSTACK_PLUS_H

#include <list>
#include "maven-PVFB.h"

//------------------------------------------------------------------------
// Multiple stack array for loop structures
//------------------------------------------------------------------------
// * for a given execution stream:
// if backwards branch: push onto next stack
// else: push onto current stack as before
//
// empty current stack before switching to next stack
//
// done when no fragments remain in either stack
class MavenPVFBStack_plus : public MavenPVFB
{
public:
  MavenPVFBStack_plus();

  // flag is used to determine how to push fragments onto stack
  // flag = pc
  void insert_frags(uint32_t flag, const MavenVPFrag& frag0, const MavenVPFrag& frag1);

  // next_stack is true if fragment should be inserted into next stack
  void insert (const MavenVPFrag& frag, bool insert_next);
  void pushfront(const MavenVPFrag& frag);
  const MavenVPFrag pop();
  bool empty();
  MavenVPFrag* member_pc(uint32_t pc);

  void dump();

//private:
  static const int NUM_STACKS = 2;

  std::list<MavenVPFrag> fragments[NUM_STACKS];

  // stack for the current loop
  int curr_stack;
};

#endif // __MAVENPVFBSTACK_PLUS
