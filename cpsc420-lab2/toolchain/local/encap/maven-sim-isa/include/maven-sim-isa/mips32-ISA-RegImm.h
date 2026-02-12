//
// REGIMM
//

template <typename InstructionType, typename StateType>
struct mips32_bltz
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if ((int32_t)proc->read_register(inst->get_rs()) < 0)
    {
      proc->branch = true;
      proc->pc = proc->npc;
      proc->npc += (int16_t)inst->get_immediate() << 2;
    }

    // microarch state
    proc->inst_branch = true;
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->b_rs = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "bltz %s,%d", regnames_mips32[inst->get_rs()], (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_bgez
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if ((int32_t)proc->read_register(inst->get_rs()) >= 0)
    {
      proc->branch = true;
      proc->pc = proc->npc;
      proc->npc += (int16_t)inst->get_immediate() << 2;
    }

    // microarch state
    proc->inst_branch = true;
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->b_rs = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "bgez %s,%d", regnames_mips32[inst->get_rs()], (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_bltzl
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if ((int32_t)proc->read_register(inst->get_rs()) < 0)
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
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "bltzl %s,%d", regnames_mips32[inst->get_rs()], (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_bgezl
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if ((int32_t)proc->read_register(inst->get_rs()) >= 0)
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
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "bgezl %s,%d", regnames_mips32[inst->get_rs()], (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_tgei
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if ((int32_t)proc->read_register(inst->get_rs()) >= (int16_t)inst->get_immediate())
    {
      proc->raise_exception(StateType::EXCEPTION_TRAP);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "tgei %s,%d", regnames_mips32[inst->get_rs()], (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_tgeiu
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (proc->read_register(inst->get_rs()) >= (uint32_t)(int16_t)inst->get_immediate())
    {
      proc->raise_exception(StateType::EXCEPTION_TRAP);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "tgeiu %s,%d", regnames_mips32[inst->get_rs()], (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_tlti
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if ((int32_t)proc->read_register(inst->get_rs()) < (int16_t)inst->get_immediate())
    {
      proc->raise_exception(StateType::EXCEPTION_TRAP);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "tlti %s,%d", regnames_mips32[inst->get_rs()], (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_tltiu
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (proc->read_register(inst->get_rs()) < (uint32_t)(int16_t)inst->get_immediate())
    {
      proc->raise_exception(StateType::EXCEPTION_TRAP);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "tltiu %s,%d", regnames_mips32[inst->get_rs()], (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_teqi
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if ((int32_t)proc->read_register(inst->get_rs()) == (int16_t)inst->get_immediate())
    {
      proc->raise_exception(StateType::EXCEPTION_TRAP);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "teqi %s,%d", regnames_mips32[inst->get_rs()], (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_tnei
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if ((int32_t)proc->read_register(inst->get_rs()) != (int16_t)inst->get_immediate())
    {
      proc->raise_exception(StateType::EXCEPTION_TRAP);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "tnei %s,%d", regnames_mips32[inst->get_rs()], (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_bltzal
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if ((int32_t)proc->read_register(inst->get_rs()) < 0)
    {
      proc->branch = true;
      proc->write_register(31, proc->pc + 8);
      proc->pc = proc->npc;
      proc->npc += (int16_t)inst->get_immediate() << 2;
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "bltzal %s,%d", regnames_mips32[inst->get_rs()], (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_bgezal
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if ((int32_t)proc->read_register(inst->get_rs()) >= 0)
    {
      proc->branch = true;
      proc->write_register(31, proc->pc + 8);
      proc->pc = proc->npc;
      proc->npc += (int16_t)inst->get_immediate() << 2;
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "bgezal %s,%d", regnames_mips32[inst->get_rs()], (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_bltzall
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if ((int32_t)proc->read_register(inst->get_rs()) < 0)
    {
      proc->branch = true;
      proc->write_register(31, proc->pc + 8);
      proc->pc = proc->npc;
      proc->npc += (int16_t)inst->get_immediate() << 2;
    }
    else
    {
      proc->nullify = true;
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "bltzall %s,%d", regnames_mips32[inst->get_rs()], (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_bgezall
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if ((int32_t)proc->read_register(inst->get_rs()) >= 0)
    {
      proc->branch = true;
      proc->write_register(31, proc->pc + 8);
      proc->pc = proc->npc;
      proc->npc += (int16_t)inst->get_immediate() << 2;
    }
    else
    {
      proc->nullify = true;
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "bgezall %s,%d", regnames_mips32[inst->get_rs()], (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_synci
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "synci %d(%s)", (int16_t)inst->get_immediate(), regnames_mips32[inst->get_rs()]);
  }
};

