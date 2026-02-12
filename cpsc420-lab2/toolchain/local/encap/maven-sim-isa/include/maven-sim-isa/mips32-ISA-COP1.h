//
// HELPER FUNCTIONS
//

template <typename StateType>
inline void help_convert_float32_to_w(int fs, int fd, int8 rmode, StateType* proc)
{
  if (!proc->fpu.cleanup_state()) return;
  if (!proc->fpu.set_rounding_mode(rmode)) return;

  float32 f = proc->read_register_s(fs);
  int32 i = float32_to_int32(f);

  int8 allowed = float_flag_inexact | float_flag_invalid;
  if (!proc->fpu.handle_exceptions(allowed))
  {
    proc->raise_exception(StateType::EXCEPTION_FP_IEEE754);
    return;
  }

  proc->write_register_w(fd, i);
}

template <typename StateType>
inline void helper_compare_float32(int fs, int ft, uint32_t op, int fcc, StateType* proc)
{
  if (!proc->fpu.cleanup_state()) return;

  float32 f1 = proc->read_register_s(fs);
  float32 f2 = proc->read_register_s(ft);
  bool isf1nan = float32_is_nan(f1);
  bool isf2nan = float32_is_nan(f2);
  bool isf1snan = float32_is_signaling_nan(f1);
  bool isf2snan = float32_is_signaling_nan(f2);
  bool isf1qnan = isf1nan && !isf1snan;
  bool isf2qnan = isf2nan && !isf2snan;
  bool less, equal, unordered;

  if (isf1snan || isf2snan || isf1qnan || isf2qnan)
  {
    less = false;
    equal = false;
    unordered = true;

    if ((isf1snan || isf2snan) || ((op & 0x8) && (isf1qnan || isf2qnan)))
    {
      if (!proc->fpu.signal_exception(float_flag_invalid)) return;

      int8 allowed = float_flag_invalid;
      if (!proc->fpu.handle_exceptions(allowed))
      {
        proc->raise_exception(StateType::EXCEPTION_FP_IEEE754);
        return;
      }
    }
  }
  else
  {
    less = float32_lt(f1, f2);
    equal = float32_eq(f1, f2);
    unordered = false;
  }

  uint32_t condition = ((op & 0x4) && less) || ((op & 0x2) && equal) || ((op & 0x1) & unordered);

  if (proc->flag_mode)
  {
    proc->write_flag(fcc, condition);
  }
  else
  {
    proc->write_register(fcc, condition);
  }
}

template <typename StateType>
inline void help_convert_float64_to_w(int fs, int fd, int8 rmode, StateType* proc)
{
  if (!proc->fpu.cleanup_state()) return;
  if (!proc->fpu.set_rounding_mode(rmode)) return;
  if (!proc->fpu.check_double_aligned(fs))
  {
    proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
    return;
  }

  float64 f = proc->read_register_d(fs);
  int32 i = float64_to_int32(f);

  int8 allowed = float_flag_inexact | float_flag_invalid;
  if (!proc->fpu.handle_exceptions(allowed))
  {
    proc->raise_exception(StateType::EXCEPTION_FP_IEEE754);
    return;
  }

  proc->write_register_w(fd, i);
}

template <typename StateType>
inline void helper_compare_float64(int fs, int ft, uint32_t op, int fcc, StateType* proc)
{
  if (!proc->fpu.cleanup_state()) return;
  if (!proc->fpu.check_double_aligned(fs))
  {
    proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
    return;
  }
  if (!proc->fpu.check_double_aligned(ft))
  {
    proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
    return;
  }

  float64 f1 = proc->read_register_d(fs);
  float64 f2 = proc->read_register_d(ft);
  bool isf1nan = float64_is_nan(f1);
  bool isf2nan = float64_is_nan(f2);
  bool isf1snan = float64_is_signaling_nan(f1);
  bool isf2snan = float64_is_signaling_nan(f2);
  bool isf1qnan = isf1nan && !isf1snan;
  bool isf2qnan = isf2nan && !isf2snan;
  bool less, equal, unordered;

  if (isf1snan || isf2snan || isf1qnan || isf2qnan)
  {
    less = false;
    equal = false;
    unordered = true;

    if ((isf1snan || isf2snan) || ((op & 0x8) && (isf1qnan || isf2qnan)))
    {
      if (!proc->fpu.signal_exception(float_flag_invalid)) return;

      int8 allowed = float_flag_invalid;
      if (!proc->fpu.handle_exceptions(allowed))
      {
        proc->raise_exception(StateType::EXCEPTION_FP_IEEE754);
        return;
      }
    }
  }
  else
  {
    less = float64_lt(f1, f2);
    equal = float64_eq(f1, f2);
    unordered = false;
  }

  uint32_t condition = ((op & 0x4) && less) || ((op & 0x2) && equal) || ((op & 0x1) & unordered);

  if (proc->flag_mode)
  {
    proc->write_flag(fcc, condition);
  }
  else
  {
    proc->write_register(fcc, condition);
  }
}


