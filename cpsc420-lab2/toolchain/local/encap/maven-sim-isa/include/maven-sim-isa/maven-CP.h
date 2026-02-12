#ifndef __MAVENCP_H
#define __MAVENCP_H

#include "types.h"
#include "host.h"
#include "mipsdt.h"
#include "memory.h"
#include "instruction.h"
#include "mips32-FPU.h"
#include "processor.h"
#include "utils.h"

class MavenVPArray;

class MavenCPState
{
public:
  enum runstate_t
  {
    STOPPED,
    RUNNING,
  };

  enum exception_t
  {
    EXCEPTION_ADDR_MISALIGNED_LOAD = 4,
    EXCEPTION_ADDR_MISALIGNED_STORE = 5,
    EXCEPTION_ADDR_MISALIGNED_FETCH = 6,
    EXCEPTION_SYSCALL = 8,
    EXCEPTION_RESERVED_INSTRUCTION = 10,
    EXCEPTION_ALU_OVERFLOW = 12,
    EXCEPTION_TRAP,
    EXCEPTION_FP_MISALIGNED,
    EXCEPTION_FP_IEEE754,
  };

  MavenCPState(Memory& _mem, MavenVPArray& _vparray)
    : mem(_mem)
    , fpu()
    , vparray(_vparray)
    , cp_cycle(0)
    , pc(0)
    , npc(4)
    , hi(0)
    , lo(0)
    , epc(0)
    , ebase(0)
    , proc_id(0)
    , branch(false)
    , nullify(false)
    , exception(0)
    , flag_mode(false)
    , vector(false)
    , vp_mode(false)
    , stats_en(false)
  {
    R[0] = 0;
  }

  void write_register(int idx, reg_t val);
  uint32_t read_register(int idx);
  void write_flag(int idx, bool val);

  void write_register_s(int idx, float32 val)
  {
    write_register(idx, val);
  }

  void write_register_d(int idx, float64 val)
  {
    assert(!(idx&0x1));
    write_register(idx, (reg_t)val);
    write_register(idx+1, (reg_t)(val>>32));
  }

  void write_register_w(int idx, int32 val)
  {
    write_register(idx, val);
  }

  float32 read_register_s(int idx)
  {
    return read_register(idx);
  }

  float64 read_register_d(int idx)
  {
    assert(!(idx&0x1));
    return (((float64)read_register(idx+1))<<32) | ((float64)read_register(idx));
  }

  int32 read_register_w(int idx)
  {
    return read_register(idx);
  }

  void raise_exception(exception_t cause);
  void dump_state();
  void set_proc_id(int n);

  int get_vpidx()
  {
    return -1;
  }

  Memory& mem;
  MIPS32FPUState fpu;
  MavenVPArray& vparray;
  reg_t R[32];
  long long cp_cycle;
  reg_t pc, npc;
  reg_t hi, lo;
  reg_t epc;
  reg_t ebase;
  int proc_id;

  bool branch;
  bool nullify;
  char disassemble_string[1024];
  int exception;
  runstate_t runstate;
  bool flag_mode;

  bool vector;
  bool vp_mode;

  bool stats_en;
  
  int *tid_mask_ptr;

  // microarch state
  bool inst_branch;
  bool inst_jump;
  int alu;
  int rs, rt, rd;
  bool b_rs, b_rt, b_rd;
};

class MavenCP
{
public:
  MavenCP(Memory& _mem, MavenVPArray& _vparray);
  void go(addr_t entry);
  void fetch();
  void execute();
  void dump_state();
  void disassemble_instruction(MavenCPInstruction& inst);
  void set_proc_id(int n);

//private:
  MIPSGenericDispatchTable<MavenCPInstruction, MavenCPState>& dt;
  MavenCPState state;
  Memory& mem;
  MavenCPInstruction inst;
  // flag to turn scoreboarding on
  bool scoreboard; // only enable when stats_en is also true
};

#endif // __MAVENCP_H
