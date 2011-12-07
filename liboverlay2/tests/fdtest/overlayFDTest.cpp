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

#include "overlayRes.h"
#include "overlayFD.h"

#undef LOG_TAG
#define LOG_TAG "overlayFDTest"

namespace {
   enum eFB { FB0, FB1 };
}

void validate(overlay2::OvFD& fd);

void testFD(overlay2::OvFD& fd, const char* const d) {
   bool ret = fd.valid();
   OVASSERT(!ret, "testFD failed %s", d);
   ret = fd.open(d);
   OVASSERT(ret, "testFD failed open %s", d);
   validate(fd);
}

void testFD(overlay2::OvFD& fd, eFB fb, const char* const d) {
   bool ret = fd.valid();
   OVASSERT(!ret, "testFD failed %s fb=%d", d, fb);
   ret = overlay2::open(fd, fb, d);
   OVASSERT(ret, "testFD failed open %s fb=%d", d, fb);
   validate(fd);
}

void validate(overlay2::OvFD& fd) {
   bool ret = fd.valid();
   OVASSERT(ret, "validate failed for valid check");
   fd.dump();
   ret = fd.close();
   OVASSERT(ret, "validate failed for close");
}

int main(int, char**)
{
   LOGE("OverlayFDTest start");

   // Res::devTemplate = "/dev/graphics/fb%u"
   overlay2::OvFD fd;

   LOGE("Test FB0");
   testFD(fd, FB0, overlay2::Res::devTemplate);

   LOGE("Test FB1");
   testFD(fd, FB1, overlay2::Res::devTemplate);

   // Res::rotTemplate = "/dev/msm_rotator"
   LOGE("Test rotator");
   testFD(fd, overlay2::Res::rotPath);

   LOGE("OverlayFDTest end");
   return 0;
}
