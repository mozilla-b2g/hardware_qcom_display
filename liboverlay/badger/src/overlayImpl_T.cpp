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

#include "overlayRotator.h"

namespace overlay2 {

template <class P0, class P1, class P2>
OverlayImpl<P0, P1, P2>::OverlayImpl() :
   mPipe0(new P0), mPipe1(new P1), mPipe2(new P2),
   mRotP0(0), mRotP1(0), mRotP2(0), mReconf(mRotBuffer)
{}

template <class P0, class P1, class P2>
OverlayImpl<P0, P1, P2>::OverlayImpl(P0* p0, P1* p1, P2* p2) :
   mPipe0(p0), mPipe1(p1), mPipe2(p2),
   mRotP0(0), mRotP1(0), mRotP2(0), mReconf(mRotBuffer)
{}

template <class P0, class P1, class P2>
OverlayImpl<P0, P1, P2>::~OverlayImpl()
{
   // no op in the meantime. needed to be clean
   // since state machine will do delete. so we
   // do not want to close/delete pipes here
}

/* Open only one pipe/rot pair per call */
template <class P0, class P1, class P2>
bool OverlayImpl<P0, P1, P2>::openPipe(RotatorBase* rot, utils::eDest dest)
{
   OVASSERT(rot, "%s: OverlayImpl rot is null", __FUNCTION__);
   OVASSERT(utils::isValidDest(dest), "%s: OverlayImpl invalid dest=%d",
            __FUNCTION__, dest);

   // Need to down case rotator to mdp one.
   // we assume p0/p1/p2/px all use the _same_ underlying mdp structure.
   // FIXME STATIC_ASSERT here

   bool ret = true;

   if (utils::OV_PIPE0 & dest) {
      OVASSERT(mPipe0, "%s: OverlayImpl pipe0 is null", __FUNCTION__);
      LOGE_IF(DEBUG_OVERLAY, "Open pipe0");
      ret = mPipe0->open(rot);
      mRotP0 = rot;
      if(!ret) {
         LOGE("%s: OverlayImpl pipe0 failed to open", __FUNCTION__);
      }
      return ret;
   }

   if (utils::OV_PIPE1 & dest) {
      OVASSERT(mPipe1, "%s: OverlayImpl pipe1 is null", __FUNCTION__);
      LOGE_IF(DEBUG_OVERLAY, "Open pipe1");
      ret = mPipe1->open(rot);
      mRotP1 = rot;
      if(!ret) {
         LOGE("%s: OverlayImpl pipe1 failed to open", __FUNCTION__);
      }
      return ret;
   }

   if (utils::OV_PIPE2 & dest) {
      OVASSERT(mPipe2, "%s: OverlayImpl pipe2 is null", __FUNCTION__);
      LOGE_IF(DEBUG_OVERLAY, "Open pipe2");
      ret = mPipe2->open(rot);
      mRotP2 = rot;
      if(!ret) {
         LOGE("%s: OverlayImpl pipe2 failed to open", __FUNCTION__);
      }
      return ret;
   }

   // Should have returned by here
   return false;
}

/* Close pipe/rot for all specified dest */
template <class P0, class P1, class P2>
bool OverlayImpl<P0, P1, P2>::closePipe(utils::eDest dest)
{
   OVASSERT(utils::isValidDest(dest), "%s: OverlayImpl invalid dest=%d",
            __FUNCTION__, dest);

   if (utils::OV_PIPE0 & dest) {
      // Close pipe0
      OVASSERT(mPipe0, "%s: OverlayImpl pipe0 is null", __FUNCTION__);
      LOGE_IF(DEBUG_OVERLAY, "Close pipe0");
      if (!mPipe0->close()) {
         LOGE("%s: OverlayImpl failed to close pipe0", __FUNCTION__);
         return false;
      }
      delete mPipe0;
      mPipe0 = 0;

      // Close the rotator for pipe0
      OVASSERT(mRotP0, "%s: OverlayImpl rot0 is null", __FUNCTION__);
      if (!mRotP0->close()) {
         LOGE("%s: OverlayImpl failed to close rot for pipe0", __FUNCTION__);
      }
      delete mRotP0;
      mRotP0 = 0;
   }

   if (utils::OV_PIPE1 & dest) {
      // Close pipe1
      OVASSERT(mPipe1, "%s: OverlayImpl pipe1 is null", __FUNCTION__);
      LOGE_IF(DEBUG_OVERLAY, "Close pipe1");
      if (!mPipe1->close()) {
         LOGE("%s: OverlayImpl failed to close pipe1", __FUNCTION__);
         return false;
      }
      delete mPipe1;
      mPipe1 = 0;

      // Close the rotator for pipe1
      OVASSERT(mRotP1, "%s: OverlayImpl rot1 is null", __FUNCTION__);
      if (!mRotP1->close()) {
         LOGE("%s: OverlayImpl failed to close rot for pipe1", __FUNCTION__);
      }
      delete mRotP1;
      mRotP1 = 0;
   }

   if (utils::OV_PIPE2 & dest) {
      // Close pipe2
      OVASSERT(mPipe2, "%s: OverlayImpl pipe2 is null", __FUNCTION__);
      LOGE_IF(DEBUG_OVERLAY, "Close pipe2");
      if (!mPipe2->close()) {
         LOGE("%s: OverlayImpl failed to close pipe2", __FUNCTION__);
         return false;
      }
      delete mPipe2;
      mPipe2 = 0;

      // Close the rotator for pipe2
      OVASSERT(mRotP2, "%s: OverlayImpl rot2 is null", __FUNCTION__);
      if (!mRotP2->close()) {
         LOGE("%s: OverlayImpl failed to close rot for pipe2", __FUNCTION__);
      }
      delete mRotP2;
      mRotP2 = 0;
   }

   return true;
}

/* Copy pipe/rot from ov for all specified dest */
template <class P0, class P1, class P2>
bool OverlayImpl<P0, P1, P2>::copyOvPipe(OverlayImplBase* ov,
                                         utils::eDest dest)
{
   OVASSERT(ov, "%s: OverlayImpl ov is null", __FUNCTION__);
   OVASSERT(utils::isValidDest(dest), "%s: OverlayImpl invalid dest=%d",
            __FUNCTION__, dest);

   OverlayImpl<P0, P1, P2>* ovimpl = static_cast<OverlayImpl<P0, P1, P2>*>(ov);

   if (utils::OV_PIPE0 & dest) {
      mPipe0 = ovimpl->mPipe0;
      mRotP0 = ovimpl->mRotP0;
   }

   if (utils::OV_PIPE1 & dest) {
      mPipe1 = ovimpl->mPipe1;
      mRotP1 = ovimpl->mRotP1;
   }

   if (utils::OV_PIPE2 & dest) {
      mPipe2 = ovimpl->mPipe2;
      mRotP2 = ovimpl->mRotP2;
   }

   return true;
}

/* TODO open func customized for RGBx pipes */

/* Open all pipes/rot */
template <class P0, class P1, class P2>
bool OverlayImpl<P0, P1, P2>::open(RotatorBase* rot0,
                                   RotatorBase* rot1,
                                   RotatorBase* rot2)
{
   if (!this->openPipe(rot0, utils::OV_PIPE0)) {
      if (!this->close()) {
         LOGE("%s: failed to close at least one pipe", __FUNCTION__);
      }
      return false;
   }

   if (!this->openPipe(rot1, utils::OV_PIPE1)) {
      if (!this->close()) {
         LOGE("%s: failed to close at least one pipe", __FUNCTION__);
      }
      return false;
   }

   if (!this->openPipe(rot2, utils::OV_PIPE2)) {
      if (!this->close()) {
         LOGE("%s: failed to close at least one pipe", __FUNCTION__);
      }
      return false;
   }

   // Open rotator - just fd open
   if(!mRotBuffer.open()) {
      LOGE("%s: OverlayImpl failed to open rot buffer", __FUNCTION__);
      return false;
   }

   return true;
}

/* Close all pipes/rot */
template <class P0, class P1, class P2>
bool OverlayImpl<P0, P1, P2>::close()
{
   if (!this->closePipe(utils::OV_PIPE_ALL)) {
      return false;
   }

   // reconfig rot
   if(!mRotBuffer.close()) {
      LOGE("Rotator reconf close failed");
      return false;
   }

   return true;
}

template <class P0, class P1, class P2>
bool OverlayImpl<P0, P1, P2>::commit(utils::eDest dest)
{
   OVASSERT(mPipe0 && mPipe1 && mPipe2,
            "%s: Pipes are null p0=%p p1=%p p2=%p",
            __FUNCTION__, mPipe0, mPipe1, mPipe2);

   if (utils::OV_PIPE0 & dest) {
      if(!mPipe0->commit()) {
         LOGE("OverlayImpl p0 failed to commit");
         return false;
      }
   }

   if (utils::OV_PIPE1 & dest) {
      if(!mPipe1->commit()) {
         LOGE("OverlayImpl p1 failed to commit");
         return false;
      }
   }

   if (utils::OV_PIPE2 & dest) {
      if(!mPipe2->commit()) {
         LOGE("OverlayImpl p2 failed to commit");
         return false;
      }
   }

   return true;
}

template <class P0, class P1, class P2>
bool OverlayImpl<P0, P1, P2>::setCrop(const utils::Dim& d, utils::eDest dest)
{
   OVASSERT(mPipe0 && mPipe1 && mPipe2,
            "%s: Pipes are null p0=%p p1=%p p2=%p",
            __FUNCTION__, mPipe0, mPipe1, mPipe2);

   if (utils::OV_PIPE0 & dest) {
      if(!mPipe0->setCrop(d)) {
         LOGE("OverlayImpl p0 failed to crop");
         return false;
      }
   }

   if (utils::OV_PIPE1 & dest) {
      if(!mPipe1->setCrop(d)) {
         LOGE("OverlayImpl p1 failed to crop");
         return false;
      }
   }

   if (utils::OV_PIPE2 & dest) {
      if(!mPipe2->setCrop(d)) {
         LOGE("OverlayImpl p2 failed to crop");
         return false;
      }
   }

   return true;
}

template <class P0, class P1, class P2>
bool OverlayImpl<P0, P1, P2>::setPosition(const utils::Dim& d,
                                          utils::eDest dest)
{
   OVASSERT(mPipe0 && mPipe1 && mPipe2,
            "%s: Pipes are null p0=%p p1=%p p2=%p",
            __FUNCTION__, mPipe0, mPipe1, mPipe2);

   if (utils::OV_PIPE0 & dest) {
      if(!mPipe0->setPosition(d)) {
         LOGE("OverlayImpl p0 failed to setpos");
         return false;
      }
   }

   if (utils::OV_PIPE1 & dest) {
      if(!mPipe1->setPosition(d)) {
         LOGE("OverlayImpl p1 failed to setpos");
         return false;
      }
   }

   if (utils::OV_PIPE2 & dest) {
      if(!mPipe2->setPosition(d)) {
         LOGE("OverlayImpl p2 failed to setpos");
         return false;
      }
   }

   return true;
}

template <class P0, class P1, class P2>
bool OverlayImpl<P0, P1, P2>::setParameter(const utils::Params& param,
                                           utils::eDest dest)
{
   OVASSERT(mPipe0 && mPipe1 && mPipe2,
            "%s: Pipes are null p0=%p p1=%p p2=%p",
            __FUNCTION__, mPipe0, mPipe1, mPipe2);

   if (utils::OV_PIPE0 & dest) {
      if(!mPipe0->setParameter(param)) {
         LOGE("OverlayImpl p0 failed to setparam");
         return false;
      }
   }

   if (utils::OV_PIPE1 & dest) {
      if(!mPipe1->setParameter(param)) {
         LOGE("OverlayImpl p1 failed to setparam");
         return false;
      }
   }

   if (utils::OV_PIPE2 & dest) {
      if(!mPipe2->setParameter(param)) {
         LOGE("OverlayImpl p2 failed to setparam");
         return false;
      }
   }

   return true;
}

template <class P0, class P1, class P2>
bool OverlayImpl<P0, P1, P2>::setSource(const utils::PipeArgs args[utils::MAX_PIPES],
                                        utils::eDest dest)
{
   OVASSERT(mPipe0 && mPipe1 && mPipe2,
            "%s: Pipes are null p0=%p p1=%p p2=%p",
            __FUNCTION__, mPipe0, mPipe1, mPipe2);

   if (utils::OV_PIPE0 & dest) {
      if(!mPipe0->setSource(args[0])) {
         LOGE("OverlayImpl p0 failed to setsrc");
         return false;
      }
   }

   if (utils::OV_PIPE1 & dest) {
      if(!mPipe1->setSource(args[1])) {
         LOGE("OverlayImpl p1 failed to setsrc");
         return false;
      }
   }

   if (utils::OV_PIPE2 & dest) {
      if(!mPipe2->setSource(args[2])) {
         LOGE("OverlayImpl p2 failed to setsrc");
         return false;
      }
   }

   return true;
}

template <class P0, class P1, class P2>
bool OverlayImpl<P0, P1, P2>::queueBuffer(uint32_t offset, utils::eDest dest)
{
   OVASSERT(mPipe0 && mPipe1 && mPipe2,
            "%s: Pipes are null p0=%p p1=%p p2=%p",
            __FUNCTION__, mPipe0, mPipe1, mPipe2);

   if (utils::OV_PIPE0 & dest) {
      if(!mPipe0->queueBuffer(offset)) {
         LOGE("OverlayImpl p0 failed to queueBuffer");
         return false;
      }
   }

   if (utils::OV_PIPE1 & dest) {
      if(!mPipe1->queueBuffer(offset)) {
         LOGE("OverlayImpl p1 failed to queueBuffer");
         return false;
      }
   }

   if (utils::OV_PIPE2 & dest) {
      if(!mPipe2->queueBuffer(offset)) {
         LOGE("OverlayImpl p2 failed to queueBuffer");
         return false;
      }
   }

   return true;
}

template <class P0, class P1, class P2>
bool OverlayImpl<P0, P1, P2>::dequeueBuffer(void*& buf, utils::eDest dest)
{
   OVASSERT(mPipe0 && mPipe1 && mPipe2,
            "%s: Pipes are null p0=%p p1=%p p2=%p",
            __FUNCTION__, mPipe0, mPipe1, mPipe2);

   if (utils::OV_PIPE0 & dest) {
      if(!mPipe0->dequeueBuffer(buf)) {
         LOGE("OverlayImpl p0 failed to dequeueBuffer");
         return false;
      }
   }

   if (utils::OV_PIPE1 & dest) {
      if(!mPipe1->dequeueBuffer(buf)) {
         LOGE("OverlayImpl p1 failed to dequeueBuffer");
         return false;
      }
   }

   if (utils::OV_PIPE2 & dest) {
      if(!mPipe2->dequeueBuffer(buf)) {
         LOGE("OverlayImpl p2 failed to dequeueBuffer");
         return false;
      }
   }

   return true;
}

template <class P0, class P1, class P2>
bool OverlayImpl<P0, P1, P2>::waitForVsync(utils::eDest dest)
{
   OVASSERT(mPipe0 && mPipe1 && mPipe2,
            "%s: Pipes are null p0=%p p1=%p p2=%p",
            __FUNCTION__, mPipe0, mPipe1, mPipe2);

   if (utils::OV_PIPE0 & dest) {
      if(!mPipe0->waitForVsync()) {
         LOGE("OverlayImpl p0 failed to waitForVsync");
         return false;
      }
   }

   if (utils::OV_PIPE1 & dest) {
      if(!mPipe1->waitForVsync()) {
         LOGE("OverlayImpl p1 failed to waitForVsync");
         return false;
      }
   }

   if (utils::OV_PIPE2 & dest) {
      if(!mPipe2->waitForVsync()) {
         LOGE("OverlayImpl p2 failed to waitForVsync");
         return false;
      }
   }

   return true;
}

template <class P0, class P1, class P2>
void OverlayImpl<P0, P1, P2>::setMemoryId(int id, utils::eDest dest)
{
   OVASSERT(mPipe0 && mPipe1 && mPipe2,
            "%s: Pipes are null p0=%p p1=%p p2=%p",
            __FUNCTION__, mPipe0, mPipe1, mPipe2);

   if (utils::OV_PIPE0 & dest) {
      mPipe0->setMemoryId(id);
   }

   if (utils::OV_PIPE1 & dest) {
      mPipe1->setMemoryId(id);
   }

   if (utils::OV_PIPE2 & dest) {
      mPipe2->setMemoryId(id);
   }
}

// FIXME, do we need args[] here as in params?
template <class P0, class P1, class P2>
bool OverlayImpl<P0, P1, P2>::reconfigure(const utils::ReconfArgs& args)
{
   OVASSERT(mPipe0 && mPipe1 && mPipe2,
            "%s: Pipes are null p0=%p p1=%p p2=%p",
            __FUNCTION__, mPipe0, mPipe1, mPipe2);
   switch(args.reconf) {
   case utils::RECONFIG_ON:
   {
      LOGE_IF(DEBUG_OVERLAY, "%s reconfig is on", __FUNCTION__);
      // reconfig the buffers here
      utils::PlayInfo play;
      if(!mReconf.setupBuf(args, play)) {
         LOGE("%s failed to reconf setupargs", __FUNCTION__);
         return false;
      }

      // regular setXXX interface calls here
      utils::ReconfArgs reargs(args);
      reargs.play = play;
      utils::PipeArgs parg0(mPipe0->getArgs());
      parg0 = reargs;
      utils::PipeArgs parg1(mPipe1->getArgs());
      parg1 = reargs;
      utils::PipeArgs parg2(mPipe2->getArgs());
      parg2 = reargs;
      utils::PipeArgs pargs[utils::MAX_PIPES] = {parg0, parg1, parg2};
      if(!this->setSource(pargs)) {
         LOGE("%s failed to setCrop", __FUNCTION__);
         return false;
      }
      if(!this->setCrop(reargs.crop)) {
         LOGE("%s failed to setCrop", __FUNCTION__);
         return false;
      }
      if(!this->setPosition(reargs.pos)) {
         LOGE("%s failed to setPosition", __FUNCTION__);
         return false;
      }
      utils::Params param(utils::OVERLAY_TRANSFORM, reargs.orientation);
      if(!this->setParameter(param)) {
         LOGE("%s failed to setParameter", __FUNCTION__);
         return false;
      }
      if(!this->commit()){
         LOGE("%s failed to commit", __FUNCTION__);
         return false;
      }
      break;
   }
   case utils::RECONFIG_OFF:
      LOGE_IF(DEBUG_OVERLAY, "%s reconfig is off", __FUNCTION__);
      mReconf.reset();
      break;
   default:
      OVASSERT(false, "%s unknown reconfig state %d",
               __FUNCTION__, args.reconf);
      return false;
   }
   return true;
}

template <class P0, class P1, class P2>
utils::eOverlayPipeType OverlayImpl<P0, P1, P2>::getOvPipeType(utils::eDest dest) const
{
   OVASSERT(utils::isValidDest(dest), "%s: OverlayImpl invalid dest=%d",
            __FUNCTION__, dest);

   if (utils::OV_PIPE0 & dest) {
      OVASSERT(mPipe0, "%s: OverlayImpl pipe0 is null", __FUNCTION__);
      return mPipe0->getOvPipeType();
   }

   if (utils::OV_PIPE1 & dest) {
      OVASSERT(mPipe1, "%s: OverlayImpl pipe1 is null", __FUNCTION__);
      return mPipe1->getOvPipeType();
   }

   if (utils::OV_PIPE2 & dest) {
      OVASSERT(mPipe2, "%s: OverlayImpl pipe2 is null", __FUNCTION__);
      return mPipe2->getOvPipeType();
   }

   // Should never get here
   return utils::OV_PIPE_TYPE_NULL;
}

template <class P0, class P1, class P2>
void OverlayImpl<P0, P1, P2>::dump() const
{
   OVASSERT(mPipe0 && mPipe1 && mPipe2,
            "%s: Pipes are null p0=%p p1=%p p2=%p",
            __FUNCTION__, mPipe0, mPipe1, mPipe2);
   LOGE("== Dump OverlayImpl dump start ROT p0 ==");
   mRotP0->dump();
   LOGE("== Dump OverlayImpl dump end ROT p0 ==");
   LOGE("== Dump OverlayImpl dump start ROT p1 ==");
   mRotP1->dump();
   LOGE("== Dump OverlayImpl dump end ROT p1 ==");
   LOGE("== Dump OverlayImpl dump start ROT p2 ==");
   mRotP2->dump();
   LOGE("== Dump OverlayImpl dump end ROT p2 ==");
   LOGE("== Dump OverlayImpl dump start p0 ==");
   mPipe0->dump();
   LOGE("== Dump OverlayImpl dump end p0 ==");
   LOGE("== Dump OverlayImpl dump start p1 ==");
   mPipe1->dump();
   LOGE("== Dump OverlayImpl dump end p1 ==");
   LOGE("== Dump OverlayImpl dump start p2 ==");
   mPipe2->dump();
   LOGE("== Dump OverlayImpl dump end p2 ==");
}

} // overlay2
