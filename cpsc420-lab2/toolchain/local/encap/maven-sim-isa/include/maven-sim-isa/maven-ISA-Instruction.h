//========================================================================
// maven-ISA-Instruction.h
//========================================================================

#include "syscfg.h"
#include "maven-Processor.h"
#include "maven-global.h"

//------------------------------------------------------------------------
// Note from cbatten
//------------------------------------------------------------------------
// Although technically we should check to see if certain feilds are
// zero and raise an illegal instruction exception if they are not, this
// adds quite a bit of branching on the critical inner loop. For our
// purposes just catching illegal opcodes with the dispatch table is
// enough. Let's not worry about illegal fields (eg. fields which should
// be zero but may incorrectly be non-zero) for now. We should however
// of course check fields which change the behavior of the instruction.

//------------------------------------------------------------------------
// Illegal Instruction
//------------------------------------------------------------------------

template <typename InstructionType, typename StateType>
struct maven_illegal
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "maven illegal opcode");
  }
};

//------------------------------------------------------------------------
// Move From Cop0
//------------------------------------------------------------------------

template <typename InstructionType, typename StateType>
struct maven_mfc0
{
  static void execute( InstructionType* inst, StateType* proc )
  {
    // Core ID Number
    if ( inst->get_rd() == MAVEN_SYSCFG_REGDEF_COP0_CORE_ID )
      proc->write_register( inst->get_rt(), proc->proc_id );

    // Count Lo Register (will truncate 64b cop0_count)
    else if ( inst->get_rd() == MAVEN_SYSCFG_REGDEF_COP0_COUNT_LO )
      proc->write_register( inst->get_rt(), maven::g_cop0_count );

    // Count Hi Register (shift down 64b cop0_count)
    else if ( inst->get_rd() == MAVEN_SYSCFG_REGDEF_COP0_COUNT_HI )
      proc->write_register( inst->get_rt(), (maven::g_cop0_count >> 32) );

    // Enable statistics
    else if ( inst->get_rd() == MAVEN_SYSCFG_REGDEF_COP0_STATS_EN )
      proc->write_register( inst->get_rt(), proc->stats_en );
    
    else if ( inst->get_rd() == MAVEN_SYSCFG_REGDEF_COP0_TID_MASK )
      proc->write_register( inst->get_rt(), *proc->tid_mask_ptr );

    else {
      proc->raise_exception( StateType::EXCEPTION_RESERVED_INSTRUCTION );
    }
  }
  static void disassemble( InstructionType* inst, StateType* proc )
  {
    sprintf( proc->disassemble_string, "mfc0 %s,%d", 
             regnames_mips32[inst->get_rt()], inst->get_rd());
  }
};

template <typename InstructionType, typename StateType>
struct maven_mtc0
{
  static void execute( InstructionType* inst, StateType* proc )
  {
    // Enable statistics
    if ( inst->get_rd() == MAVEN_SYSCFG_REGDEF_COP0_STATS_EN )
    {
      // alex: enable vparray stats too
      proc->stats_en = proc->read_register( inst->get_rt() );
      proc->vparray.stats_en = proc->read_register( inst->get_rt() );
    }
    
    // Modify active thread mask
    else if ( inst->get_rd() == MAVEN_SYSCFG_REGDEF_COP0_TID_MASK )
      *proc->tid_mask_ptr = proc->read_register( inst->get_rt() ) | 0x1;
    else if ( inst->get_rd() == MAVEN_SYSCFG_REGDEF_COP0_TID_STOP )
    {
      if (proc->proc_id != 0)
        *proc->tid_mask_ptr &= ~(1 << proc->proc_id); 
    }
    else {
      proc->raise_exception( StateType::EXCEPTION_RESERVED_INSTRUCTION );
    }
  }
  static void disassemble( InstructionType* inst, StateType* proc )
  {
    sprintf( proc->disassemble_string, "mtc0 %s,%d", 
             regnames_mips32[inst->get_rt()], inst->get_rd());
  }
};

//------------------------------------------------------------------------
// Return from exception
//------------------------------------------------------------------------

template <typename InstructionType, typename StateType>
struct maven_eret
{
  static void execute( InstructionType* inst, StateType* proc )
  {
    proc->npc = proc->epc;
  }
  static void disassemble( InstructionType* inst, StateType* proc )
  {
    sprintf( proc->disassemble_string, "eret" );
  }
};

//------------------------------------------------------------------------
// Extra CP/VP Synchronization Instructions
//------------------------------------------------------------------------

template <typename InstructionType, typename StateType>
struct maven_stop
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (    inst->get_rs() == 0 && inst->get_rt() == 0 
         && inst->get_rd() == 0 && inst->get_sa() == 0 )
    {
      proc->runstate = StateType::STOPPED;
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "stop");
  }
};

// As with the sync.*.v and sync.*.cv the new sync.l instruction really
// doesn't need to do anything because we always execute instructions in
// order, memory operations are instantaneous and atomic, and we execute
// VPs to completion before executing the next CP command.

template <typename InstructionType, typename StateType>
struct maven_sync_l
{
  static void execute(InstructionType* inst, StateType* proc)
  { }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "sync.l");
  }
};

