//
// SPECIAL
//

template <typename InstructionType, typename StateType>
struct mips32_sll
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_rs() == 0)
    {
      proc->write_register(inst->get_rd(), proc->read_register(inst->get_rt()) << inst->get_sa());
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rt();
    proc->rd = inst->get_rd();
    proc->b_rs = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_rt() == 0 && inst->get_rd() == 0)
    {
      uint32_t sa = inst->get_sa();

      if (sa == 0)
      {
        sprintf(proc->disassemble_string, "nop");
      }
      else if (sa == 1)
      {
        sprintf(proc->disassemble_string, "ssnop");
      }
      else
      {
        sprintf(proc->disassemble_string, "sll %s,%s,%d", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rt()], inst->get_sa());
      }
    }
    else
    {
      sprintf(proc->disassemble_string, "sll %s,%s,%d", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rt()], inst->get_sa());
    }
  }
};

template <typename InstructionType, typename StateType>
struct mips32_movf
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_sa() == 0)
    {
      uint32_t fcc = inst->get_cc();

      if ((fcc && (proc->fpu.fcsr.reg.fcc << (fcc-1)) == 0) || proc->fpu.fcsr.reg.fcc0 == 0)
      {
        reg_t rs = proc->read_register(inst->get_rs());

        proc->write_register(inst->get_rd(), rs);
      }
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "movf %s,%s,%d", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rs()], inst->get_cc());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_movt
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_sa() == 0)
    {
      uint32_t fcc = inst->get_cc();

      if ((fcc && (proc->fpu.fcsr.reg.fcc << (fcc-1))) || proc->fpu.fcsr.reg.fcc0 == 1)
      {
        reg_t rs = proc->read_register(inst->get_rs());

        proc->write_register(inst->get_rd(), rs);
      }
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "movt %s,%s,%d", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rs()], inst->get_cc());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_srl
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_rs() == 0)
    {
      proc->write_register(inst->get_rd(), proc->read_register(inst->get_rt()) >> inst->get_sa());
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rt();
    proc->rd = inst->get_rd();
    proc->b_rs = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "srl %s,%s,%d", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rt()], inst->get_sa());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_sra
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_rs() == 0)
    {
      proc->write_register(inst->get_rd(), (int32_t)proc->read_register(inst->get_rt()) >> inst->get_sa());
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rt();
    proc->rd = inst->get_rd();
    proc->b_rs = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "sra %s,%s,%d", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rt()], inst->get_sa());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_sllv
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_sa() == 0)
    {
      proc->write_register(inst->get_rd(), proc->read_register(inst->get_rt()) << (proc->read_register(inst->get_rs()) & 0x1f));
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rt = inst->get_rt();
    proc->rd = inst->get_rd();
    proc->b_rs = true;
    proc->b_rt = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "sllv %s,%s,%s", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rt()], regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_srlv
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_sa() == 0)
    {
      proc->write_register(inst->get_rd(), proc->read_register(inst->get_rt()) >> (proc->read_register(inst->get_rs()) & 0x1f));
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rt = inst->get_rt();
    proc->rd = inst->get_rd();
    proc->b_rs = true;
    proc->b_rt = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "srlv %s,%s,%s", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rt()], regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_srav
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_sa() == 0)
    {
      proc->write_register(inst->get_rd(), (int32_t)proc->read_register(inst->get_rt()) >> (proc->read_register(inst->get_rs()) & 0x1f));
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rt = inst->get_rt();
    proc->rd = inst->get_rd();
    proc->b_rs = true;
    proc->b_rt = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "srav %s,%s,%s", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rt()], regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_jr
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->branch = true;
    proc->pc = proc->npc;
    proc->npc = proc->read_register(inst->get_rs());
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "jr %s", regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_jalr
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->branch = true;
    proc->write_register(inst->get_rd(), proc->pc + 4);
    proc->pc = proc->npc;
    proc->npc = proc->read_register(inst->get_rs());
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "jalr %s,%s", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_movz
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    uint32_t reg = proc->read_register(inst->get_rs());

    if (proc->read_register(inst->get_rt()) == 0)
    {
      proc->write_register(inst->get_rd(), reg);
    }

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rt = inst->get_rt();
    proc->rd = inst->get_rd();
    proc->b_rs = true;
    proc->b_rt = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "movz %s,%s,%s", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_movn
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    uint32_t reg = proc->read_register(inst->get_rs());

    if (proc->read_register(inst->get_rt()) != 0)
    {
      proc->write_register(inst->get_rd(), reg);
    }

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rt = inst->get_rt();
    proc->rd = inst->get_rd();
    proc->b_rs = true;
    proc->b_rt = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "movn %s,%s,%s", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_syscall
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_SYSCALL);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "syscall");
  }
};

