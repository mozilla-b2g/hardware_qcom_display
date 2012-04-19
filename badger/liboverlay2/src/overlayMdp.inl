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

#include "mdpWrapper.h"
#include <linux/msm_mdp.h>

#include "overlayRotator.h"

#undef LOG_TAG
#define LOG_TAG "overlay2"

namespace overlay2 {

   namespace utils {
      inline bool openDev(OvFD& fd, int fb,
                          const char* const s,
                          int flags) {
         return overlay2::open(fd, fb, Res::devTemplate, O_RDWR);
      }
   }

   template <class T>
   inline void init(T& t) {
      memset(&t, 0, sizeof(T));
   }

   /////   MdpCtrl  //////

   inline MdpCtrl::MdpCtrl() : mSize(0) {
      reset();
   }

   inline MdpCtrl::~MdpCtrl() {
      close();
   }

   inline int MdpCtrl::getOrient() const {
      return getUserData();
   }

   inline int MdpCtrl::getId() const {
      return mOVInfo.id;
   }

   inline int MdpCtrl::getFd() const {
      return mFd.getFD();
   }

   inline int MdpCtrl::getFlags() const {
      return mOVInfo.flags;
   }

   inline void MdpCtrl::setFlags(int f) {
      mOVInfo.flags = f;
   }

   inline void MdpCtrl::setZ(overlay2::utils::eZorder z) {
      mOVInfo.z_order = z;
   }

   inline void MdpCtrl::setWait(overlay2::utils::eWait wait) {
      mOVInfo.flags = utils::setWait(wait, mOVInfo.flags);
   }

   inline void MdpCtrl::setIsFg(overlay2::utils::eIsFg isFg) {
      mOVInfo.is_fg = isFg;
   }

   inline bool MdpCtrl::ovChanged() const {
      // 0 means same
      if(0 == ::memcmp(&mOVInfo, &mLkgo, sizeof (mdp_overlay))) {
         return false;
      }
      return true;
   }

   inline void MdpCtrl::save() {
      if(static_cast<ssize_t>(mOVInfo.id) == -1) {
         LOGE("MdpCtrl current ov has id -1, will not save");
         // FIXME dump both?
         return;
      }
      mLkgo = mOVInfo;
   }

   inline void MdpCtrl::restore() {
      if(static_cast<ssize_t>(mLkgo.id) == -1) {
         LOGE("MdpCtrl Lkgo ov has id -1, will not restore");
         // FIXME dump both?
         return;
      }
      mOVInfo = mLkgo;
   }

   inline overlay2::utils::Whf MdpCtrl::getSrcWhf() const {
      return utils::Whf(mOVInfo.src.width,
                        mOVInfo.src.height,
                        mOVInfo.src.format);
   }

   inline void MdpCtrl::setSrcWhf(const overlay2::utils::Whf& whf) {
      mOVInfo.src.width  = whf.w;
      mOVInfo.src.height = whf.h;
      mOVInfo.src.format = whf.format;
   }

   inline overlay2::utils::Dim MdpCtrl::getSrcRectDim() const {
      return utils::Dim(mOVInfo.src_rect.x,
                        mOVInfo.src_rect.y,
                        mOVInfo.src_rect.w,
                        mOVInfo.src_rect.h);
   }

   inline void MdpCtrl::setSrcRectDim(const overlay2::utils::Dim d) {
      mOVInfo.src_rect.x = d.x;
      mOVInfo.src_rect.y = d.y;
      mOVInfo.src_rect.w = d.w;
      mOVInfo.src_rect.h = d.h;
   }

   inline overlay2::utils::Dim MdpCtrl::getDstRectDim() const {
      return utils::Dim(mOVInfo.dst_rect.x,
                        mOVInfo.dst_rect.y,
                        mOVInfo.dst_rect.w,
                        mOVInfo.dst_rect.h);
   }

