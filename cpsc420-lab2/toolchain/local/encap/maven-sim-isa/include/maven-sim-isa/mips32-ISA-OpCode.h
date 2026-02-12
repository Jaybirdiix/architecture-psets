//
// ILLEGAL
//

template <typename InstructionType, typename StateType>
struct mips32_illegal
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "mips32 illegal opcode");
  }
};


//
// OPCODE
//

template <typename InstructionType, typename StateType>
struct mips32_j
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->branch = true;
    proc->pc = proc->npc;
    proc->npc = (proc->npc & 0xf0000000) | (inst->get_jump() << 2);

    // microarch state
    proc->inst_jump = true;
    proc->alu = ALU_INT;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "j %d",
      inst->get_jump());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_jal
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->branch = true;
    proc->write_register(31, proc->pc + 4);
    proc->pc = proc->npc;
    proc->npc = (proc->npc & 0xf0000000) | (inst->get_jump() << 2);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "jal %d",
      inst->get_jump());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_beq
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (proc->read_register(inst->get_rs()) == proc->read_register(inst->get_rt()))
    {
      proc->branch = true;
      proc->pc = proc->npc;
      proc->npc += (int16_t)inst->get_immediate() << 2;
    }

    // microarch state
    proc->inst_branch = true;
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rt = inst->get_rt();
    proc->b_rs = true;
    proc->b_rt = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "beq %s,%s,%d",
      regnames_mips32[inst->get_rs()],
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_bne
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (proc->read_register(inst->get_rs()) != proc->read_register(inst->get_rt()))
    {
      proc->branch = true;
      proc->pc = proc->npc;
      proc->npc += (int16_t)inst->get_immediate() << 2;
    }

    // microarch state
    proc->inst_branch = true;
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rt = inst->get_rt();
    proc->b_rs = true;
    proc->b_rt = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "bne %s,%s,%d",
      regnames_mips32[inst->get_rs()],
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_blez
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_rt() == 0)
    {
      if ((int32_t)proc->read_register(inst->get_rs()) <= 0)
      {
        proc->branch = true;
        proc->pc = proc->npc;
        proc->npc += (int16_t)inst->get_immediate() << 2;
      }
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->inst_branch = true;
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->b_rs = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "blez %s,%d",
      regnames_mips32[inst->get_rs()],
      (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_bgtz
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_rt() == 0)
    {
      if ((int32_t)proc->read_register(inst->get_rs()) > 0)
      {
        proc->branch = true;
        proc->pc = proc->npc;
        proc->npc += (int16_t)inst->get_immediate() << 2;
      }
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->inst_branch = true;
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->b_rs = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "bgtz %s,%d",
      regnames_mips32[inst->get_rs()],
      (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_addi
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    reg_t rs = proc->read_register(inst->get_rs());
    reg_t imm = (int16_t)inst->get_immediate();
    reg_t sum = rs + imm;

    if (is_overflow_add(sum, rs, imm))
    {
      proc->raise_exception(StateType::EXCEPTION_ALU_OVERFLOW);
    }
    else
    {
      proc->write_register(inst->get_rt(), sum);
    }

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rd = inst->get_rt();
    proc->b_rs = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "addi %s,%s,%d",
      regnames_mips32[inst->get_rt()],
      regnames_mips32[inst->get_rs()],
      (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_addiu
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->write_register(inst->get_rt(), proc->read_register(inst->get_rs()) + (int16_t)inst->get_immediate());

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rd = inst->get_rt();
    proc->b_rs = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "addiu %s,%s,%d",
      regnames_mips32[inst->get_rt()],
      regnames_mips32[inst->get_rs()],
      (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_slti
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->write_register(inst->get_rt(), ((int32_t)proc->read_register(inst->get_rs()) < (int16_t)inst->get_immediate()) ? 1 : 0);

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rd = inst->get_rt();
    proc->b_rs = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "slti %s,%s,%d",
      regnames_mips32[inst->get_rt()],
      regnames_mips32[inst->get_rs()],
      (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_sltiu
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->write_register(inst->get_rt(), (proc->read_register(inst->get_rs()) < (uint32_t)(int16_t)inst->get_immediate()) ? 1 : 0);

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rd = inst->get_rt();
    proc->b_rs = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "sltiu %s,%s,%d",
      regnames_mips32[inst->get_rt()],
      regnames_mips32[inst->get_rs()],
      (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_andi
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->write_register(inst->get_rt(), proc->read_register(inst->get_rs()) & inst->get_immediate());

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rd = inst->get_rt();
    proc->b_rs = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "andi %s,%s,%04x",
      regnames_mips32[inst->get_rt()],
      regnames_mips32[inst->get_rs()],
      inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_ori
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->write_register(inst->get_rt(), proc->read_register(inst->get_rs()) | inst->get_immediate());

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rd = inst->get_rt();
    proc->b_rs = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "ori %s,%s,%04x",
      regnames_mips32[inst->get_rt()],
      regnames_mips32[inst->get_rs()],
      inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_xori
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->write_register(inst->get_rt(), proc->read_register(inst->get_rs()) ^ inst->get_immediate());

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rd = inst->get_rt();
    proc->b_rs = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "xori %s,%s,%04x",
      regnames_mips32[inst->get_rt()],
      regnames_mips32[inst->get_rs()],
      inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_lui
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_rs() == 0)
    {
      proc->write_register(inst->get_rt(), inst->get_immediate() << 16);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->alu = ALU_INT;
    proc->rd = inst->get_rt();
    proc->b_rs = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "lui %s,%d",
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_beql
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (proc->read_register(inst->get_rs()) == proc->read_register(inst->get_rt()))
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
    sprintf(proc->disassemble_string, "beql %s,%s,%d",
      regnames_mips32[inst->get_rs()],
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_bnel
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (proc->read_register(inst->get_rs()) != proc->read_register(inst->get_rt()))
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
    sprintf(proc->disassemble_string, "bnel %s,%s,%d",
      regnames_mips32[inst->get_rs()],
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_blezl
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_rt() == 0)
    {
      if ((int32_t)proc->read_register(inst->get_rs()) <= 0)
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
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "blezl %s,%s,%d",
      regnames_mips32[inst->get_rs()],
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_bgtzl
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_rt() == 0)
    {
      if ((int32_t)proc->read_register(inst->get_rs()) > 0)
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
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "bgtzl %s,%s,%d",
      regnames_mips32[inst->get_rs()],
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_lb
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    addr_t addr = proc->read_register(inst->get_rs()) + (int16_t)inst->get_immediate();

    proc->write_register(inst->get_rt(), proc->mem.read_mem_int8(addr));

    // microarch state
    proc->alu = ALU_LD;
    proc->rs = inst->get_rs();
    proc->rd = inst->get_rt();
    proc->b_rs = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "lb %s,%d(%s)",
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_lh
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    addr_t addr = proc->read_register(inst->get_rs()) + (int16_t)inst->get_immediate();

    if (addr & 0x1)
    {
      proc->raise_exception(StateType::EXCEPTION_ADDR_MISALIGNED_LOAD);
    }
    else
    {
      proc->write_register(inst->get_rt(), proc->mem.read_mem_int16(addr));
    }

    // microarch state
    proc->alu = ALU_LD;
    proc->rs = inst->get_rs();
    proc->rd = inst->get_rt();
    proc->b_rs = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "lh %s,%d(%s)",
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_lwl
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    addr_t addr = proc->read_register(inst->get_rs()) + (int16_t)inst->get_immediate();
    uint32_t rval = proc->read_register(inst->get_rt());
    uint32_t mval = proc->mem.read_mem_uint32(addr & 0xFFFFFFFC);
    uint32_t mask = 0xFFFFFFFF;

    mask = mask << (8*(addr%4));
    mval = mval << (8*(addr%4));
        
    rval &= ~mask;
    rval |= mval;

    proc->write_register(inst->get_rt(), rval);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "lwl %s,%d(%s)",
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_lw
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    addr_t addr = proc->read_register(inst->get_rs()) + (int16_t)inst->get_immediate();

    if (addr & 0x3)
    {
      proc->raise_exception(StateType::EXCEPTION_ADDR_MISALIGNED_LOAD);
    }
    else
    {
      proc->write_register(inst->get_rt(), proc->mem.read_mem_int32(addr));
    }

    // microarch state
    proc->alu = ALU_LD;
    proc->rs = inst->get_rs();
    proc->rd = inst->get_rt();
    proc->b_rs = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "lw %s,%d(%s)",
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_lbu
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    addr_t addr = proc->read_register(inst->get_rs()) + (int16_t)inst->get_immediate();

    proc->write_register(inst->get_rt(), proc->mem.read_mem_uint8(addr));

    // microarch state
    proc->alu = ALU_LD;
    proc->rs = inst->get_rs();
    proc->rd = inst->get_rt();
    proc->b_rs = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "lbu %s,%d(%s)",
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_lhu
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    addr_t addr = proc->read_register(inst->get_rs()) + (int16_t)inst->get_immediate();

    if (addr & 0x1)
    {
      proc->raise_exception(StateType::EXCEPTION_ADDR_MISALIGNED_LOAD);
    }
    else
    {
      proc->write_register(inst->get_rt(), proc->mem.read_mem_uint16(addr));
    }

    // microarch state
    proc->alu = ALU_LD;
    proc->rs = inst->get_rs();
    proc->rd = inst->get_rt();
    proc->b_rs = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "lhu %s,%d(%s)",
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_lwr
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    addr_t addr = proc->read_register(inst->get_rs()) + (int16_t)inst->get_immediate();
    uint32_t rval = proc->read_register(inst->get_rt());
    uint32_t mval = proc->mem.read_mem_uint32(addr & 0xFFFFFFFC);
    uint32_t mask = 0xFFFFFFFF;
 
    mask = mask >> (8*(3-(addr%4)));
    mval = mval >> (8*(3-(addr%4)));

    rval &= ~mask;
    rval |= mval;

    proc->write_register(inst->get_rt(), rval);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "lwr %s,%d(%s)",
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_sb
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    addr_t addr = proc->read_register(inst->get_rs()) + (int16_t)inst->get_immediate();

    proc->mem.write_mem_uint8(addr, proc->read_register(inst->get_rt()));

    // microarch state
    proc->alu = ALU_ST;
    proc->rs = inst->get_rs();
    proc->rt = inst->get_rt();
    proc->b_rs = true;
    proc->b_rt = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "sb %s,%d(%s)",
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_sh
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    addr_t addr = proc->read_register(inst->get_rs()) + (int16_t)inst->get_immediate();

    if (addr & 0x1)
    {
      proc->raise_exception(StateType::EXCEPTION_ADDR_MISALIGNED_STORE);
    }
    else
    {
      proc->mem.write_mem_uint16(addr, proc->read_register(inst->get_rt()));
    }

    // microarch state
    proc->alu = ALU_ST;
    proc->rs = inst->get_rs();
    proc->rt = inst->get_rt();
    proc->b_rs = true;
    proc->b_rt = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "sh %s,%d(%s)",
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_swl
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    addr_t addr = proc->read_register(inst->get_rs()) + (int16_t)inst->get_immediate();
    uint32_t rval = proc->read_register(inst->get_rt());
    uint32_t mval = proc->mem.read_mem_uint32(addr & 0xFFFFFFFC);
    uint32_t mask = 0xFFFFFFFF;

    mask = mask >> (8*(addr%4));
    rval = rval >> (8*(addr%4));

    mval &= ~mask;
    mval |= rval;

    proc->mem.write_mem_uint32(addr & 0xFFFFFFFC, mval);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "swl %s,%d(%s)",
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_sw
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    addr_t addr = proc->read_register(inst->get_rs()) + (int16_t)inst->get_immediate();

    if (addr & 0x3)
    {
      proc->raise_exception(StateType::EXCEPTION_ADDR_MISALIGNED_STORE);
    }
    else
    {
      proc->mem.write_mem_uint32(addr, proc->read_register(inst->get_rt()));
    }

    // microarch state
    proc->alu = ALU_ST;
    proc->rs = inst->get_rs();
    proc->rt = inst->get_rt();
    proc->b_rs = true;
    proc->b_rt = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "sw %s,%d(%s)",
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_swr
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    addr_t addr = proc->read_register(inst->get_rs()) + (int16_t)inst->get_immediate();
    uint32_t rval = proc->read_register(inst->get_rt());
    uint32_t mval = proc->mem.read_mem_uint32(addr & 0xFFFFFFFC);
    uint32_t mask = 0xFFFFFFFF;

    mask = mask << (8*(3-(addr%4)));
    rval = rval << (8*(3-(addr%4)));

    mval &= ~mask;
    mval |= rval;

    proc->mem.write_mem_uint32(addr & 0xFFFFFFFC, mval);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "swr %s,%d(%s)",
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_cache
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "cache %d,%d(%s)",
      inst->get_rt(),
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_ll
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "ll %s,%d(%s)",
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_lwc1
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    addr_t addr = proc->read_register(inst->get_rs()) + (int16_t)inst->get_immediate();

    if (addr & 0x3)
    {
      proc->raise_exception(StateType::EXCEPTION_ADDR_MISALIGNED_LOAD);
    }
    else
    {
      proc->write_register(inst->get_ft(), proc->mem.read_mem_int32(addr));
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "lwc1 %s,%d(%s)",
      regnames_cop1_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_lwc2
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "lwc2 %s,%d(%s)",
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_pref
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "pref %d,%d(%s)",
      inst->get_rt(),
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_ldc1
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    addr_t addr = proc->read_register(inst->get_rs()) + (int16_t)inst->get_immediate();

    if ((addr & 0x7) || (inst->get_ft() & 0x1))
    {
      proc->raise_exception(StateType::EXCEPTION_ADDR_MISALIGNED_LOAD);
    }
    else
    {
      proc->write_register(inst->get_ft(), proc->mem.read_mem_int32(addr+4));
      proc->write_register(inst->get_ft()+1, proc->mem.read_mem_int32(addr));
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "ldc1 %s,%d(%s)",
      regnames_cop1_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_ldc2
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "ldc2 %s,%d(%s)",
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_sc
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "sc %s,%d(%s)",
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_swc1
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    addr_t addr = proc->read_register(inst->get_rs()) + (int16_t)inst->get_immediate();

    if (addr & 0x3)
    {
      proc->raise_exception(StateType::EXCEPTION_ADDR_MISALIGNED_STORE);
    }
    else
    {
      proc->mem.write_mem_uint32(addr, proc->read_register(inst->get_ft()));
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "swc1 %s,%d(%s)",
      regnames_cop1_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_swc2
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "swc2 %s,%d(%s)",
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_sdc1
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    addr_t addr = proc->read_register(inst->get_rs()) + (int16_t)inst->get_immediate();

    if ((addr & 0x7) || (inst->get_ft() & 0x1))
    {
      proc->raise_exception(StateType::EXCEPTION_ADDR_MISALIGNED_STORE);
    }
    else
    {
      proc->mem.write_mem_uint32(addr, proc->read_register(inst->get_ft()+1));
      proc->mem.write_mem_uint32(addr+4, proc->read_register(inst->get_ft()));
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "sdc1 %s,%d(%s)",
      regnames_cop1_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_sdc2
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "sdc2 %s,%d(%s)",
      regnames_mips32[inst->get_rt()],
      (int16_t)inst->get_immediate(),
      regnames_mips32[inst->get_rs()]);
  }
};
