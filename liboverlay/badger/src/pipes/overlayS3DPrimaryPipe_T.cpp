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

#include "overlayUtils.h"

namespace overlay2 {
   template <int FB, int CHAN>
   inline S3DPrimaryPipe<FB, CHAN>::S3DPrimaryPipe() : mS3Dfmt(0) {}
   template <int FB, int CHAN>
   inline S3DPrimaryPipe<FB, CHAN>::~S3DPrimaryPipe() { close(); }
   template <int FB, int CHAN>
   inline bool S3DPrimaryPipe<FB, CHAN>::open(RotatorBase* rot) {
      LOGE_IF(DEBUG_OVERLAY, "S3DPrimaryPipe open");
      if(!mS3d.open(rot)) {
         LOGE("3Dpipe failed to open");
         return false;
      }
      // set the ctrl fd
      mCtrl3D.setFd(mS3d.getCtrlFd());
      return true;
   }
   template <int FB, int CHAN>
   inline bool S3DPrimaryPipe<FB, CHAN>::close() {
      if(!utils::enableBarrier(0)) {
         LOGE("S3DExtPipe close failed enable barrier");
      }
      mCtrl3D.close();
      return mS3d.close();
   }
   template <int FB, int CHAN>
   inline bool S3DPrimaryPipe<FB, CHAN>::commit() { return mS3d.commit(); }
   template <int FB, int CHAN>
   inline void S3DPrimaryPipe<FB, CHAN>::setId(int id) { mS3d.setId(id); }
   template <int FB, int CHAN>
   inline void S3DPrimaryPipe<FB, CHAN>::setMemoryId(int id) { mS3d.setMemoryId(id); }
   template <int FB, int CHAN>
   inline bool S3DPrimaryPipe<FB, CHAN>::queueBuffer(uint32_t offset) {
      return mS3d.queueBuffer(offset); }
   template <int FB, int CHAN>
   inline bool S3DPrimaryPipe<FB, CHAN>::dequeueBuffer(void*& buf) {
      return mS3d.dequeueBuffer(buf); }
   template <int FB, int CHAN>
   inline bool S3DPrimaryPipe<FB, CHAN>::waitForVsync() {
      return mS3d.waitForVsync(); }
   template <int FB, int CHAN>
   inline bool S3DPrimaryPipe<FB, CHAN>::setCrop(const utils::Dim& d) {
      utils::Dim _dim;
      if(!utils::getCropS3D<CHAN>(d, _dim, mS3Dfmt)){
         LOGE("S3DPrimaryPipe setCrop failed to getCropS3D");
         _dim = d;
      }
      return mS3d.setCrop(_dim);
   }
   template <int FB, int CHAN>
   inline bool S3DPrimaryPipe<FB, CHAN>::start(const utils::PipeArgs& args) {
      if(!mS3d.start(args)) {
         LOGE("S3DPrimaryPipe start failed");
         return false;
      }
      return true;
   }
   template <int FB, int CHAN>
   inline bool S3DPrimaryPipe<FB, CHAN>::setPosition(const utils::Dim& d)
   {
      utils::Whf fbwhf(mS3d.getScreenInfo().mFBWidth,
                       mS3d.getScreenInfo().mFBHeight,
                       0 /* fmt dont care*/);
      mCtrl3D.setWh(fbwhf);
      if(!mCtrl3D.useVirtualFB()) {
         LOGE("Failed to use VFB on %d (non fatal)", FB);
         return false;
      }
      utils::Dim _dim;
      // original setPositionHandleState has getPosisitonS3D(...,true)
      // which means format is HAL_3D_OUT_SBS_MASK
      // HAL_3D_OUT_SBS_MASK is 0x1000 >> 12 == 0x1 as the orig
      // code suggets
      utils::Whf _whf(d.w, d.h, utils::HAL_3D_OUT_SBS_MASK);
      if(!utils::getPosisitonS3D<CHAN>(_whf, _dim)) {
         LOGE("S3DPrimaryPipe setPosition err in getPosisitonS3D");
         _dim = d;
      }
      return mS3d.setPosition(_dim);
   }

   /* for S3DPrimaryPipe, we need to have barriers once
    * So the easiest way to achieve it, is to make sure FB0 is having it before
    * setParam is running */
   template <>
   inline bool S3DPrimaryPipe<utils::FB0, utils::OV_PIPE0>::setParameter(
      const utils::Params& param) {
      if(utils::OVERLAY_TRANSFORM == param.param){
         uint32_t barrier=0;
         switch(param.value) {
         case HAL_TRANSFORM_ROT_90:
         case HAL_TRANSFORM_ROT_270:
            barrier = utils::BARRIER_LAND;
            break;
         default:
            barrier = utils::BARRIER_PORT;
            break;
         }
         if(!utils::enableBarrier(barrier)) {
            LOGE("S3DPrimaryPipe setParameter failed to enable barrier");
         }
      }
      return mS3d.setParameter(param);
   }

   template <int FB, int CHAN>
   inline bool S3DPrimaryPipe<FB, CHAN>::setParameter(const utils::Params& param) {
      return mS3d.setParameter(param);
   }
   template <int FB, int CHAN>
   inline bool S3DPrimaryPipe<FB, CHAN>::setSource(const utils::PipeArgs& args)
   {
      mS3Dfmt = utils::getS3DFormat(args.whf.format);
      if(mS3d.isClosed()){
         if(!this->start(args)) {
            LOGE("S3DPrimaryPipe setSource failed to start");
            return false;
         }
      }
      return mS3d.setSource(args);
   }
   template <int FB, int CHAN>
   inline const utils::PipeArgs& S3DPrimaryPipe<FB, CHAN>::getArgs() const {
      return mS3d.getArgs();
   }
   template <int FB, int CHAN>
   inline utils::eOverlayPipeType S3DPrimaryPipe<FB, CHAN>::getOvPipeType() const {
      return utils::OV_PIPE_TYPE_S3D_PRIMARY;
   }
   template <int FB, int CHAN>
   inline void S3DPrimaryPipe<FB, CHAN>::dump() const {
      LOGE("S3DPrimaryPipe Pipe fmt=%d", mS3Dfmt);
      mS3d.dump();
   }

}// overlay2
