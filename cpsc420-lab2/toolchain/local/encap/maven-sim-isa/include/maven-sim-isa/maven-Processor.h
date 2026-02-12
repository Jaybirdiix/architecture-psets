#ifndef __MAVENPROCESSOR_H
#define __MAVENPROCESSOR_H

#include "syscfg.h"
#include "processor.h"
#include "memory.h"
#include "maven-ISA.h"
#include "maven-PVFB.h"
#include "maven-PVFBQueue.h"
#include "maven-PVFBStack.h"
#include "maven-PVFBStack-plus.h"
#include "maven-VPFrag.h"
#include "maven-ScoreBoard.h"
#include "stdio.h"

#include <map>

class MavenVPArray;

//------------------------------------------------------------------------
// Divergence Data Collector
//------------------------------------------------------------------------
class DivergenceData
{
public:
  // update stats
  void update (const MavenVPFrag& frag, MavenVPFrag& frag0, MavenVPFrag& frag1, bool done, bool branch);

  void process (FILE* logfile);

  DivergenceData(MavenVPArray& _vparray);

// private:
  MavenVPArray& vparray;

  int numvfs;

  // track total number of vector unit cycles
  uint32_t total_cycles;
  uint32_t vf_cycle;

  // track branch totals
  uint32_t total_branches;

  // track number of cycles a certain fragment size is together
  uint32_t  vl_freq[MAVEN_SYSCFG_VLEN_MAX + 1]; // from 0 to MAX

  // track frequency each pc causes a fragment split
  uint32_t split_total;
  std::map<uint32_t, uint32_t> splits; // <pc> : number times split
};

//------------------------------------------------------------------------
// Maven VP array
//------------------------------------------------------------------------
class MavenVPArray
{
public:

  // used by vload/vstore 
  enum
  {
    WORD_T    = 0x00,
    HWORD_T   = 0x01,
    UHWORD_T  = 0x11,
    BYTE_T    = 0x03,
    UBYTE_T   = 0x13,
  };

  MavenVPArray( Memory& _mem);
  ~MavenVPArray();
  void go(addr_t entry);
  void test(uint32_t npc, uint32_t mask, MavenVPFrag& frag0, MavenVPFrag& frag1);
  void execute();
  bool isnotdone();
  void vcfg(int nregs);
  int setvl(int request);
  void vload(addr_t addr, int rv, int stride, int n_elm, int op_size_t);
  void vload_x(int r_vdst, addr_t base_addr, int r_voff, int op_size_t);
  void vstore(addr_t addr, int rv, int stride, int n_elm, int op_size_t);
  void vstore_x(int r_vsrc, addr_t base_addr, int r_voff, int op_size_t);
  void mtvp(int vp_id, int r_vdst, reg_t value);
  uint32_t mfvp(int vp_id, int r_vsrc);
  void mtvps(int r_vdst, reg_t value);
  void arith_vv(int r_vdst, int r_vsrc1, int r_vsrc2, int r_mask, int cmd_code);
  void arith_vs(int r_vdst, reg_t sdata, int r_vsrc, int r_mask, int cmd_code);
  void arith_v(int r_vdst, int r_vsrc, int r_mask, int cmd_code);
  void arith_s_vv(int r_vdst, int r_vsrc1, int r_vsrc2, int r_mask, int cmd_code);
  void arith_s_vs(int r_vdst, reg_t sdata, int r_vsrc, int r_mask, int cmd_code);
  void arith_s_v(int r_vdst, int r_vsrc, int r_mask, int cmd_code);
  void setflag(int r_fdst, int r_vsrc1, int r_vsrc2, int cmd_code);
  void sf_setflag_vv(int r_fdst, int r_vsrc1, int r_vsrc2, int cmd_code);
  void sf_setflag_vs(int r_fdst, int r_vsrc, reg_t rsrc, int cmd_code);
  void sf_setflag_sv(int r_fdst, reg_t rsrc, int r_vsrc, int cmd_code);
  void flagops(int r_fdst, int r_fsrc1, int r_fsrc2, int func_code);
  reg_t popcf(int r_fsrc);
  reg_t findfonef(int r_fsrc);
  void mtmfvpsf(int r_dst, int r_src, int func_code);
  void set_impl_flags(bool vxudebug);
  void amo_vv(int r_vdst, int r_vaddr, int r_vsrc, int r_mask, int cmd_code);
  void utidx_v(int r_vdst, int cmd_code);

  void set_num_physical_regs( int nregs );

//private:
  Memory& mem;
  MavenPVFB* pvfb;
  MavenScoreBoard sb;
  MavenVP* vp[MAVEN_SYSCFG_VLEN_MAX];
  int vl;
  int vlmax;
  int num_physical_regs;

  // uarch stats_en
  bool stats_en;

  // implementation flags
  bool reconvergence;
  bool density;
  bool scoreboard; // only enable when stats_en is also true

  // logfile for traces
  FILE* logfile;

  // stats collector
  DivergenceData stats;
};

class MavenProcessor
{
public:
  MavenProcessor( Memory& _mem );
  void go(addr_t entry);
  void execute();
  void set_proc_id(int n);
  void set_num_physical_regs( int nregs );
  void set_impl_flags(int reconvergence, bool density, bool scoreboard, bool bc, bool vxudebug);

//private:
  MavenCP cp;
  MavenVPArray vparray;
};

#endif // __MAVENPROCESSOR
