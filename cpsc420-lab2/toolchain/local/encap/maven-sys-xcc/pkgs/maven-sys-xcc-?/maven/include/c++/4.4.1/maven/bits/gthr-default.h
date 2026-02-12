#ifndef _GLIBCXX_GCC_GTHR_BTHREAD_H
#define _GLIBCXX_GCC_GTHR_BTHREAD_H

#define __GTHREADS 1

#include <machine/syscfg.h>
#include <machine/syscall.h>
#include <errno.h>
//#include <bthread.h>
// manually including bthread.h

#define __BTHREAD_MUTEX_INIT 0
#define __BTHREAD_ONCE_INIT  { __BTHREAD_MUTEX_INIT, 0 }

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int __bthread_t;
typedef unsigned int __bthread_key_t;
typedef unsigned int __bthread_mutex_t;

typedef struct {
  __bthread_mutex_t mutex;
  int once;
} __bthread_once_t;

static inline __bthread_t __bthread_self()
{
  unsigned int procid;
  __asm__ ( "mfc0 %0, $%1" : "=r"(procid) 
                           : "i"(MAVEN_SYSCFG_REGDEF_COP0_CORE_ID) );
  return procid;
}

// returns true if there is more than 1 core in the system
static inline int __bthread_threading()
{
  int numcores, error_flag;
  MAVEN_SYSCALL_ARG0(NUMCORES, numcores, error_flag);

  return (numcores > 1);
}

static inline int __bthread_mutex_trylock(__bthread_mutex_t* lock)
{
  int val = 1;
  __asm__ __volatile__ ("amo.or %0, %1, %2" : "=r"(val) : "r"(lock), "0"(val) : "memory");
  return val;
}

static inline int __bthread_mutex_lock(__bthread_mutex_t* lock)
{
  while(__bthread_mutex_trylock(lock))
    while(*(volatile int*)lock);
    
  return 0;
}

static inline int __bthread_mutex_unlock(__bthread_mutex_t* lock)
{
  *lock = 0;
  return 0;
}

static inline int
__bthread_once (__bthread_once_t *__once, void (*__func) (void))
{
//  if (! __gthread_active_p ())
//    return -1;

  if (__once == 0 || __func == 0)
    return EINVAL;

  if (__once->once == 0)
    {
      int __status = __bthread_mutex_lock (&__once->mutex);
      if (__status != 0)
	return __status;
      if (__once->once == 0)
	{
	  (*__func) ();
	  __once->once++;
	}
      __bthread_mutex_unlock (&__once->mutex);
    }

  return 0;  
}

static inline int 
__bthread_key_create( __bthread_key_t* __key, void (*__dtor) (void*) )
                                        
{
  int result, error_flag;
  MAVEN_SYSCALL_ARG2(BTHREAD_KEY_CREATE,result,error_flag,__key,__dtor);
  if (error_flag)
  {
//    errno = result;
    return -1;
  }
  
  return result;
}

static inline int
__bthread_key_delete( __bthread_key_t __key )
{
  int result, error_flag;
  int val, *pval;
  void (*dtor)(void*);
  void *pdtor = &dtor;
  pval = &val;
  
  MAVEN_SYSCALL_ARG3(BTHREAD_KEY_DELETE,result,error_flag,__key,pdtor,pval);
  if (error_flag)
  {
//    errno = result;
    return -1;
  }
  
  if (dtor)
    dtor((void *)val);
  
  return result;
}

static inline int
__bthread_setspecific( __bthread_key_t __key, const void* __ptr )
{
  int result, error_flag;
  MAVEN_SYSCALL_ARG2(BTHREAD_KEY_SETSPECIFIC,result,error_flag,__key,__ptr);
  if (error_flag)
  {
//    errno = result;
    return -1;
  }
  
  return result;
}

static inline void* 
__bthread_getspecific( __bthread_key_t __key )
{
  int result, error_flag;
  MAVEN_SYSCALL_ARG1(BTHREAD_KEY_GETSPECIFIC,result,error_flag,__key);
  if (error_flag)
  {
//    errno = result;
    return (void*) -1;
  }
  
  return (void *) result;
}

#ifdef __cplusplus
}
#endif

// bthread.h

#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef __bthread_key_t __gthread_key_t;
typedef __bthread_once_t __gthread_once_t;
typedef __bthread_mutex_t __gthread_mutex_t;

typedef struct {
  long depth;
  __bthread_t owner;
  __bthread_mutex_t actual;
} __gthread_recursive_mutex_t;

#define __GTHREAD_MUTEX_INIT __BTHREAD_MUTEX_INIT
#define __GTHREAD_ONCE_INIT __BTHREAD_ONCE_INIT

