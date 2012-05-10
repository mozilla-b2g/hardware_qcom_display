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
#include "mdpWrapper.h"

#undef LOG_TAG
#define LOG_TAG "mdpWrapperTest"

namespace ovutils = overlay2::utils;
namespace mdpwrap = overlay2::mdp_wrapper;

int main(int, char**)
{
   LOGE("mdpWrapperTest start");

   // TODO FIXME add mdpwrapper related functions.

   // Exercise dumps
   msmfb_overlay_data ovdata;
   ovutils::memset0(ovdata);
   mdpwrap::dump("dump ovdata", ovdata);
   msmfb_data data;
   ovutils::memset0(data);
   data.offset=1;
   data.memory_id=2;
   data.id=3;
   data.flags=4;
   data.priv=5;
   mdpwrap::dump("dump data", data);
   mdp_overlay ov;
   ovutils::memset0(ov);
   mdpwrap::dump("dump ov", ov);
   msmfb_overlay_3d ov3d;
   ovutils::memset0(ov3d);
   ov3d.is_3d=1;
   ov3d.width=2;
   ov3d.height=3;
   mdpwrap::dump("dump ov3d", ov3d);
   msm_rotator_img_info rotinfo;
   ovutils::memset0(rotinfo);
   mdpwrap::dump("dump rotinfo", rotinfo);
   msm_rotator_data_info rotdata;
   ovutils::memset0(rotdata);
   mdpwrap::dump("dump rotdata", rotdata);

   fb_fix_screeninfo finfo;
   ovutils::memset0(finfo);
   mdpwrap::dump("dump screen info", finfo);

   fb_var_screeninfo vinfo;
   ovutils::memset0(vinfo);
   mdpwrap::dump("dump screen vinfo", vinfo);

   LOGE("mdpWrapperTest end");
   return 0;
}
