/* Author: Yunsup Lee
 *         Parallel Computing Laboratory
 *         Electrical Engineering and Computer Sciences
 *         University of California, Berkeley
 *
 * Copyright (c) 2008, The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE REGENTS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __MEMIF_MIPSFS_H
#define __MEMIF_MIPSFS_H

#include "memif.h"
#include "simulator.h"

class memif_mavenfs_t : public memif_t
{
public:
    memif_mavenfs_t(simulator_t& _sim);

    memif_t::error read(uint32_t addr, uint32_t len, uint8_t* bytes);
    memif_t::error write(uint32_t addr, uint32_t len, const uint8_t* bytes);
    
    memif_t::error read_uint8(uint32_t addr, uint8_t* word);
    memif_t::error write_uint8(uint32_t addr, uint8_t word);
    
    memif_t::error read_int8(uint32_t addr, int8_t* word);
    memif_t::error write_int8(uint32_t addr, int8_t word);
    
    memif_t::error read_uint16(uint32_t addr, uint16_t* word);
    memif_t::error write_uint16(uint32_t addr, uint16_t word);
    
    memif_t::error read_int16(uint32_t addr, int16_t* word);
    memif_t::error write_int16(uint32_t addr, int16_t word);
    
    memif_t::error read_uint32(uint32_t addr, uint32_t* word);
    memif_t::error write_uint32(uint32_t addr, uint32_t word);
    
    memif_t::error read_int32(uint32_t addr, int32_t* word);
    memif_t::error write_int32(uint32_t addr, int32_t word);
    
    memif_t::error read_uint64(uint32_t addr, uint64_t* word);
    memif_t::error write_uint64(uint32_t addr, uint64_t word);
    
    memif_t::error read_int64(uint32_t addr, int64_t* word);
    memif_t::error write_int64(uint32_t addr, int64_t word);

private:
    simulator_t& sim;
};

#endif // MEMIF_MIPSFS_H
