#ifndef __MAVENPVFBSTACK_H
#define __MAVENPVFBSTACK_H

#include <list>
#include "maven-PVFB.h"

class MavenPVFBStack : public MavenPVFB
{
public:
  MavenPVFBStack(){};
  
  // flag unused in base stack impl
  void insert_frags(uint32_t flag, const MavenVPFrag& frag0, const MavenVPFrag& frag1);

  void insert(const MavenVPFrag& frag);
  void pushfront(const MavenVPFrag& frag);
  const MavenVPFrag pop();
  bool empty();
  MavenVPFrag* member_pc(uint32_t pc);

  void dump();

//private:
  std::list<MavenVPFrag> fragments;
};

#endif // __MAVENPVFBSTACK