//
// COP1
//

template <typename InstructionType, typename StateType>
struct mips32_mfc1
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fd() == 0 && inst->get_func() == 0)
    {
      proc->write_register(inst->get_rt(), proc->read_register(inst->get_fs()));
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "mfc1 %s,%s",
      regnames_mips32[inst->get_rt()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_cfc1
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    uint32_t reg;

    switch (inst->get_fs())
    {
    case 0:
      reg = proc->fpu.fir.bits;
      break;
    case 25:
      fccr_t fccr;
      fccr.reg.fcc = (proc->fpu.fcsr.reg.fcc << 1) | proc->fpu.fcsr.reg.fcc0;
      reg = fccr.bits;
      break;
    case 26:
      fexr_t fexr;
      fexr.reg.flags = proc->fpu.fcsr.reg.flags;
      fexr.reg.cause = proc->fpu.fcsr.reg.cause;
      reg = fexr.bits;
      break;
    case 28:
      fenr_t fenr;
      fenr.reg.rm = proc->fpu.fcsr.reg.rm;
      fenr.reg.fs = proc->fpu.fcsr.reg.fs;
      fenr.reg.enables = proc->fpu.fcsr.reg.enables;
      reg = fenr.bits;
      break;
    case 31:
      reg = proc->fpu.fcsr.bits;
      break;
    default:
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
      return;
    }
    proc->write_register(inst->get_rt(), reg);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "cfc1 %s,$%d",
      regnames_mips32[inst->get_rt()],
      inst->get_fs());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_mtc1
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fd() == 0 && inst->get_func() == 0)
    {
      proc->write_register(inst->get_fs(), proc->read_register(inst->get_rt()));
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "mtc1 %s,%s",
      regnames_mips32[inst->get_rt()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_ctc1
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    uint32_t reg = proc->read_register(inst->get_rt());

    switch (inst->get_fs())
    {
    case 25:
      fccr_t fccr;
      fccr.bits = reg;
      fccr.reg._zeros = 0;
      proc->fpu.fcsr.reg.fcc = fccr.reg.fcc >> 1;
      proc->fpu.fcsr.reg.fcc0 = fccr.reg.fcc & 1;
      break;
    case 26:
      fexr_t fexr;
      fexr.bits = reg;
      fexr.reg._zeros0 = 0;
      fexr.reg._zeros1 = 0;
      fexr.reg._zeros2 = 0;
      proc->fpu.fcsr.reg.flags = fexr.reg.flags;
      proc->fpu.fcsr.reg.cause = fexr.reg.cause;
      break;
    case 28:
      fenr_t fenr;
      fenr.bits = reg;
      fenr.reg._zeros0 = 0;
      fenr.reg._zeros1 = 0;
      proc->fpu.fcsr.reg.rm = fenr.reg.rm;
      proc->fpu.fcsr.reg.fs = fenr.reg.fs;
      proc->fpu.fcsr.reg.enables = fenr.reg.enables;
      break;
    case 31:
      proc->fpu.fcsr.bits = reg;
      proc->fpu.fcsr.reg._zeros = 0;
      proc->fpu.fcsr.reg._impl = 0;
      break;
    default:
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
      return;
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "ctc1 %s,$%d",
      regnames_mips32[inst->get_rt()],
      inst->get_fs());
  }
};


//
// COP1 BC1
//

