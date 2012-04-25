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

#ifndef OVERLAY_UTILS_H
#define OVERLAY_UTILS_H

#include <stdlib.h>
#include <sys/types.h>
#include <linux/msm_mdp.h> // MDP_OV_PLAY_NOWAIT etc ...
#include <hardware/hardware.h>
#include <hardware/gralloc.h> // buffer_handle_t
#include <utils/Timers.h>
#include <cutils/log.h> // LOGE, etc
#include <fcntl.h> // open, O_RDWR, etc
#include <linux/android_pmem.h>

#include <string.h>

/*
 *
 * Collection of utilities functions/structs/enums etc...
 *
 * */

// comment that out if you want to remove asserts
// or put it as -D in Android.mk. your choice.
#define OVERLAY_HAS_ASSERT

#ifdef OVERLAY_HAS_ASSERT
# define OVASSERT(x, ...) if(!(x)) { LOGE(__VA_ARGS__); abort(); }
#else
# define OVASSERT(x, ...) LOGE_IF(!(x), __VA_ARGS__)
#endif // OVERLAY_HAS_ASSERT

#define DEBUG_OVERLAY 0
#define PROFILE_OVERLAY 0

namespace overlay2 {

// fwd
class Overlay;

namespace utils {
   struct Whf;
   struct Dim;
   template <class T>
   inline void even_out(T& x) { if (x & 0x0001) --x; }

   inline uint32_t getBit(uint32_t x, uint32_t mask) {
      return (x & mask);
   }

   inline uint32_t setBit(uint32_t x, uint32_t mask) {
      return (x | mask);
   }

   inline uint32_t clrBit(uint32_t x, uint32_t mask) {
      return (x & ~mask);
   }

   /*
    * Used to profile length of runtime for a function
    *
    * Usage:
    *    utils::Timer t("foo");
    */
   struct Timer {
      Timer(const char *s);
      ~Timer();
      android::DurationTimer timer;
      const char* const func;
   };

   /* Utility class to help avoid copying instances by making the copy ctor
    * and assignment operator private
    *
    * Usage: 
    *    class SomeClass : utils::NoCopy {...};
    */
   class NoCopy {
      protected:
         NoCopy(){}
         ~NoCopy() {}
      private:
         NoCopy(const NoCopy&);
         const NoCopy& operator=(const NoCopy&);
   };

   /*
    * Utility class to query the framebuffer info for primary display
    *
    * Usage:
    *    Outside of functions:
    *       utils::FrameBufferInfo* utils::FrameBufferInfo::sFBInfoInstance = 0;
    *    Inside functions:
    *       utils::FrameBufferInfo::getInstance()->supportTrueMirroring()
    */
   class FrameBufferInfo {

      public:
         /* ctor init */
         explicit FrameBufferInfo();

         /* Gets an instance if one does not already exist */
         static FrameBufferInfo* getInstance();

         /* Gets width of primary framebuffer */
         int getWidth() const;

         /* Gets height of primary framebuffer */
         int getHeight() const;

         /* Indicates whether true mirroring is supported */
         bool supportTrueMirroring() const;

      private:
         int mFBWidth;
         int mFBHeight;
         bool mBorderFillSupported;
         static FrameBufferInfo *sFBInfoInstance;
   };

   /* 3D related utils, defines etc...
    * The compound format passed to the overlay is
    * ABCCC where A is the input 3D format
    * B is the output 3D format
    * CCC is the color format e.g YCbCr420SP YCrCb420SP etc */
   enum { SHIFT_OUT_3D = 12,
          SHIFT_TOT_3D = 16 };
   enum { INPUT_3D_MASK = 0xFFFF0000,
          OUTPUT_3D_MASK = 0x0000FFFF };
   enum { BARRIER_LAND = 1,
          BARRIER_PORT = 2 };

   inline uint32_t format3D(uint32_t x) { return x & 0xFF000; }
   inline uint32_t colorFormat(uint32_t x) { return x & 0xFFF; }
   inline uint32_t format3DOutput(uint32_t x) {
      return (x & 0xF000) >> SHIFT_OUT_3D; }
   inline uint32_t format3DInput(uint32_t x) { return x & 0xF0000; }
   uint32_t getColorFormat(uint32_t format);

   bool isHDMIConnected ();
   bool is3DTV();
   bool isPanel3D();
   bool usePanel3D();
   bool send3DInfoPacket (uint32_t fmt);
   bool enableBarrier (uint32_t orientation);
   uint32_t getS3DFormat(uint32_t fmt);
   template <int CHAN>
   bool getPosisitonS3D(const Whf& whf, Dim& out);
   template <int CHAN>
   bool getCropS3D(const Dim& in, Dim& out, uint32_t fmt);
   template <class Type>
   void swapWidthHeight(Type& width, Type& height);

