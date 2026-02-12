//
// SPECIAL2
//

template <typename InstructionType, typename StateType>
struct mips32_madd
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "madd %s,%s", regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_maddu
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "maddu %s,%s", regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_mul
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_sa() == 0)
    {
      int64_t rs = (int64_t)proc->read_register(inst->get_rs());
      int64_t rt = (int64_t)proc->read_register(inst->get_rt());
      uint64_t product = rs * rt;

      proc->write_register(inst->get_rd(), (uint32_t)product);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->alu = ALU_IMUL;
    proc->rs = inst->get_rs();
    proc->rt = inst->get_rt();
    proc->rd = inst->get_rd();
    proc->b_rs = true;
    proc->b_rt = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "mul %s,%s,%s", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_msub
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "msub %s,%s", regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_msubu
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "msubu %s,%s", regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_clz
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_rt() == inst->get_rd())
    {
      int cnt = 0;
      reg_t rs = proc->read_register(inst->get_rs());

      for (int i=0; i<32; i++)
      {
        if (rs & 0x80000000)
        {
          break;
        }

        rs = rs << 1;
        cnt++;
      }

      proc->write_register(inst->get_rd(), cnt);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "clz %s,%s", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_clo
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_rt() == inst->get_rd())
    {
      int cnt = 0;
      reg_t rs = proc->read_register(inst->get_rs());

      for (int i=0; i<32; i++)
      {
        if (!(rs & 0x80000000))
        {
          break;
        }

        rs = rs << 1;
        cnt++;
      }

      proc->write_register(inst->get_rd(), cnt);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "clo %s,%s", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rs()]);
  }
};
