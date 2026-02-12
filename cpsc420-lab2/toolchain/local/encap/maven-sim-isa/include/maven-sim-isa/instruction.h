#ifndef __INSTRUCTION_H
#define __INSTRUCTION_H

#include "types.h"

class Instruction
{
public:
  unsigned int operator>>(int shift) { return inst.bits >> shift; }
  void set_bits(unsigned int _bits)  { inst.bits = _bits; }
  uint32_t get_bits()      { return inst.bits; }
  uint32_t get_opcode()    { return inst.opcode_rs_rt_rd_sa_func.opcode; }
  uint32_t get_rs()        { return inst.opcode_rs_rt_rd_sa_func.rs; }
  uint32_t get_rt()        { return inst.opcode_rs_rt_rd_sa_func.rt; }
  uint32_t get_rd()        { return inst.opcode_rs_rt_rd_sa_func.rd; }
  uint32_t get_rv()        { return inst.opcode_rs_rt_rv_u_s_n.rv; }
  uint32_t get_sa()        { return inst.opcode_rs_rt_rd_sa_func.sa; }
  uint32_t get_func()      { return inst.opcode_rs_rt_rd_sa_func.func; }
  uint32_t get_code()      { return inst.opcode_code_func.code; }
  uint32_t get_cc()        { return inst.opcode_rs_cc_nd_td_rd_sa_func.cc; }
  uint32_t get_immediate() { return inst.opcode_rs_rt_immediate.immediate; }
  uint32_t get_jump()      { return inst.opcode_jump.jump; }
  uint32_t get_fs()        { return inst.opcode_fmt_ft_fs_fd_func.fs; }
  uint32_t get_ft()        { return inst.opcode_fmt_ft_fs_fd_func.ft; }
  uint32_t get_fd()        { return inst.opcode_fmt_ft_fs_fd_func.fd; }
  uint32_t get_fcc()       { return inst.opcode_fmt_ft_fs_cc_a_fc_cond.cc; }
  uint32_t get_msk()       { return inst.opcode_rs_rt_rv_u_s_n.msk; }
  uint32_t get_u()         { return inst.opcode_rs_rt_rv_u_s_n.u; }
  uint32_t get_s()         { return inst.opcode_rs_rt_rv_u_s_n.s; }
  uint32_t get_n()         { return inst.opcode_rs_rt_rv_u_s_n.n; }
  uint32_t get_cmd()       { return inst.opcode_rs_rt_rd_cmd.cmd; }
   
  void set_opcode_rs_rt_rd_cmd(uint32_t opcode, uint32_t rs, uint32_t rt, uint32_t rd, uint32_t cmd)
  {
    inst.opcode_rs_rt_rd_cmd.opcode = opcode;
    inst.opcode_rs_rt_rd_cmd.rs = rs;
    inst.opcode_rs_rt_rd_cmd.rt = rt;
    inst.opcode_rs_rt_rd_cmd.rd = rd;
    inst.opcode_rs_rt_rd_cmd.cmd = cmd;
  }
   
  void set_opcode_rs_rt_rd_sa_func(uint32_t opcode, uint32_t rs, uint32_t rt, uint32_t rd, uint32_t sa, uint32_t func)
  {
    inst.opcode_rs_rt_rd_sa_func.opcode = opcode;
    inst.opcode_rs_rt_rd_sa_func.rs = rs;
    inst.opcode_rs_rt_rd_sa_func.rt = rt;
    inst.opcode_rs_rt_rd_sa_func.rd = rd;
    inst.opcode_rs_rt_rd_sa_func.sa = sa;
    inst.opcode_rs_rt_rd_sa_func.func = func;
  }

  void set_opcode_fmt_ft_fs_cc_cond(uint32_t opcode, uint32_t fmt, uint32_t ft, uint32_t fs, uint32_t cc, uint32_t cond)
  {
    inst.opcode_fmt_ft_fs_cc_a_fc_cond.opcode = opcode;
    inst.opcode_fmt_ft_fs_cc_a_fc_cond.fmt = fmt;
    inst.opcode_fmt_ft_fs_cc_a_fc_cond.ft = ft;
    inst.opcode_fmt_ft_fs_cc_a_fc_cond.fs = fs;
    inst.opcode_fmt_ft_fs_cc_a_fc_cond.cc = cc;
    // dmlockhart - HACK: hard-coding fc bits to 0b11 for the  c.<cond>.s
    // instructions in Maven. These are created by the sf_setflag_{vv,vs,sv}
    // functions in order to execute the c.<cond>.s.f.{vv,vs,sv} instructions
    // on the VPs. Since the sf_setflag_{*} functions in maven-Processor.cc
    // are the only places where this set_opcode_fmt_ft_fs_cc_cond(...) method
    // is currently invoked, this should be okay for now.
    // See doc/isa/maven-isa-sketch.txt for the c.<cond>.s encoding.
    //inst.opcode_fmt_ft_fs_cc_a_fc_cond.fc = 0;
    inst.opcode_fmt_ft_fs_cc_a_fc_cond.fc = 0x3;
    inst.opcode_fmt_ft_fs_cc_a_fc_cond.cond = cond;
  }
    
