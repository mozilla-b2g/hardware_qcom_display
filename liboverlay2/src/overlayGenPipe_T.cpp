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

namespace overlay2 {

template <class CTRL_DATA, int FB>
GenericPipe<CTRL_DATA, FB>::GenericPipe() : mRot(0), mFlags(CLOSED) {}

template <class CTRL_DATA, int FB>
GenericPipe<CTRL_DATA, FB>::~GenericPipe() {
   close();
}

template <class CTRL_DATA, int FB>
bool GenericPipe<CTRL_DATA, FB>::open(RotatorBase* rot)
{
   OVASSERT(rot, "rot is null");
   // open ctrl and data
   uint32_t fbnum = FB;
   LOGE_IF(DEBUG_OVERLAY, "GenericPipe open");
   if(!mCtrlData.ctrl.open(fbnum, rot)) {
      LOGE("GenericPipe failed to open ctrl");
      return false;
   }
   if(!mCtrlData.data.open(fbnum, rot)) {
      LOGE("GenericPipe failed to open data");
      return false;
   }
   mRot = rot;

   // NOTE: we won't have the flags as non CLOSED since we
   // consider the pipe opened for business only when we call
   // start()

   return true;
}

template <class CTRL_DATA, int FB>
bool GenericPipe<CTRL_DATA, FB>::close() {
   if(isClosed()) return true;
   bool ret = true;
   if(!mCtrlData.ctrl.close()) {
      LOGE("GenericPipe failed to close ctrl");
      ret = false;
   }
   if (!mCtrlData.data.close()) {
      LOGE("GenericPipe failed to close data");
      ret = false;
   }
   setClosed();
   return ret;
}

template <class CTRL_DATA, int FB>
inline bool GenericPipe<CTRL_DATA, FB>::commit(){
   OVASSERT(isOpen(), "State is closed, cannot commit");
   return mCtrlData.ctrl.commit();
}

template <class CTRL_DATA, int FB>
inline void GenericPipe<CTRL_DATA, FB>::setMemoryId(int id) {
   OVASSERT(isOpen(), "State is closed, cannot setMemoryId");
   if(utils::RECONFIG_ON == mArgs.reconf) {
      id = mArgs.play.fd;
      OVASSERT(-1 != id, "%s id is -1", __FUNCTION__);
   }
   mCtrlData.data.setMemoryId(id);
}

template <class CTRL_DATA, int FB>
inline void GenericPipe<CTRL_DATA, FB>::setId(int id) {
   mCtrlData.data.setId(id); }

template <class CTRL_DATA, int FB>
inline int GenericPipe<CTRL_DATA, FB>::getCtrlFd() const {
   return mCtrlData.ctrl.getFd();
}

template <class CTRL_DATA, int FB>
inline bool GenericPipe<CTRL_DATA, FB>::setCrop(
   const overlay2::utils::Dim& d) {
   OVASSERT(isOpen(), "State is closed, cannot setCrop");
   return mCtrlData.ctrl.setCrop(d);
}

template <class CTRL_DATA, int FB>
bool GenericPipe<CTRL_DATA, FB>::start(const utils::PipeArgs& args)
{
   /* open before your start control rotator */
   uint32_t sz = args.whf.size; //utils::getSizeByMdp(args.whf);
   OVASSERT(sz, "GenericPipe sz=%d", sz);
   if(!mRot->open()) {
      LOGE("GenericPipe start failed to open rot");
      return false;
   }

   if(!mCtrlData.ctrl.start(args)){
      LOGE("GenericPipe failed to start");
      return false;
   }

   int ctrlId = mCtrlData.ctrl.getId();
   OVASSERT(-1 != ctrlId, "Ctrl ID should not be -1");
   // set ID requeset to assoc ctrl to data
   setId(ctrlId);
   // set ID request to assoc MDP data to ROT MDP data
   mRot->setDataReqId(mCtrlData.data.getId());

   // cache the args for future reference.
   mArgs = args;

   // we got here so we are open+start and good to go
   mFlags = 0; // clear flags from CLOSED
               // TODO make it more robust when more flags
               // are added

   return true;
}

template <class CTRL_DATA, int FB>
inline const utils::PipeArgs& GenericPipe<CTRL_DATA, FB>::getArgs() const
{
   return mArgs;
}

template <class CTRL_DATA, int FB>
bool GenericPipe<CTRL_DATA, FB>::startRotator() {
   // kick off rotator
   if(!mRot->start()) {
      LOGE("GenericPipe failed to start rotator");
      return false;
   }
   return true;
}

template <class CTRL_DATA, int FB>
inline bool GenericPipe<CTRL_DATA, FB>::queueBuffer(uint32_t offset) {
   OVASSERT(isOpen(), "State is closed, cannot queueBuffer");
   // when dealing with reconfig - we need to make sure data
   // channel is setup with the proper offset/fd as the src
   // for rot. The dst fd/src of the reconfig rotator
   // is the src of the queueBuf

   if(utils::RECONFIG_ON == mArgs.reconf) {
      offset = mArgs.play.offset;
   }

   return mCtrlData.data.queueBuffer(offset);
}

template <class CTRL_DATA, int FB>
inline bool GenericPipe<CTRL_DATA, FB>::dequeueBuffer(void*&) {
   OVASSERT(isOpen(), "State is closed, cannot dequeueBuffer");
   // can also set error to NOTSUPPORTED in the future
   return false;
}

template <class CTRL_DATA, int FB>
inline bool GenericPipe<CTRL_DATA, FB>::waitForVsync() {
   OVASSERT(isOpen(), "State is closed, cannot waitForVsync");

   return mCtrlData.data.waitForVsync();
}

template <class CTRL_DATA, int FB>
inline bool GenericPipe<CTRL_DATA, FB>::setPosition(const utils::Dim& dim)
{
   OVASSERT(isOpen(), "State is closed, cannot setPosition");
   return mCtrlData.ctrl.setPosition(dim);
}

template <class CTRL_DATA, int FB>
inline bool GenericPipe<CTRL_DATA, FB>::setParameter(
   const utils::Params& param)
{
   OVASSERT(isOpen(), "State is closed, cannot setParameter");
   // Currently setParameter would start rotator
   if(!mCtrlData.ctrl.setParameter(param)) {
      LOGE("GenericPipe failed to setparam");
      return false;
   }
   // if rot flags are ENABLED it means we would always
   // like to have rot. Even with 0 rot. (solves tearing)
   if(utils::ROT_FLAG_ENABLED == mArgs.rotFlags) {
      mRot->setEnable();
   }
   return startRotator();
}

template <class CTRL_DATA, int FB>
inline bool GenericPipe<CTRL_DATA, FB>::setSource(
   const utils::PipeArgs& args)
{
   // cache the recent args.
   mArgs = args;
   // setSource is the 1st thing that is being called on a pipe.
   // If pipe is closed, we should start everything.
   // we assume it is being opened with the correct FDs.
   if(isClosed()) {
      if(!this->start(args)) {
         LOGE("GenericPipe setSource failed to start");
         return false;
      }
      return true;
   }

   return mCtrlData.ctrl.setSource(args);
}

template <class CTRL_DATA, int FB>
inline utils::Dim GenericPipe<CTRL_DATA, FB>::getAspectRatio(
   const utils::Whf& whf) const
{
   return mCtrlData.ctrl.getAspectRatio(whf);
}

template <class CTRL_DATA, int FB>
inline utils::Dim GenericPipe<CTRL_DATA, FB>::getAspectRatio(
   const utils::Dim& dim) const
{
   return mCtrlData.ctrl.getAspectRatio(dim);
}

template <class CTRL_DATA, int FB>
inline utils::ScreenInfo GenericPipe<CTRL_DATA, FB>::getScreenInfo() const
{
   return mCtrlData.ctrl.getScreenInfo();
}

template <class CTRL_DATA, int FB>
inline utils::Dim GenericPipe<CTRL_DATA, FB>::getCrop() const
{
   return mCtrlData.ctrl.getCrop();
}

template <class CTRL_DATA, int FB>
inline utils::eOverlayPipeType GenericPipe<CTRL_DATA, FB>::getOvPipeType() const {
   return utils::OV_PIPE_TYPE_GENERIC;
}

template <class CTRL_DATA, int FB>
void GenericPipe<CTRL_DATA, FB>::dump() const
{
   LOGE("== Dump Generic pipe start ==");
   LOGE("flags=0x%x", mFlags);
   OVASSERT(mRot, "GenericPipe should have a valid Rot");
   mCtrlData.ctrl.dump();
   mCtrlData.data.dump();
   mRot->dump();
   LOGE("== Dump Generic pipe end ==");
}

template <class CTRL_DATA, int FB>
inline bool GenericPipe<CTRL_DATA, FB>::isClosed() const  {
   return utils::getBit(mFlags, CLOSED);
}

template <class CTRL_DATA, int FB>
inline bool GenericPipe<CTRL_DATA, FB>::isOpen() const  {
   return !isClosed();
}

template <class CTRL_DATA, int FB>
inline bool GenericPipe<CTRL_DATA, FB>::setClosed() {
   return utils::setBit(mFlags, CLOSED);
}

} // overlay2