template <typename InstructionType, typename StateType>
struct maven_amo_add
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    addr_t addr = proc->read_register(inst->get_rs());
    int32_t temp;

    if (addr & 0x3)
    {
      proc->raise_exception(StateType::EXCEPTION_ADDR_MISALIGNED_LOAD);
    }
    else
    {
      temp = proc->mem.read_mem_int32(addr);
      proc->mem.write_mem_uint32( addr, 
        temp + proc->read_register(inst->get_rt()) );
      proc->write_register(inst->get_rd(), temp);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "amo.add %s,%s,%s", 
      regnames_mips32[inst->get_rd()],
      regnames_mips32[inst->get_rs()],
      regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct maven_amo_and
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    addr_t addr = proc->read_register(inst->get_rs());
    int32_t temp;

    if (addr & 0x3)
    {
      proc->raise_exception(StateType::EXCEPTION_ADDR_MISALIGNED_LOAD);
    }
    else
    {
      temp = proc->mem.read_mem_int32(addr);
      proc->mem.write_mem_uint32( addr, 
        temp & proc->read_register(inst->get_rt()) );
      proc->write_register(inst->get_rd(), temp);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "amo.and %s,%s,%s", 
      regnames_mips32[inst->get_rd()],
      regnames_mips32[inst->get_rs()],
      regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct maven_amo_or
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    addr_t addr = proc->read_register(inst->get_rs());
    int32_t temp;

    if (addr & 0x3)
    {
      proc->raise_exception(StateType::EXCEPTION_ADDR_MISALIGNED_LOAD);
    }
    else
    {
      temp = proc->mem.read_mem_int32(addr);
      proc->mem.write_mem_uint32( addr, 
        temp | proc->read_register(inst->get_rt()) );
      proc->write_register(inst->get_rd(), temp);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "amo.or %s,%s,%s", 
      regnames_mips32[inst->get_rd()],
      regnames_mips32[inst->get_rs()],
      regnames_mips32[inst->get_rt()]);
  }
};

//------------------------------------------------------------------------
// VTU Config Instructions
//------------------------------------------------------------------------

template <typename InstructionType, typename StateType>
struct maven_vcfgivl
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->vparray.vcfg((int16_t)inst->get_immediate()+1);
    proc->write_register( inst->get_rt(),
    proc->vparray.setvl(proc->read_register(inst->get_rs())) );

    // alex: dump app vl
    //printf("* app-vl %d\n", proc->read_register(inst->get_rs())) ;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf( proc->disassemble_string, "vcfgivl %s,%s,%d",
             regnames_mips32[inst->get_rt()],
             regnames_mips32[inst->get_rs()],
             (int16_t)inst->get_immediate()+1 );
  }
};

template <typename InstructionType, typename StateType>
struct maven_setvl
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    proc->write_register( inst->get_rd(),
    proc->vparray.setvl(proc->read_register(inst->get_rs())) );
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf( proc->disassemble_string, "setvl %s,%s",
             regnames_mips32[inst->get_rd()],
             regnames_mips32[inst->get_rs()] );
  }
};

//------------------------------------------------------------------------
// Vector Synchronization Instructions
//------------------------------------------------------------------------
// If inst->get_rs() == 0 then sync_v should execute a sync.l.v
// instruction and if inst->get_rs() == 1 then sync_v should execute a
// sync.g.v instruction but there really is no difference in the ISA
// simulator since we always execute CP instructions in order and memory
// operations are instantaneous and atomic. We don't really need to do
// anything either with respect to the VTU since we always execute the
// VPs to completion before continuing on to the next CP instruction. So
// all we need to do in the synchronization instruction is simply make
// sure that the rs field is valid since this field will have important
// implications on the real machine.

template <typename InstructionType, typename StateType>
struct maven_sync_v
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if ( (inst->get_rs() != 0) && (inst->get_rs() != 1) )
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if ( inst->get_rs() == 0 )
      sprintf(proc->disassemble_string, "sync.l.v");
    else if ( inst->get_rs() == 0 )
      sprintf(proc->disassemble_string, "sync.g.v");
    else 
      sprintf(proc->disassemble_string, "?");
  }
};

template <typename InstructionType, typename StateType>
struct maven_sync_cv
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if ( (inst->get_rs() != 0) && (inst->get_rs() != 1) )
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if ( inst->get_rs() == 0 )
      sprintf(proc->disassemble_string, "sync.l.cv");
    else if ( inst->get_rs() == 0 )
      sprintf(proc->disassemble_string, "sync.g.cv");
    else 
      sprintf(proc->disassemble_string, "?");
  }
};

//------------------------------------------------------------------------
// Vector Fetch Instructions
//------------------------------------------------------------------------

template <typename InstructionType, typename StateType>
struct maven_vf
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    if (inst->get_rs() == 0 && inst->get_rt() == 0)
    {
      proc->vector = true;
      proc->vparray.go(proc->npc + ((int16_t)inst->get_immediate() << 2));
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf( proc->disassemble_string, "vf %d", 
             (int16_t)inst->get_immediate() );
  }
};

//------------------------------------------------------------------------
// Vector Memory Instructions
//------------------------------------------------------------------------

template <typename InstructionType, typename StateType>
struct maven_vload
{
  // note: vload currently supports
  //  - l{w,h,hu,b,bu}.v
  //  - l{w,h,hu,b,bu}st.v
  //  - l{w,h,hu,b,bu}seg.v
  //  - l{w,h,hu,b,bu}segst.v
    
