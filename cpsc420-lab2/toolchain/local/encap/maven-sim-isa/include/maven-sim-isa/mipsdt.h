#ifndef __MIPSDT_H
#define __MIPSDT_H

#include <vector>

#include "sdt.h"

enum DispatchFunctions
{
  DISPATCH_EXECUTE,
  DISPATCH_DISASSEMBLE,
  DISPATCH_COUNT
};

template <typename MIPS32InstructionStruct, typename InstructionType, typename StateType>
std::vector<void (*)(InstructionType*, StateType*)> make_dispatch_func_vector()
{
  std::vector<void (*)(InstructionType*, StateType*)> func_vector(DISPATCH_COUNT);
  func_vector[DISPATCH_EXECUTE] = MIPS32InstructionStruct::execute;
  func_vector[DISPATCH_DISASSEMBLE] = MIPS32InstructionStruct::disassemble;
  return func_vector;
}

template <typename InstructionType, typename StateType>
class MIPSGenericDispatchTable
  : public SmartDispatchTable<InstructionType, StateType, DISPATCH_COUNT>
{
public:
  MIPSGenericDispatchTable(int _shift, int _bits)
    : SmartDispatchTable<InstructionType, StateType, DISPATCH_COUNT>(_shift, _bits)
  {
  }

  void execute(InstructionType* inst, StateType* proc)
  {
    dispatch(DISPATCH_EXECUTE, inst, proc);
  }

  void disassemble(InstructionType* inst, StateType* proc)
  {
    dispatch(DISPATCH_DISASSEMBLE, inst, proc);
  }
};

#endif // MIPSDT_H
