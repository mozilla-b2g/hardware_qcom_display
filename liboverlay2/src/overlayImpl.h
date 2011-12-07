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

#ifndef OVERLAY_IMPL_H
#define OVERLAY_IMPL_H

#include "overlayUtils.h"
#include "overlayRotator.h"
#include "overlayReconf.h"

// FIXME make int to be uint32 whenever possible

namespace overlay2 {

// Interface only. No member, no definiton (except ~ which can
// also be =0 with impl in cpp)
class OverlayImplBase {
public:
   /* empty dtor. can be =0 with cpp impl*/
   virtual ~OverlayImplBase() {}

   /* Open pipe/rot for one dest */
   virtual bool openPipe(RotatorBase* rot, utils::eDest dest) = 0;

   /* Close pipe/rot for all specified dest */
   virtual bool closePipe(utils::eDest dest) = 0;

   /* Copy specified pipe/rot from ov passed in (used by state machine only) */
   virtual bool copyOvPipe(OverlayImplBase* ov, utils::eDest dest) = 0;

   /* TODO open func customized for RGBx pipes */

   /* Open all pipes
    * To open just one pipe, use openPipe()
    * */
   virtual bool open(RotatorBase* rot0,
                     RotatorBase* rot1,
                     RotatorBase* rot2) = 0;

   /* Close all pipes
    * To close just one pipe, use closePipe()
    * */
   virtual bool close() = 0;

   /*
    * Commit changes to the overlay
    * */
   virtual bool commit(utils::eDest dest = utils::OV_PIPE_ALL) = 0;

   /* Queue buffer with offset*/
   virtual bool queueBuffer(uint32_t offset,
                            utils::eDest dest = utils::OV_PIPE_ALL) = 0;

   /* For RGBx pipes, dequeue buffer (that is fb chunk)*/
   virtual bool dequeueBuffer(void*& buf,
                              utils::eDest dest = utils::OV_PIPE_ALL) = 0;

   /* Wait for vsync to be done on dest */
   virtual bool waitForVsync(utils::eDest dest = utils::OV_PIPE1) = 0;

   /* Crop existing destination using Dim coordinates */
   virtual bool setCrop(const utils::Dim& d,
                        utils::eDest dest = utils::OV_PIPE_ALL) = 0;

   /* Set new position using Dim */
   virtual bool setPosition(const utils::Dim& dim,
                            utils::eDest dest = utils::OV_PIPE_ALL) = 0;

   /* Set parameters - usually needed for Rotator, but would
    * be passed down the stack as well */
   virtual bool setParameter(const utils::Params& param,
                              utils::eDest dest = utils::OV_PIPE_ALL) = 0;

   /* Set new source including orientation */
   virtual bool setSource(const utils::PipeArgs[utils::MAX_PIPES],
                          utils::eDest dest = utils::OV_PIPE_ALL) = 0;

   /* set memory id to the underlying pipes */
   virtual void setMemoryId(int id, utils::eDest dest = utils::OV_PIPE_ALL) = 0;

   /* start reconfigure session, copying the buffer into a rotator
    * buffers. There is no "dest" here */
   virtual bool reconfigure(const utils::ReconfArgs& args) = 0;

   /* Get the overlay pipe type */
   virtual utils::eOverlayPipeType getOvPipeType(utils::eDest dest) const = 0;

   /* Dump underlying state */
   virtual void dump() const = 0;
};

class NullPipe {
public:
   /* TODO open func customized for RGBx pipes */
   bool open(RotatorBase* rot);
   bool close();
   bool start(const utils::PipeArgs& args);
   bool commit();
   bool setCrop(const utils::Dim& d);
   bool setPosition(const utils::Dim& dim);
   bool setParameter(const utils::Params& param);
   bool setSource(const utils::PipeArgs& args);
   bool queueBuffer(uint32_t offset);
   bool dequeueBuffer(void*& buf);
   bool waitForVsync();
   void setMemoryId(int id);
   utils::PipeArgs getArgs() const;
   utils::eOverlayPipeType getOvPipeType() const;
   void dump() const;
};

/*
 * Each pipe is not specific to a display (primary/external). The order in the
 * template params, will setup the priorities of the pipes.
 * */
template <class P0, class P1=NullPipe, class P2=NullPipe>
class OverlayImpl : public OverlayImplBase {
public:
   typedef P0 pipe0;
   typedef P1 pipe1;
   typedef P2 pipe2;

   /* ctor */
   OverlayImpl();
   OverlayImpl(P0* p0, P1* p1, P2* p2);

   /*
    * Comments of the below functions are the same as the one
    * in OverlayImplBase.
    * */
   virtual ~OverlayImpl();

   virtual bool openPipe(RotatorBase* rot, utils::eDest dest);
   virtual bool closePipe(utils::eDest dest);
   virtual bool copyOvPipe(OverlayImplBase* ov, utils::eDest dest);

   /* TODO open func customized for RGBx pipes */
   virtual bool open(RotatorBase* rot0,
                     RotatorBase* rot1,
                     RotatorBase* rot2);
   virtual bool close();
   virtual bool commit(utils::eDest dest = utils::OV_PIPE_ALL);
   virtual bool setCrop(const utils::Dim& d,
                        utils::eDest dest = utils::OV_PIPE_ALL);
   virtual bool setPosition(const utils::Dim& dim,
                            utils::eDest dest = utils::OV_PIPE_ALL);
   virtual bool setParameter(const utils::Params& param,
                              utils::eDest dest = utils::OV_PIPE_ALL);
   virtual bool setSource(const utils::PipeArgs[utils::MAX_PIPES],
                          utils::eDest dest = utils::OV_PIPE_ALL);
   virtual bool queueBuffer(uint32_t offset,
                            utils::eDest dest = utils::OV_PIPE_ALL);
   virtual bool dequeueBuffer(void*& buf,
                              utils::eDest dest = utils::OV_PIPE_ALL);
   virtual bool waitForVsync(utils::eDest dest = utils::OV_PIPE1);
   virtual void setMemoryId(int id, utils::eDest dest = utils::OV_PIPE_ALL);

   virtual bool reconfigure(const utils::ReconfArgs& args);

   virtual utils::eOverlayPipeType getOvPipeType(utils::eDest dest) const;

   virtual void dump() const;

private:
   /* FIXME sp<>? */
   P0* mPipe0;
   P1* mPipe1;
   P2* mPipe2;
   // More Px here in the future as needed

   /*  */

   /* Each Px has it's own Rotator here.
    * will pass rotator to the lower layer in stack
    * but only overlay is allowed to control the lifetime
    * of the rotator instace */
   RotatorBase* mRotP0;
   RotatorBase* mRotP1;
   RotatorBase* mRotP2;

   /*
    * Real rotator to act as a buffer. It is used during
    * reconfigurations in case of Netflix.
    * Should be generic to be used for any use case
    * where there is a need for a temp buffer.
    *
    * That rotator is owned by the OverlayImpl
    * and should be passed to both pipes as needed.
    * Pipes should not close/delete etc... that buffer
    * only OverlayImpl should.
    * */
   Rotator mRotBuffer;

   /* Reconfig helper class */
   OverlayReconf mReconf;
};

} // overlay2

#include "overlayImpl_T.cpp"

#endif // OVERLAY_IMPL_H