  static void execute(InstructionType* inst, StateType* proc)
  {
    int opcode      = inst->get_opcode();
    int rv          = inst->get_rv(); // destination vector register
    addr_t addr     = proc->read_register(inst->get_rt());
    int n_elm       = inst->get_n() + 1;
    int stride;

    // Note that the stride is just the number of bytes in an element
    // for unit-stride accesses, and it is the total number of bytes for
    // all elements in a segment for segment accesses, unless of course
    // we are doing a segmented access in which case the stride is in a
    // scalar register.
    
    switch (inst->get_s()) // switch on size (w,h,b)
    {
      case (0x0) : 

        stride = n_elm * 4;
        if ( opcode == MAVEN_CP_VLOAD_ST )
          stride = proc->read_register(inst->get_rs());

        proc->vparray.vload( addr, rv, stride, n_elm, 
                             MavenVPArray::WORD_T );
        break;

      case (0x1) :

        stride = n_elm * 2;
        if ( opcode == MAVEN_CP_VLOAD_ST )
          stride = proc->read_register(inst->get_rs());

        if (inst->get_u()) 
          proc->vparray.vload( addr, rv, stride, n_elm, 
                               MavenVPArray::UHWORD_T );
        else 
          proc->vparray.vload( addr, rv, stride, n_elm, 
                               MavenVPArray::HWORD_T );
        break;
      
      case (0x3) :

        stride = n_elm;
        if ( opcode == MAVEN_CP_VLOAD_ST )
          stride = proc->read_register(inst->get_rs());

        if (inst->get_u()) 
          proc->vparray.vload( addr, rv, stride, n_elm, 
                               MavenVPArray::UBYTE_T );
        else 
          proc->vparray.vload( addr, rv, stride, n_elm, 
                               MavenVPArray::BYTE_T );
        break;
      
      default:
        proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
        break; 
    }

    // microarch state
    proc->alu = ALU_VLD;
    proc->rd = inst->get_rv();
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    const char* opcode_s;
    bool is_strided = (inst->get_opcode() == MAVEN_CP_VLOAD_ST);
    
    // cbatten - Where is the disassembly for vector segment loads?

    switch (inst->get_s())
    {
      case (0x0) :
        opcode_s = is_strided ? "lwst.v" : "lw.v";
        break;
      case (0x1) :
        if (inst->get_u())
          opcode_s = is_strided ? "lhust.v" : "lhu.v";
        else
          opcode_s = is_strided ? "lhst.v" : "lh.v";
        break;
      case (0x3) :
        if (inst->get_u())
          opcode_s = is_strided ? "lbust.v" : "lbu.v";
        else
          opcode_s = is_strided ? "lbst.v" : "lb.v";
        break;
      default:
        opcode_s = is_strided ? "l???st.v" : "l???.v";
        break;
    }
    sprintf( proc->disassemble_string, "%s %s,%s", opcode_s, 
             regnames_vregs_maven[inst->get_rv()], 
             regnames_mips32[inst->get_rt()] );
  }
};

template <typename InstructionType, typename StateType>
struct maven_vload_x
{
  //perform indexed vector load (gather)
  static void execute(InstructionType* inst, StateType* proc)
  {
    int r_vdst       = inst->get_rd();
    addr_t base_addr = proc->read_register(inst->get_rt());
    int r_voffset    = inst->get_rs();
    int op_size;

    switch (inst->get_s())
    {
      case (0x0) :
        op_size = MavenVPArray::WORD_T;
        break;
      case (0x1) :
        if ( inst->get_u() )
          op_size = MavenVPArray::UHWORD_T;
        else
          op_size = MavenVPArray::HWORD_T;
        break;
      case (0x3) :
        if ( inst->get_u() )
          op_size = MavenVPArray::UBYTE_T;
        else
          op_size = MavenVPArray::BYTE_T;
        break;
      default:
        proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
        op_size = MavenVPArray::WORD_T;
        break; 
    }

    proc->vparray.vload_x(r_vdst, base_addr, r_voffset, op_size);

    // microarch state
    proc->alu = ALU_LD;
    proc->rs = inst->get_rt();
    proc->rd = inst->get_rv();
    proc->b_rs = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    const char* opcode_s;

    switch (inst->get_s())
    {
      case (0x0) :
        opcode_s = "lwx.v";
        break;
      case (0x1) :
        if (inst->get_u())
          opcode_s = "lhux.v";
        else
          opcode_s = "lhx.v";
        break;
      case (0x3) :
        if (inst->get_u())
          opcode_s = "lbux.v";
        else
          opcode_s = "lbx.v";
        break;
      default:
        opcode_s = "l???x.v";
        break;
    }
    sprintf( proc->disassemble_string, "%s %s,%s,%s", 
             opcode_s, 
             regnames_vregs_maven[inst->get_rv()], 
             regnames_mips32[inst->get_rt()],
             regnames_vregs_maven[inst->get_rs()]
            );
  }
};

template <typename InstructionType, typename StateType>
struct maven_vstore
{
  // note: vstore currently supports
  //  - s{w,h,b}.v
  //  - s{w,h,b}st.v
  //  - s{w,h,b}seg.v
  //  - s{w,h,b}segst.v
  
