#ifndef __MAVENISA_H
#define __MAVENISA_H

#include <vector>

#include "mipsdt.h"
#include "instruction.h"
#include "processor.h"
#include "mips32-ISA.h"

class MavenCPISA
{
public:
  template <typename InstructionType, typename StateType>
  static void fill_dispatch_table(MIPSGenericDispatchTable<InstructionType, StateType>& dt);

  static MIPSGenericDispatchTable<MavenCPInstruction, MavenCPState>& dt();
};

class MavenVPISA
{
public:
  template <typename InstructionType, typename StateType>
  static void fill_dispatch_table(MIPSGenericDispatchTable<InstructionType, StateType>& dt);

  static MIPSGenericDispatchTable<MavenVPInstruction, MavenVPState>& dt();
};

enum
{
  MAVEN_CP_COP0     = 0x10,
  MAVEN_CP_COP2     = 0x12,
  MAVEN_CP_VCFGIVL  = 0x37,
  MAVEN_CP_VF       = 0x34,
  MAVEN_CP_VLOAD    = 0x18,
  MAVEN_CP_VLOAD_ST = 0x19,
  MAVEN_CP_VLOAD_X  = 0x2c,
  MAVEN_CP_VSTORE   = 0x1a,
  MAVEN_CP_VSTORE_ST= 0x1b,
  MAVEN_CP_VSTORE_X = 0x2d,
  MAVEN_MISC        = 0x27,
};

enum
{
  MAVEN_CP_COP0_MFC0 = 0x00,
  MAVEN_CP_COP0_MTC0 = 0x04,
  MAVEN_CP_COP0_ERET = 0x10,
};
  
enum
{
  MAVEN_CP_COP2_SETVL   = 0x02,
  MAVEN_CP_COP2_SYNC_V  = 0x05,
  MAVEN_CP_COP2_SYNC_CV = 0x06,
  MAVEN_CP_COP2_MTVP    = 0x07,
  MAVEN_CP_COP2_MFVP    = 0x09,
  MAVEN_CP_COP2_MTVPS   = 0x08,
  
  //traditional vector ops (0x80-0xFF)
  MAVEN_CP_COP2_ADDU_VV        = 0x81,
  MAVEN_CP_COP2_ADDU_VS        = 0x82,
  MAVEN_CP_COP2_SUBU_VV        = 0x83,
  MAVEN_CP_COP2_SUBU_VS        = 0x84,
  MAVEN_CP_COP2_SUBU_SV        = 0x85,
  MAVEN_CP_COP2_MUL_VV         = 0x86,
  MAVEN_CP_COP2_MUL_VS         = 0x87,
  // alex: mulhi added, nonconsecutive cmd code space
  MAVEN_CP_COP2_MULHI_VV       = 0xfe,
  MAVEN_CP_COP2_MULHI_VS       = 0xff,
   
  MAVEN_CP_COP2_DIV_VV         = 0x88,
  MAVEN_CP_COP2_DIV_VS         = 0x89,
  MAVEN_CP_COP2_DIV_SV         = 0x8a,
  MAVEN_CP_COP2_REM_VV         = 0x8b,
  MAVEN_CP_COP2_REM_VS         = 0x8c,
  MAVEN_CP_COP2_REM_SV         = 0x8d,
   
  MAVEN_CP_COP2_DIVU_VV        = 0x80,
  MAVEN_CP_COP2_DIVU_VS        = 0x9f,
  MAVEN_CP_COP2_DIVU_SV        = 0xbd,
  MAVEN_CP_COP2_REMU_VV        = 0xbe,
  MAVEN_CP_COP2_REMU_VS        = 0xbf,
  MAVEN_CP_COP2_REMU_SV        = 0xf0,
  
  MAVEN_CP_COP2_SLL_VV         = 0x8e,
  MAVEN_CP_COP2_SLL_VS         = 0x8f,
  MAVEN_CP_COP2_SLL_SV         = 0x90,
  MAVEN_CP_COP2_SRL_VV         = 0x91,
  MAVEN_CP_COP2_SRL_VS         = 0x92,
  MAVEN_CP_COP2_SRL_SV         = 0x93,
  MAVEN_CP_COP2_SRA_VV         = 0x94,
  MAVEN_CP_COP2_SRA_VS         = 0x95,
  MAVEN_CP_COP2_SRA_SV         = 0x96,
  MAVEN_CP_COP2_AND_VV         = 0x97,
  MAVEN_CP_COP2_AND_VS         = 0x98,
  MAVEN_CP_COP2_OR_VV          = 0x99,
  MAVEN_CP_COP2_OR_VS          = 0x9a,
  MAVEN_CP_COP2_XOR_VV         = 0x9b,
  MAVEN_CP_COP2_XOR_VS         = 0x9c,
  MAVEN_CP_COP2_NOR_VV         = 0x9d,
  MAVEN_CP_COP2_NOR_VS         = 0x9e,

