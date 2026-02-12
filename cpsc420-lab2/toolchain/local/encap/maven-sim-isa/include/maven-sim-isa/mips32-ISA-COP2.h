//
// COP2
//

template <typename InstructionType, typename StateType>
struct mips32_mfc2
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "mfc2 %s,%s", regnames_mips32[inst->get_rt()], regnames_mips32[inst->get_rd()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_cfc2
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "cfc2 %s,%s", regnames_mips32[inst->get_rt()], regnames_mips32[inst->get_rd()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_mfhc2
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "mfhc2 %s,%s", regnames_mips32[inst->get_rt()], regnames_mips32[inst->get_rd()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_mtc2
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "mtc2 %s,%s", regnames_mips32[inst->get_rt()], regnames_mips32[inst->get_rd()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_ctc2
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "ctc2 %s,%s", regnames_mips32[inst->get_rt()], regnames_mips32[inst->get_rd()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_mthc2
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "mthc2 %s,%s", regnames_mips32[inst->get_rt()], regnames_mips32[inst->get_rd()]);
  }
};


//
// COP2 BC2
//

template <typename InstructionType, typename StateType>
struct mips32_bc2f
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "bc2f %d,%d", inst->get_cc(), (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_bc2t
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "bc2t %d,%d", inst->get_cc(), (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_bc2fl
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "bc2fl %d,%d", inst->get_cc(), (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_bc2tl
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "bc2tl %d,%d", inst->get_cc(), (int16_t)inst->get_immediate());
  }
};
