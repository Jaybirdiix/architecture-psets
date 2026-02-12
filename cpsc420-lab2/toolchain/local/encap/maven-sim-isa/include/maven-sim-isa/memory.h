#ifndef __MEMORY_H
#define __MEMORY_H

#include "host.h"
#include "syscfg.h"
#include <assert.h>

//BUG the below statement does not work on
//Mac OS X 10.5 Core 2 Duo. 
// NTOH* do not work

class Memory
{
public:
  Memory()
  {
    mem = new uint8_t [MAVEN_SYSCFG_MEMORY_SIZE];
  }

  ~Memory()
  {
    delete [] mem;
  }

  int8_t read_mem_int8(addr_t addr)
  {
    //printf("read_mem_int8: %08x\n", addr);
    assert( addr < MAVEN_SYSCFG_MEMORY_SIZE );
    return (int8_t)mem[addr];
  }

  uint8_t read_mem_uint8(addr_t addr)
  {
    //printf("read_mem_uint8: %08x\n", addr);
    assert( addr < MAVEN_SYSCFG_MEMORY_SIZE );
    return (uint8_t)mem[addr];
  }

  int16_t read_mem_int16(addr_t addr)
  {
    //printf("read_mem_int16: %08x\n", addr);
//    int16_t temp = NTOHS( *((int16_t*) &mem[addr]) );
    assert( addr < MAVEN_SYSCFG_MEMORY_SIZE );
    int16_t temp = ( *((int16_t*) &mem[addr]) );
    return temp;
  }

  uint16_t read_mem_uint16(addr_t addr)
  {
    //printf("read_mem_uint16: %08x\n", addr);
//    uint16_t temp = NTOHS( (*(uint16_t*) &mem[addr]) );
    assert( addr < MAVEN_SYSCFG_MEMORY_SIZE );
    uint16_t temp = ( (*(uint16_t*) &mem[addr]) );
    return temp;
  }

  int32_t read_mem_int32(addr_t addr)
  {
    //printf("read_mem_int32: %08x\n", addr);
//    int32_t temp = NTOHL( *((int32_t*) &mem[addr]) );
    assert( addr < MAVEN_SYSCFG_MEMORY_SIZE );
    int32_t temp = ( *((int32_t*) &mem[addr]) );
    return temp;
  }

  uint32_t read_mem_uint32(addr_t addr)
  {
    //printf("read_mem_uint32: %08x\n", addr);
    //BUG the below statement does not work on
    //Mac OS X 10.5 Core 2 Duo. 
//    uint32_t temp = NTOHL( *((uint32_t*) &mem[addr]) );
    assert( addr < MAVEN_SYSCFG_MEMORY_SIZE );
    uint32_t temp = ( *((uint32_t*) &mem[addr]) );
    return temp;
  }

  void write_mem_uint8(addr_t addr, uint8_t val)
  {
    //printf("write_mem_uint8: %08x\n", addr);
    assert( addr < MAVEN_SYSCFG_MEMORY_SIZE );
    //if ( addr == 0 )
    //  printf("warning : writing to address zero\n");
    mem[addr] = val;
  }

  void write_mem_uint16(addr_t addr, uint16_t val)
  {
    //printf("write_mem_uint16: %08x\n", addr);
//    uint16_t temp = HTONS(val);
    assert( addr < MAVEN_SYSCFG_MEMORY_SIZE );
    //if ( addr == 0 )
    //  printf("warning : writing to address zero\n");
    uint16_t temp = (val);
    *((uint16_t*) &mem[addr]) = temp;
  }

  void write_mem_uint32(addr_t addr, uint32_t val)
  {
    //printf("write_mem_uint32: %08x\n", addr);
//    uint32_t temp = HTONL(val);
    assert( addr < MAVEN_SYSCFG_MEMORY_SIZE );
    //if ( addr == 0 )
    //  printf("warning : writing to address zero\n");
    uint32_t temp = (val);
    *((uint32_t*) &mem[addr]) = temp;
  }

//private:
  uint8_t* mem;
};

#endif // __MEMORY_H