  static void execute(InstructionType* inst, StateType* proc)
  {
    int opcode      = inst->get_opcode();
    int rv          = inst->get_rv(); // source vector register
    addr_t addr     = proc->read_register(inst->get_rt());
    int n_elm       = inst->get_n() + 1;
    int stride;
    
    if (inst->get_u() == 1)
    {
      switch (inst->get_s()) 
      {
        case (0x0) : 

          stride = n_elm * 4;
          if ( opcode == MAVEN_CP_VSTORE_ST )
            stride = proc->read_register(inst->get_rs());

          proc->vparray.vstore( addr, rv, stride, n_elm, 
                                MavenVPArray::WORD_T );
          break;

        case (0x1) :

          stride = n_elm * 2;
          if ( opcode == MAVEN_CP_VSTORE_ST )
            stride = proc->read_register(inst->get_rs());

          proc->vparray.vstore( addr, rv, stride, n_elm, 
                                MavenVPArray::UHWORD_T );
          break;
        
        case (0x3) :

          stride = n_elm;
          if ( opcode == MAVEN_CP_VSTORE_ST )
            stride = proc->read_register(inst->get_rs());

          proc->vparray.vstore( addr, rv, stride, n_elm, 
                                MavenVPArray::UBYTE_T );
          break;
        
        default:
          proc->raise_exception( StateType::EXCEPTION_RESERVED_INSTRUCTION );
          break; 
      }
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->alu = ALU_VST;
    proc->rt = inst->get_rv();
    proc->b_rt = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    const char* opcode_s;
    bool is_strided = (inst->get_opcode() == MAVEN_CP_VSTORE_ST);

    // cbatten - Where is the disassembly for vector segment loads?
    
    switch (inst->get_s())
    {
      case (0x0) :
        opcode_s = is_strided ? "swst.v" : "sw.v";
        break;
      case (0x1) :
        opcode_s = is_strided ? "shst.v" : "sh.v";
        break;
      case (0x3) :
        opcode_s = is_strided ? "sbst.v" : "sb.v";
        break;
      default:
        opcode_s = is_strided ? "s???st.v" : "s???.v";
        break;
    }
    sprintf( proc->disassemble_string, "%s %s,%s", opcode_s, 
             regnames_vregs_maven[inst->get_rv()], 
             regnames_mips32[inst->get_rt()] );
  }
};
 
template <typename InstructionType, typename StateType>
struct maven_vstore_x
{
  //indexed store (scatter)
  static void execute(InstructionType* inst, StateType* proc)
  {
    int    r_vsrc    = inst->get_rd();
    addr_t base_addr = proc->read_register(inst->get_rt());
    int    r_voffset = inst->get_rs();

    int op_size;
    if (inst->get_u() == 1)
    {
      switch (inst->get_s())
      {
        case (0x0) :
          op_size = MavenVPArray::WORD_T;
          break;
        case (0x1) :
          op_size = MavenVPArray::UHWORD_T;
          break;
        case (0x3) :
          op_size = MavenVPArray::UBYTE_T;
          break;
        default:
          proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
          op_size = MavenVPArray::WORD_T;
          break; 
      }
     
      proc->vparray.vstore_x( r_vsrc, base_addr, r_voffset, op_size);
    }
    else
    {
      proc->raise_exception(StateType::EXCEPTION_RESERVED_INSTRUCTION);
    }

    // microarch state
    proc->alu = ALU_ST;
    proc->rs = inst->get_rt();
    proc->rt = inst->get_rv();
    proc->b_rs = true;
    proc->b_rt = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    const char* opcode_s;

    switch (inst->get_s())
    {
      case (0x0) :
        opcode_s = "swx.v";
        break;
      case (0x1) :
        opcode_s = "shx.v";
        break;
      case (0x3) :
        opcode_s = "sbx.v";
        break;
      default:
        opcode_s = "s???x.v";
        break;
    }
    
    sprintf( proc->disassemble_string, "%s %s,%s,%s", 
             opcode_s, 
             regnames_vregs_maven[inst->get_rv()], 
             regnames_mips32[inst->get_rt()],
             regnames_vregs_maven[inst->get_rv()]
           );
  }
};
 
template <typename InstructionType, typename StateType>
struct maven_mtvp
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    int vp_id = proc->read_register(inst->get_rs());
    int r_vdst= inst->get_rd();
    reg_t value = proc->read_register(inst->get_rt());

    proc->vparray.mtvp(vp_id, r_vdst, value);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf( proc->disassemble_string, "mtvp %s,%s,%s", 
             regnames_mips32[inst->get_rs()],
             regnames_vregs_maven[inst->get_rd()], 
             regnames_mips32[inst->get_rt()] );
  }
};
 
template <typename InstructionType, typename StateType>
struct maven_mfvp
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    int vp_id   = proc->read_register(inst->get_rs());
    int r_vsrc  = inst->get_rd();
    reg_t value = proc->vparray.mfvp(vp_id, r_vsrc);
    
    proc->write_register(inst->get_rt(), value);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf( proc->disassemble_string, "mfvp %s,%s,%s", 
             regnames_mips32[inst->get_rs()],
             regnames_vregs_maven[inst->get_rd()], 
             regnames_mips32[inst->get_rt()] );
  }
};
 
template <typename InstructionType, typename StateType>
struct maven_mtvps
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    int r_vdst= inst->get_rd();
    reg_t value = proc->read_register(inst->get_rt());

    proc->vparray.mtvps(r_vdst, value);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf( proc->disassemble_string, "mtvps %s,%s", 
             regnames_vregs_maven[inst->get_rd()], 
             regnames_mips32[inst->get_rt()] );
  }
};
                                                         
//------------------------------------------------------------------------
// Vector Unit Integer Arithmetic Instructions
//------------------------------------------------------------------------

template <typename InstructionType, typename StateType>
struct maven_arith_vv
{
  //perform vector unit arithmetic for the following instructions
  //
  // - addu.vv, addu.vs
  // - subu.vv, subu.vs, subu.sv
  // - mul.vv, mul.vs
  // - mul.vv, mul.vs
  // - sll.vv,  sll.vs,  sll.sv
  // - srl.vv,  srl.vs,  srl.sv
  // - sra.vv,  sra.vs,  sra.sv
  // - and.vv,  and.vs
  // - or.vv,   or.vs
  // - xor.vv,  xor.vs
  // - nor.vv,  nor.vs

