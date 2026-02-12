#ifndef __SYS_LOCK_H__
#define __SYS_LOCK_H__

typedef int _LOCK_T;
typedef long long _LOCK_RECURSIVE_T;

#define __LOCK_INIT(class,lock) static _LOCK_T lock = 0
#define __LOCK_INIT_RECURSIVE(class,lock) static _LOCK_RECURSIVE_T lock = 0;
#define __lock_init(lock) (lock = 0)
#define __lock_init_recursive(lock) (lock = 0)
#define __lock_acquire(lock) newlib_lock(&lock)
#define __lock_try_acquire(lock) newlib_trylock(&lock)
#define __lock_close(lock) (lock = 0)
#define __lock_close_recursive(lock)
#define __lock_acquire_recursive(lock) newlib_recursive_lock(&lock)
#define __lock_try_acquire_recursive(lock) newlib_recursive_trylock(&lock)
#define __lock_release(lock) (lock = 0)
#define __lock_release_recursive(lock) newlib_recursive_unlock(&lock)

int hart_self();
int newlib_trylock(int* lock);
void newlib_lock(int* lock);
void newlib_recursive_lock(long long* lock);
int newlib_recursive_trylock(long long* lock);
void newlib_recursive_unlock(long long* lock);

/* dummy lock routines for single-threaded aps */
/*
typedef int _LOCK_T;
typedef int _LOCK_RECURSIVE_T;
 
#include <_ansi.h>

#define __LOCK_INIT(class,lock) static int lock = 0;
#define __LOCK_INIT_RECURSIVE(class,lock) static int lock = 0;
#define __lock_init(lock) (_CAST_VOID 0)
#define __lock_init_recursive(lock) (_CAST_VOID 0)
#define __lock_close(lock) (_CAST_VOID 0)
#define __lock_close_recursive(lock) (_CAST_VOID 0)
#define __lock_acquire(lock) (_CAST_VOID 0)
#define __lock_acquire_recursive(lock) (_CAST_VOID 0)
#define __lock_try_acquire(lock) (_CAST_VOID 0)
#define __lock_try_acquire_recursive(lock) (_CAST_VOID 0)
#define __lock_release(lock) (_CAST_VOID 0)
#define __lock_release_recursive(lock) (_CAST_VOID 0) 
*/

#endif /* __SYS_LOCK_H__ */
