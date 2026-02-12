#ifndef __MIPS32FPU_H
#define __MIPS32FPU_H

#include <assert.h>
#include <stdio.h>

#include "types.h"
#include "softfloat.h"

class MIPS32FPUState
{
public:
  MIPS32FPUState();

  bool cleanup_state();
  bool set_rounding_mode(int8 mode);
  bool check_double_aligned(int idx);
  bool signal_exception(int8 exception);
  bool handle_exceptions(int8 allowed);

//private:
  reg_t R[32];
  fir_t fir;
  fcsr_t fcsr;
};

#endif // __MIPS32FPU_H
