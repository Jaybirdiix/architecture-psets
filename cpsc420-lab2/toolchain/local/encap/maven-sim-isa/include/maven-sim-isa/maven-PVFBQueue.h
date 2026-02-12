#ifndef __MAVENPVFBQUEUE_H
#define __MAVENPVFBQUEUE_H

#include "maven-PVFB.h"

class MavenPVFBQueue : public MavenPVFB
{
public:
  MavenPVFBQueue();

  // flag unused in queue impl
  void insert_frags(uint32_t flag, const MavenVPFrag& frag0, const MavenVPFrag& frag1);
  void pushfront(const MavenVPFrag& frag);
  void pushend(const MavenVPFrag& frag);
  const MavenVPFrag pop();
  bool empty();
  MavenVPFrag* member_pc( uint32_t pc );

  void dump();

//private:
  MavenVPFrag fragments[MAVEN_SYSCFG_VLEN_MAX];
  int start;
  int end;
};

#endif // __MAVENPVFBQUEUE
