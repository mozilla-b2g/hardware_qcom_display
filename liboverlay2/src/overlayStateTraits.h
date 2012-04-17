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

#ifndef OVERLAY_STATE_TRAITS_H
#define OVERLAY_STATE_TRAITS_H

#include "overlayUtils.h"
#include "overlayImpl.h"
#include "overlayRotator.h"
#include "overlayGenPipe.h"
#include "pipes/overlayBypassPipe.h"
#include "pipes/overlayHdmiPipe.h"
#include "pipes/overlayS3DPrimaryPipe.h"
#include "pipes/overlayS3DExtPipe.h"
#include "pipes/overlayM3DPrimaryPipe.h"
#include "pipes/overlayM3DExtPipe.h"
#include "pipes/overlayUIMirrorPipe.h"

namespace overlay2 {

// primary has nothing
template <int STATE> struct StateTraits {};

/*
 * For 3D_xxx we need channel ID besides the FBx since
 * get crop/position 3D need that to determine pos/crop
 * info.
 * */

template <> struct StateTraits<utils::OV_2D_VIDEO_ON_PANEL>
{
   typedef utils::CtrlData<Ctrl, Data> ctrlData_t;

   typedef overlay2::GenericPipe<ctrlData_t, utils::FB0> pipe0;
   typedef overlay2::NullPipe pipe1;   // place holder
   typedef overlay2::NullPipe pipe2;   // place holder

   typedef Rotator rot0;
   typedef NullRotator rot1;
   typedef NullRotator rot2;

   typedef overlay2::OverlayImpl<pipe0> ovimpl;
};

template <> struct StateTraits<utils::OV_2D_VIDEO_ON_PANEL_TV>
{
   typedef utils::CtrlData<Ctrl, Data> ctrlData_t;

   typedef overlay2::GenericPipe<ctrlData_t, utils::FB0> pipe0;
   typedef overlay2::HdmiPipe pipe1;
   typedef overlay2::NullPipe pipe2;   // place holder

   typedef Rotator rot0;
   typedef NullRotator rot1;
   typedef NullRotator rot2;

   typedef overlay2::OverlayImpl<pipe0, pipe1> ovimpl;
};

template <> struct StateTraits<utils::OV_3D_VIDEO_ON_2D_PANEL>
{
   typedef overlay2::M3DPrimaryPipe<utils::FB0, utils::OV_PIPE0> pipe0;
   typedef overlay2::NullPipe pipe1;   // place holder
   typedef overlay2::NullPipe pipe2;   // place holder

   typedef Rotator rot0;
   typedef NullRotator rot1;
   typedef NullRotator rot2;

   typedef overlay2::OverlayImpl<pipe0> ovimpl;
};

template <> struct StateTraits<utils::OV_3D_VIDEO_ON_3D_PANEL>
{
   typedef overlay2::S3DPrimaryPipe<utils::FB0, utils::OV_PIPE0> pipe0;
   typedef overlay2::S3DPrimaryPipe<utils::FB0, utils::OV_PIPE1> pipe1;
   typedef overlay2::NullPipe pipe2;   // place holder

   typedef Rotator rot0;
   typedef Rotator rot1;
   typedef NullRotator rot2;

   typedef overlay2::OverlayImpl<pipe0, pipe1> ovimpl;
};

template <> struct StateTraits<utils::OV_3D_VIDEO_ON_3D_TV>
{
   typedef overlay2::S3DExtPipe<utils::FB1, utils::OV_PIPE0> pipe0;
   typedef overlay2::S3DExtPipe<utils::FB1, utils::OV_PIPE1> pipe1;
   typedef overlay2::NullPipe pipe2;   // place holder

   typedef NullRotator rot0;
   typedef NullRotator rot1;
   typedef NullRotator rot2;

   typedef overlay2::OverlayImpl<pipe0, pipe1> ovimpl;
};

template <> struct StateTraits<utils::OV_3D_VIDEO_ON_2D_PANEL_2D_TV>
{
   typedef overlay2::M3DPrimaryPipe<utils::FB0, utils::OV_PIPE0> pipe0;
   typedef overlay2::M3DExtPipe<utils::FB1, utils::OV_PIPE1> pipe1;
   typedef overlay2::NullPipe pipe2;   // place holder

   typedef Rotator rot0;
   typedef NullRotator rot1;
   typedef NullRotator rot2;

   typedef overlay2::OverlayImpl<pipe0, pipe1> ovimpl;
};

template <> struct StateTraits<utils::OV_UI_MIRROR>
{
   typedef overlay2::UIMirrorPipe pipe0;
   typedef overlay2::NullPipe pipe1;   // place holder
   typedef overlay2::NullPipe pipe2;   // place holder

   typedef Rotator rot0;
   typedef NullRotator rot1;
   typedef NullRotator rot2;

   typedef overlay2::OverlayImpl<pipe0> ovimpl;
};

template <> struct StateTraits<utils::OV_2D_TRUE_UI_MIRROR>
{
   typedef utils::CtrlData<Ctrl, Data> ctrlData_t;

   typedef overlay2::GenericPipe<ctrlData_t, utils::FB0> pipe0;
   typedef overlay2::HdmiPipe pipe1;
   typedef overlay2::UIMirrorPipe pipe2;

   typedef Rotator rot0;
   typedef NullRotator rot1;
   typedef Rotator rot2;

   typedef overlay2::OverlayImpl<pipe0, pipe1, pipe2> ovimpl;
};

template <> struct StateTraits<utils::OV_BYPASS_1_LAYER>
{
   typedef utils::CtrlData<Ctrl, Data> ctrlData_t;

   typedef overlay2::BypassPipe<utils::OV_MDP_PIPE_VG, utils::IS_FG_SET, utils::WAIT, utils::ZORDER_0> pipe0;
   typedef overlay2::NullPipe pipe1;   // place holder
   typedef overlay2::NullPipe pipe2;   // place holder

   typedef NullRotator rot0;
   typedef NullRotator rot1;
   typedef NullRotator rot2;

   typedef overlay2::OverlayImpl<pipe0> ovimpl;
};

template <> struct StateTraits<utils::OV_BYPASS_2_LAYER>
{
   typedef utils::CtrlData<Ctrl, Data> ctrlData_t;

   typedef overlay2::BypassPipe<utils::OV_MDP_PIPE_VG, utils::IS_FG_SET, utils::NO_WAIT, utils::ZORDER_0> pipe0;
   typedef overlay2::BypassPipe<utils::OV_MDP_PIPE_VG, utils::IS_FG_OFF, utils::WAIT, utils::ZORDER_1> pipe1;
   typedef overlay2::NullPipe pipe2;   // place holder

   typedef NullRotator rot0;
   typedef NullRotator rot1;
   typedef NullRotator rot2;

   typedef overlay2::OverlayImpl<pipe0, pipe1> ovimpl;
};

template <> struct StateTraits<utils::OV_BYPASS_3_LAYER>
{
   typedef overlay2::BypassPipe<utils::OV_MDP_PIPE_VG, utils::IS_FG_SET, utils::NO_WAIT, utils::ZORDER_0> pipe0;
   typedef overlay2::BypassPipe<utils::OV_MDP_PIPE_VG, utils::IS_FG_OFF, utils::NO_WAIT, utils::ZORDER_1> pipe1;
   typedef overlay2::BypassPipe<utils::OV_MDP_PIPE_RGB, utils::IS_FG_OFF, utils::WAIT, utils::ZORDER_2> pipe2;

   typedef NullRotator rot0;
   typedef NullRotator rot1;
   typedef NullRotator rot2;

   typedef overlay2::OverlayImpl<pipe0, pipe1, pipe2> ovimpl;
};

} // overlay2

#endif // OVERLAY_STATE_TRAITS_H
