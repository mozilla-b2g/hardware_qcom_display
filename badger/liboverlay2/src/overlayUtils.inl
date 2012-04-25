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

#include <utils/Log.h>

namespace overlay2 {

namespace utils {

   // FIXME, taken from gralloc_priv.h. Need to
   // put it back as soon as overlay2 takes place of the old one
   /* possible formats for 3D content*/
   enum {
      HAL_NO_3D                         = 0x0000,
      HAL_3D_IN_SIDE_BY_SIDE_L_R        = 0x10000,
      HAL_3D_IN_TOP_BOTTOM              = 0x20000,
      HAL_3D_IN_INTERLEAVE              = 0x40000,
      HAL_3D_IN_SIDE_BY_SIDE_R_L        = 0x80000,
      HAL_3D_OUT_SIDE_BY_SIDE           = 0x1000,
      HAL_3D_OUT_TOP_BOTTOM             = 0x2000,
      HAL_3D_OUT_INTERLEAVE             = 0x4000,
      HAL_3D_OUT_MONOSCOPIC             = 0x8000
   };

   enum { HAL_3D_OUT_SBS_MASK =
          HAL_3D_OUT_SIDE_BY_SIDE >> overlay2::utils::SHIFT_OUT_3D,
          HAL_3D_OUT_TOP_BOT_MASK =
          HAL_3D_OUT_TOP_BOTTOM >> overlay2::utils::SHIFT_OUT_3D,
          HAL_3D_OUT_INTERL_MASK =
          HAL_3D_OUT_INTERLEAVE >> overlay2::utils::SHIFT_OUT_3D,
          HAL_3D_OUT_MONOS_MASK =
          HAL_3D_OUT_MONOSCOPIC >> overlay2::utils::SHIFT_OUT_3D };

   inline Timer::Timer(const char* s) : func(s) {
      timer.start();
   }

   inline Timer::~Timer() {
      timer.stop();
      LOGE_IF(PROFILE_OVERLAY, "%s: duration %llu us", func, timer.durationUsecs());
   }

   inline bool isYuv(uint32_t format) {
        switch(format){
            case MDP_Y_CBCR_H2V1:
            case MDP_Y_CBCR_H2V2:
            case MDP_Y_CRCB_H2V2:
            case MDP_Y_CRCB_H2V2_TILE:
                return true;
            default:
                return false;
        }
        return false;
   }

   inline bool isRgb(uint32_t format) {
       switch(format) {
           case MDP_RGBA_8888:
           case MDP_BGRA_8888:
           case MDP_RGBX_8888:
           case MDP_RGB_565:
                return true;
           default:
                return false;
       }
       return false;
   }

   inline bool isValidDest(eDest dest)
   {
      if ((OV_PIPE0 & dest) ||
          (OV_PIPE1 & dest) ||
          (OV_PIPE2 & dest)) {
         return true;
      }
      return false;
   }

   inline const char* getFormatString(uint32_t format){
      static const char* const formats[] = {
         "MDP_RGB_565",
         "MDP_XRGB_8888",
         "MDP_Y_CBCR_H2V2",
         "MDP_ARGB_8888",
         "MDP_RGB_888",
         "MDP_Y_CRCB_H2V2",
         "MDP_YCRYCB_H2V1",
         "MDP_Y_CRCB_H2V1",
         "MDP_Y_CBCR_H2V1",
         "MDP_RGBA_8888",
         "MDP_BGRA_8888",
         "MDP_RGBX_8888",
         "MDP_Y_CRCB_H2V2_TILE",
         "MDP_Y_CBCR_H2V2_TILE",
         "MDP_Y_CR_CB_H2V2",
         "MDP_Y_CB_CR_H2V2",
         "MDP_IMGTYPE_LIMIT",
         "MDP_BGR_565",
         "MDP_FB_FORMAT",
         "MDP_IMGTYPE_LIMIT2"
      };
      OVASSERT(format < sizeof(formats) / sizeof(formats[0]),
               "getFormatString wrong fmt %d", format);
      return formats[format];
   }