static inline int
__gthread_recursive_mutex_init_function(__gthread_recursive_mutex_t *__mutex);
#define __GTHREAD_RECURSIVE_MUTEX_INIT_FUNCTION __gthread_recursive_mutex_init_function

#if __GXX_WEAK__ && _GLIBCXX_GTHREAD_USE_WEAK
# define __gthrw(name) \
  static __typeof(name) __gthrw_ ## name __attribute__ ((__weakref__(#name)));
# define __gthrw_(name) __gthrw_ ## name
#else
# define __gthrw(name)
# define __gthrw_(name) name
#endif

__gthrw(__bthread_once)
__gthrw(__bthread_key_create)
__gthrw(__bthread_key_delete)
__gthrw(__bthread_getspecific)
__gthrw(__bthread_setspecific)

__gthrw(__bthread_self)

__gthrw(__bthread_mutex_lock)
__gthrw(__bthread_mutex_trylock)
__gthrw(__bthread_mutex_unlock)

__gthrw(__bthread_threading)

// todo: verify this works as intended
#if __GXX_WEAK__ && _GLIBCXX_GTHREAD_USE_WEAK

static inline int
__gthread_active_p (void)
{
  return (__bthread_threading)();
}

#else /* not __GXX_WEAK__ */

static inline int
__gthread_active_p (void)
{
  return 1;
}

#endif /* __GXX_WEAK__ */

static inline int
__gthread_once (__gthread_once_t *__once, void (*__func) (void))
{
  if (__gthread_active_p ())
    return __gthrw_(__bthread_once) (__once, __func);
  else
    return -1;
}

static inline int
__gthread_key_create (__gthread_key_t *__key, void (*__dtor) (void *))
{
  return __gthrw_(__bthread_key_create) (__key, __dtor);
}

static inline int
__gthread_key_delete (__gthread_key_t __key)
{
  return __gthrw_(__bthread_key_delete) (__key);
}

static inline void *
__gthread_getspecific (__gthread_key_t __key)
{
  return __gthrw_(__bthread_getspecific) (__key);
}

static inline int
__gthread_setspecific (__gthread_key_t __key, const void *__ptr)
{
  return __gthrw_(__bthread_setspecific) (__key, __ptr);
}

static inline int
__gthread_mutex_destroy (__gthread_mutex_t *__mutex)
{
  return 0;
}

static inline int
__gthread_mutex_lock (__gthread_mutex_t *__mutex)
{
  if (__gthread_active_p ())
    return __gthrw_(__bthread_mutex_lock) (__mutex);
  else
    return 0;
}

static inline int
__gthread_mutex_trylock (__gthread_mutex_t *__mutex)
{
  if (__gthread_active_p ())
    return __gthrw_(__bthread_mutex_trylock) (__mutex);
  else
    return 0;
}

static inline int
__gthread_mutex_unlock (__gthread_mutex_t *__mutex)
{
  if (__gthread_active_p ())
    return __gthrw_(__bthread_mutex_unlock) (__mutex);
  else
    return 0;
}

static inline int
__gthread_recursive_mutex_init_function (__gthread_recursive_mutex_t *__mutex)
{
  __mutex->depth = 0;
  __mutex->owner = __gthrw_(__bthread_self) ();
  __mutex->actual = __BTHREAD_MUTEX_INIT;
  return 0;
}

static inline int
__gthread_recursive_mutex_lock (__gthread_recursive_mutex_t *__mutex)
{
  if (__gthread_active_p ())
    {
      __bthread_t __me = __gthrw_(__bthread_self) ();

      if (__mutex->owner != __me)
	{
	  __gthrw_(__bthread_mutex_lock) (&__mutex->actual);
	  __mutex->owner = __me;
	}

      __mutex->depth++;
    }
  return 0;
}

static inline int
__gthread_recursive_mutex_trylock (__gthread_recursive_mutex_t *__mutex)
{
  if (__gthread_active_p ())
    {
      __bthread_t __me = __gthrw_(__bthread_self) ();

      if (__mutex->owner != __me)
	{
	  if (__gthrw_(__bthread_mutex_trylock) (&__mutex->actual))
	    return 1;
	  __mutex->owner = __me;
	}

      __mutex->depth++;
    }
  return 0;
}

// todo: add error checking - what to do if this is called when
// we aren't the owner of the lock?
static inline int
__gthread_recursive_mutex_unlock (__gthread_recursive_mutex_t *__mutex)
{
  if (__gthread_active_p ())
    {
      if (--__mutex->depth == 0)
	  {
	   __mutex->owner = (__bthread_t) 0;
	   __gthrw_(__bthread_mutex_unlock) (&__mutex->actual);
	  }
    }
  return 0;
}

#ifdef __cplusplus
}
#endif

#endif
