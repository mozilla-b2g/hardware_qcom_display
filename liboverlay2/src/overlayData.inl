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

#include "overlayRotator.h"

namespace ovutils = overlay2::utils;

namespace overlay2{

inline Data::Data() : mRot(0) {
   mMdp.reset();
}

inline Data::~Data() { close(); }

inline void Data::setRot(RotatorBase* rot) { mRot = rot; }

inline void Data::setMemoryId(int id) { mMdp.setMemoryId(id); }

// really a reqid
inline void Data::setId(int id) { mMdp.setId(id); }

inline int Data::getId() const { return mMdp.getId(); }

inline bool Data::open(uint32_t fbnum,
                       RotatorBase* rot) {
   if(!mMdp.open(fbnum)) {
      LOGE("Data cannot open mdp");
      return false;
   }

   OVASSERT(rot, "rot is null");
   mRot = rot;

   // rotator should be already opened here
   return true;
}

inline bool Data::close() {
   if(!mMdp.close()) {
      LOGE("Data close failed");
      return false;
   }
   return true;
}

inline bool Data::queueBuffer(uint32_t offset) {
   // FIXME asserts on state validity

   mMdp.setOffset(offset);
   mRot->setRotDataSrcMemId(mMdp.getMemoryId());
   // will play if succeeded
   if(!mRot->prepareQueueBuf(offset)) {
      LOGE("Data failed to prepareQueueBuf");
      return false;
   }
   // Play can go either from mdp or rot
   if(!this->play()){
      LOGE("Data error in MDP/ROT play");
      return false;
   }

   return true;
}

inline bool Data::waitForVsync() {

   // Call mdp playWait
   if(!this->playWait()){
      LOGE("Error in MDP playWait");
      return false;
   }

   return true;
}

inline bool Data::play() {
   int fd = mMdp.getFd();
   return mRot->enabled() ? mRot->play(fd) : mMdp.play();
}

inline bool Data::playWait() {
   return mMdp.playWait();
}

inline void Data::dump() const {
   LOGE("== Dump Data MDP start ==");
   mMdp.dump();
   mRot->dump();
   LOGE("== Dump Data MDP end ==");
}
} // overlay2