  void set_opcode_fmt_ft_fs_fd_func(uint32_t opcode, uint32_t fmt, uint32_t ft, uint32_t fs, uint32_t fd, uint32_t func)
  {
    inst.opcode_fmt_ft_fs_fd_func.opcode = opcode;
    inst.opcode_fmt_ft_fs_fd_func.fmt    = fmt;
    inst.opcode_fmt_ft_fs_fd_func.ft     = ft;
    inst.opcode_fmt_ft_fs_fd_func.fs     = fs;
    inst.opcode_fmt_ft_fs_fd_func.fd     = fd;
    inst.opcode_fmt_ft_fs_fd_func.func   = func;
  }
 
private:
  typedef struct
  {
    uint32_t func : 6;
    uint32_t sa : 5;
    uint32_t rd : 5;
    uint32_t rt : 5;
    uint32_t rs : 5;
    uint32_t opcode : 6;
  } opcode_rs_rt_rd_sa_func_t;

  typedef struct
  {
    uint32_t func : 6;
    uint32_t code : 20;
    uint32_t opcode : 6;
  } opcode_code_func_t;

  typedef struct
  {
    uint32_t func : 6;
    uint32_t sa : 5;
    uint32_t rd : 5;
    uint32_t td : 1;
    uint32_t nd : 1;
    uint32_t cc : 3;
    uint32_t rs : 5;
    uint32_t opcode : 6;
  } opcode_rs_cc_nd_td_rd_sa_func_t;
         
  typedef struct
  {
    uint32_t cmd    : 8;
    uint32_t _zero  : 3;
    uint32_t rd     : 5;
    uint32_t rt     : 5;
    uint32_t rs     : 5;
    uint32_t opcode : 6;
  } opcode_rs_rt_rd_cmd_t;

  typedef struct
  {
    uint32_t n     : 5;
    uint32_t s     : 2;
    uint32_t u     : 1;
    uint32_t msk   : 3;
    uint32_t rv    : 5;
    uint32_t rt    : 5;
    uint32_t rs    : 5;
    uint32_t opcode : 6;
  } opcode_rs_rt_rv_u_s_n_t;

  typedef struct
  {
    uint32_t immediate : 16;
    uint32_t rt : 5;
    uint32_t rs : 5;
    uint32_t opcode : 6;
  } opcode_rs_rt_immediate_t;

  typedef struct
  {
    uint32_t jump : 26;
    uint32_t opcode : 6;
  } opcode_jump_t;

  typedef struct
  {
    uint32_t func : 6;
    uint32_t fd : 5;
    uint32_t fs : 5;
    uint32_t ft : 5;
    uint32_t fmt : 5;
    uint32_t opcode : 6;
  } opcode_fmt_ft_fs_fd_func_t;

  typedef struct
  {
    uint32_t cond : 4;
    uint32_t fc : 2;
    uint32_t cc : 5;
    uint32_t fs : 5;
    uint32_t ft : 5;
    uint32_t fmt : 5;
    uint32_t opcode : 6;
  } opcode_fmt_ft_fs_cc_a_fc_cond_t;

  union inst_t
  {
    opcode_rs_rt_rd_sa_func_t opcode_rs_rt_rd_sa_func;
    opcode_code_func_t opcode_code_func;
    opcode_rs_cc_nd_td_rd_sa_func_t opcode_rs_cc_nd_td_rd_sa_func;
    opcode_rs_rt_immediate_t opcode_rs_rt_immediate;
    opcode_jump_t opcode_jump;
    opcode_fmt_ft_fs_fd_func_t opcode_fmt_ft_fs_fd_func;
    opcode_fmt_ft_fs_cc_a_fc_cond_t opcode_fmt_ft_fs_cc_a_fc_cond;
    opcode_rs_rt_rd_cmd_t opcode_rs_rt_rd_cmd;
    opcode_rs_rt_rv_u_s_n_t opcode_rs_rt_rv_u_s_n;
    uint32_t bits;
  };

  inst_t inst;
};

typedef Instruction MIPS32Instruction;
typedef Instruction MavenCPInstruction;
typedef Instruction MavenVPInstruction;

#endif // __INSTRUCTION_H
