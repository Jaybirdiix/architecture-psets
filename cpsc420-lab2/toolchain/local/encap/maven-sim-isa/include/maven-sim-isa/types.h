#ifndef __TYPES_H
#define __TYPES_H

#define __STDC_LIMIT_MACROS
#include <stdint.h>

typedef uint32_t     reg_t;
typedef uint32_t     addr_t;

//TODO make this MAXLEN
//typedef bool[32] rflag_t;

// Condition Flag Register
//typedef struct
//{
//  bool 
//} rflag_t;

// Floating Point Implementation Register (FIR/FCR0)
typedef struct
{
  uint32_t revision : 8;
  uint32_t processorid : 8;
  uint32_t s : 1;
  uint32_t d : 1;
  uint32_t ps : 1;
  uint32_t threed : 1;
  uint32_t w : 1;
  uint32_t l : 1;
  uint32_t f64 : 1;
  uint32_t _zeros0 : 1;
  uint32_t _impl : 4;
  uint32_t _zeros1 : 4;
} fir_reg_t;

// Floating Point Control/Status Register (FCSR/FCR31)
typedef struct
{
  uint32_t rm : 2;
  uint32_t flags : 5;
  uint32_t enables : 5;
  uint32_t cause : 6;
  uint32_t _zeros : 3;
  uint32_t _impl : 2;
  uint32_t fcc0 : 1;
  uint32_t fs : 1;
  uint32_t fcc : 7;
} fcsr_reg_t;

// Floating Point Condition Codes Register (FCCR/FCR25)
typedef struct
{
  uint32_t fcc : 8;
  uint32_t _zeros : 24;
} fccr_reg_t;

// Floating Point Exceptions Register (FEXR/FCR26)
typedef struct
{
  uint32_t _zeros0 : 2;
  uint32_t flags : 5;
  uint32_t _zeros1 : 5;
  uint32_t cause : 6;
  uint32_t _zeros2 : 14;
} fexr_reg_t;

// Floating Point Enables Register (FENR/FCR28)
typedef struct
{
  uint32_t rm : 2;
  uint32_t fs : 1;
  uint32_t _zeros0 : 4;
  uint32_t enables : 5;
  uint32_t _zeros1 : 20;
} fenr_reg_t;

union fir_t
{
  fir_reg_t reg;
  uint32_t bits;
};

union fcsr_t
{
  fcsr_reg_t reg;
  uint32_t bits;
};

union fccr_t
{
  fccr_reg_t reg;
  uint32_t bits;
};

union fexr_t
{
  fexr_reg_t reg;
  uint32_t bits;
};

union fenr_t
{
  fenr_reg_t reg;
  uint32_t bits;
};

#endif // __TYPES_H