   inline const char* getStateString(eOverlayState state){
      switch (state) {
         case OV_CLOSED:
            return "OV_CLOSED";
         case OV_2D_VIDEO_ON_PANEL:
            return "OV_2D_VIDEO_ON_PANEL";
         case OV_2D_VIDEO_ON_PANEL_TV:
            return "OV_2D_VIDEO_ON_PANEL_TV";
         case OV_3D_VIDEO_ON_2D_PANEL:
            return "OV_3D_VIDEO_ON_2D_PANEL";
         case OV_3D_VIDEO_ON_3D_PANEL:
            return "OV_3D_VIDEO_ON_3D_PANEL";
         case OV_3D_VIDEO_ON_3D_TV:
            return "OV_3D_VIDEO_ON_3D_TV";
         case OV_3D_VIDEO_ON_2D_PANEL_2D_TV:
            return "OV_3D_VIDEO_ON_2D_PANEL_2D_TV";
         case OV_UI_MIRROR:
            return "OV_UI_MIRROR";
         case OV_2D_TRUE_UI_MIRROR:
            return "OV_2D_TRUE_UI_MIRROR";
         case OV_BYPASS_1_LAYER:
            return "OV_BYPASS_1_LAYER";
         case OV_BYPASS_2_LAYER:
            return "OV_BYPASS_2_LAYER";
         case OV_BYPASS_3_LAYER:
            return "OV_BYPASS_3_LAYER";
         default:
            return "UNKNOWN_STATE";
      }
      return "BAD_STATE";
   }

   inline uint32_t getSizeByMdp(const Whf& whf) {
      Whf _whf(whf);
      int fmt = getMdpFormat(whf.format);
      OVASSERT(-1 != fmt, "getSizeByMdp error in format %d",
         whf.format);
      _whf.format = fmt;
      return getSize(_whf);
   }

   inline void Whf::dump() const {
      LOGE("== Dump WHF w=%d h=%d f=%d s=%d start/end ==",
           w, h, format, size);
   }

   inline void Dim::dump() const {
      LOGE("== Dump Dim x=%d y=%d w=%d h=%d start/end ==", x, y, w, h);
   }

   inline int getMdpOrient(eTransform rotation) {
      LOGE_IF(DEBUG_OVERLAY, "%s: rot=%d", __FUNCTION__, rotation);
      switch(rotation)
      {
      case OVERLAY_TRANSFORM_0 : return 0;
      case HAL_TRANSFORM_FLIP_V:  return MDP_FLIP_UD;
      case HAL_TRANSFORM_FLIP_H:  return MDP_FLIP_LR;
      case HAL_TRANSFORM_ROT_90:  return MDP_ROT_90;
      case HAL_TRANSFORM_ROT_90|HAL_TRANSFORM_FLIP_V:
         return MDP_ROT_90|MDP_FLIP_LR;
      case HAL_TRANSFORM_ROT_90|HAL_TRANSFORM_FLIP_H:
         return MDP_ROT_90|MDP_FLIP_UD;
      case HAL_TRANSFORM_ROT_180: return MDP_ROT_180;
      case HAL_TRANSFORM_ROT_270: return MDP_ROT_270;
      default:
         LOGE("%s: invalid rotation value (value = 0x%x",
              __FUNCTION__, rotation);
      }
      return -1;
   }

   inline int getRotOutFmt(uint32_t format) {
      switch (format) {
      case MDP_Y_CRCB_H2V2_TILE:
         return MDP_Y_CRCB_H2V2;
      case MDP_Y_CB_CR_H2V2:
         return MDP_Y_CBCR_H2V2;
      default:
         return format;
      }
      // not reached
      OVASSERT(false, "%s not reached", __FUNCTION__);
      return -1;
   }

   template<>
   struct RotOutFmt<ROT_OUT_FMT_DEFAULT>
   {
      static inline int fmt(uint32_t format) {
         return getRotOutFmt(format);
      }
   };

   template<>
   struct RotOutFmt<ROT_OUT_FMT_Y_CRCB_H2V2>
   {
      static inline int fmt(uint32_t) {
         return MDP_Y_CRCB_H2V2;
      }
   };

   inline uint32_t getColorFormat(uint32_t format)
   {
      return (format == HAL_PIXEL_FORMAT_YV12) ?
         format : colorFormat(format);
   }

   // FB0
   template <int CHAN>
   inline Dim getPosisitonS3DImpl(const Whf& whf)
   {
      switch (whf.format & OUTPUT_3D_MASK)
      {
      case HAL_3D_OUT_SBS_MASK:
         // x, y, w, h
         return Dim(0, 0, whf.w/2, whf.h);
      case HAL_3D_OUT_TOP_BOT_MASK:
         return Dim(0, 0, whf.w, whf.h/2);
      case HAL_3D_OUT_MONOS_MASK:
         return Dim();
      case HAL_3D_OUT_INTERL_MASK:
         // FIXME error?
         LOGE("%s HAL_3D_OUT_INTERLEAVE_MASK", __FUNCTION__);
         return Dim();
      default:
         LOGE("%s Unsupported 3D output format %d", __FUNCTION__,
              whf.format);
      }
      return Dim();
   }

