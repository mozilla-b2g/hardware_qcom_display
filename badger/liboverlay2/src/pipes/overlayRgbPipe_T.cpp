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

#include "overlayRes.h"
#include "mdpWrapper.h"

namespace overlay2 {

template <int FB>
RgbPipe<FB>::RgbPipe() : mFlags(0), mFbIndex(0), mFbSize(0) {
   utils::memset0(mFBbase);
}

template <int FB>
RgbPipe<FB>::~RgbPipe()
{
   if(!close()) {
      LOGE("RgbPipe close failed");
   }
}

template <int FB>
bool RgbPipe<FB>::isClosed() const{
   return utils::getBit(mFlags, CLOSED);
}

template <int FB>
bool RgbPipe<FB>::setClosed() {
   return utils::setBit(mFlags, CLOSED);
}

template <int FB>
bool RgbPipe<FB>::open()
{
   LOGE_IF(DEBUG_OVERLAY, "RgbPipe open");

   // use open defined in overlayFD file
   if(!overlay2::open(mFd, FB, Res::devTemplate)) {
      LOGE("RgbPipe failed open");
      return false;
   }
   if(!mdp_wrapper::getFScreenInfo(mFd.getFD(), mFixInfo)) {
      LOGE("RgbPipe failed getFScreenInfo");
      return false;
   }
   if(!mdp_wrapper::getVScreenInfo(mFd.getFD(), mVarInfo)) {
      LOGE("RgbPipe failed getVScreenInfo");
      return false;
   }

   // FIXME not used?
//    uint32_t memsize = (mVarInfo.xres * 
//                        mVarInfo.yres * (mVarInfo.bits_per_pixel/8));

   mVarInfo.reserved[0] = 0;
   mVarInfo.reserved[1] = 0;
   mVarInfo.reserved[2] = 0;
   mVarInfo.xoffset = 0;
   mVarInfo.yoffset = 0;
   mVarInfo.activate = FB_ACTIVATE_NOW;

   mVarInfo.yres_virtual = mVarInfo.yres * utils::RGB_PIPE_NUM_BUFS;
   if(!mdp_wrapper::setVScreenInfo(mFd.getFD(), mVarInfo)) {
      mVarInfo.yres_virtual = mVarInfo.yres;
      LOGE("RgbPipe failed setVScreenInfo. "
           "page flipping for fb1 not supported");
   }

   if(!mdp_wrapper::getFScreenInfo(mFd.getFD(), mFixInfo)) {
      LOGE("RgbPipe failed getFScreenInfo after setting %d bufs", 
           utils::RGB_PIPE_NUM_BUFS);
      return false;
   }

   if(!mdp_wrapper::getVScreenInfo(mFd.getFD(), mVarInfo)) {
      LOGE("RgbPipe failed getVScreenInfo after setting %d bufs", 
         utils::RGB_PIPE_NUM_BUFS);
      return false;
   }

   mFbSize = mFixInfo.line_length * mVarInfo.yres_virtual;

   // map memory
   mFBMapAddr = static_cast<void *> (::mmap(0, mFbSize * 1 /*numbufs*/,
                                     PROT_READ | PROT_WRITE,
                                     MAP_SHARED, mFd.getFD(), 0));

   if(MAP_FAILED == mFBMapAddr) {
      LOGE("RgbPipe failed open memmap to fb %d", FB);
      return false;
   }

   mFBbase[0] = mFBMapAddr;
   mFBbase[1] = static_cast<char*>(mFBMapAddr) + 
      (mFbSize / utils::RGB_PIPE_NUM_BUFS);

   return true;
}

template <int FB>
bool RgbPipe<FB>::close()
{
   if(isClosed()) return true;
   int retmap = ::munmap(mFBMapAddr, mFbSize * 1 /*numbufs*/);
   if(0 != retmap) {
      LOGE("RgbPipe failed close memory map");
   }
   if(!mFd.close()) {
      LOGE("RgbPipe failed close fd");
   }
   mFBMapAddr = MAP_FAILED;
   mFbSize = 0;
   return true;
}

template <int FB>
bool RgbPipe<FB>::queueBuffer(uint32_t)
{
   const size_t offset = (mFbIndex == 0) ? 3686400 : 0;
   mVarInfo.activate = FB_ACTIVATE_VBL;
   mVarInfo.yoffset = offset / mFixInfo.line_length;
   mVarInfo.activate = FB_ACTIVATE_VBL;

   if(!mdp_wrapper::setVScreenInfo(mFd.getFD(), mVarInfo)) {
      LOGE("RgbPipe failed setVScreenInfo");
      return false;
   }
   return true;
}

template <int FB>
bool RgbPipe<FB>::dequeueBuffer(void*& buf)
{
   if(!(mFd.valid() && mFBMapAddr != MAP_FAILED)) {
      return false;
   }
   buf = (void*)(mFBbase[mFbIndex++ % utils::RGB_PIPE_NUM_BUFS]);
   return true;
}

template <int FB>
bool RgbPipe<FB>::waitForVsync()
{
   return true;
}

template <int FB>
inline utils::eOverlayPipeType RgbPipe<FB>::getOvPipeType() const {
   return utils::OV_PIPE_TYPE_RGB;
}

template <int FB>
void RgbPipe<FB>::dump() const
{
   LOGE("== Dump RgbPipe start ==");
   LOGE("flags=0x%x fbidx=%d fbsz=%d base[0]=%p base[1]=%p",
        mFlags, mFbIndex % utils::RGB_PIPE_NUM_BUFS, mFbSize,
        mFBbase[0], mFBbase[1]);
   mFd.dump();
   mdp_wrapper::dump("fixinfo", mFixInfo);
   mdp_wrapper::dump("varinfo", mVarInfo);
   LOGE("== Dump RgbPipe end ==");
}

} // overlay2
