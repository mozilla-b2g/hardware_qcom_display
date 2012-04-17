/*
 * Copyright (C) 2008 The Android Open Source Project
 * Copyright (c) 2010-2012, Code Aurora Forum. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mdpWrapper.h"

namespace {
   // just a helper func for Rotator common operations x-(y+z)
   int compute(uint32_t x, uint32_t y, uint32_t z) {
      return x-(y+z);
   }
}

namespace overlay2 {

   //// MdpRot ////
   inline MdpRot::MdpRot() { reset(); }
   inline bool MdpRot::start(const utils::PipeArgs& args) {
      return this->start<utils::ROT_OUT_FMT_DEFAULT>(args);
   }

   inline void MdpRot::setDataMemId(int fd) { mData.data.memory_id = fd; }
   inline void MdpRot::setRotDataSrcMemId(int fd) {
      mRotDataInfo.src.memory_id = fd; }

   inline void MdpRot::setEnable() { mRotImgInfo.enable = 1; }
   inline void MdpRot::setDisable() { mRotImgInfo.enable = 0; }
   inline bool MdpRot::enabled() const { return mRotImgInfo.enable; }

   inline void MdpRot::setRotations(uint32_t r) { mRotImgInfo.rotations = r; }
   inline void MdpRot::setDataReqId(int id) { mData.id = id; }
   inline void MdpRot::swapDstWH() {
      overlay2::utils::swap(mRotImgInfo.dst.width,
                            mRotImgInfo.dst.height); }

   inline overlay2::utils::Whf MdpRot::getSrcWhf() const {
      return overlay2::utils::Whf(mRotImgInfo.src.width,
                                  mRotImgInfo.src.height,
                                  mRotImgInfo.src.format);
   }

   inline int MdpRot::getDstMemId() const {
      return mRotDataInfo.dst.memory_id;
   }
   inline uint32_t MdpRot::getDstOffset() const {
      return mRotDataInfo.dst.offset;
   }

   inline void MdpRot::setSrcWhf(const overlay2::utils::Whf& whf) {
      mRotImgInfo.src.width = whf.w;
      mRotImgInfo.src.height = whf.h;
      mRotImgInfo.src.format = whf.format;
   }

   inline int MdpRot::getSessId() const { return mRotImgInfo.session_id; }

   ///// Null Rotator /////
   inline NullRotator::~NullRotator() {}
   inline bool NullRotator::open() {
      return true; }
   inline bool NullRotator::remap(uint32_t numbufs,
                                  const utils::PipeArgs& args){
      return true; }
   inline bool NullRotator::close() { return true; }
   inline bool NullRotator::start(const utils::PipeArgs& args)
   { return true; }

   inline bool NullRotator::start() { return true; }
   inline bool NullRotator::overlayTransform(MdpCtrl& o,
                                             utils::eTransform& rot)
   { return true; }
   inline void NullRotator::setSrcWhf(const overlay2::utils::Whf& wfh) {}
   inline void NullRotator::setRotations(uint32_t) {}
   inline void NullRotator::setDataReqId(int id) {}
   inline void NullRotator::setEnable() {}
   inline void NullRotator::setDisable() {}
   inline bool NullRotator::enabled() const { return false; }
   inline int NullRotator::getSessId() const { return -1; }
   inline overlay2::utils::Whf NullRotator::getSrcWhf() const {
      return overlay2::utils::Whf(); }
   inline bool NullRotator::prepareQueueBuf(uint32_t offset)
   { return true; }
   inline bool NullRotator::setSourceReconf(const utils::PipeArgs& args){
      return true; }
   inline bool NullRotator::play(int fd)
   { return true; }
   inline void NullRotator::setDataMemId(int fd) {}
   inline void NullRotator::setRotDataSrcMemId(int fd) {}
   inline void NullRotator::dump() const {
      LOGE("== Dump NullRotator dump (null) start/end ==");
   }

   ///// Rotator /////
   inline Rotator::Rotator() { }

   inline Rotator::~Rotator() {
      mRot.close(); // also will do reset
   }

   inline bool Rotator::open() {
      if(!mRot.open()) {
         LOGE("Rotator::open failed");
         return false;
      }
      return true;
   }

   template <int ROT_OUT_FMT>
   inline bool Rotator::start(const utils::PipeArgs& args) {
      return mRot.start<ROT_OUT_FMT>(args);
   }

   inline bool Rotator::remap(uint32_t numbufs,
                              const utils::PipeArgs& args){
      if(!mRot.remap(numbufs, args)) {
         LOGE("%s failed", __FUNCTION__);
         return false;
      }
      return true;
   }

   inline bool Rotator::close() {
      return mRot.close();
   }

   inline bool Rotator::start() {
      return mRot.start();
   }

   inline bool Rotator::start(const utils::PipeArgs& args) {
      return mRot.start(args);
   }

   inline bool Rotator::unmapNonCurrent() {
      return mRot.unmapNonCurrent();
   }

   inline void Rotator::setEnable(){ mRot.setEnable(); }
   inline void Rotator::setDisable(){ mRot.setDisable(); }
   inline bool Rotator::enabled() const { return mRot.enabled(); }
   inline void Rotator::setDataMemId(int fd) {
      mRot.setDataMemId(fd); }

   inline void Rotator::setRotDataSrcMemId(int fd) {
      mRot.setRotDataSrcMemId(fd);
   }

   inline int Rotator::getDstMemId() const {
      return mRot.getDstMemId();
   }
   inline uint32_t Rotator::getDstOffset() const {
      return mRot.getDstOffset();
   }

   inline void Rotator::setDataReqId(int id) {
      mRot.setDataReqId(id);
   }

   inline void Rotator::setSrcWhf(
      const overlay2::utils::Whf& whf) {
      mRot.setSrcWhf(whf);
   }

   inline bool Rotator::setSourceReconf(const utils::PipeArgs& args){
      return mRot.setSourceReconf(args); }

   inline void Rotator::setRotations(uint32_t rot) {
      mRot.setRotations (rot);
   }

   inline int Rotator::getSessId() const {
      return mRot.getSessId(); }

   inline void Rotator::dump() const {
      LOGE("== Dump Rotator start ==");
      mRot.dump();
      LOGE("== Dump Rotator end ==");
   }

   inline overlay2::utils::Whf Rotator::getSrcWhf() const {
      return mRot.getSrcWhf(); }

   inline bool Rotator::prepareQueueBuf(uint32_t offset)
   {
      return mRot.prepareQueueBuf(offset);
   }

   inline bool Rotator::play(int fd)
   {
      return mRot.play(fd);
   }

   template <int ROT_OUT_FMT>
   bool MdpRot::start(const utils::PipeArgs& args) {
      // Do nothing when no orientation
      if(utils::OVERLAY_TRANSFORM_0 == args.orientation &&
         utils::ROT_FLAG_ENABLED != args.rotFlags) {
         return true;
      }
      utils::Whf whf(args.whf);
      mRotImgInfo.src.format = whf.format;
      mRotImgInfo.src.width = whf.w;
      mRotImgInfo.src.height = whf.h;
      mRotImgInfo.src_rect.w = whf.w;
      mRotImgInfo.src_rect.h = whf.h;
      mRotImgInfo.dst.width = whf.w;
      mRotImgInfo.dst.height = whf.h;
      if(whf.format == MDP_Y_CRCB_H2V2_TILE) {
         mRotImgInfo.src.width =  utils::alignup(whf.w, 64);
         mRotImgInfo.src.height = utils::alignup(whf.h, 32);
         mRotImgInfo.src_rect.w = utils::alignup(whf.w, 64);
         mRotImgInfo.src_rect.h = utils::alignup(whf.h, 32);
         mRotImgInfo.dst.width  = utils::alignup(whf.w, 64);
         mRotImgInfo.dst.height = utils::alignup(whf.h, 32);
         mRotImgInfo.dst.format = MDP_Y_CRCB_H2V2;
      }
      // either utils::getRotOutFmt(whf.format); or supplied fmt
      // utils::RotOutFmt<ROT_OUT_FMT_DEFAULT>::fmt;
      mRotImgInfo.dst.format = utils::RotOutFmt<ROT_OUT_FMT>::fmt(whf.format);
      mRotImgInfo.dst_x = 0;
      mRotImgInfo.dst_y = 0;
      mRotImgInfo.src_rect.x = 0;
      mRotImgInfo.src_rect.y = 0;
      mRotImgInfo.rotations = 0;
      // ROT_FLAG_DISABLED / ENABLED
      // UI channel in the original code indicates hint
      // of which pmem pool we need to allocate rotator
      // bufs from. Refer to overlayUtils.h eRotFlags
      // for more info
      mRotImgInfo.enable = args.rotFlags;
      mRotImgInfo.session_id = mRotImgInfo.session_id ?
         mRotImgInfo.session_id : 0;

      return start();
   }

} // overlay2
