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

#ifndef OVERLAY_GENERIC_PIPE_H
#define OVERLAY_GENERIC_PIPE_H

#include "overlayUtils.h"
#include "overlayRotator.h"

// FIXME make int to be uint32 whenever possible

namespace overlay2 {

template <class CTRL_DATA, int FB>
class GenericPipe : utils::NoCopy {
public:
   /* ctor init */
   explicit GenericPipe();

   /* dtor close */
   ~GenericPipe();

   /* CTRL/DATA/ROT open */
   bool open(RotatorBase* rot);

   /* CTRL/DATA close. Not owning rotator, will not close it */
   bool close();

   /* commit changes to the overlay "set"*/
   bool commit();

   /* "Data" related interface */

   /* set ID directly to data channel */
   void setId(int id);

   /* Set FD / memid */
   void setMemoryId(int id);

   /* queue buffer to the overlay */
   bool queueBuffer(uint32_t offset);

   /* dequeue buffer to the overlay NOTSUPPORTED */
   bool dequeueBuffer(void*& buf);

   /* wait for vsync to be done */
   bool waitForVsync();

   /* set crop data FIXME setROI (Region Of Intrest) */
   bool setCrop(const utils::Dim& d);

   /* "Ctrl" related interface */

   /*
    * Start a session, opens the rotator
    * FIXME, we might want to open the rotator separately
    */
   bool start(const utils::PipeArgs& args);

   /* set mdp posision using dim */
   bool setPosition(const utils::Dim& dim);

   /* set param using Params (param,value pair) */
   bool setParameter(const utils::Params& param);

   /* set source using whf, orient and wait flag */
   bool setSource(const utils::PipeArgs& args);

   /* return cached startup args */
   const utils::PipeArgs& getArgs() const;

   /* retrieve screen info */
   utils::ScreenInfo getScreenInfo() const;

   /* retrieve cached crop data */
   utils::Dim getCrop() const;

   /* return aspect ratio from ctrl data */
   utils::Dim getAspectRatio(const utils::Whf& whf) const;

   /* return aspect ratio from ctrl data for true UI mirroring */
   utils::Dim getAspectRatio(const utils::Dim& dim) const;

   /* is closed */
   bool isClosed() const;

   /* is open */
   bool isOpen() const;

   /* return Ctrl fd. Used for S3D */
   int getCtrlFd() const;

   /* Get the overlay pipe type */
   utils::eOverlayPipeType getOvPipeType() const;

   /* dump the state of the object */
   void dump() const;
private:
   /* set Closed channel */
   bool setClosed();
   // kick off rotator.
   bool startRotator();

   /* Ctrl/Data aggregator */
   CTRL_DATA mCtrlData;

   /* caching startup params. useful when need
    * to have the exact copy of that pipe.
    * For example when HDMI is connected, and we would
    * like to open/start the pipe with the args */
   utils::PipeArgs mArgs;

   /* rotator mdp base
    * Can point to NullRotator or to Rotator*/
   RotatorBase* mRot;

   /* my flags */
   enum { CLOSED = 1<<0 };
   uint32_t mFlags;
};

}

#include "overlayGenPipe_T.cpp"

#endif // OVERLAY_GENERIC_PIPE_H
