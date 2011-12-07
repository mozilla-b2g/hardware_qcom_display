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

#include <utils/Log.h>
#include <errno.h>

#include "overlayUtils.h"

// FIXME missing LOG area here

namespace overlay2{

   namespace mdp_wrapper{
      inline bool getFScreenInfo(int fd, fb_fix_screeninfo& finfo) {
         if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo) == -1) {
            LOGE("Failed to call ioctl FBIOGET_FSCREENINFO err=%d", errno);
            return false;
         }
         return true;
      }

      inline bool getVScreenInfo(int fd, fb_var_screeninfo& vinfo) {
         if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
            LOGE("Failed to call ioctl FBIOGET_VSCREENINFO err=%d", errno);
            return false;
         }
         return true;
      }

      inline bool setVScreenInfo(int fd, fb_var_screeninfo& vinfo) {
         if (ioctl(fd, FBIOPUT_VSCREENINFO, &vinfo) == -1) {
            LOGE("Failed to call ioctl FBIOPUT_VSCREENINFO err=%d", errno);
            return false;
         }
         return true;
      }

      inline bool startRotator(int fd, msm_rotator_img_info& rot) {
         if (ioctl(fd, MSM_ROTATOR_IOCTL_START, &rot) == -1){
            LOGE("Failed to call ioctl MSM_ROTATOR_IOCTL_START err=%d", errno);
            return false;
         }
         return true;
      }

      inline bool rotate(int fd, msm_rotator_data_info& rot) {
         if (ioctl(fd, MSM_ROTATOR_IOCTL_ROTATE, &rot) == -1) {
            LOGE("Failed to call ioctl MSM_ROTATOR_IOCTL_ROTATE err=%d", errno);
            return false;
         }
         return true;
      }

      inline bool setOverlay(int fd, mdp_overlay& ov) {
         if (ioctl(fd, MSMFB_OVERLAY_SET, &ov) == -1) {
            LOGE("Failed to call ioctl MSMFB_OVERLAY_SET err=%d", errno);
            return false;
         }
         return true;
      }

      inline bool endRotator(int fd, int sessionId) {
         if (ioctl(fd, MSM_ROTATOR_IOCTL_FINISH, &sessionId) == -1) {
            LOGE("Failed to call ioctl MSM_ROTATOR_IOCTL_FINISH err=%d", errno);
            return false;
         }
         return true;
      }

      inline bool unsetOverlay(int fd, int ovId) {
         if (ioctl(fd, MSMFB_OVERLAY_UNSET, &ovId) == -1) {
            LOGE("Failed to call ioctl MSMFB_OVERLAY_UNSET err=%d", errno);
            return false;
         }
         return true;
      }

      inline bool getOverlay(int fd, mdp_overlay& ov) {
         if (ioctl(fd, MSMFB_OVERLAY_GET, &ov) == -1) {
            LOGE("Failed to call ioctl MSMFB_OVERLAY_GET err=%d", errno);
            return false;
         }
         return true;
      }

      inline bool play(int fd, msmfb_overlay_data& od) {
         if (ioctl(fd, MSMFB_OVERLAY_PLAY, &od) == -1) {
            LOGE("Failed to call ioctl MSMFB_OVERLAY_PLAY err=%d", errno);
            return false;
         }
         return true;
      }

      inline bool playWait(int fd, msmfb_overlay_data& od) {
         if (ioctl(fd, MSMFB_OVERLAY_PLAY_WAIT, &od) == -1) {
            LOGE("Failed to call ioctl MSMFB_OVERLAY_PLAY_WAIT err=%d", errno);
            return false;
         }
         return true;
      }

      inline bool set3D(int fd, msmfb_overlay_3d& ov) {
         if (ioctl(fd, MSMFB_OVERLAY_3D, &ov) == -1) {
            LOGE("Failed to call ioctl MSMFB_OVERLAY_3D err=%d", errno);
            return false;
         }
         return true;
      }

      /* dump funcs */
      inline void dump(const char* const s, const msmfb_overlay_data& ov) {
         LOGE("%s msmfb_overlay_data id=%d",
              s, ov.id);
         dump("data", ov.data);
      }
      inline void dump(const char* const s, const msmfb_data& ov) {
         LOGE("%s msmfb_data offset=%d memid=%d id=%d flags=0x%x priv=%d",
              s, ov.offset, ov.memory_id, ov.id, ov.flags, ov.priv);
      }
      inline void dump(const char* const s, const mdp_overlay& ov) {
         LOGE("%s mdp_overlay z=%d fg=%d alpha=%d mask=%d flags=0x%x id=%d",
              s, ov.z_order, ov.is_fg, ov.alpha,
              ov.transp_mask, ov.flags, ov.id);
         dump("src", ov.src);
         dump("src_rect", ov.src_rect);
         dump("dst_rect", ov.dst_rect);
         dump("user_data", ov.user_data,
              sizeof(ov.user_data)/sizeof(ov.user_data[0]));
      }
      inline void dump(const char* const s, const msmfb_img& ov) {
         LOGE("%s msmfb_img w=%d h=%d format=%d %s",
              s, ov.width, ov.height, ov.format,
              overlay2::utils::getFormatString(ov.format));
      }
      inline void dump(const char* const s, const mdp_rect& ov) {
         LOGE("%s mdp_rect x=%d y=%d w=%d h=%d",
              s, ov.x, ov.y, ov.w, ov.h);
      }

      inline void dump(const char* const s, const msmfb_overlay_3d& ov) {
         LOGE("%s msmfb_overlay_3d 3d=%d w=%d h=%d",
              s, ov.is_3d, ov.width, ov.height);

      }
      inline void dump(const char* const s, const uint32_t u[], uint32_t cnt) {
         LOGE("%s user_data cnt=%d", s, cnt);
         for(uint32_t i=0; i < cnt; ++i) {
            LOGE("i=%d val=%d", i, u[i]);
         }
      }
      inline void dump(const char* const s, const msm_rotator_img_info& rot) {
         LOGE("%s msm_rotator_img_info sessid=%d dstx=%d dsty=%d rot=%d, ena=%d",
              s, rot.session_id, rot.dst_x, rot.dst_y,
              rot.rotations, rot.enable);
         dump("src", rot.src);
         dump("dst", rot.dst);
         dump("src_rect", rot.src_rect);
      }
      inline void dump(const char* const s, const msm_rotator_data_info& rot) {
         LOGE("%s msm_rotator_data_info sessid=%d verkey=%d",
              s, rot.session_id, rot.version_key);
         dump("src", rot.src);
         dump("dst", rot.dst);
         dump("src_chroma", rot.src_chroma);
         dump("dst_chroma", rot.dst_chroma);
      }
      inline void dump(const char* const s, const fb_fix_screeninfo& finfo) {
         LOGE("%s fb_fix_screeninfo type=%d", s, finfo.type);
      }
      inline void dump(const char* const s, const fb_var_screeninfo& vinfo) {
         LOGE("%s fb_var_screeninfo xres=%d yres=%d",
              s, vinfo.xres, vinfo.yres);
      }

   } // mdp_wrapper

} // overlay2
