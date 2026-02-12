#ifndef __STATIC_DISPATCH_TABLE
#define __STATIC_DISPATCH_TABLE

#include <vector>
#include <cstring>

template <
  typename InstructionType,
  typename StateType,
  int FunctionCount
>
class SmartDispatchTable
{
public:
  typedef void (*SmartDispatchTableFunction) (InstructionType*, StateType*);

  SmartDispatchTable(int _shift, int _bits)
  {
    shift = _shift;
    mask = (1 << _bits) - 1;
    entry = 1; for (int i=0; i<_bits; i++) entry *= 2;
    func_table = new SmartDispatchTableFunction [entry*FunctionCount];
    class_table = new SmartDispatchTable* [entry];
    memset(func_table, 0, sizeof(SmartDispatchTableFunction)*entry*FunctionCount);
    memset(class_table, 0, sizeof(SmartDispatchTable*)*entry);
  }

  ~SmartDispatchTable()
  {
    delete [] func_table;
  }

  void register_instruction(int index, std::vector<SmartDispatchTableFunction> funcs)
  {
    index *= FunctionCount;

    for (int i=0; i<FunctionCount; i++)
    {
      func_table[index+i] = funcs[i];
    }
  }

  void register_instruction_lv1(int index0, int index1, std::vector<SmartDispatchTableFunction> funcs)
  {
    index1 *= FunctionCount;

    for (int i=0; i<FunctionCount; i++)
    {
      class_table[index0]->func_table[index1+i] = funcs[i];
    }
  }

  void register_all_instructions(std::vector<SmartDispatchTableFunction> funcs)
  {
    for (int i=0; i<entry; i++)
    {
      for (int j=0; j<FunctionCount; j++)
      {
        func_table[i*FunctionCount+j] = funcs[j];
      }
    }
  }

  void register_dispatch_table(int index, SmartDispatchTable& dt)
  {
    class_table[index] = &dt;
    for (int i=0; i<FunctionCount; i++)
    {
      func_table[index*FunctionCount+i] = NULL;
    }
  }

  void dispatch(int functionidx, InstructionType* inst, StateType* proc)
  {
    unsigned int index = ((*inst)>>shift) & mask;
    SmartDispatchTableFunction func = func_table[index*FunctionCount+functionidx];
    SmartDispatchTable* prev = this;

    while (!func)
    {
      SmartDispatchTable* next = prev->class_table[index];
      index = ((*inst)>>next->shift) & (next->mask);
      func = next->func_table[index*FunctionCount+functionidx];
      prev = next;
    }

    func(inst, proc);
  }

private:
  int shift;
  unsigned int mask;
  int entry;
  SmartDispatchTableFunction* func_table;
  SmartDispatchTable** class_table;
};

#endif // __STATIC_DISPATCH_TABLE
