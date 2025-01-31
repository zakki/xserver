/*
 *Copyright (C) 2004-2005 Kensuke Matsuzaki. All Rights Reserved.
 *
 *Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 *"Software"), to deal in the Software without restriction, including
 *without limitation the rights to use, copy, modify, merge, publish,
 *distribute, sublicense, and/or sell copies of the Software, and to
 *permit persons to whom the Software is furnished to do so, subject to
 *the following conditions:
 *
 *The above copyright notice and this permission notice shall be
 *included in all copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *NONINFRINGEMENT. IN NO EVENT SHALL KENSUKE MATSUZAKI BE LIABLE FOR
 *ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 *CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *Except as contained in this notice, the name of Kensuke Matsuzaki
 *shall not be used in advertising or otherwise to promote the sale, use
 *or other dealings in this Software without prior written authorization
 *from Kensuke Matsuzaki.
 *
 * Authors:	Kensuke Matsuzaki <zakki@peppermint.jp>
 *		Y.Arai
 */

#ifndef _WINIME_H_
#define _WINIME_H_

#include <X11/Xfuncproto.h>

#define X_WinIMEQueryVersion		 0
#define X_WinIMESelectInput		 1
#define X_WinIMECreateContext		 2
#define X_WinIMESetFocus		 6
#define X_WinIMEGetLastContext		12
#define X_WinIMEClearContext		13
#define X_WinIMEDestroyContext		17

#define X_WinIMESetOpenStatus		 3
#define X_WinIMESetCompositionWindow	 4
#define X_WinIMEGetCompositionString	 5
#define X_WinIMESetCompositionDraw	 7
#define X_WinIMEGetCursorPosition	 8
#define X_WinIMEGetConversionStatus	 9
#define X_WinIMEGetOpenStatus		10
#define X_WinIMEGetTargetClause		11
#define X_WinIMEGetTargetString		14
#define X_WinIMESetCandidateWindow	15
#define X_WinIMEStartIME		16

/* Events */
#define WinIMEControllerNotify		0
#define WinIMENumberEvents		1

/* Masks */
#define WinIMENotifyMask		(1L << 0)

/* "Kinds" of ControllerNotify events */
#define WinIMEOpenStatus		0
#define WinIMEComposition		1
#define WinIMEStartComposition		2
#define WinIMEEndComposition		3
#define WinIMEOpenCand			4
#define WinIMEIgnoreNextKey		5

/* "args" of WinIMEComposition events */
#define WinIMECMPCompReadStr 1
#define WinIMECMPCompReadAttr 2
#define WinIMECMPCompReadClause 4
#define WinIMECMPCompStr 8
#define WinIMECMPCompAttr 16
#define WinIMECMPCompClause 32
#define WinIMECMPCursorPos 128
#define WinIMECMPDeltaStart 256
#define WinIMECMPResultReadStr 512
#define WinIMECMPResultReadClause 1024
#define WinIMECMPResultStr 2048
#define WinIMECMPResultClause 4096

/* Errors */
#define WinIMEClientNotLocal		0
#define WinIMEOperationNotSupported	1
#define WinIMEDisabled			2
#define WinIMENumberErrors		3

/* Composition Window Styles (CFS_*) */
#define WinIMECSDefault			0
#define WinIMECSRect			1
#define WinIMECSPoint			2
#define WinIMECSForcePosition		32

#define WinIMEAttrInput			0
#define WinIMEAttrTargetConverted	1
#define WinIMEAttrConverted		2
#define WinIMEAttrTargetNotconverted	3
#define WinIMEAttrInputError		4
#define WinIMEAttrFixedconverted	5

#ifndef _WINIME_SERVER_

typedef struct {
  int	type;		/* of event */
  unsigned long serial;	/* # of last request processed by server */
  Bool send_event;	/* true if this came frome a SendEvent request */
  Display *display;	/* Display the event was read from */
  int context;		/* context of event */
  Time time;		/* server timestamp when event happened */
  int kind;		/* subtype of event */
  int arg;
  Window window;
  HWND hwnd;
} XWinIMENotifyEvent;

_XFUNCPROTOBEGIN

Bool XWinIMEQueryExtension (Display *dpy, int *event_base, int *error_base);

Bool XWinIMEQueryVersion (Display *dpy, int *majorVersion,
			  int *minorVersion, int *patchVersion);

Bool XWinIMESelectInput (Display *dpy, unsigned long mask);

Bool XWinIMECreateContext (Display *dpy, int* context);

Bool XWinIMESetFocus (Display *dpy, int context, Bool focus, Window window);


/* $BI,$:$7$b(BImmXXX$B7O$N(BAPI$B$H#1BP#1BP1~$G$O$J$$$N$GCm0U(B */
Bool XWinIMEGetLastContext (Display *dpy, int* context);
Bool XWinIMEClearContext (Display *dpy, int context, BOOL *fmodify);
Bool XWinIMEDestroyContext (Display *dpy, int context);

Bool XWinIMESetOpenStatus (Display *dpy, int context, Bool state);
Bool XWinIMESetCompositionWindow (Display *dpy, int context,
				  int style,
				  short cf_x, short cf_y,
				  short cf_w, short cf_h);
int XWinIMEGetCompositionString (Display *dpy, int context,
				  int index,
				  int count,
				  wchar* str_return);
Bool XWinIMESetCompositionDraw (Display *dpy, int context, Bool draw);
Bool XWinIMEGetCursorPosition (Display* dpy, int context, int *cursor, int *numClause, int *curClause, int *offset);	// Change Y.Arai
Bool XWinIMEGetConversionStatus (Display *dpy, int context, Bool* fopen, DWORD* conversion, DWORD* sentence, Bool* fmodify);
Bool XWinIMEGetOpenStatus (Display *dpy, int context, Bool* fopen);
int XWinIMEGetTargetClause (Display *dpy, int context, int target, wchar *data, int *attr);
int XWinIMEGetTargetString (Display *dpy, int context, int target, int offset, wchar *data);
Bool XWinIMESetCandidateWindow (Display *dpy, int context, int x, int y, int listnum);
Bool XWinIMEStartIME (Display *dpy, int context);

_XFUNCPROTOEND

#endif /* _WINIME_SERVER_ */
#endif /* _WINIME_H_ */
