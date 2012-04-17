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

#ifndef OVERLAY_FD_H
#define OVERLAY_FD_H

#include <fcntl.h>
#include "overlayUtils.h"

/*
 * Simple file descriptor wrapper for overlay.
 *
 * */
namespace overlay2 {

class OvFD;

/* helper function to open by using fbnum */
bool open(OvFD& fd, uint32_t fbnum, const char* const dev,
          int flags = O_RDWR);

/*
 * Holds one FD
 * Dtor will NOT close the underlying FD.
 * That enables us to copy that object around
 * */
class OvFD {
public:
   /* Ctor */
   explicit OvFD();

   /* dtor will NOT close the underlying FD */
   ~OvFD();

   /* Open fd using the path given by dev.
    * return false in failure */
   bool open(const char* const dev,
             int flags = O_RDWR);

   /* populate path */
   void setPath(const char* const dev);

   /* Close fd if we have a valid fd. */
   bool close();

   /* returns underlying fd.*/
   int getFD() const;

   /* returns true if fd is valid */
   bool valid() const;

   /* like operator= */
   void copy(int fd);

   /* dump the state of the instance */
   void dump() const;
private:
   /* helper enum for determine valid/invalid fd */
   enum { INVAL = -1 };

   /* actual os fd */
   int mFD;

   /* path, for debugging */
   char mPath[utils::MAX_PATH_LEN];
};

} // overlay2

#include "overlayFD.inl"

#endif // OVERLAY_FD_H
