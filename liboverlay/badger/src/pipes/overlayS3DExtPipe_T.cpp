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
   inline S3DExtPipe<FB, CHAN>::S3DExtPipe() : mS3Dfmt(0) {}
   template <int FB, int CHAN>
   inline S3DExtPipe<FB, CHAN>::~S3DExtPipe() { close(); }
   template <int FB, int CHAN>
   inline bool S3DExtPipe<FB, CHAN>::open(RotatorBase* rot) {
      LOGE_IF(DEBUG_OVERLAY, "S3DExtPipe open");
      if(!mS3d.open(rot)) {
         LOGE("3Dpipe failed to open");
         return false;
      }
      return true;
   }
   template <int FB, int CHAN>
   inline bool S3DExtPipe<FB, CHAN>::close() {
      if(!utils::send3DInfoPacket(0)) {
         LOGE("S3DExtPipe close failed send3D info packet");
      }
      return mS3d.close();
   }
   template <int FB, int CHAN>
   inline bool S3DExtPipe<FB, CHAN>::commit() { return mS3d.commit(); }
   template <int FB, int CHAN>
   inline void S3DExtPipe<FB, CHAN>::setId(int id) { mS3d.setId(id); }
   template <int FB, int CHAN>
   inline void S3DExtPipe<FB, CHAN>::setMemoryId(int id) { mS3d.setMemoryId(id); }
   template <int FB, int CHAN>
   inline bool S3DExtPipe<FB, CHAN>::queueBuffer(uint32_t offset) {
      //this->dump();
      return mS3d.queueBuffer(offset); }
   template <int FB, int CHAN>
   inline bool S3DExtPipe<FB, CHAN>::dequeueBuffer(void*& buf) {
      return mS3d.dequeueBuffer(buf); }
   template <int FB, int CHAN>
   inline bool S3DExtPipe<FB, CHAN>::waitForVsync() {
      return mS3d.waitForVsync(); }
   template <int FB, int CHAN>
   inline bool S3DExtPipe<FB, CHAN>::setCrop(const utils::Dim& d) {
      utils::Dim _dim;
      if(!utils::getCropS3D<CHAN>(d, _dim, mS3Dfmt)){
         LOGE("S3DExtPipe setCrop failed to getCropS3D");
         _dim = d;
      }
      return mS3d.setCrop(_dim);
   }
   template <int FB, int CHAN>
   inline bool S3DExtPipe<FB, CHAN>::start(const utils::PipeArgs& args) {
      OVASSERT(mS3Dfmt, "S3DExtPipe mS3Dfmt should not be 0 here");
      if(!mS3d.start(args)) {
         LOGE("S3DExtPipe start failed");
         return false;
      }
      uint32_t fmt = mS3Dfmt & utils::OUTPUT_3D_MASK;
      if(!utils::send3DInfoPacket(fmt)){
         LOGE("Error S3DExtPipe start error send3DInfoPacket %d", fmt);
         return false;
      }
      return true;
   }
   template <int FB, int CHAN>
   inline bool S3DExtPipe<FB, CHAN>::setPosition(const utils::Dim& d)
   {
      utils::Dim _dim;
      utils::Whf _whf(mS3d.getScreenInfo().mFBWidth,
                      mS3d.getScreenInfo().mFBHeight,
                      mS3Dfmt);
      if(!utils::getPosisitonS3D<CHAN>(_whf, _dim)) {
         LOGE("S3DExtPipe setPosition err in getPosisitonS3D");
         _dim = d;
      }
      return mS3d.setPosition(_dim);
   }
   template <int FB, int CHAN>
   inline bool S3DExtPipe<FB, CHAN>::setParameter(const utils::Params& param) {
      return mS3d.setParameter(param);
   }
   template <int FB, int CHAN>
   inline bool S3DExtPipe<FB, CHAN>::setSource(const utils::PipeArgs& args) {
      mS3Dfmt = utils::getS3DFormat(args.whf.format);
      if(mS3d.isClosed()){
         if(!this->start(args)) {
            LOGE("S3DExtPipe setSource failed to start");
            return false;
         }
      }
      return mS3d.setSource(args);
   }
   template <int FB, int CHAN>
   inline const utils::PipeArgs& S3DExtPipe<FB, CHAN>::getArgs() const {
      return mS3d.getArgs();
   }
   template <int FB, int CHAN>
   inline utils::eOverlayPipeType S3DExtPipe<FB, CHAN>::getOvPipeType() const {
      return utils::OV_PIPE_TYPE_S3D_EXTERNAL;
   }
   template <int FB, int CHAN>
   inline void S3DExtPipe<FB, CHAN>::dump() const {
      LOGE("S3DExtPipe Pipe fmt=%d", mS3Dfmt);
      mS3d.dump();
   }

}// overlay2
