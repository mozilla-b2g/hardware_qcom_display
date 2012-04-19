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

#ifndef OVERLAY_STATE_H
#define OVERLAY_STATE_H

#include "overlayUtils.h"

// FIXME make int to be uint32 whenever possible
// FIXME TODO sp<> instead of "*"

namespace overlay2 {

class RotatorBase;

class OverlayImplBase;

/*
 * Used by Overlay class. Invokes each event
 * */

/* TODO case of RGBx will call mOv open with diff
 * params customized for RGBx pipes */

class OverlayState : utils::NoCopy {
public:
   /**/
   explicit OverlayState();

   /**/
   ~OverlayState();

   /* return current state */
   utils::eOverlayState state() const;

   /* Overlay Event */

   /* Hard reset to a new state. If the state is the same
    * as the current one, it would be a no-op */
   OverlayImplBase* reset(utils::eOverlayState s);

   /* Caller pass the state to the handleEvent function.
    * The input is the current OverlayImplBase*, and output is
    * a pointer to (possibly new) instance of OverlayImplBase
    * The eFormat can be 2D/3D etc. */
   OverlayImplBase* handleEvent(utils::eOverlayState s,
                                OverlayImplBase* ov);

   /* Transitions from XXX to XXX */
   OverlayImplBase* handle_closed(utils::eOverlayState s);
   OverlayImplBase* handle_2D_2DPanel(utils::eOverlayState s,
                                      OverlayImplBase* ov);
   OverlayImplBase* handle_2D_2DTV(utils::eOverlayState s,
                                   OverlayImplBase* ov);
   OverlayImplBase* handle_3D_2DPanel(utils::eOverlayState s,
                                      OverlayImplBase* ov);
   OverlayImplBase* handle_3D_3DPanel(utils::eOverlayState s,
                                      OverlayImplBase* ov);
   OverlayImplBase* handle_3D_3DTV(utils::eOverlayState s,
                                   OverlayImplBase* ov);
   OverlayImplBase* handle_3D_2DTV(utils::eOverlayState s,
                                   OverlayImplBase* ov);
   OverlayImplBase* handle_UI_Mirror(utils::eOverlayState s,
                                     OverlayImplBase* ov);
   OverlayImplBase* handle_2D_trueUI_Mirror(utils::eOverlayState s,
                                            OverlayImplBase* ov);
   OverlayImplBase* handle_bypass(utils::eOverlayState s,
                                  OverlayImplBase* ov);

   /* Transition from any state to 2D video on 2D panel */
   OverlayImplBase* handle_xxx_to_2D_2DPanel(OverlayImplBase* ov);

   /* Transition from any state to 2D video on 2D panel and 2D TV */
   OverlayImplBase* handle_xxx_to_2D_2DTV(OverlayImplBase* ov);

   /* Transition from any state to 3D video on 2D panel */
   OverlayImplBase* handle_xxx_to_3D_2DPanel(OverlayImplBase* ov);

   /* Transition from any state to 3D video on 2D panel and 2D TV */
   OverlayImplBase* handle_xxx_to_3D_2DTV(OverlayImplBase* ov);

   /* Transition from any state to 2D video true UI mirroring (2D video + UI) */
   OverlayImplBase* handle_xxx_to_2D_trueUI_Mirror(OverlayImplBase* ov);

   /* Transitions from any state to 1 layer composition bypass */
   OverlayImplBase* handle_xxx_to_bypass1(OverlayImplBase* ov);

   /* Transitions from any state to 2 layers composition bypass */
   OverlayImplBase* handle_xxx_to_bypass2(OverlayImplBase* ov);

   /* Transitions from any state to 3 layers composition bypass */
   OverlayImplBase* handle_xxx_to_bypass3(OverlayImplBase* ov);

   /* Dump */
   void dump() const;
private:
   /* States here */
   utils::eOverlayState mState;
};

} // overlay2

#include "overlayState.inl"

#endif // OVERLAY_STATE_H
