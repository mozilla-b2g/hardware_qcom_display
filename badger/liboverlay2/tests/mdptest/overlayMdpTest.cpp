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
#include "overlayFD.h"
#include "overlayMdp.h"

#undef LOG_TAG
#define LOG_TAG "overlayMdpTest"

enum { FB0, FB1 };

namespace ovutils = overlay2::utils;

int main(int, char**)
{
   LOGE("OverlayMdpTest start");

   LOGE("OverlayMdpTest CTRL start");
   overlay2::MdpCtrl ctrl;
   bool ret = ctrl.open(FB0);
   OVASSERT(ret, "ctrl failed to open FB1");

   // setInfo
   overlay2::RotatorBase* r = new overlay2::NullRotator();
   ovutils::Whf whf(10, 20, 0 /*fmt*/);
   ovutils::ScreenInfo info;
   ret = ctrl.getScreenInfo(info);
   OVASSERT(ret, "ctrl failed to get screen info");
   ovutils::eMdpFlags mflags = ovutils::OV_MDP_FLAGS_NONE;
   ovutils::PipeArgs args(mflags, ovutils::OVERLAY_TRANSFORM_0,
                          whf, ovutils::WAIT,
                          ovutils::ZORDER_0,
                          ovutils::IS_FG_OFF,
                          ovutils::ROT_FLAG_DISABLED,
                          ovutils::PMEM_SRC_SMI,
                          ovutils::RECONFIG_OFF);
   ret = ctrl.setInfo(r, args, info);
   ctrl.dump();
   OVASSERT(ret, "ctrl failed to setInfo");
   ret = ctrl.set();
   OVASSERT(ret, "ctrl failed to set");
   ret = ctrl.get();
   OVASSERT(ret, "ctrl failed to get");
   ctrl.setFlags(1);
   int flags = ctrl.getFlags();
   OVASSERT(flags == 1, "flags should be 1");
   LOGE("Got flags 0x%x", flags);
   int orient = ctrl.getOrient();
   LOGE("Got orient %d", orient);

   OVASSERT(whf == ctrl.getSrcWhf(), "operator== failed for whf");

   ctrl.swapSrcWH();
   ovutils::Whf whf2(20, 10, 0);
   OVASSERT(whf2 == ctrl.getSrcWhf(), "operator== failed for whf");

   ovutils::Dim dim = ctrl.getSrcRectDim();
   dim.dump();
   ovutils::Dim dim2(0, 0, 10, 20);
   OVASSERT(dim == dim2, "operator== failed for dim");
   ctrl.swapSrcRectWH();
   ctrl.dump();
   OVASSERT(ovutils::Dim(0, 0, 20, 10) == ctrl.getSrcRectDim(),
            "operator== failed for dim");
   ovutils::Dim dim3(0, 0, 20, 30);
   ctrl.setSrcRectDim(dim3);
   OVASSERT(dim3 == ctrl.getSrcRectDim(), "operator== failed for dim3");
   ctrl.dump();

   LOGE("setPosition...");
   ret = ctrl.setPosition(ovutils::Dim(0, 0, 20, 10), /*w,h,x,y*/
                          300 /*fbw*/,
                          200 /*fbh*/);
   OVASSERT(ret, "setPosition failed");
   ctrl.getDstRectDim().dump();
   ctrl.dump();

   // setSrcFormat
   LOGE("calling setSrcFormat");
   ctrl.setSrcFormat(r->getSrcWhf());
   ctrl.dump();

   LOGE("OverlayMdpTest DATA start");
   overlay2::MdpData data;

   ret = data.open(FB0);
   OVASSERT(ret, "data failed open fb1");
   data.dump();

   enum { MEMID = 17 };
   data.setMemoryId(MEMID);
   data.setId(ctrl.getId());
   OVASSERT(MEMID == data.getMemoryId(), "Mem ID shoould be %d", MEMID);
   OVASSERT(data.getId() == ctrl.getId(),
            "Ov ID shoould be %d", ctrl.getId());
   data.dump();
   // FIXME setCrop will be moved/copied to Ctrl.
   // The following call will be deprecated soon.
   LOGE("Call setCrop");
   ctrl.setCrop(ovutils::Dim(0, 0, 5, 5));
   ctrl.dump();
   ctrl.get();
   ctrl.dump();
   data.dump();
   ret = data.close();
   OVASSERT(ret, "data failed to close");

   ret = ctrl.close();
   OVASSERT(ret, "ctrl close failed");
   delete r;
   r = 0;
   LOGE("OverlayMdpTest end");
   return 0;
}