  static void execute(InstructionType* inst, StateType* proc)
  {
    int r_vdst   = inst->get_rd();
    int r_vsrc1  = inst->get_rs();
    int r_vsrc2  = inst->get_rt();
    int r_mask   = inst->get_msk();
    int cmd_code = inst->get_cmd();
    
    proc->vparray.arith_vv(r_vdst, r_vsrc1, r_vsrc2, r_mask, cmd_code);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    //note: does not handle differenitiating between mov.vv and add.vv, etc.
    int r_mask = inst->get_msk();
    sprintf( proc->disassemble_string, "%s %s,%s,%s%s%s", 
             cnames_cop2[inst->get_cmd()&0x7F], 
             regnames_vregs_maven[inst->get_rd()], 
             regnames_vregs_maven[inst->get_rs()], 
             regnames_vregs_maven[inst->get_rt()],
             (r_mask != 0) ? "," : "",
             (r_mask != 0) ? regnames_flags_flood[r_mask] : ""
             );
  }
};
                                                        
template <typename InstructionType, typename StateType>
struct maven_arith_vs
{
  //perform vector unit arithmetic for the following instructions
  //
  // - addu.vs
  // - subu.vs
  // - sll.vs
  // - srl.vs
  // - sra.vs
  // - and.vs
  // - or.vs
  // - xor.vs
  // - nor.vs
  static void execute(InstructionType* inst, StateType* proc)
  {
    int r_vdst   = inst->get_rd();
    reg_t sdata  = proc->read_register(inst->get_rs());
    int r_vsrc   = inst->get_rt();
    int r_mask   = inst->get_msk();
    int cmd_code = inst->get_cmd();
    
    proc->vparray.arith_vs(r_vdst, sdata, r_vsrc, r_mask, cmd_code);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    //note: doesn't distingush between mov and add
    int r_mask = inst->get_msk();
    sprintf( proc->disassemble_string, "%s %s,%s,%s%s%s", 
             cnames_cop2[inst->get_cmd()&0x7F], 
             regnames_vregs_maven[inst->get_rd()], 
             regnames_vregs_maven[inst->get_rt()],
             regnames_mips32[inst->get_rs()], 
             (r_mask != 0) ? "," : "",
             (r_mask != 0) ? regnames_flags_flood[r_mask] : ""
             );
  }
};
                                                         
template <typename InstructionType, typename StateType>
struct maven_arith_sv
{
  //perform vector unit arithmetic for the following instructions
  //
  // - subu.sv
  // - sll.sv
  // - srl.sv
  // - sra.sv
  static void execute(InstructionType* inst, StateType* proc)
  {
    int r_vdst   = inst->get_rd();
    reg_t sdata  = proc->read_register(inst->get_rs());
    int r_vsrc   = inst->get_rt();
    int r_mask   = inst->get_msk();
    int cmd_code = inst->get_cmd();
    
    //both .sv and .vs use the same vparray function
    proc->vparray.arith_vs(r_vdst, sdata, r_vsrc, r_mask, cmd_code);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    //note: doesn't distingush between mov and add
    int r_mask = inst->get_msk();
    sprintf( proc->disassemble_string, "%s %s,%s,%s%s%s", 
             cnames_cop2[inst->get_cmd()&0x7F], 
             regnames_vregs_maven[inst->get_rd()], 
             regnames_mips32[inst->get_rs()], 
             regnames_vregs_maven[inst->get_rt()],
             (r_mask != 0) ? "," : "",
             (r_mask != 0) ? regnames_flags_flood[r_mask] : ""
             );
  }
};                                            

template <typename InstructionType, typename StateType>
struct maven_arith_v
{
  //perform vector unit arithmetic for the following instructions
  //
  // - bitrev.v
  static void execute(InstructionType* inst, StateType* proc)
  {
    int r_vdst   = inst->get_rd();
    int r_vsrc   = inst->get_rt();
    int r_mask   = inst->get_msk();
    int cmd_code = inst->get_cmd();
    
    proc->vparray.arith_v(r_vdst, r_vsrc, r_mask, cmd_code);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    int r_mask = inst->get_msk();
    sprintf( proc->disassemble_string, "%s %s,%s%s%s", 
             cnames_cop2[inst->get_cmd()&0x7F], 
             regnames_vregs_maven[inst->get_rd()], 
             regnames_vregs_maven[inst->get_rs()], 
             (r_mask != 0) ? "," : "",
             (r_mask != 0) ? regnames_flags_flood[r_mask] : ""
             );
  }
};
//------------------------------------------------------------------------
// Vector Unit Floating Point Arithmetic Instructions
//------------------------------------------------------------------------
                                         
template <typename InstructionType, typename StateType>
struct maven_arith_s_vv
{
  //perform vector unit arithmetic for the following instructions
  //
  // - add.s.vv
  // - sub.s.vv
  // - mul.s.vv
  // - div.s.vv
  static void execute(InstructionType* inst, StateType* proc)
  {
    int r_vdst   = inst->get_rd();
    int r_vsrc1  = inst->get_rs();
    int r_vsrc2  = inst->get_rt();
    int r_mask   = inst->get_msk();
    int cmd_code = inst->get_cmd();
    
    proc->vparray.arith_s_vv(r_vdst, r_vsrc1, r_vsrc2, r_mask, cmd_code);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    int r_mask = inst->get_msk();
    sprintf( proc->disassemble_string, "%s %s,%s,%s%s%s", 
             cnames_cop2[inst->get_cmd()&0x7F], 
             regnames_vregs_maven[inst->get_rd()], 
             regnames_vregs_maven[inst->get_rs()], 
             regnames_vregs_maven[inst->get_rt()],
             (r_mask != 0) ? "," : "",
             (r_mask != 0) ? regnames_flags_flood[r_mask] : ""
             );
  }
};
                                                      
