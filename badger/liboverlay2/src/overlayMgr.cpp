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

#include <cutils/log.h>

#include "overlayMgr.h"
#include "overlayUtils.h"

namespace overlay2 {
   OverlayMgr::OverlayMgr(){}

   overlay2::Overlay& OverlayMgr::ov()
   { return mOverlay; }

   bool OverlayMgr::open() {
      // FIXME guard against multiple opens. either here or
      // in overlay itself

      if(!mOverlay.open()) {
         LOGE("OverlayMgr error in ov2 open");
         return false;
      }
      return true;
   }

   bool OverlayMgr::close() {
      if(!mOverlay.close()) {
         LOGE("OverlayMgr error in ov2 close");
         return false;
      }
      return true;
   }

   /**
    * The purpose of HDMI pending is to have a way to set HDMI on/off
    * in setHDMIStatus() and when client calls eventNotification
    * it is using getHDMIStatus() and pass it to eventNotification.
    *
    * So code path works like that:
    * client calls setHDMIStatus(true) - hdmi is pending
    * client calls eventNotification(getHDMIStatus(), fmt)
    * getHDMIStatus will give true and that is passed to
    * eventNotification and only then we will move
    * hdmi state to be ON.
    *
    * All of that code be avoided if hwc/framebuffer.cpp won't
    * have crazy sync issues.
    *
    * setHDMIStatus is being called from HWC with hdmi on/off arg.
    * We retrieve the ov state in order to manipulate it based on
    * the hdmi state (on/off).
    * In the case where our currnt HDMI is on (hdmi is connected)
    * or HDMI is pending (which means setHDMIStatus(true) was called
    * and we set pending to HDMI_ON so who ever calls getHDMIStatus()
    * will get HDMI_ON).
    *
    * In case of setHDMIStatus(false) we would like to quickly
    * response to it before eventNotification is being called.
    * That is because suspend will call setHDMIStatus(false)
    * but won't call eventNotification.
    *
    * In that case we just clear the bits of EXT/UI
    * and call handleevent with the new state.
    * if fmt is DISPLAY_FMT_NONE, we just send CLOSED.
    *
    * FIXME - we cannot react the same wai when setHDMIStatus(true)
    * comes in because we do not have the fmt of the buffers here.
    * but we still could just set EXT bit on and call the overlay.
    * */

   /*
    * You can call eventNotification with the state directly.
    * hdmi/fmt set accordingly when it's closed event.
    * */
   void OverlayMgr::setState(utils::eOverlayState s) {
      mOverlay.handleEvent(s);
   }
}// overlay2