   // FB1
   template <>
   inline Dim getPosisitonS3DImpl<utils::OV_PIPE1>(const Whf& whf)
   {
      switch (whf.format & OUTPUT_3D_MASK)
      {
      case HAL_3D_OUT_SBS_MASK:
         return Dim(whf.w/2, 0, whf.w/2, whf.h);
      case HAL_3D_OUT_TOP_BOT_MASK:
         return Dim(0, whf.h/2, whf.w, whf.h/2);
      case HAL_3D_OUT_MONOS_MASK:
         return Dim(0, 0, whf.w, whf.h);
      case HAL_3D_OUT_INTERL_MASK:
         // FIXME error?
         LOGE("%s HAL_3D_OUT_INTERLEAVE_MASK", __FUNCTION__);
         return Dim();
      default:
         LOGE("%s Unsupported 3D output format %d", __FUNCTION__,
              whf.format);
      }
      return Dim();
   }

   template <int CHAN>
   inline bool getPosisitonS3D(const Whf& whf, Dim& out) {
      out = getPosisitonS3DImpl<CHAN>(whf);
      return (out != Dim());
   }

   template <int CHAN>
   inline Dim getCropS3DImpl(const Dim& in, uint32_t fmt) {
      switch (fmt & INPUT_3D_MASK)
      {
      case HAL_3D_IN_SIDE_BY_SIDE_L_R:
         return Dim(0, 0, in.w/2, in.h);
      case HAL_3D_IN_SIDE_BY_SIDE_R_L:
         return Dim(in.w/2, 0, in.w/2, in.h);
      case HAL_3D_IN_TOP_BOTTOM:
         return Dim(0, 0, in.w, in.h/2);
      case HAL_3D_IN_INTERLEAVE:
         LOGE("%s HAL_3D_IN_INTERLEAVE", __FUNCTION__);
         break;
      default:
         LOGE("%s Unsupported 3D format %d", __FUNCTION__, fmt);
         break;
      }
      return Dim();
   }

   template <>
   inline Dim getCropS3DImpl<utils::OV_PIPE1>(const Dim& in, uint32_t fmt) {
      switch (fmt & INPUT_3D_MASK)
      {
      case HAL_3D_IN_SIDE_BY_SIDE_L_R:
         return Dim(in.w/2, 0, in.w/2, in.h);
      case HAL_3D_IN_SIDE_BY_SIDE_R_L:
         return Dim(0, 0, in.w/2, in.h);
      case HAL_3D_IN_TOP_BOTTOM:
         return Dim(0, in.h/2, in.w, in.h/2);
      case HAL_3D_IN_INTERLEAVE:
         LOGE("%s HAL_3D_IN_INTERLEAVE", __FUNCTION__);
         break;
      default:
         LOGE("%s Unsupported 3D format %d", __FUNCTION__, fmt);
         break;
      }
      return Dim();
   }

   template <int CHAN>
   inline bool getCropS3D(const Dim& in, Dim& out, uint32_t fmt)
   {
      out = getCropS3DImpl<CHAN>(in, fmt);
      return (out != Dim());
   }

   template <class Type>
   void swapWidthHeight(Type& width, Type& height) {
      Type tmp = width;
      width = height;
      height = tmp;
   }

   inline void ScreenInfo::dump(const char* const s) const {
      LOGE("== Dump %s ScreenInfo w=%d h=%d"
           " bpp=%d stride=%d start/end ==",
           s, mFBWidth, mFBHeight, mFBbpp, mFBystride);
   }

   inline void setSrcRectDim(const overlay2::utils::Dim d,
                             mdp_overlay& ov) {
      ov.src_rect.x = d.x;
      ov.src_rect.y = d.y;
      ov.src_rect.w = d.w;
      ov.src_rect.h = d.h;
   }

   inline void setDstRectDim(const overlay2::utils::Dim d,
                             mdp_overlay& ov) {
      ov.dst_rect.x = d.x;
      ov.dst_rect.y = d.y;
      ov.dst_rect.w = d.w;
      ov.dst_rect.h = d.h;
   }

   inline overlay2::utils::Whf getSrcWhf(const mdp_overlay& ov) {
      return overlay2::utils::Whf(ov.src.width,
                                  ov.src.height,
                                  ov.src.format);
   }

   inline overlay2::utils::Dim getSrcRectDim(const mdp_overlay& ov) {
      return overlay2::utils::Dim(ov.src_rect.x,
                                  ov.src_rect.y,
                                  ov.src_rect.w,
                                  ov.src_rect.h);
   }

   inline overlay2::utils::Dim getDstRectDim(const mdp_overlay& ov) {
      return overlay2::utils::Dim(ov.dst_rect.x,
                                  ov.dst_rect.y,
                                  ov.dst_rect.w,
                                  ov.dst_rect.h);
   }
}

}