template <typename InstructionType, typename StateType>
struct mips32_bc1f
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_cc() == 0)
    {
      if (proc->fpu.fcsr.reg.fcc0 == 0)
      {
        proc->branch = true;
        proc->pc = proc->npc;
        proc->npc += (int16_t)inst->get_immediate() << 2;
      }
    }
    else
    {
      if ((proc->fpu.fcsr.reg.fcc & (1 << (proc->fpu.fcsr.reg.fcc-1))) == 0)
      {
        proc->branch = true;
        proc->pc = proc->npc;
        proc->npc += (int16_t)inst->get_immediate() << 2;
      }
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_cc() == 0)
    {
      sprintf(proc->disassemble_string, "bc1f %d",
        (int16_t)inst->get_immediate());
    }
    else
    {
      sprintf(proc->disassemble_string, "bc1f %d,%d",
        inst->get_cc(),
        (int16_t)inst->get_immediate());
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_bc1t
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_cc() == 0)
    {
      if (proc->fpu.fcsr.reg.fcc0 == 1)
      {
        proc->branch = true;
        proc->pc = proc->npc;
        proc->npc += (int16_t)inst->get_immediate() << 2;
      }
    }
    else
    {
      if (proc->fpu.fcsr.reg.fcc & (1 << (proc->fpu.fcsr.reg.fcc-1)))
      {
        proc->branch = true;
        proc->pc = proc->npc;
        proc->npc += (int16_t)inst->get_immediate() << 2;
      }
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_cc() == 0)
    {
      sprintf(proc->disassemble_string, "bc1t %d",
        (int16_t)inst->get_immediate());
    }
    else
    {
      sprintf(proc->disassemble_string, "bc1t %d,%d",
        inst->get_cc(),
        (int16_t)inst->get_immediate());
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_bc1fl
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_cc() == 0)
    {
      if (proc->fpu.fcsr.reg.fcc0 == 0)
      {
        proc->branch = true;
        proc->pc = proc->npc;
        proc->npc += (int16_t)inst->get_immediate() << 2;
      }
      else
      {
        proc->nullify = true;
      }
    }
    else
    {
      if ((proc->fpu.fcsr.reg.fcc & (1 << (proc->fpu.fcsr.reg.fcc-1))) == 0)
      {
        proc->branch = true;
        proc->pc = proc->npc;
        proc->npc += (int16_t)inst->get_immediate() << 2;
      }
      else
      {
        proc->nullify = true;
      }
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_cc() == 0)
    {
      sprintf(proc->disassemble_string, "bc1fl %d",
        (int16_t)inst->get_immediate());
    }
    else
    {
      sprintf(proc->disassemble_string, "bc1fl %d,%d",
        inst->get_cc(),
        (int16_t)inst->get_immediate());
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_bc1tl
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_cc() == 0)
    {
      if (proc->fpu.fcsr.reg.fcc0 == 1)
      {
        proc->branch = true;
        proc->pc = proc->npc;
        proc->npc += (int16_t)inst->get_immediate() << 2;
      }
      else
      {
        proc->nullify = true;
      }
    }
    else
    {
      if (proc->fpu.fcsr.reg.fcc & (1 << (proc->fpu.fcsr.reg.fcc-1)))
      {
        proc->branch = true;
        proc->pc = proc->npc;
        proc->npc += (int16_t)inst->get_immediate() << 2;
      }
      else
      {
        proc->nullify = true;
      }
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_cc() == 0)
    {
      sprintf(proc->disassemble_string, "bc1tl %d",
        (int16_t)inst->get_immediate());
    }
    else
    {
      sprintf(proc->disassemble_string, "bc1tl %d,%d",
        inst->get_cc(),
        (int16_t)inst->get_immediate());
    }
  }
};


//
// COP1 S
//

