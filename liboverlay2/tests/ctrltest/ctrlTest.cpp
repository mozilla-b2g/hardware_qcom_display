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
#include "overlayRes.h"
#include "overlayFD.h"
#include "overlayMdp.h"
#include "overlayRotator.h"
#include "overlayCtrl.h"

#undef LOG_TAG
#define LOG_TAG "overlayCtrlTest"

enum { ROT_NUM_BUF = 2, ROT_BUF_SZ = 1024*500*4};

namespace ovutils = overlay2::utils;

int main(int, char**)
{
   LOGE("OverlayCtrlTest start");

   overlay2::Ctrl ctrl;
   overlay2::RotatorBase* rot = new overlay2::NullRotator();

   // open rot here
   bool ret = rot->open();
   OVASSERT(ret, "Rotator failed to open");
   ovutils::PipeArgs args(ovutils::OV_MDP_FLAGS_NONE,//flags PIPE SHARED
                          ovutils::OVERLAY_TRANSFORM_0,
                          ovutils::Whf(),
                          ovutils::WAIT,
                          ovutils::ZORDER_0,
                          ovutils::IS_FG_OFF,
                          ovutils::ROT_FLAG_ENABLED,
                          ovutils::PMEM_SRC_ADSP,
                          ovutils::RECONFIG_OFF);
   ret = rot->remap(ROT_NUM_BUF, args);
   OVASSERT(ret, "Rotator failed to remap");

   ret = ctrl.open(ovutils::FB0, rot);
   OVASSERT(ret, "Ctrl failed to open");
   ovutils::Whf whf(100, 200, HAL_PIXEL_FORMAT_RGBA_8888);
   ovutils::eMdpFlags flags = ovutils::OV_MDP_FLAGS_NONE;
   ret = ctrl.start(args);
   OVASSERT(ret, "Ctrl failed to start");
   ctrl.dump();

   LOGE("setPosition ...");
   ovutils::Dim dim(0, 0, 50, 100);
   ret = ctrl.setPosition(dim);
   OVASSERT(ret, "Ctrl failed to setPosition");
   ctrl.dump();

   LOGE("setParameter ...");
   ovutils::Params p ( ovutils::OVERLAY_DITHER, 0 );
   ret = ctrl.setParameter(p);
   OVASSERT(ret, "Ctrl failed to setParameter");
   ctrl.dump();

   LOGE("setSource ...");
   ovutils::PipeArgs args1(flags,
                           ovutils::OVERLAY_TRANSFORM_0,
                           ovutils::Whf(75, 150, HAL_PIXEL_FORMAT_RGBA_8888),
                           ovutils::WAIT,
                           ovutils::ZORDER_0,
                           ovutils::IS_FG_OFF,
                           ovutils::ROT_FLAG_DISABLED,
                           ovutils::PMEM_SRC_SMI,
                           ovutils::RECONFIG_OFF);
   ret = ctrl.setSource(args1);
   OVASSERT(ret, "Ctrl failed to setSource");
   ctrl.commit();
   ctrl.dump();

   ctrl.close();
   rot->close();
   delete rot;

   LOGE("OverlayCtrlTest end");
   return 0;
}