template <typename InstructionType, typename StateType>
struct maven_arith_s_vs
{
  //perform vector unit arithmetic for the following instructions
  //
  // - add.s.vs
  // - sub.s.vs
  // - mul.s.vs
  // - div.s.vs
  static void execute(InstructionType* inst, StateType* proc)
  {
    int r_vdst   = inst->get_rd();
    reg_t sdata  = proc->read_register(inst->get_rs());
    int r_vsrc   = inst->get_rt();
    int r_mask   = inst->get_msk();
    int cmd_code = inst->get_cmd();
    
    proc->vparray.arith_s_vs(r_vdst, sdata, r_vsrc, r_mask, cmd_code);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    int r_mask = inst->get_msk();
    sprintf( proc->disassemble_string, "%s %s,%s,%s%s%s", 
             cnames_cop2[inst->get_cmd()&0x7F], 
             regnames_vregs_maven[inst->get_rd()], 
             regnames_vregs_maven[inst->get_rt()],
             regnames_mips32[inst->get_rs()], 
             (r_mask != 0) ? "," : "",
             (r_mask != 0) ? regnames_flags_flood[r_mask] : ""
             );
  }
};
                                                                                 
template <typename InstructionType, typename StateType>
struct maven_arith_s_sv
{
  //perform vector unit arithmetic for the following instructions
  //
  // - add.s.sv
  // - sub.s.sv
  // - mul.s.sv
  // - div.s.sv
  static void execute(InstructionType* inst, StateType* proc)
  {
    int r_vdst   = inst->get_rd();
    reg_t sdata  = proc->read_register(inst->get_rs());
    int r_vsrc   = inst->get_rt();
    int r_mask   = inst->get_msk();
    int cmd_code = inst->get_cmd();
    
    //.sv and .vs go to the same vparray function
    proc->vparray.arith_s_vs(r_vdst, sdata, r_vsrc, r_mask, cmd_code);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    int r_mask = inst->get_msk();
    sprintf( proc->disassemble_string, "%s %s,%s,%s%s%s", 
             cnames_cop2[inst->get_cmd()&0x7F], 
             regnames_vregs_maven[inst->get_rd()], 
             regnames_mips32[inst->get_rs()], 
             regnames_vregs_maven[inst->get_rt()],
             (r_mask != 0) ? "," : "",
             (r_mask != 0) ? regnames_flags_flood[r_mask] : ""
             );
  }
};
                                                     
template <typename InstructionType, typename StateType>
struct maven_arith_s_v
{
  //perform vector unit arithmetic for the following instructions
  //
  // - abs.s.v
  // - neg.s.v
  // - round.s.v
  // - trunc.s.v
  // - ceil.s.v
  // - floor.s.v
  // - recip.s.v
  // - rsqrt.s.v
  // - sqrt.s.v
  // - cvt.s.w.v
  // - cvt.w.s.v
  static void execute(InstructionType* inst, StateType* proc)
  {
    int r_vdst   = inst->get_rd();
    int r_vsrc   = inst->get_rt();
    int r_mask   = inst->get_msk();
    int cmd_code = inst->get_cmd();
    
    proc->vparray.arith_s_v(r_vdst, r_vsrc, r_mask, cmd_code);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    int r_mask = inst->get_msk();
    sprintf( proc->disassemble_string, "%s %s,%s%s%s", 
             cnames_cop2[inst->get_cmd()&0x7F], 
             regnames_vregs_maven[inst->get_rd()], 
             regnames_vregs_maven[inst->get_rt()],
             (r_mask != 0) ? "," : "",
             (r_mask != 0) ? regnames_flags_flood[r_mask] : ""
             );
  }
};
                                                                      
//------------------------------------------------------------------------
// Vector Unit Set Flag Instructions
//------------------------------------------------------------------------

template <typename InstructionType, typename StateType>
struct maven_setflags
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    int cmd_code = inst->get_cmd();
    int r_fdst  = inst->get_rd();
    int r_vsrc1 = inst->get_rs();
    int r_vsrc2 = inst->get_rt();
    
    proc->vparray.setflag(r_fdst, r_vsrc1, r_vsrc2, cmd_code);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_cmd()==MAVEN_CP_COP2_SEQ_F_VV)
      sprintf( proc->disassemble_string, "seq.f.vv %s,%s,%s", 
               regnames_flags_flood[inst->get_rd()], 
               regnames_vregs_maven[inst->get_rs()], 
               regnames_vregs_maven[inst->get_rt()] );
    else if (inst->get_cmd()==MAVEN_CP_COP2_SLT_F_VV)
      sprintf( proc->disassemble_string, "slt.f.vv %s,%s,%s", 
               regnames_flags_flood[inst->get_rd()], 
               regnames_vregs_maven[inst->get_rs()], 
               regnames_vregs_maven[inst->get_rt()] );
    else if (inst->get_cmd()==MAVEN_CP_COP2_SLT_F_VV)
      sprintf( proc->disassemble_string, "sltu.f.vv %s,%s,%s",
               regnames_flags_flood[inst->get_rd()],
               regnames_vregs_maven[inst->get_rs()],
               regnames_vregs_maven[inst->get_rt()] );
    else 
      sprintf( proc->disassemble_string, "error in setflag");
  }
};
                                                          
