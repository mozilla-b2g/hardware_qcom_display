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

/* for open/close */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <cutils/log.h>

#include "overlayRes.h"

namespace overlay2 {

inline bool open(OvFD& fd, uint32_t fbnum, const char* const dev, int flags)
{
   char dev_name[64] = {0};
   snprintf(dev_name, sizeof(dev_name), dev, fbnum);
   return fd.open(dev_name, flags);
}

inline OvFD::OvFD() : mFD (INVAL) {
   mPath[0] = 0;
}

inline OvFD::~OvFD() { /* no op in the meantime */ }

inline bool OvFD::open(const char* const dev, int flags)
{
   mFD = ::open(dev, flags, 0);
   if (mFD < 0) {
      // FIXME errno, strerror in bionic?
      LOGE("Cant open device %s err=%d", dev, errno);
      return false;
   }
   setPath(dev);
   return true;
}

inline void OvFD::setPath(const char* const dev)
{
   ::strncpy(mPath, dev, utils::MAX_PATH_LEN);
}

inline bool OvFD::close()
{
   int ret = 0;
   if(valid()) {
      ret = ::close(mFD);
      mFD = INVAL;
   }
   return (ret == 0);
}

inline bool OvFD::valid() const
{
   return (mFD != INVAL);
}

inline int OvFD::getFD() const { return mFD; }

inline void OvFD::copy(int fd) {
   mFD = fd;
}

inline void OvFD::dump() const
{
   LOGE("== Dump OvFD fd=%d path=%s start/end ==",
        mFD, mPath);
}

}