   struct Dim {
      Dim () : x(0), y(0),
               w(0), h(0),
               o(0) {}
      Dim(uint32_t _x, uint32_t _y, uint32_t _w, uint32_t _h) :
         x(_x), y(_y),
         w(_w), h(_h) {}
      Dim(uint32_t _x, uint32_t _y, uint32_t _w, uint32_t _h, uint32_t _o) :
         x(_x), y(_y),
         w(_w), h(_h),
         o(_o) {}
      bool check(uint32_t _w, uint32_t _h) const {
         return (x+w <= _w && y+h <= _h);

      }

      bool operator==(const Dim& d) const {
         return d.x == x && d.y == y &&
                d.w == w && d.h == h &&
                d.o == o;
      }

      bool operator!=(const Dim& d) const {
         return !operator==(d);
      }

      void even_out() {
         utils::even_out(x);
         utils::even_out(y);
         utils::even_out(w);
         utils::even_out(h);
      }

      void dump() const;
      uint32_t x;
      uint32_t y;
      uint32_t w;
      uint32_t h;
      uint32_t o;
   };

   // TODO have Whfz

   struct Whf {
      Whf() : w(0), h(0), format(0), size(0) {}
      Whf(uint32_t wi, uint32_t he, uint32_t f) :
         w(wi), h(he), format(f), size(0) {}
      Whf(uint32_t wi, uint32_t he, uint32_t f, uint32_t s) :
         w(wi), h(he), format(f), size(s) {}
      // FIXME not comparing size at the moment
      bool operator==(const Whf& whf) const {
         return whf.w == w && whf.h == h &&
            whf.format == format;
      }
      bool operator!=(const Whf& whf) const {
         return !operator==(whf);
      }
      void dump() const;
      uint32_t w;
      uint32_t h;
      // FIXME need to be int32_t ?
      uint32_t format;
      uint32_t size;
   };

   enum { MAX_PATH_LEN = 256 };

   enum eParams {
      OVERLAY_DITHER,
      OVERLAY_TRANSFORM,
      OVERLAY_TRANSFORM_UI
   };

   struct Params{
      Params(eParams p, int v) : param(p), value(v) {}
      eParams param;
      int value;
   };


   /**
    * Rotator flags: not to be confused with orientation flags.
    * Ususally, you want to open the rotator to make sure it is
    * ready for business.
    * ROT_FLAG_DISABLED: Rotator would not kick in. (ioctl will emit errors).
    * ROT_FLAG_ENABLED: and when rotation is needed.
    *                   (prim video playback)
    *                   (UI mirroring on HDMI w/ 0 degree rotator. - just memcpy)
    * In the original code, uichannel bool is used for allocating
    * rotator buffers from pmem smi OR adsp pool.
    * uichannel is true - adsp pmem. (UI mirroring)
    * false - smi pmem (regular video playback e.g.).
    * For video and camera, rotator memory is used from adsp.
    * For UI mirroing it is allocated from adsp pool.
    * In HDMI UI mirroring, rotator is always used.
    * Even when w/o orienation change on primary,
    * we do 0 rotation on HDMI and using rotator buffers.
    * That is because we might see tearing otherwise. so
    * we use another buffer (rotator, adsp pmem).
    * When a simple video playback on HDMI, no rotator is being used.(null r).
    * */
   enum eRotFlags {
      ROT_FLAG_DISABLED = 0,
      ROT_FLAG_ENABLED = 1 // needed in rot
   };

   /* Used for rotator open.
    * FIXME that is default, might be configs */
   enum { ROT_NUM_BUFS = 2 };

   /* Would be used for Rotator pmem src */
   enum ePmemSource {
      PMEM_SRC_ADSP,
      PMEM_SRC_SMI
   };

   /* Wait/No wait for waiting for vsync
    * WAIT - wait for vsync, ignore fb (no need to compose w/ fb)
    * NO_WAIT - do not wait for vsync and return immediatly since
    * we need to run composition code */
   enum eWait {
      WAIT,
      NO_WAIT
   };

   /* The values for is_fg flag for control alpha and transp
    * IS_FG_OFF means is_fg = 0
    * IS_FG_SET means is_fg = 1
    */
   enum eIsFg {
      IS_FG_OFF = 0,
      IS_FG_SET = 1
   };

