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

#ifndef OVERLAY_MDP_H
#define OVERLAY_MDP_H

#include "overlayUtils.h"
#include "overlayFD.h"

namespace overlay2{

class RotatorBase;

/*
 * Mdp Ctrl holds corresponding fd and MDP related struct.
 * It is simple wrapper to MDP services
 * */
class MdpCtrl {
public:
   /* ctor reset */
   explicit MdpCtrl();

   /* dtor close */
   ~MdpCtrl();

   /* Open underlying device using fbnum */
   bool open(uint32_t fbnum);

   /* unset overlay, reset and close fd */
   bool close();

   /* reset and set ov id to -1*/
   void reset();

   /* get orient / user_data[0] */
   int getOrient() const;

   /* returns session id */
   int getId() const;

   /* returns the fd associated to ctrl*/
   int getFd() const;

   /* Get screen info. out: info*/
   bool getScreenInfo(utils::ScreenInfo& info);

   /* overlay get */
   bool get();

   /* returns flags from mdp structure.
    * Flags are WAIT/NOWAIT/PIPE SHARED*/
   int getFlags() const;

   /* set flags to mdp structure */
   void setFlags(int f);

   /* code taken from OverlayControlChannel::setOverlayInformation  */
   bool setInfo(RotatorBase* r,
                const utils::PipeArgs& args,
                const utils::ScreenInfo& info);

   /* given whf, update src */
   void updateSource(RotatorBase* r,
                     const utils::PipeArgs& args,
                     const utils::ScreenInfo& info);
   
   /* Special source setting for reconf buffers */
   bool setSourceReconf(RotatorBase* r,
                        const utils::PipeArgs& args,
                        const utils::ScreenInfo& info);

   /* set z order */
   void setZ(utils::eZorder z);

   /* set Wait/nowait */
   void setWait(utils::eWait wait);

   /* set isFg flag */
   void setIsFg(utils::eIsFg isFg);

   /* calls overlay set
    * Set would always consult last good known ov instance.
    * Only if it is different, set would actually exectue ioctl.
    * On a sucess ioctl. last good known ov instance is updated */
   bool set();

   /* return a copy of src whf*/
   utils::Whf getSrcWhf() const;

   /* set src whf */
   void setSrcWhf(const utils::Whf& whf);

   /* set source format based on rot info */
   void setSrcFormat(const utils::Whf& whf);

   /* swap src w/h*/
   void swapSrcWH();

   /* swap src rect w/h */
   void swapSrcRectWH();

   /* returns a copy to src rect dim */
   utils::Dim getSrcRectDim() const;

   /* set src/dst rect dim */
   void setSrcRectDim(const utils::Dim d);
   void setDstRectDim(const utils::Dim d);

   /* returns a copy ro dst rect dim */
   utils::Dim getDstRectDim() const;

   /* returns user_data[0]*/
   int getUserData() const;

   /* sets user_data[0] */
   void setUserData(int v);

   /* return true if current overlay is different
    * than lask known good overlay */
   bool ovChanged() const;

   /* save mOVInfo to be last known good ov*/
   void save();

   /* restore last known good ov to be the current */
   void restore();

   /*
    * Sets ROI, the unpadded region, for source buffer.
    * Should be called before a setPosition, for small clips.
    * Dim - ROI dimensions.
    */
   bool setCrop(const utils::Dim& d);

   /* given a dim and w/h, set overlay dim */
   bool setPosition(const utils::Dim& dim, int w, int h);

   /* using user_data, sets/unsets roationvalue in mdp flags */
   void setRotationFlags();

   /* dump state of the object */
   void dump() const;
private:

   /* last good known ov info */
   mdp_overlay   mLkgo;

   /* Actual overlay mdp structure */
   mdp_overlay   mOVInfo;

   /* FD for the mdp fbnum */
   OvFD          mFd;

   /* cached size FIXME do we need it? */
   uint32_t mSize;
};


/* MDP 3D related ctrl */
class MdpCtrl3D {
public:
   /* ctor reset data */
   MdpCtrl3D();
   /* calls MSMFB_OVERLAY_3D */
   bool close();
   /* set w/h. format is ignored*/
   void setWh(const utils::Whf& whf);
   /* set is_3d calls MSMFB_OVERLAY_3D */
   bool useVirtualFB();
   /* set fd to be used in ioctl */
   void setFd(int fd);
   /* dump */
   void dump() const;
private:
   /* reset */
   void reset();
   /* actual MSM 3D info */
   msmfb_overlay_3d m3DOVInfo;
   /* FD for the mdp 3D */
   OvFD mFd;
};

/* MDP data */
class MdpData {
public:
   /* ctor reset data */
   explicit MdpData();

   /* dtor close*/
   ~MdpData();

   /* open FD */
   bool open(uint32_t fbnum);

   /* memset0 the underlying mdp object */
   void reset();

   /* close fd, and reset */
   bool close();

   /* Set FD / memid */
   void setMemoryId(int id);

   /* set id of mdp data */
   void setId(int id);

   /* return ses id of data */
   int getId() const;

   /* get underlying fd*/
   int getFd() const;

   /* get memory_id */
   int getMemoryId() const;

   /* set offset in underlying mdp obj */
   void setOffset(uint32_t o);

   /* calls wrapper play */
   bool play();

   /* calls wrapper playWait */
   bool playWait();

   /* dump state of the object */
   void dump() const;
private:

   /* actual overlay mdp data */
   msmfb_overlay_data mOvData;

   /* fd to mdp fbnum */
   OvFD mFd;
};

} // overlay2

#include "overlayMdp.inl"

#endif // OVERLAY_MDP_H
