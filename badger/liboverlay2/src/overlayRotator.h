/*
 * Copyright (c) 2011, Code Aurora Forum. All rights reserved.
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

#ifndef OVERlAY_ROTATOR_H
#define OVERlAY_ROTATOR_H

#include <stdlib.h>

#include "mdpWrapper.h"

#include "overlayUtils.h"
#include "overlayFD.h"
#include "overlayMem.h"
#include "overlayMdp.h"

namespace overlay2 {

/*
 * MDP rot holds MDP's rotation related structures.
 *
 * */
class MdpRot {
public:
   /* ctor */
   explicit MdpRot();

   /* open fd for rotator. map bufs is defered */
   bool open();

   /* remap rot buffers */
   bool remap(uint32_t numbufs, const utils::PipeArgs& args);

   /* Unmap everything that is not current */
   bool unmapNonCurrent();

   /* setSource with a context of reconf */
   bool setSourceReconf(const utils::PipeArgs& args);

   /* close fd, mem */
   bool close();

   /* reset underlying data, basically memset 0 */
   void reset();

   /* calls underlying wrappers to start rotator */
   bool start();

   /* start underlying but use given whf and flags */
   bool start(const utils::PipeArgs& args);

   /* start underlying but use given whf and flags.
    * Has the ability to parameterize the dst fmt */
   template <int ROT_OUT_FMT>
   bool start(const utils::PipeArgs& args);

   /* assign memory id to mdp structure */
   void setDataMemId(int fd);
   void setRotDataSrcMemId(int fd);

   /* get dst (for offset and memory id) non-virt */
   int getDstMemId() const;
   uint32_t getDstOffset() const;

   /* set enable/disable flag */
   void setEnable();
   void setDisable();
   bool enabled() const;

   /* set rotator flag*/
   void setRotations(uint32_t r);

   /* set the req data id in mData */
   void setDataReqId(int id);

   /* swap rot info dst w/h */
   void swapDstWH();

   /* returns a copy of src whf */
   utils::Whf getSrcWhf() const;

   /* setup rotator data before queue buf calls
    * call play if rotate call succeed. return false if failed */
   bool prepareQueueBuf(uint32_t offset);

   /* call play on mdp*/
   bool play(int fd);

   /* set src whf */
   void setSrcWhf(const utils::Whf& whf);

   /* returns rotator session id */
   int getSessId() const;

   /* dump the state of the object */
   void dump() const;

private:
   /* open pmem with specific src */
   bool open_i(uint32_t numbufs, uint32_t bufsz, utils::ePmemSource p);

   /* max buf no for offset */
   enum { ROT_MAX_BUF_OFFSET = 2 };
   /* rot info*/
   msm_rotator_img_info mRotImgInfo;
   /* rot data */
   msm_rotator_data_info mRotDataInfo;
   /* data needed for rotator */
   msmfb_overlay_data mData;
   /* rotator fd */
   OvFD mFd;
   /* Array of memory map for rotator (smi/adsp)
    * The array enable us to change rot buffers/mapping
    * on the fly*/
   struct RotMem {
      enum {MAX_ROT_MEM = 2};
      struct Mem {
         Mem() : mCurrOffset(0) {utils::memset0(mRotOffset); }
         bool valid() { return m.valid(); }
         bool close() { return m.close(); }
         uint32_t size() const { return m.bufSz(); }
         /* rotator data info dst offset */
         uint32_t mRotOffset[ROT_MAX_BUF_OFFSET];
         /* current offset slot from mRotOffset */
         uint32_t mCurrOffset;
         OvMem m;
      };
      RotMem() : _curr(0) {}
      Mem& curr() { return m[_curr % MAX_ROT_MEM]; }
      const Mem& curr() const { return m[_curr % MAX_ROT_MEM]; }
      Mem& prev() { return m[(_curr+1) % MAX_ROT_MEM]; }
      RotMem& operator++() { ++_curr; return *this; }
      bool close();
      uint32_t _curr;
      Mem m[MAX_ROT_MEM];
   } mMem;
};

/*
 * RotatorBase. No memebers, just interface.
 * ~ can also be =0 with empty impl in cpp.
 * */
