#ifndef __UTILS_H
#define __UTILS_H

#include "types.h"
#include "host.h"

#define debug_assert(cond) \
  do { \
    if(!(cond)) { \
      fprintf(stderr,"%s[%d]: assertion (%s) failed\n",__FILE__,__LINE__,#cond); \
      exit(-1); \
    } \
  } while(0)

#define notice(debuglevel,str,args...) \
  do { \
    if(debug_level >= (debuglevel)) \
      printf(str,##args);              \
  } while(0)

#define warn(str,args...) \
  do { \
    fprintf(stderr,"%s[%d]: ",__FILE__,__LINE__); \
    fprintf(stderr,str"\n",##args); \
  } while(0)

#define error(str,args...) \
  do { \
    warn(str,##args); \
    exit(-1); \
  } while(0)

#define trace(file,str,args...) \
  do { \
    if(vptrace)  \
      fprintf(file,str,##args);              \
  } while(0)

bool is_overflow_add(int32_t sum, int32_t a, int32_t b);
bool is_overflow_sub(int32_t sub, int32_t a, int32_t b);
void multiply_helper(int32_t op1, int32_t op2, reg_t& reslo, reg_t& reshi);
int parse_args(int argc, char** argv, char* opts, ...);

#endif // __UTILS_H
