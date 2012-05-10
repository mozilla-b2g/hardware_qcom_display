/*
 * Copyright (c) 2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *    * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef OVERLAY_MEM_H
#define OVERLAY_MEM_H

#include <fcntl.h>

#include <alloc_controller.h>
#include <memalloc.h>

#include "overlayFD.h"

namespace overlay2 {

/*
 * Holds pmem address, offset and the fd
 * */
class OvMem {
public:
   /* ctor init*/
   explicit OvMem();

   /* dtor DO NOT call close so it can be copied */
   ~OvMem();

   /* Use libgralloc to retrieve fd, pmem addr, alloc type */
   bool open(utils::ePmemSource pmemsrc, uint32_t numbufs,
             uint32_t bufSz, int flags = O_RDWR);

   /* close fd. assign pmem address to invalid*/
   bool close();

   /* return underlying fd */
   int getFD() const;

   /* return true if fd is valid and pmem address is valid */
   bool valid() const;

   /* dump the state of the object */
   void dump() const;

   /* return underlying address */
   void* addr() const;

   /* return pmem source type */
   utils::ePmemSource getPmemSrc() const;

   /* return underlying offset */
   uint32_t bufSz() const;

   /* return number of bufs */
   uint32_t numBufs() const ;

private:
   /* actual os fd */
   int mFd;

   /* points to pmem addr (mmap)*/
   void* mPmemAddr;

   /* allocated buffer type determined by gralloc (adsp, ashmem, ion, etc) */
   int mAllocType;

   /* pmem source type (adsp or smi) */
   utils::ePmemSource mPmemSrc;

   /* holds buf size */
   uint32_t mBufSz;

   /* num of bufs */
   uint32_t mNumBuffers;

   /* gralloc alloc controller */
   android::sp<gralloc::IAllocController> mAlloc;
};

} // overlay2

#include "overlayMem.inl"

#endif // OVERLAY_MEM_H
