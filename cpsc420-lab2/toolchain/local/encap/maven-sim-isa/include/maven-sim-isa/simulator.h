#ifndef __SIMULATOR_H
#define __SIMULATOR_H

#include <vector>

#include "types.h"
#include "processor.h"

class simulator_t
{
public:
  simulator_t();

  static int pack_argc_argv(uint8 buf[MAVEN_SYSCFG_ARGS_SIZE], uint32_t args_start, int argc, char** argv);

  void set_reset(int val);
  void set_run(int val);
  int get_tohost();
  void set_fromhost(int val);
  int run_to_tohost(int orig_tohost);
  int set_num_cores(int n);
  void set_num_physical_regs(int nregs);
  void set_impl_flags(int reconvergence, bool density, bool scoreboard, bool bc, bool vxudebug);
  void set_logfile(FILE* logfile);
  uint32_t get_cycle();
  
  // simulator's dumpstats
  void dumpstats(FILE* logfile, double seconds = 0.0);

//private:
  MavenProcessor** procs;
  int nproc;
  Memory mem;
  
  int tid_mask;

  uint32_t magicmemaddr;
  long long cycle;
};

#endif // __SIMULATOR_H
