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
#include <cassert>
#include "overlayUtils.h"
#include "pipes/overlayRgbPipe.h"
#include "overlay.h"

#undef LOG_TAG
#define LOG_TAG "rgbPipeTest"

namespace ovutils = overlay2::utils;

/**
 * For that test you would need to luanch camera
 * so it would punch a hole for you.
 */
int main(int, char**)
{
   LOGE("%s start", LOG_TAG);

   overlay2::RgbPipe<ovutils::FB0> pipe;
   bool ret = pipe.open();
   OVASSERT(ret, "Failed to open pipe");

   LOGE("start ...");

   LOGE("Before play ...");
   pipe.dump();
   enum { NUM_PLAY = 10 };
   for (uint32_t i=0; i < NUM_PLAY; ++i) {
      void* buf;
      ret = pipe.dequeueBuffer(buf);
      OVASSERT(ret, "Failed to dequeueBuffer");
      ret = pipe.queueBuffer(0); // offset 0 ignored
      OVASSERT(ret, "Failed to queueBuffer");
      sleep(1);
   }

   ret = pipe.close();
   OVASSERT(ret, "Error in pipe close");

   /* use the overlay now */
   overlay2::Overlay ov;
   ret = ov.open();

   // FIXME FIXME FIXME, need to shoot an event that
   // will trigger open rgb pipes - state machine NOT IMPL yet

   OVASSERT(ret, "Failed to open overlay");
   LOGE("Before play ...");
   ov.dump();
   for (uint32_t i=0; i < NUM_PLAY; ++i) {
      void* buf;
      ret = ov.dequeueBuffer(buf);
      OVASSERT(ret, "Failed to dequeueBuffer");
      ret = ov.queueBuffer(0); // offset 0 ignored
      OVASSERT(ret, "Failed to queueBuffer");
      sleep(1);
   }

   ret = ov.close();
   OVASSERT(ret, "Error in ov close");

   LOGE("%s end", LOG_TAG);
   return 0;
}