template <typename InstructionType, typename StateType>
struct mips32_add_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (!proc->fpu.cleanup_state()) return;

    float32 f1 = proc->read_register_s(inst->get_fs());
    float32 f2 = proc->read_register_s(inst->get_ft());
    float32 f = float32_add(f1, f2);

    int8 allowed = float_flag_inexact | float_flag_underflow | float_flag_overflow | float_flag_invalid;
    if (!proc->fpu.handle_exceptions(allowed))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_IEEE754);
      return;
    }

    proc->write_register_s(inst->get_fd(), f);

    // microarch state
    proc->alu = ALU_FADDSUB;
    proc->rs = inst->get_fs();
    proc->rt = inst->get_ft();
    proc->rd = inst->get_fd();
    proc->b_rs = true;
    proc->b_rt = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "add.s %s,%s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()],
      regnames_cop1_mips32[inst->get_ft()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_sub_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (!proc->fpu.cleanup_state()) return;

    float32 f1 = proc->read_register_s(inst->get_fs());
    float32 f2 = proc->read_register_s(inst->get_ft());
    float32 f = float32_sub(f1, f2);

    int8 allowed = float_flag_inexact | float_flag_underflow | float_flag_overflow | float_flag_invalid;
    if (!proc->fpu.handle_exceptions(allowed))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_IEEE754);
      return;
    }

    proc->write_register_s(inst->get_fd(), f);

    // microarch state
    proc->alu = ALU_FADDSUB;
    proc->rs = inst->get_fs();
    proc->rt = inst->get_ft();
    proc->rd = inst->get_fd();
    proc->b_rs = true;
    proc->b_rt = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "sub.s %s,%s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()],
      regnames_cop1_mips32[inst->get_ft()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_mul_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (!proc->fpu.cleanup_state()) return;

    float32 f1 = proc->read_register_s(inst->get_fs());
    float32 f2 = proc->read_register_s(inst->get_ft());
    float32 f = float32_mul(f1, f2);

    int8 allowed = float_flag_inexact | float_flag_underflow | float_flag_overflow | float_flag_invalid;
    if (!proc->fpu.handle_exceptions(allowed))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_IEEE754);
      return;
    }

    proc->write_register_s(inst->get_fd(), f);

    // microarch state
    proc->alu = ALU_FMUL;
    proc->rs = inst->get_fs();
    proc->rt = inst->get_ft();
    proc->rd = inst->get_fd();
    proc->b_rs = true;
    proc->b_rt = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "mul.s %s,%s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()],
      regnames_cop1_mips32[inst->get_ft()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_div_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (!proc->fpu.cleanup_state()) return;

    float32 f1 = proc->read_register_s(inst->get_fs());
    float32 f2 = proc->read_register_s(inst->get_ft());
    float32 f = float32_div(f1, f2);

    int8 allowed = float_flag_inexact | float_flag_underflow | float_flag_overflow | float_flag_divbyzero | float_flag_invalid;
    if (!proc->fpu.handle_exceptions(allowed))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_IEEE754);
      return;
    }

    proc->write_register_s(inst->get_fd(), f);

    // microarch state
    proc->alu = ALU_FDIV;
    proc->rs = inst->get_fs();
    proc->rt = inst->get_ft();
    proc->rd = inst->get_fd();
    proc->b_rs = true;
    proc->b_rt = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "div.s %s,%s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()],
      regnames_cop1_mips32[inst->get_ft()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_sqrt_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_ft() == 0)
    {
      if (!proc->fpu.cleanup_state()) return;

      float32 f1 = proc->read_register_s(inst->get_fs());
      float32 f = float32_sqrt(f1);

      int8 allowed = float_flag_inexact | float_flag_invalid;
      if (!proc->fpu.handle_exceptions(allowed))
      {
        proc->raise_exception(StateType::EXCEPTION_FP_IEEE754);
        return;
      }

      proc->write_register_s(inst->get_fd(), f);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->alu = ALU_FSQRT;
    proc->rs = inst->get_fs();
    proc->rd = inst->get_fd();
    proc->b_rs = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "sqrt.s %s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_abs_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_ft() == 0)
    {
      if (!proc->fpu.cleanup_state()) return;

      float32 f1 = proc->read_register_s(inst->get_fs());
      float32 f = f1 & ~(0x80000000);

      proc->write_register_s(inst->get_fd(), f);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "abs.s %s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_mov_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_ft() == 0)
    {
      if (!proc->fpu.cleanup_state()) return;

      float32 f = proc->read_register_s(inst->get_fs());

      proc->write_register_s(inst->get_fd(), f);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "mov.s %s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_neg_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_ft() == 0)
    {
      if (!proc->fpu.cleanup_state()) return;

      float32 f1 = proc->read_register_s(inst->get_fs());
      float32 f = f1 ^ (0x80000000);

      proc->write_register_s(inst->get_fd(), f);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "neg.s %s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_round_w_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_ft() == 0)
    {
      help_convert_float32_to_w<StateType>(inst->get_fs(), inst->get_fd(), float_round_nearest_even, proc);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "round.w.s %s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_trunc_w_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_ft() == 0)
    {
      help_convert_float32_to_w<StateType>(inst->get_fs(), inst->get_fd(), float_round_to_zero, proc);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "trunc.w.s %s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_ceil_w_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_ft() == 0)
    {
      help_convert_float32_to_w<StateType>(inst->get_fs(), inst->get_fd(), float_round_up, proc);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "ceil.w.s %s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_floor_w_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_ft() == 0)
    {
      help_convert_float32_to_w<StateType>(inst->get_fs(), inst->get_fd(), float_round_down, proc);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "floor.w.s %s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_movf_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (!proc->fpu.cleanup_state()) return;

    uint32_t fcc = inst->get_cc();

    if ((fcc && (proc->fpu.fcsr.reg.fcc << (fcc-1)) == 0) || proc->fpu.fcsr.reg.fcc0 == 0)
    {
      float32 f = proc->read_register_s(inst->get_fs());

      proc->write_register_s(inst->get_fd(), f);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "movf.s %s,%s,%d",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()],
      inst->get_cc());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_movt_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (!proc->fpu.cleanup_state()) return;

    uint32_t fcc = inst->get_cc();

    if ((fcc && (proc->fpu.fcsr.reg.fcc << (fcc-1))) || proc->fpu.fcsr.reg.fcc0 == 1)
    {
      float32 f = proc->read_register_s(inst->get_fs());

      proc->write_register_s(inst->get_fd(), f);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "movt.s %s,%s,%d",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()],
      inst->get_cc());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_movz_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (!proc->fpu.cleanup_state()) return;

    if (proc->read_register(inst->get_rt()) == 0)
    {
      float32 f = proc->read_register_s(inst->get_fs());

      proc->write_register_s(inst->get_fd(), f);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "movz.s %s,%s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()],
      regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_movn_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (!proc->fpu.cleanup_state()) return;

    if (proc->read_register(inst->get_rt()))
    {
      float32 f = proc->read_register_s(inst->get_fs());

      proc->write_register_s(inst->get_fd(), f);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "movn.s %s,%s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()],
      regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_cvt_d_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_ft() == 0)
    {
      if (!proc->fpu.cleanup_state()) return;
      if (!proc->fpu.check_double_aligned(inst->get_fd()))
      {
        proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
        return;
      }

      float32 f1 = proc->read_register_s(inst->get_fs());
      float64 f = float32_to_float64(f1);

      int8 allowed = float_flag_invalid;
      if (!proc->fpu.handle_exceptions(allowed))
      {
        proc->raise_exception(StateType::EXCEPTION_FP_IEEE754);
        return;
      }

      proc->write_register_d(inst->get_fd(), f);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "cvt.d.s %s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_cvt_w_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_ft() == 0)
    {
      if (!proc->fpu.cleanup_state()) return;

      float32 f = proc->read_register_s(inst->get_fs());
      int32 i = float32_to_int32(f);

      int8 allowed = float_flag_inexact | float_flag_invalid;
      if (!proc->fpu.handle_exceptions(allowed))
      {
        proc->raise_exception(StateType::EXCEPTION_FP_IEEE754);
        return;
      }

      proc->write_register_w(inst->get_fd(), i);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "cvt.w.s %s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_f_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float32<StateType>(inst->get_fs(), inst->get_ft(), 0x0, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.f.s %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.f.s %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_un_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float32<StateType>(inst->get_fs(), inst->get_ft(), 0x1, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.un.s %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.un.s %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_eq_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float32<StateType>(inst->get_fs(), inst->get_ft(), 0x2, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.eq.s %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.eq.s %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_ueq_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float32<StateType>(inst->get_fs(), inst->get_ft(), 0x3, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.ueq.s %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.ueq.s %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_olt_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float32<StateType>(inst->get_fs(), inst->get_ft(), 0x4, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.olt.s %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.olt.s %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_ult_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float32<StateType>(inst->get_fs(), inst->get_ft(), 0x5, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.ult.s %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.ult.s %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_ole_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float32<StateType>(inst->get_fs(), inst->get_ft(), 0x6, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.ole.s %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.ole.s %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_ule_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float32<StateType>(inst->get_fs(), inst->get_ft(), 0x7, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.ule.s %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.ule.s %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_sf_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float32<StateType>(inst->get_fs(), inst->get_ft(), 0x8, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.sf.s %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.sf.s %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_ngle_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float32<StateType>(inst->get_fs(), inst->get_ft(), 0x9, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.ngle.s %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.ngle.s %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_seq_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float32<StateType>(inst->get_fs(), inst->get_ft(), 0xA, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.seq.s %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.seq.s %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_ngl_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float32<StateType>(inst->get_fs(), inst->get_ft(), 0xB, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.ngl.s %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.ngl.s %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_lt_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float32<StateType>(inst->get_fs(), inst->get_ft(), 0xC, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.lt.s %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.lt.s %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_nge_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float32<StateType>(inst->get_fs(), inst->get_ft(), 0xD, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.nge.s %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.nge.s %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_le_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float32<StateType>(inst->get_fs(), inst->get_ft(), 0xE, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.le.s %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.le.s %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_ngt_s
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float32<StateType>(inst->get_fs(), inst->get_ft(), 0xF, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.ngt.s %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.ngt.s %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};


//
// COP1 D
//

template <typename InstructionType, typename StateType>
struct mips32_add_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (!proc->fpu.cleanup_state()) return;
    if (!proc->fpu.check_double_aligned(inst->get_fs()))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
      return;
    }
    if (!proc->fpu.check_double_aligned(inst->get_ft()))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
      return;
    }
    if (!proc->fpu.check_double_aligned(inst->get_fd()))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
      return;
    }

    float64 f1 = proc->read_register_d(inst->get_fs());
    float64 f2 = proc->read_register_d(inst->get_ft());
    float64 f = float64_add(f1, f2);

    int8 allowed = float_flag_inexact | float_flag_underflow | float_flag_overflow | float_flag_invalid;
    if (!proc->fpu.handle_exceptions(allowed))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_IEEE754);
      return;
    }

    proc->write_register_d(inst->get_fd(), f);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "add.d %s,%s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()],
      regnames_cop1_mips32[inst->get_ft()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_sub_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (!proc->fpu.cleanup_state()) return;
    if (!proc->fpu.check_double_aligned(inst->get_fs()))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
      return;
    }
    if (!proc->fpu.check_double_aligned(inst->get_ft()))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
      return;
    }
    if (!proc->fpu.check_double_aligned(inst->get_fd()))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
      return;
    }

    float64 f1 = proc->read_register_d(inst->get_fs());
    float64 f2 = proc->read_register_d(inst->get_ft());
    float64 f = float64_sub(f1, f2);

    int8 allowed = float_flag_inexact | float_flag_underflow | float_flag_overflow | float_flag_invalid;
    if (!proc->fpu.handle_exceptions(allowed))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_IEEE754);
      return;
    }

    proc->write_register_d(inst->get_fd(), f);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "sub.d %s,%s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()],
      regnames_cop1_mips32[inst->get_ft()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_mul_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (!proc->fpu.cleanup_state()) return;
    if (!proc->fpu.check_double_aligned(inst->get_fs()))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
      return;
    }
    if (!proc->fpu.check_double_aligned(inst->get_ft()))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
      return;
    }
    if (!proc->fpu.check_double_aligned(inst->get_fd()))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
      return;
    }

    float64 f1 = proc->read_register_d(inst->get_fs());
    float64 f2 = proc->read_register_d(inst->get_ft());
    float64 f = float64_mul(f1, f2);

    int8 allowed = float_flag_inexact | float_flag_underflow | float_flag_overflow | float_flag_invalid;
    if (!proc->fpu.handle_exceptions(allowed))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_IEEE754);
      return;
    }

    proc->write_register_d(inst->get_fd(), f);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "mul.d %s,%s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()],
      regnames_cop1_mips32[inst->get_ft()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_div_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (!proc->fpu.cleanup_state()) return;
    if (!proc->fpu.check_double_aligned(inst->get_fs()))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
      return;
    }
    if (!proc->fpu.check_double_aligned(inst->get_ft()))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
      return;
    }
    if (!proc->fpu.check_double_aligned(inst->get_fd()))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
      return;
    }

    float64 f1 = proc->read_register_d(inst->get_fs());
    float64 f2 = proc->read_register_d(inst->get_ft());
    float64 f = float64_div(f1, f2);

    int8 allowed = float_flag_inexact | float_flag_underflow | float_flag_overflow | float_flag_divbyzero | float_flag_invalid;
    if (!proc->fpu.handle_exceptions(allowed))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_IEEE754);
      return;
    }

    proc->write_register_d(inst->get_fd(), f);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "div.d %s,%s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()],
      regnames_cop1_mips32[inst->get_ft()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_sqrt_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_ft() == 0)
    {
      if (!proc->fpu.cleanup_state()) return;
      if (!proc->fpu.check_double_aligned(inst->get_fs()))
      {
        proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
        return;
      }
      if (!proc->fpu.check_double_aligned(inst->get_fd()))
      {
        proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
        return;
      }

      float64 f1 = proc->read_register_d(inst->get_fs());
      float64 f = float64_sqrt(f1);

      int8 allowed = float_flag_inexact | float_flag_invalid;
      if (!proc->fpu.handle_exceptions(allowed))
      {
        proc->raise_exception(StateType::EXCEPTION_FP_IEEE754);
        return;
      }

      proc->write_register_d(inst->get_fd(), f);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "sqrt.d %s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_abs_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_ft() == 0)
    {
      if (!proc->fpu.cleanup_state()) return;
      if (!proc->fpu.check_double_aligned(inst->get_fs()))
      {
        proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
        return;
      }
      if (!proc->fpu.check_double_aligned(inst->get_fd()))
      {
        proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
        return;
      }

      float64 f1 = proc->read_register_d(inst->get_fs());
      float64 f = f1 & ~(0x8000000000000000LL);

      proc->write_register_d(inst->get_fd(), f);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "abs.d %s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_mov_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_ft() == 0)
    {
      if (!proc->fpu.cleanup_state()) return;
      if (!proc->fpu.check_double_aligned(inst->get_fs()))
      {
        proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
        return;
      }
      if (!proc->fpu.check_double_aligned(inst->get_fd()))
      {
        proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
        return;
      }

      float64 f = proc->read_register_d(inst->get_fs());

      proc->write_register_d(inst->get_fd(), f);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "mov.d %s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_neg_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_ft() == 0)
    {
      if (!proc->fpu.cleanup_state()) return;
      if (!proc->fpu.check_double_aligned(inst->get_fs()))
      {
        proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
        return;
      }
      if (!proc->fpu.check_double_aligned(inst->get_fd()))
      {
        proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
        return;
      }

      float64 f1 = proc->read_register_d(inst->get_fs());
      float64 f = f1 ^ (0x8000000000000000LL);

      proc->write_register_d(inst->get_fd(), f);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "neg.d %s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_round_w_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_ft() == 0)
    {
      help_convert_float64_to_w<StateType>(inst->get_fs(), inst->get_fd(), float_round_nearest_even, proc);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "round.w.d %s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_trunc_w_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_ft() == 0)
    {
      help_convert_float64_to_w<StateType>(inst->get_fs(), inst->get_fd(), float_round_to_zero, proc);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "trunc.w.d %s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_ceil_w_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_ft() == 0)
    {
      help_convert_float64_to_w<StateType>(inst->get_fs(), inst->get_fd(), float_round_up, proc);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "ceil.w.d %s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_floor_w_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_ft() == 0)
    {
      help_convert_float64_to_w<StateType>(inst->get_fs(), inst->get_fd(), float_round_down, proc);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "floor.w.d %s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_movf_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (!proc->fpu.cleanup_state()) return;
    if (!proc->fpu.check_double_aligned(inst->get_fs()))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
      return;
    }
    if (!proc->fpu.check_double_aligned(inst->get_fd()))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
      return;
    }

    uint32_t fcc = inst->get_cc();

    if ((fcc && (proc->fpu.fcsr.reg.fcc << (fcc-1)) == 0) || proc->fpu.fcsr.reg.fcc0 == 0)
    {
      float64 f = proc->read_register_d(inst->get_fs());

      proc->write_register_d(inst->get_fd(), f);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "movf.d %s,%s,%d",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()],
      inst->get_cc());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_movt_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (!proc->fpu.cleanup_state()) return;
    if (!proc->fpu.check_double_aligned(inst->get_fs()))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
      return;
    }
    if (!proc->fpu.check_double_aligned(inst->get_fd()))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
      return;
    }

    uint32_t fcc = inst->get_cc();

    if ((fcc && (proc->fpu.fcsr.reg.fcc << (fcc-1))) || proc->fpu.fcsr.reg.fcc0 == 1)
    {
      float64 f = proc->read_register_d(inst->get_fs());

      proc->write_register_d(inst->get_fd(), f);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "movt.d %s,%s,%d",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()],
      inst->get_cc());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_movz_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (!proc->fpu.cleanup_state()) return;
    if (!proc->fpu.check_double_aligned(inst->get_fs()))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
      return;
    }
    if (!proc->fpu.check_double_aligned(inst->get_fd()))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
      return;
    }

    if (proc->read_register(inst->get_rt()) == 0)
    {
      float64 f = proc->read_register_d(inst->get_fs());

      proc->write_register_d(inst->get_fd(), f);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "movz.d %s,%s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()],
      regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_movn_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (!proc->fpu.cleanup_state()) return;
    if (!proc->fpu.check_double_aligned(inst->get_fs()))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
      return;
    }
    if (!proc->fpu.check_double_aligned(inst->get_fd()))
    {
      proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
      return;
    }

    if (proc->read_register(inst->get_rt()))
    {
      float64 f = proc->read_register_d(inst->get_fs());

      proc->write_register_d(inst->get_fd(), f);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "movn.d %s,%s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()],
      regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_cvt_s_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_ft() == 0)
    {
      if (!proc->fpu.cleanup_state()) return;
      if (!proc->fpu.check_double_aligned(inst->get_fs()))
      {
        proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
        return;
      }

      float64 f1 = proc->read_register_d(inst->get_fs());
      float32 f = float64_to_float32(f1);

      int8 allowed = float_flag_inexact | float_flag_underflow | float_flag_overflow | float_flag_invalid;
      if (!proc->fpu.handle_exceptions(allowed))
      {
        proc->raise_exception(StateType::EXCEPTION_FP_IEEE754);
        return;
      }

      proc->write_register_s(inst->get_fd(), f);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "cvt.s.d %s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_cvt_w_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_ft() == 0)
    {
      if (!proc->fpu.cleanup_state()) return;
      if (!proc->fpu.check_double_aligned(inst->get_fs()))
      {
        proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
        return;
      }

      float64 f = proc->read_register_d(inst->get_fs());
      int32 i = float64_to_int32(f);

      int8 allowed = float_flag_inexact | float_flag_invalid;
      if (!proc->fpu.handle_exceptions(allowed))
      {
        proc->raise_exception(StateType::EXCEPTION_FP_IEEE754);
        return;
      }

      proc->write_register_w(inst->get_fd(), i);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "cvt.w.d %s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_f_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float64<StateType>(inst->get_fs(), inst->get_ft(), 0x0, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.f.d %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.f.d %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_un_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float64<StateType>(inst->get_fs(), inst->get_ft(), 0x1, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.un.d %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.un.d %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_eq_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float64<StateType>(inst->get_fs(), inst->get_ft(), 0x2, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.eq.d %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.eq.d %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_ueq_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float64<StateType>(inst->get_fs(), inst->get_ft(), 0x3, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.ueq.d %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.ueq.d %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_olt_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float64<StateType>(inst->get_fs(), inst->get_ft(), 0x4, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.olt.d %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.olt.d %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_ult_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float64<StateType>(inst->get_fs(), inst->get_ft(), 0x5, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.ult.d %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.ult.d %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_ole_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float64<StateType>(inst->get_fs(), inst->get_ft(), 0x6, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.ole.d %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.ole.d %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_ule_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float64<StateType>(inst->get_fs(), inst->get_ft(), 0x7, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.ule.d %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.ule.d %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_sf_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float64<StateType>(inst->get_fs(), inst->get_ft(), 0x8, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.sf.d %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.sf.d %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_ngle_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float64<StateType>(inst->get_fs(), inst->get_ft(), 0x9, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.ngle.d %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.ngle.d %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_seq_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float64<StateType>(inst->get_fs(), inst->get_ft(), 0xA, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.seq.d %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.seq.d %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_ngl_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float64<StateType>(inst->get_fs(), inst->get_ft(), 0xB, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.ngl.d %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.ngl.d %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_lt_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float64<StateType>(inst->get_fs(), inst->get_ft(), 0xC, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.lt.d %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.lt.d %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_nge_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float64<StateType>(inst->get_fs(), inst->get_ft(), 0xD, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.nge.d %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.nge.d %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_le_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float64<StateType>(inst->get_fs(), inst->get_ft(), 0xE, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.f.d %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.f.d %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_c_ngt_d
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    helper_compare_float64<StateType>(inst->get_fs(), inst->get_ft(), 0xF, inst->get_fcc(), proc);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_fcc() == 0)
    {
      sprintf(proc->disassemble_string, "c.f.d %s,%s",
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
    else
    {
      sprintf(proc->disassemble_string, "c.f.d %d,%s,%s",
        inst->get_fcc(),
        regnames_cop1_mips32[inst->get_fs()],
        regnames_cop1_mips32[inst->get_ft()]);
    }
  }
};


//
// COP1 W
//

template <typename InstructionType, typename StateType>
struct mips32_cvt_s_w
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_ft() == 0)
    {
      if (!proc->fpu.cleanup_state()) return;

      int32 i = proc->read_register_w(inst->get_fs());
      float32 f = int32_to_float32(i);

      int8 allowed = float_flag_inexact;
      if (!proc->fpu.handle_exceptions(allowed))
      {
        proc->raise_exception(StateType::EXCEPTION_FP_IEEE754);
        return;
      }

      proc->write_register_s(inst->get_fd(), f);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "cvt.s.w %s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_cvt_d_w
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_ft() == 0)
    {
      if (!proc->fpu.cleanup_state()) return;
      if (!proc->fpu.check_double_aligned(inst->get_fd()))
      {
        proc->raise_exception(StateType::EXCEPTION_FP_MISALIGNED);
        return;
      }

      int32 i = proc->read_register_w(inst->get_fs());
      float64 f = int32_to_float64(i);

      proc->write_register_d(inst->get_fd(), f);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "c.d.w %s,%s",
      regnames_cop1_mips32[inst->get_fd()],
      regnames_cop1_mips32[inst->get_fs()]);
  }
};