   inline void MdpCtrl::setDstRectDim(const overlay2::utils::Dim d) {
      mOVInfo.dst_rect.x = d.x;
      mOVInfo.dst_rect.y = d.y;
      mOVInfo.dst_rect.w = d.w;
      mOVInfo.dst_rect.h = d.h;
   }

   inline int MdpCtrl::getUserData() const { return mOVInfo.user_data[0]; }

   inline void MdpCtrl::setUserData(int v) { mOVInfo.user_data[0] = v; }

   inline void MdpCtrl::setRotationFlags() {
      const int u = getUserData();
      if (u == MDP_ROT_90 || u == MDP_ROT_270)
         mOVInfo.flags |= MDP_SOURCE_ROTATED_90;
      else
         mOVInfo.flags &= ~MDP_SOURCE_ROTATED_90;
   }


   inline void MdpCtrl::swapSrcWH() {
      utils::swap(mOVInfo.src.width,
                  mOVInfo.src.height); }

   inline void MdpCtrl::swapSrcRectWH() {
      utils::swap(mOVInfo.src_rect.h,
                  mOVInfo.src_rect.w); }

   ///////    MdpCtrl3D //////

   inline MdpCtrl3D::MdpCtrl3D() { reset(); }
   inline bool MdpCtrl3D::close() {
      if (m3DOVInfo.is_3d) {
         m3DOVInfo.is_3d = 0;
         if(!mdp_wrapper::set3D(mFd.getFD(), m3DOVInfo)) {
            LOGE("MdpCtrl3D close failed set3D with 0");
            return false;
         }
      }
      reset();
      return true;
   }
   inline void MdpCtrl3D::reset() {
      utils::memset0(m3DOVInfo);
   }

   inline void MdpCtrl3D::setFd(int fd) {
      mFd.copy(fd);
      OVASSERT(mFd.valid(), "MdpCtrl3D setFd, FD should be valid");
   }

   inline void MdpCtrl3D::setWh(const utils::Whf& whf) {
      // ignore fmt. Needed for useVirtualFB callflow
      m3DOVInfo.width = whf.w;
      m3DOVInfo.height = whf.h;
   }

   inline bool MdpCtrl3D::useVirtualFB() {
      if(!m3DOVInfo.is_3d) {
         m3DOVInfo.is_3d = 1;
         if(!mdp_wrapper::set3D(mFd.getFD(), m3DOVInfo)) {
            LOGE("MdpCtrl3D close failed set3D with 0");
            return false;
         }
      }
      return true;
   }
   
   ///////    MdpData   //////

   inline MdpData::MdpData() { reset(); }

   inline MdpData::~MdpData() { close(); }

   inline bool MdpData::open(uint32_t fbnum) {
      // FD open
      if(!utils::openDev(mFd, fbnum, Res::devTemplate, O_RDWR)){
         LOGE("Ctrl failed to open fbnum=%d", fbnum);
         return false;
      }
      return true;
   }

   inline void MdpData::reset() {
      overlay2::utils::memset0(mOvData);
      mOvData.data.memory_id = -1;
   }

   inline bool MdpData::close() {
      if(-1 == mOvData.data.memory_id) return true;
      reset();
      if(!mFd.close()) {
         return false;
      }
      return true;
   }

   inline void MdpData::setMemoryId(int id) { mOvData.data.memory_id = id; }
   inline int MdpData::getMemoryId() const { return mOvData.data.memory_id; }

   inline void MdpData::setId(int id) { mOvData.id = id; }

   inline int MdpData::getId() const { return mOvData.id; }

   inline int MdpData::getFd() const { return mFd.getFD(); }

   inline void MdpData::setOffset(uint32_t o) { mOvData.data.offset = o; }

   inline bool MdpData::play() {
      if(!mdp_wrapper::play(mFd.getFD(), mOvData)){
         LOGE("MdpData failed to play");
         return false;
      }
      return true;
   }

   inline bool MdpData::playWait() {
      if(!mdp_wrapper::playWait(mFd.getFD(), mOvData)){
         LOGE("MdpData failed to playWait");
         return false;
      }
      return true;
   }
}
