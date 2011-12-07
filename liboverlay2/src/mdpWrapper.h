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

#ifndef MDP_WRAPPER_H
#define MDP_WRAPPER_H

/*
 * In order to make overlay2::mdp_wrapper shorter, please do something like:
 * namespace mdpwrap = overlay2::mdp_wrapper;
 * */

#include <linux/msm_mdp.h>
#include <linux/msm_rotator.h>
#include <linux/android_pmem.h>

namespace overlay2{

   namespace mdp_wrapper{
      /* FBIOGET_FSCREENINFO */
      bool getFScreenInfo(int fd, fb_fix_screeninfo& finfo);

      /* FBIOGET_VSCREENINFO */
      bool getVScreenInfo(int fd, fb_var_screeninfo& vinfo);

      /* FBIOPUT_VSCREENINFO */
      bool setVScreenInfo(int fd, fb_var_screeninfo& vinfo);

      /* MSM_ROTATOR_IOCTL_START */
      bool startRotator(int fd, msm_rotator_img_info& rot);

      /* MSM_ROTATOR_IOCTL_ROTATE */
      bool rotate(int fd, msm_rotator_data_info& rot);

      /* MSMFB_OVERLAY_SET */
      bool setOverlay(int fd, mdp_overlay& ov);

      /* MSM_ROTATOR_IOCTL_FINISH */
      bool endRotator(int fd, int sessionId);

      /* MSMFB_OVERLAY_UNSET */
      bool unsetOverlay(int fd, int ovId);

      /* MSMFB_OVERLAY_GET */
      bool getOverlay(int fd, mdp_overlay& ov);

      /* MSMFB_OVERLAY_PLAY */
      bool play(int fd, msmfb_overlay_data& od);

      /* MSMFB_OVERLAY_PLAY_WAIT */
      bool playWait(int fd, msmfb_overlay_data& od);

      /* MSMFB_OVERLAY_3D */
      bool set3D(int fd, msmfb_overlay_3d& ov);

      /* the following are helper functions for dumping
       * msm_mdp and friends*/
      void dump(const char* const s, const msmfb_overlay_data& ov);
      void dump(const char* const s, const msmfb_data& ov);
      void dump(const char* const s, const mdp_overlay& ov);
      void dump(const char* const s, const msmfb_overlay_3d& ov);
      void dump(const char* const s, const uint32_t u[], uint32_t cnt);
      void dump(const char* const s, const msmfb_img& ov);
      void dump(const char* const s, const mdp_rect& ov);

      /* and rotator */
      void dump(const char* const s, const msm_rotator_img_info& rot);
      void dump(const char* const s, const msm_rotator_data_info& rot);

      /* info */
      void dump(const char* const s, const fb_fix_screeninfo& finfo);
      void dump(const char* const s, const fb_var_screeninfo& vinfo);
   } // mdp_wrapper

} // overlay2

#include "mdpWrapper.inl"

#endif // MDP_WRAPPER_H
