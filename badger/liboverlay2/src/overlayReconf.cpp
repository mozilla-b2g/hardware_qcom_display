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

#include "overlayReconf.h"

namespace overlay2 {

OverlayReconf::OverlayReconf(Rotator& rot) : mRot(rot)
{}

bool OverlayReconf::close() {
   return true;
}

void OverlayReconf::reset() {
   mArgs.reset();
}

// setupBuf will be always called when need to do reconf.
// No need for a state.
// output - the Player info needed to pass to Data channel
bool OverlayReconf::setupBuf(const utils::ReconfArgs& args,
                             utils::PlayInfo& playInfo)
{
   // nothing to do in case of the same.
   if(args.whf == mArgs.whf) return true;

   const uint32_t numBufs = 1;
   utils::PipeArgs pargs(args);
   // make sure stuff is set correctly.
   pargs.rotFlags = utils::ROT_FLAG_ENABLED;
   // start needs whf.format to be mdp not hal fmt
   pargs.whf.format = utils::getMdpFormat(pargs.whf.format);
   if(!mRot.remap(numBufs, pargs)) {
      LOGE("%s failed setup rot session/remap", __FUNCTION__);
      return false;
   }

   if(!mRot.unmapNonCurrent()) {
      LOGE("%s failed unmap old rot bufs", __FUNCTION__);
      return false;
   }

   // will also set enable as 1 since we used FLAG_ENABLED above
   if(!mRot.start<utils::ROT_OUT_FMT_Y_CRCB_H2V2>(pargs)) {
      LOGE("%s failed start rot session", __FUNCTION__);
      return false;
   }

   mRot.setRotDataSrcMemId(args.play.fd);

   // will set all offset/fd and rotate.
   if(!mRot.prepareQueueBuf(args.play.offset)) {
      LOGE("%s failed prepareQueueBuf/rotate", __FUNCTION__);
      return false;
   }

   // cache the rot dst fd memory id and offset
   mArgs = args;
   mArgs.play.fd = mRot.getDstMemId();
   mArgs.play.offset = mRot.getDstOffset();
   return true;
}

} // overlay2
