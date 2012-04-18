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

#ifndef OVERLAY_DATA_H
#define OVERLAY_DATA_H

#include "overlayUtils.h"
#include "overlayMdp.h"

namespace overlay2 {

class RotatorBase;

// FIXME make int to be uint32 whenever possible

/*
 * MDP = DataMdp, ROT = CtrlMdp usually since Rotator<>
 * is instansiated with Ctrl data structure.
 * */
class Data : utils::NoCopy {
public:
   /* init, reset */
   explicit Data();

   /* calls close */
   ~Data();

   /* should open devices? or start()? */
   bool open(uint32_t fbnum, RotatorBase* rot);

   /* calls underlying mdp close */
   bool close();

   /* set the rotator */
   void setRot(RotatorBase* rot);

   /* set memory id in the mdp struct */
   void setMemoryId(int id);

   /* set overlay id in the mdp struct */
   void setId(int id);

   /* get overlay id in the mdp struct */
   int getId() const;

   /* queue buffer to the overlay */
   bool queueBuffer(uint32_t offset);

   /* wait for vsync to be done */
   bool waitForVsync();

   /* sump the state of the obj */
   void dump() const;
private:
   /* play wrapper */
   bool play();

   /* playWait wrapper */
   bool playWait();

   // mdp data struct
   MdpData mMdp;

   // Rotator
   // FIXME sp<>
   RotatorBase* mRot;
};

}  // overlay2

#include "overlayData.inl"

#endif // OVERLAY_DATA_H