template <typename InstructionType, typename StateType>
struct mips32_break
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "break");
  }
};

template <typename InstructionType, typename StateType>
struct mips32_sync
{
  static void execute(InstructionType* inst, StateType* proc)
  { }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "sync %d", inst->get_sa());
  }
};

template <typename InstructionType, typename StateType>
struct mips32_mfhi
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_rs() == 0 && inst->get_rt() == 0 && inst->get_sa() == 0)
    {
      proc->write_register(inst->get_rd(), proc->hi);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "mfhi %s", regnames_mips32[inst->get_rd()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_mthi
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_rt() == 0 && inst->get_rd() == 0 && inst->get_sa() == 0)
    {
      proc->hi = proc->read_register(inst->get_rs());
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "mthi %s", regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_mflo
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_rs() == 0 && inst->get_rt() == 0 && inst->get_sa() == 0)
    {
      proc->write_register(inst->get_rd(), proc->lo);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "mflo %s", regnames_mips32[inst->get_rd()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_mtlo
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_rt() == 0 && inst->get_rd() == 0 && inst->get_sa() == 0)
    {
      proc->lo = proc->read_register(inst->get_rs());
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "mtlo %s", regnames_mips32[inst->get_rs()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_mult
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_rd() == 0 && inst->get_sa() == 0)
    {
      int64_t rs = (int64_t)proc->read_register(inst->get_rs());
      int64_t rt = (int64_t)proc->read_register(inst->get_rt());
      uint64_t product = rs * rt;

      proc->hi = (uint32_t)(product >> 32);
      proc->lo = (uint32_t)product;
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
    sprintf(proc->disassemble_string, "mult %s,%s", regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_multu
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_rd() == 0 && inst->get_sa() == 0)
    {
      uint64_t rs = proc->read_register(inst->get_rs());
      uint64_t rt = proc->read_register(inst->get_rt());
      uint64_t product = rs * rt;

      proc->hi = (uint32_t)(product >> 32);
      proc->lo = (uint32_t)product;
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
    sprintf(proc->disassemble_string, "multu %s,%s", regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_div
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_rd() == 0 && inst->get_sa() == 0)
    {
      int32_t rs = (int32_t)proc->read_register(inst->get_rs());
      int32_t rt = (int32_t)proc->read_register(inst->get_rt());

      if (rt == 0)
      {
        proc->lo = (rs >= 0) ? -1 : 1;
        proc->hi = rs;
        printf("divide by zero!\n");
      }
      else if ((rs == INT32_MIN) && (rt == -1))
      {
        proc->lo = INT32_MIN;
        proc->hi = 0;
        printf("divide overflow!\n");
      }
      else
      {
        proc->lo = rs / rt;
        proc->hi = rs % rt;
      }
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->alu = ALU_IDIVREM;
    proc->rs = inst->get_rs();
    proc->rt = inst->get_rt();
    proc->rd = inst->get_rd();
    proc->b_rs = true;
    proc->b_rt = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "div %s,%s", regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_divu
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_rd() == 0 && inst->get_sa() == 0)
    {
      uint32_t rs = proc->read_register(inst->get_rs());
      uint32_t rt = proc->read_register(inst->get_rt());

      if (rt == 0)
      {
        proc->lo = 0xffffffff;
        proc->hi = rs;
        printf("unsigned divide by zero!\n");
      }
      else
      {
        proc->lo = rs / rt;
        proc->hi = rs % rt;
      }
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->alu = ALU_IDIVREM;
    proc->rs = inst->get_rs();
    proc->rt = inst->get_rt();
    proc->rd = inst->get_rd();
    proc->b_rs = true;
    proc->b_rt = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "div %s,%s", regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_add
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_sa() == 0)
    {
      reg_t rs = proc->read_register(inst->get_rs());
      reg_t rt = proc->read_register(inst->get_rt());
      reg_t sum = rs + rt;

      if (is_overflow_add(sum, rs, rt))
      {
        proc->raise_exception(StateType::EXCEPTION_ALU_OVERFLOW);
      }
      else
      {
        proc->write_register(inst->get_rd(), sum);
      }
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rt = inst->get_rt();
    proc->rd = inst->get_rd();
    proc->b_rs = true;
    proc->b_rt = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "add %s,%s,%s", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_addu
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_sa() == 0)
    {
      proc->write_register(inst->get_rd(), proc->read_register(inst->get_rs()) + proc->read_register(inst->get_rt()));
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rt = inst->get_rt();
    proc->rd = inst->get_rd();
    proc->b_rs = true;
    proc->b_rt = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "addu %s,%s,%s", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_sub
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_sa() == 0)
    {
      reg_t rs = proc->read_register(inst->get_rs());
      reg_t rt = proc->read_register(inst->get_rt());
      reg_t sub = rs - rt;

      if (is_overflow_sub(sub, rs, rt))
      {
        proc->raise_exception(StateType::EXCEPTION_ALU_OVERFLOW);
      }
      else
      {
        proc->write_register(inst->get_rd(), sub);
      }
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rt = inst->get_rt();
    proc->rd = inst->get_rd();
    proc->b_rs = true;
    proc->b_rt = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "sub %s,%s,%s", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_subu
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_sa() == 0)
    {
      proc->write_register(inst->get_rd(), proc->read_register(inst->get_rs()) - proc->read_register(inst->get_rt()));
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rt = inst->get_rt();
    proc->rd = inst->get_rd();
    proc->b_rs = true;
    proc->b_rt = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "subu %s,%s,%s", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_and
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_sa() == 0)
    {
      proc->write_register(inst->get_rd(), proc->read_register(inst->get_rs()) & proc->read_register(inst->get_rt()));
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rt = inst->get_rt();
    proc->rd = inst->get_rd();
    proc->b_rs = true;
    proc->b_rt = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "and %s,%s,%s", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_or
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_sa() == 0)
    {
      proc->write_register(inst->get_rd(), proc->read_register(inst->get_rs()) | proc->read_register(inst->get_rt()));
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rt = inst->get_rt();
    proc->rd = inst->get_rd();
    proc->b_rs = true;
    proc->b_rt = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "or %s,%s,%s", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_xor
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_sa() == 0)
    {
      proc->write_register(inst->get_rd(), proc->read_register(inst->get_rs()) ^ proc->read_register(inst->get_rt()));
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rt = inst->get_rt();
    proc->rd = inst->get_rd();
    proc->b_rs = true;
    proc->b_rt = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "xor %s,%s,%s", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_nor
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_sa() == 0)
    {
      proc->write_register(inst->get_rd(), ~(proc->read_register(inst->get_rs()) | proc->read_register(inst->get_rt())));
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rt = inst->get_rt();
    proc->rd = inst->get_rd();
    proc->b_rs = true;
    proc->b_rt = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "nor %s,%s,%s", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_slt
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_sa() == 0)
    {
      proc->write_register(inst->get_rd(), ((int32_t)proc->read_register(inst->get_rs()) < (int32_t)proc->read_register(inst->get_rt())) ? 1 : 0);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rt = inst->get_rt();
    proc->rd = inst->get_rd();
    proc->b_rs = true;
    proc->b_rt = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "slt %s,%s,%s", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_sltu
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_sa() == 0)
    {
      proc->write_register(inst->get_rd(), (proc->read_register(inst->get_rs()) < proc->read_register(inst->get_rt())) ? 1 : 0);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rt = inst->get_rt();
    proc->rd = inst->get_rd();
    proc->b_rs = true;
    proc->b_rt = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "slt %s,%s,%s", regnames_mips32[inst->get_rd()], regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_tge
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if ((int32_t)proc->read_register(inst->get_rs()) >= (int32_t)proc->read_register(inst->get_rt()))
    {
      proc->raise_exception(StateType::EXCEPTION_TRAP);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "tge %s,%s", regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_tgeu
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (proc->read_register(inst->get_rs()) >= proc->read_register(inst->get_rt()))
    {
      proc->raise_exception(StateType::EXCEPTION_TRAP);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "tgeu %s,%s", regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_tlt
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if ((int32_t)proc->read_register(inst->get_rs()) < (int32_t)proc->read_register(inst->get_rt()))
    {
      proc->raise_exception(StateType::EXCEPTION_TRAP);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "tlt %s,%s", regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_tltu
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (proc->read_register(inst->get_rs()) < proc->read_register(inst->get_rt()))
    {
      proc->raise_exception(StateType::EXCEPTION_TRAP);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "tltu %s,%s", regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_teq
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if ((int32_t)proc->read_register(inst->get_rs()) == (int32_t)proc->read_register(inst->get_rt()))
    {
      proc->raise_exception(StateType::EXCEPTION_TRAP);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "teq %s,%s", regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct mips32_tne
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if ((int32_t)proc->read_register(inst->get_rs()) != (int32_t)proc->read_register(inst->get_rt()))
    {
      proc->raise_exception(StateType::EXCEPTION_TRAP);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "tne %s,%s", regnames_mips32[inst->get_rs()], regnames_mips32[inst->get_rt()]);
  }
};