  //alex: bitrev added
  MAVEN_CP_COP2_BITREV_V       = 0xfd,
   
  MAVEN_CP_COP2_ADD_S_VV       = 0xa0,
  MAVEN_CP_COP2_ADD_S_VS       = 0xa1,
  MAVEN_CP_COP2_ADD_S_SV       = 0xa2,
  MAVEN_CP_COP2_SUB_S_VV       = 0xa3,
  MAVEN_CP_COP2_SUB_S_VS       = 0xa4,
  MAVEN_CP_COP2_SUB_S_SV       = 0xa5,
  MAVEN_CP_COP2_MUL_S_VV       = 0xa6,
  MAVEN_CP_COP2_MUL_S_VS       = 0xa7,
  MAVEN_CP_COP2_MUL_S_SV       = 0xa8,
  MAVEN_CP_COP2_DIV_S_VV       = 0xa9,
  MAVEN_CP_COP2_DIV_S_VS       = 0xaa,
  MAVEN_CP_COP2_DIV_S_SV       = 0xab,
  MAVEN_CP_COP2_ABS_S_V        = 0xac,
  MAVEN_CP_COP2_NEG_S_V        = 0xad,
  MAVEN_CP_COP2_ROUND_W_S_V    = 0xae,
  MAVEN_CP_COP2_TRUNC_W_S_V    = 0xaf,
  MAVEN_CP_COP2_CEIL_W_S_V     = 0xb0,
  MAVEN_CP_COP2_FLOOR_W_S_V    = 0xb1,
  MAVEN_CP_COP2_RECIP_S_V      = 0xb2,
  MAVEN_CP_COP2_RSQRT_S_V      = 0xb3,
  MAVEN_CP_COP2_SQRT_S_V       = 0xb4,
  MAVEN_CP_COP2_CVT_S_W_V      = 0xb5,
  MAVEN_CP_COP2_CVT_W_S_V      = 0xb6,
  MAVEN_CP_COP2_SEQ_F_VV       = 0xb7,
  MAVEN_CP_COP2_SLT_F_VV       = 0xb8,
  MAVEN_CP_COP2_SLTU_F_VV      = 0xf6,
  
  MAVEN_CP_COP2_MTVPS_F        = 0xb9,
  MAVEN_CP_COP2_MFVPS_F        = 0xba,
  MAVEN_CP_COP2_POPC_F         = 0xbb,
  MAVEN_CP_COP2_FINDFONE_F     = 0xbc,
  
