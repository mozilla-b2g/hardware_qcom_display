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

#include <stdio.h>
#include <utils/Log.h>
#include "overlayUtils.h"
#include "overlayRes.h"
#include "overlayMem.h"

#undef LOG_TAG
#define LOG_TAG "overlayMemTest"

namespace {
   enum eBufs { NUM_BUFS2 = 2, NUM_BUFS3 };
   enum { BUFSZ = 1024*520*4 };
}

void testMem(overlay2::OvMem& mem, eBufs b, overlay2::utils::ePmemSource s) {
   bool ret = mem.valid();
   OVASSERT(!ret, "testMem failed valid state");
   ret = mem.open(s, b, BUFSZ);
   if(!ret) {
      LOGE("Cannot open pmem source %d", s);
      return;
   }
   ret = mem.valid();
   OVASSERT(ret, "testMem should be valid");
   OVASSERT(mem.getFD() >= 0, "testMem should be valid fd");
   mem.dump();
   ret = mem.close();
   OVASSERT(ret, "testMem close failed");
   OVASSERT(!mem.valid(), "testMem should be invalid now");
}

int main(int, char**)
{
   LOGE("OverlayMemTest start");

   overlay2::OvMem mem;

   LOGE("Test smi");
   testMem(mem, NUM_BUFS2, overlay2::utils::PMEM_SRC_SMI);

   LOGE("Test adsp");
   testMem(mem, NUM_BUFS2, overlay2::utils::PMEM_SRC_ADSP);

   LOGE("OverlayMemTest end");
   return 0;
}
