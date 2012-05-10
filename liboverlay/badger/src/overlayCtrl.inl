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

#include "gralloc_priv.h" // INTERLACE_MASK

namespace overlay2{

inline Ctrl::Ctrl() : mRot(0), mOrient(utils::OVERLAY_TRANSFORM_0) {
   mMdp.reset();
}

inline Ctrl::~Ctrl() {
   close();
}

inline bool Ctrl::close() {
   // do not close the rotator
   if(!mMdp.close())
      return false;
   return true;
}

inline bool Ctrl::commit() {
   if(!mMdp.set()) {
      LOGE("Ctrl commit failed set overlay");
      return false;
   }
   return true;
}

inline bool Ctrl::getScreenInfo(utils::ScreenInfo& info) {
   if(!mMdp.getScreenInfo(info)){
      LOGE("Ctrl failed to get screen info");
      return false;
   }
   return true;
}

inline bool Ctrl::setInfo(const utils::PipeArgs& args)
{
   // FIXME set flags, zorder and wait separtly
   if(!mMdp.setInfo(mRot, args, mInfo)){
      LOGE("Ctrl failed to setInfo wait=%d mdpflags=%d "
           "zorder=%d", args.wait, args.mdpFlags, args.zorder);
      return false;
   }
   return true;
}

inline int Ctrl::getId() const {
   // FIXME check channel up?
   return mMdp.getId();
}

inline int Ctrl::getFd() const {
   // FIXME check channel up?
   return mMdp.getFd();
}

inline bool Ctrl::getRotSessId(int& id) const {
   // FIXME check channel up?
   // should be -1 in case of no rot session active
   id = mRot->getSessId();
   return true;
}

inline utils::ScreenInfo Ctrl::getScreenInfo() const {
   return mInfo;
}

inline utils::Dim Ctrl::getCrop() const {
   return mCrop;
}

} // overlay2
