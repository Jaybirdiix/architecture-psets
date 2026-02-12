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

#ifndef __SYSARGS_H
#define __SYSARGS_H

#include <sys/stat.h>
#include <fcntl.h>
#include "htif.h"

struct solaris_stat 
{
    int16_t     st_dev;
    uint16_t    st_ino;
    int32_t     st_mode;
    uint16_t    st_nlink;
    uint16_t    st_uid;
    uint16_t    st_gid;
    int16_t     st_rdev;
    int32_t     st_size;
    int32_t     st_pad3;
    int32_t     _st_atime;
    int32_t     st_spare1;
    int32_t     _st_mtime;
    int32_t     st_spare2;
    int32_t     _st_ctime;
    int32_t     st_spare3;
    int32_t     st_blksize;
    int32_t     st_blocks;
    int32_t     st_spare4[2];
};

#define NEWLIB_O_RDONLY 	0
#define NEWLIB_O_WRONLY 	1
#define NEWLIB_O_RDWR 		2
#define NEWLIB_O_APPEND 	0x0008
#define NEWLIB_O_CREAT 		0x0200
#define NEWLIB_O_TRUNC 		0x0400
#define NEWLIB_O_EXCL 		0x0800
#define NEWLIB_O_SYNC 		0x2000
#define NEWLIB_O_NDELAY		NEWLIB_O_NONBLOCK 
#define NEWLIB_O_NONBLOCK 	0x4000
#define NEWLIB_O_NOCTTY 	0x8000

class sysargs_t
{
public:
    sysargs_t(htif_t& _htif, uint32_t _addr, uint32_t _len);
    ~sysargs_t();
    void* buffer();
    void* get_buffer();
    void put_buffer();
    void convert_stat(struct stat* linux_stat);
    static int convert_flag(int solaris_flag);

private:
    htif_t& htif;
    uint32_t addr;
    uint32_t len;
    void* buf;
};

#endif // __SYSARGS_H
