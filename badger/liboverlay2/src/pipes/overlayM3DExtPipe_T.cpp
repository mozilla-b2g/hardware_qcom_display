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
   inline M3DExtPipe<FB, CHAN>::M3DExtPipe() : mM3Dfmt(0) {}
   template <int FB, int CHAN>
   inline M3DExtPipe<FB, CHAN>::~M3DExtPipe() { close(); }
   template <int FB, int CHAN>
   inline bool M3DExtPipe<FB, CHAN>::open(RotatorBase* rot) {
      LOGE_IF(DEBUG_OVERLAY, "M3DExtPipe open");
      if(!mM3d.open(rot)) {
         LOGE("3Dpipe failed to open");
         return false;
      }
      return true;
   }
   template <int FB, int CHAN>
   inline bool M3DExtPipe<FB, CHAN>::close() {
      return mM3d.close();
   }
   template <int FB, int CHAN>
   inline bool M3DExtPipe<FB, CHAN>::commit() { return mM3d.commit(); }
   template <int FB, int CHAN>
   inline void M3DExtPipe<FB, CHAN>::setId(int id) { mM3d.setId(id); }
   template <int FB, int CHAN>
   inline void M3DExtPipe<FB, CHAN>::setMemoryId(int id) { mM3d.setMemoryId(id); }
   template <int FB, int CHAN>
   inline bool M3DExtPipe<FB, CHAN>::queueBuffer(uint32_t offset) {
      return mM3d.queueBuffer(offset); }
   template <int FB, int CHAN>
   inline bool M3DExtPipe<FB, CHAN>::dequeueBuffer(void*& buf) {
      return mM3d.dequeueBuffer(buf); }
   template <int FB, int CHAN>
   inline bool M3DExtPipe<FB, CHAN>::waitForVsync() {
      return mM3d.waitForVsync(); }
   template <int FB, int CHAN>
   inline bool M3DExtPipe<FB, CHAN>::setCrop(const utils::Dim& d) {
      utils::Dim _dim;
      if(!utils::getCropS3D<CHAN>(d, _dim, mM3Dfmt)){
         LOGE("M3DExtPipe setCrop failed to getCropS3D");
         _dim = d;
      }
      return mM3d.setCrop(_dim);
   }
   template <int FB, int CHAN>
   inline bool M3DExtPipe<FB, CHAN>::start(const utils::PipeArgs& args) {
      if(!mM3d.start(args)) {
         LOGE("M3DExtPipe start failed");
         return false;
      }
      return true;
   }
   template <int FB, int CHAN>
   inline bool M3DExtPipe<FB, CHAN>::setPosition(const utils::Dim& d) {
      utils::Dim _dim;
      // original setPositionHandleState has getPosisitonS3D(...,true)
      // which means format is HAL_3D_OUT_SBS_MASK
      // HAL_3D_OUT_SBS_MASK is 0x1000 >> 12 == 0x1 as the orig
      // code suggets
      utils::Whf _whf(mM3d.getScreenInfo().mFBWidth,
                      mM3d.getScreenInfo().mFBHeight,
                      mM3Dfmt);
      if(!utils::getPosisitonS3D<CHAN>(_whf, _dim)) {
         LOGE("S3DPrimaryPipe setPosition err in getPosisitonS3D");
         _dim = d;
      }
      return mM3d.setPosition(_dim);
   }
   template <int FB, int CHAN>
   inline bool M3DExtPipe<FB, CHAN>::setParameter(const utils::Params& param) {
      return mM3d.setParameter(param);
   }
   template <int FB, int CHAN>
   inline bool M3DExtPipe<FB, CHAN>::setSource(const utils::PipeArgs& args)
   {
      // extract 3D fmt
      mM3Dfmt = utils::format3DInput(utils::getS3DFormat(args.whf.format)) |
         utils::HAL_3D_OUT_MONOS_MASK;
      if(mM3d.isClosed()){
         if(!this->start(args)) {
            LOGE("M3DExtPipe setSource failed to start");
            return false;
         }
      }
      return mM3d.setSource(args);
   }
   template <int FB, int CHAN>
   inline const utils::PipeArgs& M3DExtPipe<FB, CHAN>::getArgs() const {
      return mM3d.getArgs();
   }
   template <int FB, int CHAN>
   inline utils::eOverlayPipeType M3DExtPipe<FB, CHAN>::getOvPipeType() const {
      return utils::OV_PIPE_TYPE_M3D_EXTERNAL;
   }
   template <int FB, int CHAN>
   inline void M3DExtPipe<FB, CHAN>::dump() const {
      LOGE("M3DExtPipe Pipe fmt=%d", mM3Dfmt);
      mM3d.dump();
   }

}// overlay2
