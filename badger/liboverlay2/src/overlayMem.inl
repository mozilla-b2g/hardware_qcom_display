/*
 * Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
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

#include <sys/mman.h>

#include "gralloc_priv.h"

#include "overlayUtils.h"
#include "overlayRes.h"

using android::sp;
using gralloc::IMemAlloc;
using gralloc::alloc_data;

namespace overlay2 {

inline OvMem::OvMem() {
   mFd = -1;
   mPmemAddr = MAP_FAILED;
   mAllocType = 0;
   mPmemSrc = utils::PMEM_SRC_SMI;
   mBufSz = 0;
   mNumBuffers = 0;
   mAlloc = gralloc::IAllocController::getInstance(false);
}

inline OvMem::~OvMem() { }

inline bool OvMem::open(utils::ePmemSource pmemsrc, uint32_t numbufs,
                        uint32_t bufSz, int flags)
{
   alloc_data data;
   int allocFlags = GRALLOC_USAGE_PRIVATE_ADSP_HEAP |
                    GRALLOC_USAGE_PRIVATE_SMI_HEAP |
                    GRALLOC_USAGE_PRIVATE_IOMMU_HEAP |
                    GRALLOC_USAGE_PRIVATE_MM_HEAP |
                    GRALLOC_USAGE_PRIVATE_WRITEBACK_HEAP |
                    GRALLOC_USAGE_PRIVATE_DO_NOT_MAP;
   int err = 0;

   OVASSERT(numbufs && bufSz, "numbufs=%d bufSz=%d", numbufs, bufSz);

   mPmemSrc = pmemsrc;
   mBufSz = bufSz;
   mNumBuffers = numbufs;

   data.base = 0;
   data.fd = -1;
   data.offset = 0;
   data.size = mBufSz * mNumBuffers;
   data.align = getpagesize();
   data.uncached = true;

   // Let libgralloc decide whether to use ADSP or SMI
   err = mAlloc->allocate(data, allocFlags, 0);
   if (err != 0) {
      LOGE("OvMem: error allocating memory");
   }

   mFd = data.fd;
   mPmemAddr = data.base;
   mAllocType = data.allocType;

   return true;
}

inline bool OvMem::close()
{
   int ret = 0;

   if(!valid()) {
      return true;
   }

   sp<IMemAlloc> memalloc = mAlloc->getAllocator(mAllocType);
   ret = memalloc->free_buffer(mPmemAddr, mBufSz * mNumBuffers, 0, mFd);
   if (ret != 0) {
      LOGE("OvMem: error freeing buffer");
   }

   mFd = -1;
   mPmemAddr = MAP_FAILED;
   mAllocType = 0;
   mPmemSrc = utils::PMEM_SRC_SMI;
   mBufSz = 0;
   mNumBuffers = 0;
   return ret;
}

inline bool OvMem::valid() const
{
   return (mFd != -1) && (mPmemAddr != MAP_FAILED);
}

inline int OvMem::getFD() const
{
   return mFd;
}

inline void* OvMem::addr() const
{
   return mPmemAddr;
}

inline utils::ePmemSource OvMem::getPmemSrc() const
{
   return mPmemSrc;
}

inline uint32_t OvMem::bufSz() const
{
   return mBufSz;
}

inline uint32_t OvMem::numBufs() const
{
   return mNumBuffers;
}

inline void OvMem::dump() const
{
   LOGE("== Dump OvMem start ==");
   LOGE("fd=%d addr=%p type=%d bufsz=%u", mFd, mPmemAddr, mAllocType, mBufSz);
   LOGE("== Dump OvMem end ==");
}

} //overlay2
