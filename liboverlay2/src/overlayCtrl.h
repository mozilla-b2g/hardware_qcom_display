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

#ifndef OVERLAY_CTRL_H
#define OVERLAY_CTRL_H

#include "overlayUtils.h"
#include "overlayMdp.h"

namespace overlay2{

// FIXME make int to be uint32 whenever possible
// FIXME TODO sp<> instead of "*"

class RotatorBase;

/*
 * FIXME do we want rot to be template parameter?
 * It's already using inheritance...
 *
 * Sequence to use:
 * open
 * start
 * setXXX
 * close
 *
 * Can call setRot anytime to replace rotator on-the-fly
 * */
class Ctrl : utils::NoCopy {
public:

   /* ctor */
   explicit Ctrl();

   /* dtor close */
   ~Ctrl();

   /* should open devices? or start()? */
   bool open(uint32_t fbnum, RotatorBase* rot);

   /* close underlying mdp */
   bool close();

   /* Invoke methods for opening underlying devices
    * flags - PIPE SHARED
    * wait - WAIT, NO_WAIT */
   bool start(const utils::PipeArgs& args);

   /* Dynamically set rotator*/
   void setRot(RotatorBase* rot);

   /* set mdp posision using dim */
   bool setPosition(const utils::Dim& dim);

   /* set param using Params (param,value pair)  */
   bool setParameter(const utils::Params& p);

   /* set source using whf, orient and wait flag */
   bool setSource(const utils::PipeArgs& args);

   /* set crop info and pass it down to mdp */
   bool setCrop(const utils::Dim& d);

   /* mdp set overlay/commit changes */
   bool commit();

   /* ctrl id */
   int  getId() const;
   /* ctrl fd */
   int  getFd() const;
   bool getRotSessId(int& id) const;
   utils::Dim getAspectRatio(const utils::Whf& whf) const;
   utils::Dim getAspectRatio(const utils::Dim& dim) const;

   /* access for screen info */
   utils::ScreenInfo getScreenInfo() const;

   /* retrieve cached crop data */
   utils::Dim getCrop() const;

   /* dump the state of the object */
   void dump() const;

private:
   /* Retrieve screen info from underlying mdp */
   bool getScreenInfo(utils::ScreenInfo& info);

   /* calls underlying mdp set info */
   bool setInfo(const utils::PipeArgs& args);

   /* given whf, update src */
   void updateSource(RotatorBase* r,
                     const utils::PipeArgs& args,
                     utils::ScreenInfo& info);

   // mdp ctrl struct(info e.g.)
   MdpCtrl mMdp;

   // Rotator
   // FIXME sp<>
   RotatorBase* mRot;

   /* Cache cropped value */
   utils::Dim mCrop;

   /* Screen info */
   utils::ScreenInfo mInfo;

   /* orientation cache FIXME */
   utils::eTransform mOrient;

   /* Cache last known whfz.
    * That would help us compare to a previous
    * source that was submitted */
   utils::Whf mOvBufInfo;
};

} // overlay2

#include "overlayCtrl.inl"

#endif // OVERLAY_CTRL_H