   /*
    * Various mdp flags like PIPE SHARE, DEINTERLACE etc...
    * kernel/common/linux/msm_mdp.h
    * INTERLACE_MASK: hardware/qcom/display/libgralloc/gralloc_priv.h
    * */
   enum eMdpFlags {
      OV_MDP_FLAGS_NONE = 0,
      OV_MDP_PIPE_SHARE =  MDP_OV_PIPE_SHARE,
      OV_MDP_DEINTERLACE = MDP_DEINTERLACE,
      OV_MDP_PLAY_NOWAIT = MDP_OV_PLAY_NOWAIT,
      OV_MDP_SECURE_OVERLAY_SESSION = MDP_SECURE_OVERLAY_SESSION
   };

   /* simple tuple to hold both ctrl/data */
   template <class CTRL, class DATA>
   struct CtrlData {
      CTRL ctrl;
      DATA data;
   };

   enum eOverlayPipeType {
      OV_PIPE_TYPE_NULL,
      OV_PIPE_TYPE_BYPASS,
      OV_PIPE_TYPE_GENERIC,
      OV_PIPE_TYPE_HDMI,
      OV_PIPE_TYPE_M3D_EXTERNAL,
      OV_PIPE_TYPE_M3D_PRIMARY,
      OV_PIPE_TYPE_RGB,
      OV_PIPE_TYPE_S3D_EXTERNAL,
      OV_PIPE_TYPE_S3D_PRIMARY,
      OV_PIPE_TYPE_UI_MIRROR
   };

   enum eZorder {
      ZORDER_0,
      ZORDER_1,
      ZORDER_2,
      Z_SYSTEM_ALLOC = 0xFFFF
   };

   enum eMdpPipeType {
      OV_MDP_PIPE_RGB,
      OV_MDP_PIPE_VG
   };

   /* Corresponds to pipes in eDest */
   enum eChannel {
      CHANNEL_0,
      CHANNEL_1,
      CHANNEL_2
   };
  
   // Max pipes via overlay (VG0, VG1, RGB1)
   enum { MAX_PIPES = 3 };

   /* Used to identify destination channels and
    * also 3D channels e.g. when in 3D mode with 2
    * pipes opened and it is used in get crop/pos 3D
    *
    * PLEASE NOTE : DO NOT USE eDest FOR ARRAYS
    * i.e. args[OV_PIPE1] since it is a BIT MASK
    * use CHANNELS enum instead. Each OV_PIPEX is
    * not specific to a display (primary/external).
    * */
     enum eDest {
      OV_PIPE0 = 1 << 0,
      OV_PIPE1 = 1 << 1,
      OV_PIPE2 = 1 << 2,
      OV_PIPE_ALL  = (OV_PIPE0 | OV_PIPE1 | OV_PIPE2)
   };

   /* values for copybit_set_parameter(OVERLAY_TRANSFORM) */
   enum eTransform {
      /* No rot */
      OVERLAY_TRANSFORM_0         = 0x0,
      /* flip source image horizontally */
      OVERLAY_TRANSFORM_FLIP_H    = HAL_TRANSFORM_FLIP_H,
      /* flip source image vertically */
      OVERLAY_TRANSFORM_FLIP_V    = HAL_TRANSFORM_FLIP_V,
      /* rotate source image 90 degrees */
      OVERLAY_TRANSFORM_ROT_90    = HAL_TRANSFORM_ROT_90,
      /* rotate source image 180 degrees
       * It is basically bit-or-ed  H | V == 0x3 */
      OVERLAY_TRANSFORM_ROT_180   = HAL_TRANSFORM_ROT_180,
      /* rotate source image 270 degrees
       * Basically 180 | 90 == 0x7 */
      OVERLAY_TRANSFORM_ROT_270   = HAL_TRANSFORM_ROT_270,
      /* rotate invalid like in Transform.h */
      OVERLAY_TRANSFORM_INV       = 0x80
   };

   /* start/stop reconfigure */
   enum eReconfigure {
      RECONFIG_ON,
      RECONFIG_OFF
   };

   /* offset and fd are play info */
   struct PlayInfo {
      PlayInfo() : fd(-1), offset(0) {}
      PlayInfo(int _fd, uint32_t _offset) :
         fd(_fd), offset(_offset) {}
      bool operator==(const PlayInfo& p) {
         return (fd == p.fd && offset == p.offset);
      }
      int fd;
      uint32_t offset;
   };

