/* Copyright 2005 Media Lab. Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Media Lab. Inc.
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  
 * Media Lab. Inc. makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * Author: Yasuyuki ARAI
 */

#ifndef _WinIMMDefs_h
#define _WinIMMDefs_h

/* NOTE: For some unknown reason, including Xproto.h solves
 * tons of problems with including windows.h.  Unknowns reasons
 * are usually bad, so someone should investigate this.
 * ということらしい 
 */
#include "X11/Xproto.h"

#define NONAMELESSUNION
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define ATOM DWORD
#ifndef XFree86Server
#define XFree86Server
#endif
#include "winms.h"
#undef XFree86Server

#undef wchar
#include "WStr.h"

#define LOCALEVENT_MAX 4

#endif /* _WinIMMDefs_h */