  MAVEN_CP_COP2_S_C_F_F_VV     = 0xC0,
  MAVEN_CP_COP2_S_C_UN_F_VV    = 0xC1,
  MAVEN_CP_COP2_S_C_EQ_F_VV    = 0xC2,
  MAVEN_CP_COP2_S_C_UEQ_F_VV   = 0xC3,
  MAVEN_CP_COP2_S_C_OLT_F_VV   = 0xC4,
  MAVEN_CP_COP2_S_C_ULT_F_VV   = 0xC5,
  MAVEN_CP_COP2_S_C_OLE_F_VV   = 0xC6,
  MAVEN_CP_COP2_S_C_ULE_F_VV   = 0xC7,
  MAVEN_CP_COP2_S_C_SF_F_VV    = 0xC8,
  MAVEN_CP_COP2_S_C_NGLE_F_VV  = 0xC9,
  MAVEN_CP_COP2_S_C_SEQ_F_VV   = 0xCA,
  MAVEN_CP_COP2_S_C_NGL_F_VV   = 0xCB,
  MAVEN_CP_COP2_S_C_LT_F_VV    = 0xCC,
  MAVEN_CP_COP2_S_C_NGE_F_VV   = 0xCD,
  MAVEN_CP_COP2_S_C_LE_F_VV    = 0xCE,
  MAVEN_CP_COP2_S_C_NGT_F_VV   = 0xCF,
  MAVEN_CP_COP2_S_C_F_F_VS     = 0xD0,
  MAVEN_CP_COP2_S_C_UN_F_VS    = 0xD1,
  MAVEN_CP_COP2_S_C_EQ_F_VS    = 0xD2,
  MAVEN_CP_COP2_S_C_UEQ_F_VS   = 0xD3,
  MAVEN_CP_COP2_S_C_OLT_F_VS   = 0xD4,
  MAVEN_CP_COP2_S_C_ULT_F_VS   = 0xD5,
  MAVEN_CP_COP2_S_C_OLE_F_VS   = 0xD6,
  MAVEN_CP_COP2_S_C_ULE_F_VS   = 0xD7,
  MAVEN_CP_COP2_S_C_SF_F_VS    = 0xD8,
  MAVEN_CP_COP2_S_C_NGLE_F_VS  = 0xD9,
  MAVEN_CP_COP2_S_C_SEQ_F_VS   = 0xDA,
  MAVEN_CP_COP2_S_C_NGL_F_VS   = 0xDB,
  MAVEN_CP_COP2_S_C_LT_F_VS    = 0xDC,
  MAVEN_CP_COP2_S_C_NGE_F_VS   = 0xDD,
  MAVEN_CP_COP2_S_C_LE_F_VS    = 0xDE,
  MAVEN_CP_COP2_S_C_NGT_F_VS   = 0xDF,
  MAVEN_CP_COP2_S_C_F_F_SV     = 0xE0,
  MAVEN_CP_COP2_S_C_UN_F_SV    = 0xE1,
  MAVEN_CP_COP2_S_C_EQ_F_SV    = 0xE2,
  MAVEN_CP_COP2_S_C_UEQ_F_SV   = 0xE3,
  MAVEN_CP_COP2_S_C_OLT_F_SV   = 0xE4,
  MAVEN_CP_COP2_S_C_ULT_F_SV   = 0xE5,
  MAVEN_CP_COP2_S_C_OLE_F_SV   = 0xE6,
  MAVEN_CP_COP2_S_C_ULE_F_SV   = 0xE7,
  MAVEN_CP_COP2_S_C_SF_F_SV    = 0xE8,
  MAVEN_CP_COP2_S_C_NGLE_F_SV  = 0xE9,
  MAVEN_CP_COP2_S_C_SEQ_F_SV   = 0xEA,
  MAVEN_CP_COP2_S_C_NGL_F_SV   = 0xEB,
  MAVEN_CP_COP2_S_C_LT_F_SV    = 0xEC,
  MAVEN_CP_COP2_S_C_NGE_F_SV   = 0xED,
  MAVEN_CP_COP2_S_C_LE_F_SV    = 0xEE,
  MAVEN_CP_COP2_S_C_NGT_F_SV   = 0xEF,
  
  MAVEN_CP_COP2_AND_F          = 0xf1,
  MAVEN_CP_COP2_MOV_F          = 0xf3,
  MAVEN_CP_COP2_OR_F           = 0xf7,
  MAVEN_CP_COP2_NOT_F          = 0xfc,

  MAVEN_CP_COP2_AMO_OR_VV      = 0xf9,
  MAVEN_CP_COP2_AMO_AND_VV     = 0xfa,
  MAVEN_CP_COP2_AMO_ADD_VV     = 0xfb,

  MAVEN_CP_COP2_UTIDX_V        = 0xf8,
  
};                         

enum
{
  MAVEN_MISC_STOP    = 0x00,
  MAVEN_MISC_SYNC_L  = 0x01,
  MAVEN_MISC_AMO_ADD = 0x02,
  MAVEN_MISC_AMO_AND = 0x03,
  MAVEN_MISC_AMO_OR  = 0x04,
  MAVEN_MISC_DIV     = 0x05,
  MAVEN_MISC_REM     = 0x06,
  MAVEN_MISC_DIVU    = 0x07,
  MAVEN_MISC_REMU    = 0x08,
  MAVEN_MISC_VPIDX   = 0x09,
  // alex: added mulhi, clz, bitrev
  MAVEN_MISC_MULHI   = 0x0A,
  MAVEN_MISC_CLZ     = 0x0B,
  MAVEN_MISC_BITREV  = 0x0C,
};

#endif // __MAVENISA_H