   /* Used when needed by Rotator buffer for stuff
    * like reconf */
   struct ReconfArgs {
      ReconfArgs() : orientation(OVERLAY_TRANSFORM_0),
                     reconf(RECONFIG_OFF) {}
      ReconfArgs(const Whf& _whf,
                 const Dim& _crop,
                 const Dim& _pos,
                 const PlayInfo& _play,
                 eTransform o,
                 eReconfigure r) :
         whf(_whf), crop(_crop),
         pos(_pos), play(_play),
         orientation(o), reconf(r) {}
      bool operator==(const ReconfArgs& args) {
         return (whf == args.whf &&
                 crop == args.crop &&
                 pos == args.pos &&
                 play == args.play &&
                 orientation == args.orientation &&
                 reconf == args.reconf);
      }
      void reset() {
         whf = Whf();
         crop = Dim();
         pos = Dim();
         play = PlayInfo();
         orientation = OVERLAY_TRANSFORM_0;
         reconf = RECONFIG_OFF;
      }
      Whf whf;
      Dim crop;
      Dim pos;
      PlayInfo play;
      eTransform orientation;
      eReconfigure reconf;
   };

   // Used to consolidate pipe params
   struct PipeArgs {
      PipeArgs() : mdpFlags(OV_MDP_FLAGS_NONE),
                   orientation(OVERLAY_TRANSFORM_0),
                   wait(NO_WAIT),
                   zorder(Z_SYSTEM_ALLOC),
                   isFg(IS_FG_OFF),
                   rotFlags(ROT_FLAG_DISABLED),
                   pmemsrc(PMEM_SRC_SMI),
                   reconf(RECONFIG_OFF)
         {}
      PipeArgs(eMdpFlags f, eTransform o,
               Whf _whf, eWait w,
               eZorder z, eIsFg fg, eRotFlags r,
               ePmemSource p, eReconfigure rcfg) :
         mdpFlags(f),
         orientation(o),
         whf(_whf),
         wait(w),
         zorder(z),
         isFg(fg),
         rotFlags(r),
         pmemsrc(p),
         reconf(rcfg){}
      PipeArgs(const ReconfArgs& r) : mdpFlags(OV_MDP_FLAGS_NONE),
                                      orientation(r.orientation),
                                      whf(r.whf),
                                      wait(NO_WAIT),
                                      zorder(Z_SYSTEM_ALLOC),
                                      isFg(IS_FG_OFF),
                                      rotFlags(ROT_FLAG_DISABLED),
                                      pmemsrc(PMEM_SRC_SMI),
                                      reconf(RECONFIG_OFF) {}
      PipeArgs& operator=(const ReconfArgs& r) {
         orientation = r.orientation;
         whf = r.whf;
         reconf = r.reconf;
         play = r.play;
         return *this;
      }
      eMdpFlags mdpFlags; // for mdp_overlay flags PIPE_SHARE, NO_WAIT, etc
      eTransform orientation; // FIXME docs
      Whf whf;
      eWait wait; // flags WAIT/NO_WAIT
      eZorder zorder; // stage number
      eIsFg isFg; // control alpha & transp
      eRotFlags rotFlags;
      ePmemSource pmemsrc; // pmem src, SMI def
      eReconfigure reconf; // reconfigure buffers state
      PlayInfo play;
   };

   enum eOverlayState{
      /* No pipes from overlay open */
      OV_CLOSED = 0,

      /* 2D Video */
      OV_2D_VIDEO_ON_PANEL,
      OV_2D_VIDEO_ON_PANEL_TV,

      /* 3D Video on one display (panel or TV) */
      OV_3D_VIDEO_ON_2D_PANEL,
      OV_3D_VIDEO_ON_3D_PANEL,
      OV_3D_VIDEO_ON_3D_TV,

      /* 3D Video on two displays (panel and TV) */
      OV_3D_VIDEO_ON_2D_PANEL_2D_TV,

      /* UI Mirroring */
      OV_UI_MIRROR,
      OV_2D_TRUE_UI_MIRROR,
      OV_M3D_TRUE_UI_MIRROR,  // Not yet supported

      /* Composition Bypass */
      OV_BYPASS_1_LAYER,
      OV_BYPASS_2_LAYER,
      OV_BYPASS_3_LAYER,
   };

   inline void setMdpFlags(eMdpFlags& f, eMdpFlags v) {
      f = static_cast<eMdpFlags>(setBit(f, v));
   }

   inline void clearMdpFlags(eMdpFlags& f, eMdpFlags v) {
      f = static_cast<eMdpFlags>(clrBit(f, v));
   }

   // fb0/1
   enum { FB0, FB1 };

   // number of rgb pipes bufs (max)
   // 2 for rgb0/1 double bufs
   enum { RGB_PIPE_NUM_BUFS = 2 };

