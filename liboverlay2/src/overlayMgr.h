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

#ifndef OVERLAY_MANAGER_H
#define OVERLAY_MANAGER_H

#include <sys/types.h>
#include <utils/threads.h>

#include "overlay.h"

namespace overlay2 {

/*
 * Manages *all* overlay related activities
 *
 * */
class OverlayMgr {
public:
   /* ctor init members*/
   OverlayMgr();

   /* get the overlay */
   Overlay& ov();
   
   /* open mgr for business */
   bool open();

   /* close mgr . reset state*/
   bool close();

   /* cache HDMI status setHDMIStatus */
   void setHDMIStatus(bool hdmiOn);

   /* get cached HDMI status true == on*/
   bool getHDMIStatus() const;

   /* event state notification transition */
   void setState(utils::eOverlayState s);

   /* event notification like HDMI.
    * Upper layers use bool for on/off HDMI
    * Format is from the clip src */
   utils::eOverlayState getOverlayStateForVideo(bool hdmiOn, uint32_t format);
private:
   OverlayMgr(const OverlayMgr&);
   OverlayMgr& operator==(const OverlayMgr&);
   // actual overlay instance
   overlay2::Overlay mOverlay;
}; // OverlayMgr

} // overlay2

#endif // OVERLAY_MANAGER_H
