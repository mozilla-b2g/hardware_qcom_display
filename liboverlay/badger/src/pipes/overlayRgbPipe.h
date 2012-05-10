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

#ifndef OVERLAY_RGB_PIPE_H
#define OVERLAY_RGB_PIPE_H

#include "overlayUtils.h"
#include "overlayFD.h"
#include "overlayMem.h"

namespace overlay2 {

/*
 * Pan display support for primary/secondary RGB pipes
 * */
template <int FB>
class RgbPipe : utils::NoCopy {
public:
   /* ctor init */
   explicit RgbPipe();

   /* dtor close */
   ~RgbPipe();

   /* open */
   bool open();

   /* close. Not owning rotator, will not close it */
   bool close();

   /* queue buffer to the overlay (call ioctl for pan display) */
   bool queueBuffer(uint32_t offset);
   /* get a new buffer to draw on. FIXME void*  */
   bool dequeueBuffer(void*& buf);
   /* wait for vsync to be done */
   bool waitForVsync();

   /* dump the state */
   void dump() const;

   /* For RGB pan display, the below are no-op */
   bool commit() { return true; }
   void setId(int id) {}
   void setMemoryId(int id) {}
   bool setCrop(const utils::Dim& d) { return true; }
   bool setPosition(const utils::Dim& dim) { return true; }
   bool setParameter(const utils::Params& param) { return true; }
   bool setSource(const utils::Whf& whf,
                  utils::eTransform orient,
                  utils::eWait wait) { return true; }
   utils::eOverlayPipeType getOvPipeType() const;
private:
   /* is/sed closed */
   bool isClosed() const;
   bool setClosed();

   /* my flags */
   enum { CLOSED = 1<<0 };
   uint32_t mFlags;

   /* fd to the opened device */
   OvFD mFd;

   /* mem map of the fb */
   void* mFBMapAddr;

   uint32_t mFbIndex;
   uint32_t mFbSize;
   void* mFBbase[utils::RGB_PIPE_NUM_BUFS];

   /* Fix/Var info */
   fb_fix_screeninfo mFixInfo;
   fb_var_screeninfo mVarInfo;
};

}

#include "overlayRgbPipe_T.cpp"

#endif // OVERLAY_GENERIC_PIPE_H
