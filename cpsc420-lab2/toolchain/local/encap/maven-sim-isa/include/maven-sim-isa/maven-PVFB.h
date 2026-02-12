#ifndef __MAVENPVFB_H
#define __MAVENPVFB_H

#include "types.h"
#include "syscfg.h"
#include "maven-VPFrag.h"

class MavenPVFB
{
public:
  virtual ~MavenPVFB() {};

  // flag used for more advanced stack implementations
  virtual void insert_frags(uint32_t flag, const MavenVPFrag& frag0, const MavenVPFrag& frag1) = 0;
  virtual const MavenVPFrag pop() = 0;
  virtual bool empty() = 0;
  virtual MavenVPFrag* member_pc( uint32_t pc) = 0;
  virtual void pushfront(const MavenVPFrag& frag) {};
  virtual void pushend  (const MavenVPFrag& frag) {};

  virtual void dump() = 0;
};

#endif // __MAVENPVFB
