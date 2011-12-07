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
#include "gralloc_priv.h"
#include "overlay.h"

#undef LOG_TAG
#define LOG_TAG "overlayTest"

namespace ovutils = overlay2::utils;

/**
 * For that test you would need to luanch camera
 * so it would punch a hole for you.
 */
int main(int, char**)
{
   LOGE("%s start", LOG_TAG);
   ovutils::Whf whf(1024, 600, HAL_PIXEL_FORMAT_YCbCr_420_SP_TILED);
   ovutils::eMdpFlags flags = ovutils::OV_MDP_FLAGS_NONE;
   const ovutils::PipeArgs pargs(flags,
                                 ovutils::OVERLAY_TRANSFORM_0, // orientation
                                 whf,
                                 ovutils::WAIT,
                                 ovutils::ZORDER_1,
                                 ovutils::IS_FG_OFF,
                                 ovutils::ROT_FLAG_DISABLED,
                                 ovutils::PMEM_SRC_SMI,
                                 ovutils::RECONFIG_OFF);
   const ovutils::PipeArgs pipeargs[ovutils::MAX_PIPES] = { pargs };
   LOGE("Using null rotator");
   overlay2::Overlay ov;
   bool ret = ov.setSource(pipeargs);
   OVASSERT(ret, "Failed to open overlay");

   // let's try openning /dev/zero and mmap it.
   LOGE("Open /dev/pmem_adsp");
   overlay2::OvMem m;
   uint32_t bfsz = 1024*600*4;
   ret = m.open(ovutils::PMEM_SRC_ADSP,
                1,     // one buf
                bfsz); //size
   OVASSERT(ret, "Failed to open /dev/pmem_adsp pipe");
   m.dump();
   ::memset(m.addr(), 0, bfsz);
   ov.setMemoryId(m.getFD());
   ovutils::Dim dim(0, 0, 1024, 600); // x,y,w,h
   ret = ov.setPosition(dim);
   OVASSERT(ret, "Failed to setPosition");

   LOGE("Before play ...");
   ov.commit();
   ov.dump();
   enum { NUM_PLAY = 10 };
   for (uint32_t i=0; i < NUM_PLAY; ++i) {
      ret = ov.queueBuffer(0); // offset 0
      OVASSERT(ret, "Failed to queueBuffer");
      sleep(1);
   }

   ret = ov.close();
   OVASSERT(ret, "Error in overlay close");

   ret = m.close();

   LOGE("%s end", LOG_TAG);
   return 0;
}
