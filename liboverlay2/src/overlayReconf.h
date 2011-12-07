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

#ifndef OVERLAY_RECONF_H
#define OVERLAY_RECONF_H

#include "overlayUtils.h"
#include "overlayRotator.h"

namespace overlay2 {

/*
 * Simple Reconfigure helper class in case of
 * netflix and similar use cases.
 * Holds Rotator, latest buf geom.
 * */
class OverlayReconf : utils::NoCopy {
public:
   /* reset, keep rotator obj */
   explicit OverlayReconf(Rotator& rot);

   /* close sessions */
   bool close();

   /* Setup buffers if needed (e.g. geom changed)
    * PlayInfo is out param */
   bool setupBuf(const utils::ReconfArgs& args,
                 utils::PlayInfo& playInfo);

   /* reset current status (e.g. geom) */
   void reset();
private:
   /* ref to rot */
   Rotator& mRot;
   
   /* caching of reconf args */
   utils::ReconfArgs mArgs;
};

} // overlay2

#endif // OVERLAY_RECONF_H