template <typename InstructionType, typename StateType>
struct maven_sf_setflags_vv
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    int cmd_code = inst->get_cmd();
    int r_fdst = inst->get_rd();
    int r_vsrc1 = inst->get_rs();
    int r_vsrc2 = inst->get_rt();

    proc->vparray.sf_setflag_vv(r_fdst, r_vsrc1, r_vsrc2, cmd_code);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "%s %s,%s,%s",
      cnames_s_cop1[inst->get_cmd()&0xf],
      regnames_flags_flood[inst->get_rd()],
      regnames_vregs_maven[inst->get_rs()],
      regnames_vregs_maven[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct maven_sf_setflags_vs
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    int cmd_code = inst->get_cmd();
    int r_fdst = inst->get_rd();
    int r_vsrc = inst->get_rs();
    reg_t rsrc = proc->read_register(inst->get_rt());

    proc->vparray.sf_setflag_vs(r_fdst, r_vsrc, rsrc, cmd_code);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "%s.s.f.vs %s,%s,%s",
      cnames_s_cop1[inst->get_cmd()&0xf],
      regnames_flags_flood[inst->get_rd()],
      regnames_vregs_maven[inst->get_rs()],
      regnames_vregs_maven[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct maven_sf_setflags_sv
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    int cmd_code = inst->get_cmd();
    int r_fdst = inst->get_rd();
    reg_t rsrc = proc->read_register(inst->get_rs());
    int r_vsrc = inst->get_rt();

    proc->vparray.sf_setflag_sv(r_fdst, rsrc, r_vsrc, cmd_code);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "%s.s.f.sv %s,%s,%s",
      cnames_s_cop1[inst->get_cmd()&0xf],
      regnames_flags_flood[inst->get_rd()],
      regnames_vregs_maven[inst->get_rs()],
      regnames_vregs_maven[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct maven_flagops
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    int cmd_code = inst->get_cmd();
    int r_fdst   = inst->get_rd();
    int r_fsrc1  = inst->get_rs();
    int r_fsrc2  = inst->get_rt();
    
    proc->vparray.flagops(r_fdst, r_fsrc1, r_fsrc2, cmd_code);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_cmd()==MAVEN_CP_COP2_NOT_F)
      sprintf( proc->disassemble_string, "not.f %s,%s", 
               regnames_flags_flood[inst->get_rd()], 
               regnames_flags_flood[inst->get_rs()]);
    else if (inst->get_cmd()==MAVEN_CP_COP2_MOV_F)
      sprintf( proc->disassemble_string, "mov.f %s,%s", 
               regnames_flags_flood[inst->get_rd()], 
               regnames_flags_flood[inst->get_rs()]);
    else if (inst->get_cmd()==MAVEN_CP_COP2_OR_F)
      sprintf( proc->disassemble_string, "or.f %s,%s,%s", 
               regnames_flags_flood[inst->get_rd()], 
               regnames_flags_flood[inst->get_rs()], 
               regnames_flags_flood[inst->get_rt()]);
    else if (inst->get_cmd()==MAVEN_CP_COP2_AND_F)
      sprintf( proc->disassemble_string, "and.f %s,%s,%s", 
               regnames_flags_flood[inst->get_rd()], 
               regnames_flags_flood[inst->get_rs()], 
               regnames_flags_flood[inst->get_rt()]);
    else 
      sprintf( proc->disassemble_string, "disasm: error in flagops");
  }
};
                                                         
// mtvps.f, mfvps.f
template <typename InstructionType, typename StateType>
struct maven_mtmfvpsf
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    int cmd_code = inst->get_cmd();
    
    proc->vparray.mtmfvpsf(inst->get_rd(), inst->get_rs(), cmd_code);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    if (inst->get_cmd()==MAVEN_CP_COP2_MTVPS_F)
      sprintf( proc->disassemble_string, "mtvps.f %s,%s", 
               regnames_vregs_maven[inst->get_rd()],
               regnames_flags_flood[inst->get_rs()]);
    else if (inst->get_cmd()==MAVEN_CP_COP2_MFVPS_F)
      sprintf( proc->disassemble_string, "mfvps.f %s,%s", 
               regnames_flags_flood[inst->get_rd()], 
               regnames_vregs_maven[inst->get_rs()]);
    else 
      sprintf( proc->disassemble_string, "disasm: error in flagops mt/mfvps.f");
  }
  
};

template <typename InstructionType, typename StateType>
struct maven_popcf
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    reg_t count = proc->vparray.popcf(inst->get_rs());

    proc->write_register(inst->get_rd(), count);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf( proc->disassemble_string, "popc.f %s,%s", 
             regnames_mips32[inst->get_rd()],
             regnames_flags_flood[inst->get_rs()]);
  }
  
};
                                              
template <typename InstructionType, typename StateType>
struct maven_findfonef
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    reg_t result = proc->vparray.findfonef(inst->get_rs());

    proc->write_register(inst->get_rd(), result);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf( proc->disassemble_string, "findfone.f %s,%s", 
             regnames_mips32[inst->get_rd()],
             regnames_flags_flood[inst->get_rs()]);
  }
  
};

//------------------------------------------------------------------------
// Extra CP/VP Misc Instructions
//------------------------------------------------------------------------
// alex: added bitrev instruction
template <typename InstructionType, typename StateType>
struct maven_bitrev
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    uint32_t x = proc->read_register(inst->get_rs());

    // software bit reverse
    x = ((x & 0xaaaaaaaa) >> 1)  | ((x & 0x55555555) << 1);
    x = ((x & 0xcccccccc) >> 2)  | ((x & 0x33333333) << 2);
    x = ((x & 0xf0f0f0f0) >> 4)  | ((x & 0x0f0f0f0f) << 4);
    x = ((x & 0xff00ff00) >> 8)  | ((x & 0x00ff00ff) << 8);
    uint32_t top_half = x >> 16;
    uint32_t bot_half = x << 16;

    x = top_half | bot_half;
    reg_t temp = (reg_t) x;

    proc->write_register(inst->get_rd(), temp);

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rd = inst->get_rd();
    proc->b_rs = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "bitrev %s,%s",
      regnames_mips32[inst->get_rd()],
      regnames_mips32[inst->get_rs()]);
  }
};

