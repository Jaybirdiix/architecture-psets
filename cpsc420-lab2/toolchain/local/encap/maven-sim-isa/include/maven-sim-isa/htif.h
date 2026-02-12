/*
 $Header: /projects/assam/cvsroot/scale/sims/common/htif_t.h,v 1.7 2006/09/18 15:58:44 ronny Exp $

   Defines Host-Target interface for simulators/chips running in
   umbilical mode.

 (c) 1993-1998 International Computer Science Institute
 (c) 1993-1998 The Regents of the University of California
 (c) 1998-2002 Massachusetts Institute of Technology

 See COPYING file in top level of source distribution for licence details.

 Contributors:
 Krste Asanovic, ICSI/UCB/MIT krste@mit.edu
 David Johnson, ICSI/UCB, davidj@icsi.berkeley.edu 
 */

#ifndef __HTIF_H
#define __HTIF_H

#include "memif.h"

//#include <stdio.h>

class htif_t
{
public:
    virtual ~htif_t() {};

    virtual memif_t& lmem() = 0;    // Memory port for loading code.
    //virtual MemIF& smem() = 0;    // Server memory port.

    // Set the reset pin on the processor to a given value.  Setting this
    // to 1 resets the processor.  
    virtual void set_reset(int val) = 0;

    // When set to 1 (after reset), the processor may start running.
    // apparently some changed the meaning of this function. I think val
    // is now an entry point. -cb
    virtual void set_run(int val) = 0;

    // Put or remove the processor into/from single-cycle mode.  All
    // implementations will stop once suspended.
    //virtual void set_suspend(int val) = 0;

    // Set the htif_t-controlled interrupt signal in target to a given value
    //virtual void set_intin(int val) = 0;

    // Do htif_t input and output (only 8 bits transferred)
    virtual int get_tohost() = 0;
    virtual void set_fromhost(int val) = 0;
    virtual int get_magicmemaddr() = 0;
    virtual void set_magicmemaddr(int addr) = 0;
    virtual long long get_cycle() = 0;
    
    // neither of these should really be here -cb
    virtual int set_num_cores(int n) = 0;
    virtual void set_num_physical_regs(int nregs) = 0;
    virtual void set_impl_flags(int reconvergence, bool density, bool scoreboard, bool bc, bool vxudebug) = 0;
    virtual void set_logfile(FILE* logfile) = 0;

    // Get the next PC value (what "next" means is not tightly defined).
    //virtual uint32_t get_npc() = 0;

    // Let the processor execute n "cycles" (the processor must already be
    // in suspend mode for this to work).
    //virtual void cycle(long n = 1, volatile int* stopflag_ptr = 0) = 0;

    // Run the processor until the "tohost" register is not equal to 
    // origToHost (note that the processor may continue running after this 
    // call returns).  Returns the new tohost value.
    virtual int run_to_tohost(int origToHost) = 0;

    // Flush an address from the cache
    //virtual void cache_flush(uint32_t addr) = 0;

/*
    // Allow user to switch between writing to memory and writing to I-cache
    enum WriteMemMode {
	WRITEMEM_NORMAL,
	WRITEMEM_CACHE
    };
    virtual void set_writemem_mode(WriteMemMode mode) = 0;

    // Set the I-cache into a specific mode. Note - to invalidate the
    // I-cache after downloading a program, put the cache in invalid then
    // normal mode.

    enum CacheMode {
        CACHE_NORMAL,
        CACHE_FROZEN,
        CACHE_INVALID
    };
    virtual void set_cachemode(CacheMode mode) = 0;

    // Turn on/off fast memory I/O bypassing simulation of the normal
    // memory access path
    virtual void use_fast_memio(bool) = 0;
*/

    // Return the size of memory in this target
    //virtual uint32_t memsize() = 0;

    // Dumps various statistics to logfile.  Seconds is runtime of app.
    virtual void dumpstats(FILE* logfile, double seconds = 0.0) = 0;

};
    
#endif // __HTIF_H