class RotatorBase {
public:
   /* Most of the below are No op funcs for RotatorBase */
   virtual ~RotatorBase() {}
   virtual bool open() = 0;
   virtual bool remap(uint32_t numbufs, const utils::PipeArgs& args) = 0;
   virtual bool close() = 0;
   virtual bool start(const utils::PipeArgs& args) = 0;
   virtual bool start() = 0;
   virtual mdp_overlay setInfo(const utils::PipeArgs& args,
                               const mdp_overlay& o) = 0;
   virtual bool overlayTransform(MdpCtrl& mdp,
                                 utils::eTransform& rot) = 0;
   virtual void setSrcWhf(const utils::Whf& wfh) = 0;
   virtual utils::Whf getSrcWhf() const = 0;
   virtual void setRotations(uint32_t r) = 0;
   virtual void setDataReqId(int id) = 0;
   virtual bool prepareQueueBuf(uint32_t offset) = 0;
   virtual bool setSourceReconf(const utils::PipeArgs& args) = 0;
   virtual bool play(int fd) = 0;
   virtual void setEnable() = 0;
   virtual void setDisable() = 0;
   virtual bool enabled() const = 0;
   virtual void setDataMemId(int fd) = 0;
   virtual void setRotDataSrcMemId(int fd) = 0;
   virtual int getSessId() const = 0;
   virtual void dump() const = 0;
};

/*
 * Null/Empty impl of RotatorBase
 * */
class NullRotator : public RotatorBase {
public:
   /* Most of the below are No op funcs for RotatorBase */
   virtual ~NullRotator();
   virtual bool open();
   virtual bool remap(uint32_t numbufs, const utils::PipeArgs& args);
   virtual bool close();
   virtual bool start(const utils::PipeArgs& args);
   virtual bool start();
   /* null rotator behavior should set info in a specific way */
   virtual mdp_overlay setInfo(const utils::PipeArgs& args,
                               const mdp_overlay& o);
   virtual bool overlayTransform(MdpCtrl& o,
                                 utils::eTransform& rot);
   virtual void setSrcWhf(const utils::Whf& wfh);
   virtual utils::Whf getSrcWhf() const;
   virtual void setRotations(uint32_t r);
   virtual void setDataReqId(int id);
   virtual bool prepareQueueBuf(uint32_t offset);
   virtual bool setSourceReconf(const utils::PipeArgs& args);
   virtual bool play(int fd);
   virtual void setEnable();
   virtual void setDisable();
   virtual bool enabled () const;
   virtual void setDataMemId(int fd);
   virtual void setRotDataSrcMemId(int fd);
   virtual int getSessId() const;
   virtual void dump() const;
};


/*
 * Rotator impl.
 * */
class Rotator : public RotatorBase
{
public:
   /* construct underlying object */
   explicit Rotator();

   /* close underlying rot */
   virtual ~Rotator();

   /* calls underlying open */
   virtual bool open();

   /* remap rot buffers */
   virtual bool remap(uint32_t numbufs, const utils::PipeArgs& args);

   /* calls underlying close */
   virtual bool close();

   /* calls underlying  start */
   virtual bool start();

   /* calls underlying start with whf and flags */
   virtual bool start(const utils::PipeArgs& args);

   /* non virtual - calls underlying start with whf and flags.
    * Has the ability to parameterize the dst */
   template <int ROT_OUT_FMT>
   bool start(const utils::PipeArgs& args);

   /* Unmap everything that is not current */
   bool unmapNonCurrent();

   /* set info using whf and given mdp */
   virtual mdp_overlay setInfo(const utils::PipeArgs& args,
                               const mdp_overlay& o);

   /* transform function for the MDP  */
   virtual bool overlayTransform(MdpCtrl& mdp,
                                 utils::eTransform& rot);

   /* set src whf */
   virtual void setSrcWhf(const utils::Whf& wfh);

   /* set Rotations */
   virtual void setRotations(uint32_t r);

   /* set the req data id in mData */
   virtual void setDataReqId(int id);

   /* set memory_id */
   virtual void setDataMemId(int fd);
   virtual void setRotDataSrcMemId(int fd);

   /* get dst (for offset and memory id) non-virt */
   int getDstMemId() const;
   uint32_t getDstOffset() const;

   /* set enable/disable flag */
   virtual void setEnable();
   virtual void setDisable();
   virtual bool enabled () const;

   /* return rotator sess id */
   virtual int getSessId() const;

   /* return a copy of src whf*/
   virtual utils::Whf getSrcWhf() const;

   /* prepare rot for queue buf*/
   virtual bool prepareQueueBuf(uint32_t offset);

   /* setSource with a reconf context */
   virtual bool setSourceReconf(const utils::PipeArgs& args);

   /* call play on mdp*/
   virtual bool play(int fd);

   /* dump the state of the object */
   virtual void dump() const;
private:
   /* helper functions for overlayTransform */
   void overlayTransFlipHV(MdpCtrl& mdp,
                           utils::eTransform& rot);
   void overlayTransFlipRot90(MdpCtrl& mdp,
                              utils::eTransform& rot);
   void overlayTransFlipRot180(MdpCtrl& mdp);
   void overlayTransFlipRot270(MdpCtrl& mdp);

   /* underlyig rotator MDP object */
   MdpRot mRot;
};

} // overlay2

#include "overlayRotator.inl"

#endif // OVERlAY_ROTATOR_H