// alex: added clz instruction
template <typename InstructionType, typename StateType>
struct maven_clz
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    uint32_t x = proc->read_register(inst->get_rs());

    // software clz
    uint32_t bits = 0;
    while (x > 0)
    {
      x >>= 1;
      bits++;
    }

    reg_t temp = (reg_t) (32 - bits);

    proc->write_register(inst->get_rd(), temp);

    // microarch state
    proc->alu = ALU_INT;
    proc->rs = inst->get_rs();
    proc->rd = inst->get_rd();
    proc->b_rs = true;
    proc->b_rd = true;
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf(proc->disassemble_string, "clz %s,%s",
      regnames_mips32[inst->get_rd()],
      regnames_mips32[inst->get_rs()]);
  }
};

// alex: added mulhi instruction
template <typename InstructionType, typename StateType>
struct maven_mulhi
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    uint64_t op_x = proc->read_register(inst->get_rs());
    uint64_t op_y = proc->read_register(inst->get_rt());
    reg_t temp = (reg_t) ((op_x * op_y) >> 32);
    proc->write_register(inst->get_rd(), temp);

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
    sprintf(proc->disassemble_string, "mulhi %s,%s,%s",
      regnames_mips32[inst->get_rd()],
      regnames_mips32[inst->get_rs()],
      regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct maven_div
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    int op_x = proc->read_register(inst->get_rs());
    int op_y = proc->read_register(inst->get_rt());
    reg_t temp = op_x / op_y;
    proc->write_register(inst->get_rd(), temp);

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
    sprintf(proc->disassemble_string, "div %s,%s,%s",
      regnames_mips32[inst->get_rd()],
      regnames_mips32[inst->get_rs()],
      regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct maven_divu
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    unsigned int op_x = proc->read_register(inst->get_rs());
    unsigned int op_y = proc->read_register(inst->get_rt());
    reg_t temp = op_x / op_y;
    proc->write_register(inst->get_rd(), temp);

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
    sprintf(proc->disassemble_string, "divu %s,%s,%s",
      regnames_mips32[inst->get_rd()],
      regnames_mips32[inst->get_rs()],
      regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct maven_rem
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    int op_x = proc->read_register(inst->get_rs());
    int op_y = proc->read_register(inst->get_rt());
    reg_t temp = op_x % op_y;
    proc->write_register(inst->get_rd(), temp);

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
    sprintf(proc->disassemble_string, "rem %s,%s,%s",
      regnames_mips32[inst->get_rd()],
      regnames_mips32[inst->get_rs()],
      regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct maven_remu
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    unsigned int op_x = proc->read_register(inst->get_rs());
    unsigned int op_y = proc->read_register(inst->get_rt());
    reg_t temp = op_x % op_y;
    proc->write_register(inst->get_rd(), temp);

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
    sprintf(proc->disassemble_string, "remu %s,%s,%s",
      regnames_mips32[inst->get_rd()],
      regnames_mips32[inst->get_rs()],
      regnames_mips32[inst->get_rt()]);
  }
};

template <typename InstructionType, typename StateType>
struct maven_vpidx
{
  static void execute( InstructionType* inst, StateType* proc )
  {
    proc->write_register( inst->get_rd(), proc->get_vpidx() );

    // microarch state
    proc->alu = ALU_INT;
    proc->rd = inst->get_rd();
    proc->b_rd = true;
  }
  static void disassemble( InstructionType* inst, StateType* proc )
  {
    sprintf( proc->disassemble_string, "vpidx %s",
             regnames_mips32[inst->get_rd()] );
  }
};

//------------------------------------------------------------------------
// Vector Synchronization Instructions
//------------------------------------------------------------------------

template <typename InstructionType, typename StateType>
struct maven_amo_vv
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    int r_vdst   = inst->get_rd();
    int r_vaddr  = inst->get_rs();
    int r_vsrc   = inst->get_rt();
    int r_mask   = inst->get_msk();
    int cmd_code = inst->get_cmd();

    proc->vparray.amo_vv(r_vdst, r_vaddr, r_vsrc, r_mask, cmd_code);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    int r_mask = inst->get_msk();
    sprintf( proc->disassemble_string, "%s %s,%s,%s%s%s",
             cnames_cop2[inst->get_cmd()&0x7F],
             regnames_vregs_maven[inst->get_rd()],
             regnames_vregs_maven[inst->get_rs()],
             regnames_vregs_maven[inst->get_rt()],
             (r_mask != 0) ? "," : "",
             (r_mask != 0) ? regnames_flags_flood[r_mask] : ""
             );
  }
};

//------------------------------------------------------------------------
// utidx.v
//------------------------------------------------------------------------

template <typename InstructionType, typename StateType>
struct maven_utidx_v
{
  static void execute(InstructionType* inst, StateType* proc)
  {
    int r_vdst   = inst->get_rd();
    int cmd_code = inst->get_cmd();

    proc->vparray.utidx_v(r_vdst, cmd_code);
  }
  static void disassemble(InstructionType* inst, StateType* proc)
  {
    sprintf( proc->disassemble_string, "utidx.v %s",
             regnames_vregs_maven[inst->get_rd()] );
  }
};