   struct ScreenInfo {
      ScreenInfo() : mFBWidth(0),
                     mFBHeight(0),
                     mFBbpp(0),
                     mFBystride(0) {}
      void dump(const char* const s) const;
      uint32_t mFBWidth;
      uint32_t mFBHeight;
      uint32_t mFBbpp;
      uint32_t mFBystride;
   };

   int getMdpFormat(int format);
   int getRotOutFmt(uint32_t format);
   /* flip is upside down and such. V, H flip
    * rotation is 90, 180 etc
    * It returns MDP related enum/define that match rot+flip*/
   int getMdpOrient(eTransform rotation);
   uint32_t getSize(const Whf& whf);
   uint32_t getSizeByMdp(const Whf& whf);
   const char* getFormatString(uint32_t format);
   const char* getStateString(eOverlayState state);

   inline int setWait(eWait wait, int flags) {
      return (wait == WAIT) ?
         flags &= ~MDP_OV_PLAY_NOWAIT :
         flags |= MDP_OV_PLAY_NOWAIT;
   }
   /* possible overlay formats libhardware/include/hardware/hardware.h */
   enum eFormat {
      OVERLAY_FORMAT_RGBA_8888    = HAL_PIXEL_FORMAT_RGBA_8888,
      OVERLAY_FORMAT_RGB_565      = HAL_PIXEL_FORMAT_RGB_565,
      OVERLAY_FORMAT_BGRA_8888    = HAL_PIXEL_FORMAT_BGRA_8888,
      OVERLAY_FORMAT_YCbYCr_422_I = 0x14,
      OVERLAY_FORMAT_CbYCrY_422_I = 0x16,
      OVERLAY_FORMAT_DEFAULT      = 99 // The actual color format is
                                       // determined by the overlay
   };

   // Cannot use HW_OVERLAY_MAGNIFICATION_LIMIT, since at the time
   // of integration, HW_OVERLAY_MAGNIFICATION_LIMIT was a define
   enum { HW_OV_MAGNIFICATION_LIMIT = 8,
          HW_OV_MINIFICATION_LIMIT = HW_OV_MAGNIFICATION_LIMIT
   };

   inline bool rotated(int orie) {
      return (orie == OVERLAY_TRANSFORM_ROT_90 ||
              orie == OVERLAY_TRANSFORM_ROT_270);
   }

   /* used by crop funcs in order to
    * normalizes the crop values to be all even */
   void normalizeCrop(uint32_t& xy, uint32_t& wh);

   template <class T>
   inline void memset0(T& t) { ::memset(&t, 0, sizeof(T)); }

   template <class ROT, class MDP>
   inline void swapOVRotWidthHeight(ROT& rot, MDP& mdp)
   {
      mdp.swapSrcWH();
      mdp.swapSrcRectWH();
      rot.swapDstWH();
   }

   template <class T> inline void swap ( T& a, T& b )
   {
      T c(a); a=b; b=c;
   }

   inline int alignup(int value, int a) {
      //if align = 0, return the value. Else, do alignment.
      return a ? ((((value - 1) / a) + 1) * a) : value;
   }

   // FIXME that align should replace the upper one.
   inline int align(int value, int a) {
      //if align = 0, return the value. Else, do alignment.
      return a ? ((value + (a-1)) & ~(a-1)) : value;
   }


   template <class MDP>
   inline utils::Dim getSrcRectDim(const MDP& ov) {
      return utils::Dim(ov.src_rect.x,
                        ov.src_rect.y,
                        ov.src_rect.w,
                        ov.src_rect.h);
   }
   template <class MDP>
   inline utils::Whf getSrcWhf(const MDP& ov) {
      return utils::Whf(ov.src.width,
                        ov.src.height,
                        ov.src.format);
   }
   template <class MDP>
   inline void setSrcRectDim(MDP& ov, const utils::Dim& d) {
      ov.src_rect.x = d.x;
      ov.src_rect.y = d.y;
      ov.src_rect.w = d.w;
      ov.src_rect.h = d.h;
   }
   template <class MDP>
   inline void setSrcWhf(MDP& ov, const utils::Whf& whf) {
      ov.src.width  = whf.w;
      ov.src.height = whf.h;
      ov.src.format = whf.format;
   }

   enum eRotOutFmt {
      ROT_OUT_FMT_DEFAULT,
      ROT_OUT_FMT_Y_CRCB_H2V2
   };

   template <int ROT_OUT_FMT> struct RotOutFmt;
} // utils

} // overlay2

#include "overlayUtils.inl"

#endif // OVERLAY_UTILS_H
