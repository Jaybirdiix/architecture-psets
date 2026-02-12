#ifndef __MIPS32PROCESSOR_H
#define __MIPS32PROCESSOR_H

#include "types.h"
#include "host.h"
#include "mipsdt.h"
#include "memory.h"
#include "instruction.h"
#include "mips32-FPU.h"

class MIPS32ProcessorState
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

  MIPS32ProcessorState(Memory& _mem)
    : mem(_mem)
    , fpu()
    , pc(0)
    , npc(4)
    , hi(0)
    , lo(0)
    , branch(false)
    , nullify(false)
    , exception(0)
    , flag_mode(false)
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

  Memory& mem;
  MIPS32FPUState fpu;
  reg_t R[32];
  reg_t pc, npc;
  reg_t hi, lo;

  bool branch;
  bool nullify;
  char disassemble_string[1024];
  int exception;
  runstate_t runstate;
  bool flag_mode;

  // microarch state
  bool inst_branch;
  bool inst_jump;
  int alu;
  int rs, rt, rd;
  bool b_rs, b_rt, b_rd;
};

class MIPS32Processor
{
public:
  MIPS32Processor(Memory& _mem);
  void go(addr_t entry);
  void fetch();
  void execute();
  void dump_state();
  void disassemble_instruction(MIPS32Instruction& inst);

//private:
  MIPSGenericDispatchTable<MIPS32Instruction, MIPS32ProcessorState>& dt;
  MIPS32ProcessorState state;
  Memory& mem;
  MIPS32Instruction inst;
};

#endif // __MIPS32PROCESSOR_H
