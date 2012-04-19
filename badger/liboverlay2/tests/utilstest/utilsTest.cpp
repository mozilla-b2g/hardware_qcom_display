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

#include "overlayUtils.h"

#undef LOG_TAG
#define LOG_TAG "UtilsTest"

namespace ovutils = overlay2::utils;

int main(int, char**)
{
   LOGE("UtilsTest start");

   OVASSERT(true, "This is true");

   ovutils::Dim d;
   OVASSERT(!d.x && !d.y && !d.w && !d.h, "Failed dim");

   ovutils::Whf whf;
   OVASSERT(!whf.w && !whf.h && !whf.format, "Failed whf");

   ovutils::ScreenInfo info;
   OVASSERT(!info.mFBWidth && !info.mFBHeight &&
            !info.mFBbpp && !info.mFBystride, "failed Screen info");

   // FIXME more examples here
   OVASSERT(MDP_RGBA_8888 == 
            ovutils::getMdpFormat(HAL_PIXEL_FORMAT_RGBA_8888), 
            "Failed getMdpFormat");

   whf = (ovutils::Whf(1024, 500, MDP_RGBA_8888));
   uint32_t s = ovutils::getSize(whf);
   OVASSERT(1024*500*4 == s, "Failed getSize s=%d expect %d", 
            s, 1024*500*4);

   LOGE("UtilsTest end");
   return 0;
}
