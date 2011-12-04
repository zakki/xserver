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
 */

/* THIS IS NOT AN X CONSORTIUM STANDARD */

/* iconv バージョン */

#define NEED_EVENTS
#define NEED_REPLIES

#define USE_XWIN_FULLEXTENSION

//#ifndef DWORD
//typedef unsigned int DWORD, *LPDWORD;
//#endif

#include <X11/Xlibint.h>

// >> Add Y.Arai
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#define ATOM DWORD
#ifndef XFree86Server
#define XFree86Server
#endif
#include <X11/Xproto.h>
#include <windows.h>
#include <imm.h>
#undef XFree86Server

typedef unsigned short	wchar;
// << Add Y.Arai

#include "winimestr.h"
#include <X11/extensions/Xext.h>
#include <X11/extensions/extutil.h>
#include <stdio.h>
#include <iconv.h>

static XExtensionInfo _winime_info_data;
static XExtensionInfo *winime_info = &_winime_info_data;
static char *winime_extension_name = WINIMENAME;

static int nCurContext = -1;

#define WinIMECheckExtension(dpy,i,val) \
  XextCheckExtension (dpy, i, winime_extension_name, val)

#if 1
//#define TRACE(msg)  ErrorF("WinIME:%s\n", msg);
#define TRACE(msg)  winDebug("WinIME:%s\n", msg);
#define MyErrorF winDebug
#else
#define TRACE(msg)
#define MyErrorF
#endif

#ifdef USE_XWIN_FULLEXTENSION
// >> 
/*
 * ICString.attr の値
 */

/* 1. 文字列が変換途中のテキストセグメントを表している時
 * (下記の値の bitwise-or)
 */
#define ICAttrNotConverted	0	/* まだ変換されていないセグメント */
#define ICAttrConverted		1	/* 変換済みセグメント */
#define ICAttrCurrentSegment	2	/* 現在注目しているセグメント */
#define ICAttrCurrentSubSegment 4	/* 現在注目しているサブセグメント */

/* 2. それ以外、例えば現在の入力モードなどを表している時 (常にこの値) */
#define ICAttrNormalString	(-1)
// << 
#endif

#define CHECK_XWIN_STAT
#ifdef CHECK_XWIN_STAT
extern char dispatchException;
#endif

/*****************************************************************************
 *                                                                           *
 *			   private utility routines                          *
 *                                                                           *
 *****************************************************************************/

static int close_display (Display *dpy, XExtCodes *extCodes);
static Bool wire_to_event ();
static Status event_to_wire ();

static /* const */ XExtensionHooks winime_extension_hooks = {
  NULL,				/* create_gc */
  NULL,				/* copy_gc */
  NULL,				/* flush_gc */
  NULL,				/* free_gc */
  NULL,				/* create_font */
  NULL,				/* free_font */
  close_display,		/* close_display */
  wire_to_event,		/* wire_to_event */
  event_to_wire,		/* event_to_wire */
  NULL,				/* error */
  NULL,				/* error_string */
};

static XEXT_GENERATE_FIND_DISPLAY (find_display, winime_info,
				   winime_extension_name,
				   &winime_extension_hooks,
				   WinIMENumberEvents, NULL);

static XEXT_GENERATE_CLOSE_DISPLAY (close_display, winime_info);

static Bool
wire_to_event (Display *dpy, XEvent  *re, xEvent  *event)
{
  XExtDisplayInfo *info = find_display (dpy);
  XWinIMENotifyEvent *se;
  xWinIMENotifyEvent *sevent;

  WinIMECheckExtension (dpy, info, False);

  switch ((event->u.u.type & 0x7f) - info->codes->first_event)
    {
    case WinIMEControllerNotify:
      se = (XWinIMENotifyEvent *) re;
      sevent = (xWinIMENotifyEvent *) event;
      se->type = sevent->type & 0x7f;
      se->serial = _XSetLastRequestRead(dpy,(xGenericReply *) event);
      se->send_event = (sevent->type & 0x80) != 0;
      se->display = dpy;
      se->context = sevent->context;
      se->time = sevent->time;
      se->kind = sevent->kind;
      se->arg = sevent->arg;
      se->window = sevent->window;	// Add Y.Arai
      se->hwnd = sevent->hwnd;	// Add Y.Arai
      return True;
    }
  return False;
}

static Status
event_to_wire (Display *dpy, XEvent  *re, xEvent  *event)
{
  XExtDisplayInfo *info = find_display (dpy);
  XWinIMENotifyEvent *se;
  xWinIMENotifyEvent *sevent;

  WinIMECheckExtension (dpy, info, False);

  switch ((re->type & 0x7f) - info->codes->first_event)
    {
    case WinIMEControllerNotify:
      se = (XWinIMENotifyEvent *) re;
      sevent = (xWinIMENotifyEvent *) event;
      sevent->type = se->type | (se->send_event ? 0x80 : 0);
      sevent->sequenceNumber = se->serial & 0xffff;
      sevent->context = se->context;
      sevent->kind = se->kind;
      sevent->arg = se->arg;
      sevent->time = se->time;
      sevent->window = se->window;	// Add Y.Arai
      sevent->hwnd = se->hwnd;	// Add Y.Arai
      return 1;
  }
  return 0;
}

#ifdef USE_XWIN_FULLEXTENSION
// Xatoklibから借用
/*
 * [関数名]
 *		euc2wcs( )
 * [表題]
 *              EUC文字列から wchar 型文字列への変換
 * [呼出形式]
 *		int	euc2wcs( unsigned char *euc, int elen, wchar *wbuf )
 *
 * [引数]
 *              型            : 名  称  : I O : 説      明
 *		unsigned char : *euc   : i   : EUC 文字列
 *		int           : elen   : i   : EUC 文字列の長さ
 *		wchar         : *wbuf  :   o : wchar 型文字列格納領域
 *
 * [返り値]
 *		1 : 常に１
 *
 * [使用関数]
 *		なし
 * [機能]
 *		unsigined char 型のEUC 文字列をwchar 型に変換する。
 *		EUC 文字列には、0x8f の特別なコードが含まれているので
 *		wchar に変換する時に個別処理をする。
 */
static int euc2wcs(euc, elen, wbuf)
unsigned char	*euc;
int		elen;
wchar		*wbuf;
{
	int	lb = 0, hb = 0 ;
	int	i ;
	int	n = 0 ;
	int	isSkip ;

	for( i = 0 ; i < elen ; i++ ) {
		isSkip = 0 ;
		if (  *euc == 0x8e ) {
			euc++ ;
			hb = *euc ;
			lb = 0 ;
			i++ ;
		}
		else if (  *euc & 0x80 ) {
			if ( *euc == 0x8f ) {
				isSkip=1 ;
			}
			else {
				lb = *euc ;
				euc++ ;
				hb = *euc ;
				i++ ;
			}
		}
		else {
			hb = *euc ;
			lb = 0 ;
		}
		euc++ ;
		if ( !isSkip ) {
			*wbuf = (( lb << 8 ) | hb ) & 0xffff ;
			wbuf++ ;
			n++ ;
		}
	}

	*wbuf = 0 ;
	return n ;
}

#if 0
static int
_Local_mbstowcs(
    XLCd lcd,
    XlcConv conv,
    char *str,
    int slen,
    wchar *wstr,
    int len)
{
    XPointer from, to;
    int from_left, to_left, ret;

    from = (XPointer) str;
    from_left = slen;
    to = (XPointer) wstr;
    to_left = len;

    if (_XlcConvert(conv, &from, &from_left, &to, &to_left, NULL, 0) < 0)
	ret = -1;
    else
    {
	ret = len - to_left;
	if (wstr && to_left > 0)
	    wstr[ret] = (wchar) 0;
    }

    return ret;
}
#endif

static int
_Local_wcstombs(
    iconv_t cd,
    const wchar *str,
    int flen,
    char *ustr,
    int len)
{
    XPointer from, to;
    int from_left, to_left, ret, status;

MyErrorF("  _Local_wcstombs()\n");
    from = (XPointer) str;
    from_left = flen;
    to = (XPointer) ustr;
    to_left = len;

    while(from_left > 0)
    {
MyErrorF("  from_left = %d byte\n", from_left);
	status = iconv(cd, (const char **)&from, &from_left, &to, &to_left);
	if (status < 0)
	{
	    /* 表現できない文字がきたので'？'で埋める */
MyErrorF("    from = 0x%X to ?\n", *from);
	    *to++ = 0xA1;
	    *to++ = 0xA9;
	    from += 2;
	    from_left -= 2;
	    to_left -= 2;
	}
    }

    ret = len - to_left;
MyErrorF("    converted length = %d\n", ret);
    if (ustr && to_left > 0)
	ustr[ret] = '\0';

{
int i;
MyErrorF("    euc strings:\n");
for (i=0; i<ret; i++)
	MyErrorF("      0x%X", ustr[i]);
MyErrorF("\n");
}

    return ret;
}

#define SELECTBUFSIZE 1024
// 一回EUCに変換してからWideCharに変換するバージョン
static int UCS2toWideChar(int nSize, wchar *pwszUnicodeStr, wchar* pDest)
{
    static wchar wszWideChar[SELECTBUFSIZE];
    static char szString[SELECTBUFSIZE];
    iconv_t cd = (iconv_t)-1;
    int nLen = -1;	// 文字数

    char		*language;
    char		*territory;
    char		*codeset;

MyErrorF("WinIME:UCS2toWideChar(%d Bytes)\n", nSize);

    // コード変換コンバータの準備
    cd = iconv_open("EUC-JP", "UCS-2-INTERNAL");
    if (cd == (iconv_t)-1)
    {
	TRACE("  No Converter\n");
	return nLen;
    }

    nLen = _Local_wcstombs(cd, pwszUnicodeStr, nSize, szString, SELECTBUFSIZE);
    iconv_close(cd);

    nLen = euc2wcs(szString, nLen, wszWideChar);

    if (nLen == -1)
    {
	TRACE("  Convert Error\n");
	return nLen;
    } else
    {
	TRACE("  Save data\n");


	memcpy((XPointer)pDest, (XPointer)wszWideChar, nLen * sizeof(wchar));
{
	int i;
	MyErrorF("    WideChar: ");
	for (i=0; i<nLen; i++)
	{
		MyErrorF("0x%X ", wszWideChar[i]);
	}
	MyErrorF("\n");
}
	return nLen;
    }
}
#endif

static Status
Wrap_XReply (
    register Display *dpy,
    register xReply *rep,
    int extra,		/* number of 32-bit words expected after the reply */
    Bool discard)	/* should I discard data following "extra" words? */
{
    if (dispatchException != 0)
    {
	TRACE("dispatchException is not 0.");
	return 0;
    } else
	return _XReply(dpy, rep, extra, discard);
}

/*****************************************************************************
 *                                                                           *
 *		    public WinIME Extension routines                         *
 *                                                                           *
 *****************************************************************************/

Bool
XWinIMEQueryExtension (Display *dpy,
		       int *event_basep, int *error_basep)
{
  XExtDisplayInfo *info = find_display (dpy);

  TRACE("QueryExtension...");
#ifdef CHECK_XWIN_STAT
  if (dispatchException != 0)
  {
    TRACE("dispatchException is not 0.");
    return False;
  }
#endif
  if (XextHasExtension(info))
    {
      *event_basep = info->codes->first_event;
      *error_basep = info->codes->first_error;
      TRACE("QueryExtension... return True");
      return True;
    }
  else
    {
      TRACE("QueryExtension... return False");
      return False;
    }
}

Bool
XWinIMEQueryVersion (Display* dpy, int* majorVersion,
		     int* minorVersion, int* patchVersion)
{
  XExtDisplayInfo *info = find_display (dpy);
  xWinIMEQueryVersionReply rep;
  xWinIMEQueryVersionReq *req;

  TRACE("QueryVersion...");
#ifdef CHECK_XWIN_STAT
  if (dispatchException != 0)
  {
    TRACE("dispatchException is not 0.");
    return False;
  }
#endif
  WinIMECheckExtension (dpy, info, False);

  LockDisplay(dpy);
  GetReq(WinIMEQueryVersion, req);
  req->reqType = info->codes->major_opcode;
  req->imeReqType = X_WinIMEQueryVersion;
  if (!Wrap_XReply(dpy, (xReply *)&rep, 0, xFalse))
    {
      UnlockDisplay(dpy);
      SyncHandle();
      TRACE("QueryVersion... return False");
      return False;
    }
  *majorVersion = rep.majorVersion;
  *minorVersion = rep.minorVersion;
  *patchVersion = rep.patchVersion;
  UnlockDisplay(dpy);
  SyncHandle();
  TRACE("QueryVersion... return True");
  return True;
}

Bool
XWinIMESelectInput (Display* dpy, unsigned long mask)
{
  XExtDisplayInfo *info = find_display (dpy);
  xWinIMESelectInputReq *req;

  TRACE("SelectInput...");
#ifdef CHECK_XWIN_STAT
  if (dispatchException != 0)
  {
    TRACE("dispatchException is not 0.");
    return False;
  }
#endif
  WinIMECheckExtension (dpy, info, False);

  LockDisplay(dpy);
  GetReq(WinIMESelectInput, req);
  req->reqType = info->codes->major_opcode;
  req->imeReqType = X_WinIMESelectInput;
  req->mask = mask;
  UnlockDisplay(dpy);
  SyncHandle();
  TRACE("SetlectInput... return True");
  return True;
}

Bool
XWinIMECreateContext (Display* dpy, int* context)
{
  XExtDisplayInfo *info = find_display (dpy);
  xWinIMECreateContextReply rep;
  xWinIMECreateContextReq *req;

  TRACE("CreateContext...");
#ifdef CHECK_XWIN_STAT
  if (dispatchException != 0)
  {
    TRACE("dispatchException is not 0.");
    return False;
  }
#endif
  WinIMECheckExtension (dpy, info, False);

  LockDisplay(dpy);
  GetReq(WinIMECreateContext, req);
  req->reqType = info->codes->major_opcode;
  req->imeReqType = X_WinIMECreateContext;
  if (!Wrap_XReply(dpy, (xReply *)&rep, 0, xFalse))
    {
      UnlockDisplay(dpy);
      SyncHandle();
      TRACE("CreateContext... return False");
      return False;
    }
  *context = rep.context;
//  nCurContext = rep.context;
  UnlockDisplay(dpy);
  SyncHandle();
  TRACE("CreateContext... return True");
  return True;
}

#ifdef USE_XWIN_FULLEXTENSION
Bool
XWinIMESetOpenStatus (Display* dpy, int context, Bool state)
{
  XExtDisplayInfo *info = find_display (dpy);
  xWinIMESetOpenStatusReq *req;

  TRACE("IMESetOpenStatus...");
#ifdef CHECK_XWIN_STAT
  if (dispatchException != 0)
  {
    TRACE("dispatchException is not 0.");
    return False;
  }
#endif
  WinIMECheckExtension (dpy, info, False);

  LockDisplay(dpy);
  GetReq(WinIMESetOpenStatus, req);
  req->reqType = info->codes->major_opcode;
  req->imeReqType = X_WinIMESetOpenStatus;
  req->context = (context != -1)?context:nCurContext;
  req->state = state;
  UnlockDisplay(dpy);
  SyncHandle();
  TRACE("IMESetOpenStatus... return True");

  return True;
}
#endif

#ifdef USE_XWIN_FULLEXTENSION
Bool
XWinIMESetCompositionWindow (Display* dpy, int context,
			     int style,
			     short cf_x, short cf_y,
			     short cf_w, short cf_h)
{
  XExtDisplayInfo *info = find_display (dpy);
  xWinIMESetCompositionWindowReq *req;

  TRACE("SetCompositionWindow...");
#ifdef CHECK_XWIN_STAT
  if (dispatchException != 0)
  {
    TRACE("dispatchException is not 0.");
    return False;
  }
#endif
#if 0	// 実験
  WinIMECheckExtension (dpy, info, False);

  LockDisplay(dpy);
  GetReq(WinIMESetCompositionWindow, req);
  req->reqType = info->codes->major_opcode;
  req->imeReqType = X_WinIMESetCompositionWindow;
  req->context = (context != -1)?context:nCurContext;
  req->style = style;
  req->ix = cf_x;
  req->iy = cf_y;
  req->iw = cf_w;
  req->ih = cf_h;

  UnlockDisplay(dpy);
  SyncHandle();
#endif
  TRACE("SetCompositionWindow... return True");
  return True;
}
#endif

#ifdef USE_XWIN_FULLEXTENSION
// UCS2で持つようにしたので対応 Y.Arai
// 渡す文字列のフォーマットを、Compound Text から WideChar に変更
// 戻り値: 文字数
int
XWinIMEGetCompositionString (Display *dpy, int context,
			     int index,
			     int count,
			     wchar* str_return)
{
  XExtDisplayInfo *info = find_display (dpy);
  xWinIMEGetCompositionStringReq *req;
  xWinIMEGetCompositionStringReply rep;
  char *str;
  int nLen;

  TRACE("GetCompositionString...");
#ifdef CHECK_XWIN_STAT
  if (dispatchException != 0)
  {
    TRACE("dispatchException is not 0.");
    return False;
  }
#endif
  WinIMECheckExtension (dpy, info, False);

  LockDisplay(dpy);
  GetReq(WinIMEGetCompositionString, req);
  req->reqType = info->codes->major_opcode;
  req->imeReqType = X_WinIMEGetCompositionString;
  req->context = (context != -1)?context:nCurContext;
  req->index = index;
  rep.strLength = 0;

  if (!Wrap_XReply(dpy, (xReply *)&rep, 0, xFalse))
    {
      UnlockDisplay(dpy);
      SyncHandle();
      TRACE("GetCompositionString... return False");
      return -1;
    }

  if ((str = (char *) Xmalloc(rep.strLength+1)))
    {
      _XReadPad(dpy, (XPointer)str, (long)rep.strLength);
    }
  else
    {
      _XEatData(dpy, (unsigned long) (rep.strLength + 3) & ~3);
      str_return = (char *) NULL;
      return -1;
    }

  // ここでWideCharへ変換
  nLen = UCS2toWideChar(rep.strLength, str, str_return);

  Xfree(str);

  UnlockDisplay(dpy);
  SyncHandle();
  TRACE("GetCompositionString... return True");

  return nLen;
}
#endif

// これは正しいcontext必須
Bool
XWinIMESetFocus (Display* dpy, int context, Bool focus, Window window)
{
  XExtDisplayInfo *info = find_display (dpy);
  xWinIMESetFocusReq *req;

  TRACE("IMESetFocus...");
#ifdef CHECK_XWIN_STAT
  if (dispatchException != 0)
  {
    TRACE("dispatchException is not 0.");
    return False;
  }
#endif
  WinIMECheckExtension (dpy, info, False);

  LockDisplay(dpy);
  GetReq(WinIMESetFocus, req);
  req->reqType = info->codes->major_opcode;
  req->imeReqType = X_WinIMESetFocus;
  req->context = context;
  if (focus)	// setの時だけにする
  {
    nCurContext = context;
MyErrorF("  nCurContext = %d\n", context);
  }
  req->focus = focus;
  req->window = window;
  UnlockDisplay(dpy);
  SyncHandle();
  TRACE("IMESetFocus... return True");

  return True;
}

#ifdef USE_XWIN_FULLEXTENSION
Bool
XWinIMESetCompositionDraw (Display* dpy, int context, Bool draw)
{
  XExtDisplayInfo *info = find_display (dpy);
  xWinIMESetCompositionDrawReq *req;

  TRACE("IMESetCompositionDraw...");
#ifdef CHECK_XWIN_STAT
  if (dispatchException != 0)
  {
    TRACE("dispatchException is not 0.");
    return False;
  }
#endif
#if 0	// 実験
  WinIMECheckExtension (dpy, info, False);

  LockDisplay(dpy);
  GetReq(WinIMESetCompositionDraw, req);
  req->reqType = info->codes->major_opcode;
  req->imeReqType = X_WinIMESetCompositionDraw;
  req->context = (context != -1)?context:nCurContext;
  req->draw = draw;
  UnlockDisplay(dpy);
  SyncHandle();
#endif
  TRACE("IMESetCompositionDraw... return True");

  return True;
}
#endif

#ifdef USE_XWIN_FULLEXTENSION
Bool
XWinIMEGetCursorPosition (Display* dpy, int context, int *cursor, int *numClause, int *curClause, int *offset)
{
  XExtDisplayInfo *info = find_display (dpy);
  xWinIMEGetCursorPositionReply rep;
  xWinIMEGetCursorPositionReq *req;

  TRACE("GetCursorPosition...");
#ifdef CHECK_XWIN_STAT
  if (dispatchException != 0)
  {
    TRACE("dispatchException is not 0.");
    return False;
  }
#endif
  WinIMECheckExtension (dpy, info, False);
TRACE("  *A*");

  LockDisplay(dpy);
TRACE("  *B*");
  GetReq(WinIMEGetCursorPosition, req);
TRACE("  *C*");
  req->reqType = info->codes->major_opcode;
  req->imeReqType = X_WinIMEGetCursorPosition;
  req->context = (context != -1)?context:nCurContext;
TRACE("  *D*");
  if (!Wrap_XReply(dpy, (xReply *)&rep, 0, xFalse))
    {
      UnlockDisplay(dpy);
      SyncHandle();
      TRACE("GetCursorPosition... return False");
      return False;
    }
TRACE("  *E*");
  *cursor = rep.cursor;
  *numClause = rep.numClause;	// Y.Arai
  *curClause = rep.curClause;	// Y.Arai
  *offset = rep.offset;		// Y.Arai
  UnlockDisplay(dpy);
TRACE("  *F*");
  SyncHandle();
  TRACE("GetCursorPosition... return True");
  return True;
}
#endif

#ifdef USE_XWIN_FULLEXTENSION
// >> Add Y.Arai
Bool
XWinIMEGetConversionStatus (Display *dpy, int context, Bool* fopen, DWORD* conversion, DWORD* sentence, Bool* fmodify)
{
    XExtDisplayInfo *info = find_display (dpy);
    xWinIMEGetConversionStatusReq *req;
    xWinIMEGetConversionStatusReply rep;

    TRACE("GetConversionStatus...");
#ifdef CHECK_XWIN_STAT
  if (dispatchException != 0)
  {
    TRACE("dispatchException is not 0.");
    return False;
  }
#endif
    WinIMECheckExtension (dpy, info, False);
TRACE("  *A*");

    LockDisplay(dpy);
TRACE("  *B*");
    GetReq(WinIMEGetConversionStatus, req);
TRACE("  *C*");
    req->reqType = info->codes->major_opcode;
    req->imeReqType = X_WinIMEGetConversionStatus;
    req->context = (context != -1)?context:nCurContext;

TRACE("  *D*");
    if (!Wrap_XReply(dpy, (xReply *)&rep, 0, xFalse))
    {
        UnlockDisplay(dpy);
        SyncHandle();
        TRACE("GetConversionStatus... return False");
        return False;
    }
TRACE("  *E*");

    *fopen = rep.fopen;
    *conversion = rep.conversion;
    *sentence = rep.sentence;
    *fmodify = rep.fmodechange;

    UnlockDisplay(dpy);
TRACE("  *F*");
    SyncHandle();
    TRACE("GetConversionStatus... return True");

    return True;
}
#endif

#ifdef USE_XWIN_FULLEXTENSION
Bool
XWinIMEGetOpenStatus (Display* dpy, int context, Bool* fopen)
{
    XExtDisplayInfo *info = find_display (dpy);
    xWinIMEGetOpenStatusReply rep;
    xWinIMEGetOpenStatusReq *req;

    TRACE("GetOpenStatus...");
#ifdef CHECK_XWIN_STAT
  if (dispatchException != 0)
  {
    TRACE("dispatchException is not 0.");
    return False;
  }
#endif
    WinIMECheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(WinIMEGetOpenStatus, req);
    req->reqType = info->codes->major_opcode;
    req->imeReqType = X_WinIMEGetOpenStatus;
    req->context = context;
    if (!Wrap_XReply(dpy, (xReply *)&rep, 0, xFalse))
    {
        UnlockDisplay(dpy);
        SyncHandle();
        TRACE("GetOpenStatus... return False");
        return False;
    }
    *fopen = rep.fopen;
    UnlockDisplay(dpy);
    SyncHandle();
    TRACE("GetOpenStatus... return True");
    return True;
}
#endif

#ifdef USE_XWIN_FULLEXTENSION
// GetCompositionStringと違い、UCS-2で受け渡しすることにする
/// UCS-2はやめ、WideCharでやる
// 戻り値: 文字数
int
XWinIMEGetTargetClause (Display *dpy,
			int context,
			int target,
			wchar *data,
			int *attr)
{
    XExtDisplayInfo *info = find_display (dpy);
    xWinIMEGetTargetClauseReq *req;
    xWinIMEGetTargetClauseReply rep;
    char *str;
    int nLen;

    TRACE("GetTargetClause...");
    *attr = ICAttrNormalString;
#ifdef CHECK_XWIN_STAT
  if (dispatchException != 0)
  {
    TRACE("dispatchException is not 0.");
    return -1;
  }
#endif
    WinIMECheckExtension (dpy, info, False);
TRACE("  *A*");

    LockDisplay(dpy);
TRACE("  *B*");
    GetReq(WinIMEGetTargetClause, req);
TRACE("  *C*");
    req->reqType = info->codes->major_opcode;
    req->imeReqType = X_WinIMEGetTargetClause;
    req->context = (context != -1)?context:nCurContext;
//    req->index = index;	// とりあえずindexは使わない
    req->target = target;
    rep.bytes = 0;

TRACE("  *D*");
    if (!Wrap_XReply(dpy, (xReply *)&rep, 0, xFalse))
    {
	UnlockDisplay(dpy);
	SyncHandle();
	TRACE("GetTargetClause... return False");
	return -1;
    }
TRACE("  *E*");

    if ((str = (char *) Xmalloc(rep.bytes + 1)))
    {
MyErrorF("  %d bytes.\n", rep.bytes);
	_XReadPad(dpy, (XPointer)str, (long)rep.bytes);
	str[rep.bytes + 1] = 0;
    } else
    {
TRACE("  Xmalloc error.\n");
	_XEatData(dpy, (unsigned long)rep.bytes);
	data = NULL;
	return -1;
    }

TRACE("  *F*");
    // ここでWideCharへ変換
    nLen = UCS2toWideChar(rep.bytes, str, data);

    Xfree(str);

MyErrorF("    check attr: target = %d, current = %d\n", target, rep.curClause);
    switch(rep.attr)
    {
	case ATTR_INPUT:
TRACE("    attr = ATTR_INPUT\n");
	    *attr = ICAttrNotConverted;
//	    *attr = ICAttrConverted;
	    break;
	case ATTR_TARGET_CONVERTED:
TRACE("    attr = ATTR_TARGET_CONVERTED\n");
#if 0
	    if (target == rep.curClause)
		*attr = ICAttrConverted | ICAttrCurrentSegment;
	    else
		*attr = ICAttrConverted;
#else
		*attr = ICAttrConverted | ICAttrCurrentSegment;
#endif
	    break;
	case ATTR_CONVERTED:
TRACE("    attr = ATTR_CONVERTED\n");
#if 0
	    if (target == rep.curClause)
		*attr = ICAttrConverted | ICAttrCurrentSegment;
	    else
		*attr = ICAttrConverted;
#else
		*attr = ICAttrConverted;
#endif
	    break;
	case ATTR_TARGET_NOTCONVERTED:
TRACE("    attr = ATTR_TARGET_NOTCONVERTED\n");
#if 0
	    if (target == rep.curClause)
		*attr = ICAttrConverted | ICAttrCurrentSegment;	// ICAttrNotConverted -> ICAttrConverted, from A.Yamanaka
	    else
		*attr = ICAttrNotConverted;
#else
		*attr = ICAttrConverted | ICAttrCurrentSegment;	// ICAttrNotConverted -> ICAttrConverted, from A.Yamanaka
#endif
	    break;
	default:
MyErrorF("WinIME:    attr = ( %d )\n\n", rep.attr);
	    *attr = ICAttrNotConverted;
	    break;

    }

TRACE("  *G*");
    UnlockDisplay(dpy);
TRACE("  *H*");
    SyncHandle();
    TRACE("GetTargetClause... return True");

    return nLen;
}
#endif

#ifdef USE_XWIN_FULLEXTENSION
// 指定されたtargetのoffset以降、全編集文字列の最後まで返す
// offset: 文字数
// 戻り値: 文字数
int
XWinIMEGetTargetString (Display *dpy,
			int context,
			int target,
			int offset,
			wchar *data)
{
    XExtDisplayInfo *info = find_display (dpy);
    xWinIMEGetTargetStringReq *req;
    xWinIMEGetTargetStringReply rep;
    char *str;
    int nLen;

    TRACE("GetTargetString...");
#ifdef CHECK_XWIN_STAT
  if (dispatchException != 0)
  {
    TRACE("dispatchException is not 0.");
    return -1;
  }
#endif
    WinIMECheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(WinIMEGetTargetString, req);
    req->reqType = info->codes->major_opcode;
    req->imeReqType = X_WinIMEGetTargetString;
    req->context = (context != -1)?context:nCurContext;
//    req->index = index;	// とりあえずindexは使わない
    req->target = target;
    req->offset = offset;
    rep.bytes = 0;

    if (!Wrap_XReply(dpy, (xReply *)&rep, 0, xFalse))
    {
	UnlockDisplay(dpy);
	SyncHandle();
	TRACE("GetTargetString... return False");
	return -1;
    }

    if ((str = (char *) Xmalloc(rep.bytes + 1)))
    {
	_XReadPad(dpy, (XPointer)str, (long)rep.bytes);
	str[rep.bytes + 1] = 0;
    } else
    {
	_XEatData(dpy, (unsigned long)rep.bytes);
	data = NULL;
	return -1;
    }

    // ここでWideCharへ変換
    nLen = UCS2toWideChar(rep.bytes, str, data);

    Xfree(str);

    UnlockDisplay(dpy);
    SyncHandle();
    TRACE("GetTargetString... return True");

    return nLen;
}
#endif

// 最後にアクティブになったcontextを返す
Bool
XWinIMEGetLastContext (Display* dpy, int* context)
{
    XExtDisplayInfo *info = find_display (dpy);
#if 0
    xWinIMEGetLastContextReply rep;
    xWinIMEGetLastContextReq *req;
#endif

    TRACE("GetLastContext...");
#ifdef CHECK_XWIN_STAT
  if (dispatchException != 0)
  {
    TRACE("dispatchException is not 0.");
    return False;
  }
#endif
    WinIMECheckExtension (dpy, info, False);

#if 0
    LockDisplay(dpy);
    GetReq(WinIMEGetLastContext, req);
    req->reqType = info->codes->major_opcode;
    req->imeReqType = X_WinIMEGetLastContext;
    if (!Wrap_XReply(dpy, (xReply *)&rep, 0, xTrue))
    {
	UnlockDisplay(dpy);
	SyncHandle();
	TRACE("GetLastContext... return False");
	return False;
    }
    *context = rep.context;
    UnlockDisplay(dpy);
    SyncHandle();
#else
    *context = nCurContext;
#endif
    TRACE("GetLastContext... return True");
    return True;
}

Bool
XWinIMEClearContext (Display* dpy, int context, BOOL *fmodify)
{
    XExtDisplayInfo *info = find_display (dpy);
    xWinIMEClearContextReply rep;
    xWinIMEClearContextReq *req;

    TRACE("IMEClearContext...");
#ifdef CHECK_XWIN_STAT
  if (dispatchException != 0)
  {
    TRACE("dispatchException is not 0.");
    return False;
  }
#endif
    WinIMECheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(WinIMEClearContext, req);
    req->reqType = info->codes->major_opcode;
    req->imeReqType = X_WinIMEClearContext;
    req->context = (context != -1)?context:nCurContext;

    if (!Wrap_XReply(dpy, (xReply *)&rep, 0, xTrue))
    {
	UnlockDisplay(dpy);
	SyncHandle();
	TRACE("ClearContext... return False");
	return False;
    }
    *fmodify = rep.modify;

    UnlockDisplay(dpy);
    SyncHandle();
    TRACE("IMEClearContext... return True");
    return True;
}

#ifdef USE_XWIN_FULLEXTENSION
Bool
XWinIMESetCandidateWindow (Display *dpy, int context, int x, int y, int listnum)
{
    XExtDisplayInfo *info = find_display (dpy);
    xWinIMESetCandidateWindowReq *req;

    TRACE("IMESetCandidateWindow...");
#ifdef CHECK_XWIN_STAT
  if (dispatchException != 0)
  {
    TRACE("dispatchException is not 0.");
    return False;
  }
#endif
    WinIMECheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(WinIMESetCandidateWindow, req);
    req->reqType = info->codes->major_opcode;
    req->imeReqType = X_WinIMESetCandidateWindow;
    req->context = (context != -1)?context:nCurContext;
    req->x = x;
    req->y = y;
    req->n = listnum;
    UnlockDisplay(dpy);
    SyncHandle();
    TRACE("IMESetCandidateWindow... return True");
    return True;
}
#endif

#ifdef USE_XWIN_FULLEXTENSION
Bool
XWinIMEStartIME (Display *dpy, int context)
{
    XExtDisplayInfo *info = find_display (dpy);
    xWinIMEStartIMEReq *req;

    TRACE("IMEStartIME...");
#ifdef CHECK_XWIN_STAT
  if (dispatchException != 0)
  {
    TRACE("dispatchException is not 0.");
    return False;
  }
#endif
    WinIMECheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(WinIMEStartIME, req);
    req->reqType = info->codes->major_opcode;
    req->imeReqType = X_WinIMEStartIME;
    req->context = (context != -1)?context:nCurContext;
    UnlockDisplay(dpy);
    SyncHandle();
    TRACE("IMEStartIME... return True");
    return True;
}
#endif

Bool
XWinIMEDestroyContext (Display *dpy, int context)
{
    XExtDisplayInfo *info = find_display (dpy);
    xWinIMEDestroyContextReq *req;

    TRACE("IMEDestroyContext...");
#ifdef CHECK_XWIN_STAT
  if (dispatchException != 0)
  {
    TRACE("dispatchException is not 0.");
    return False;
  }
#endif
    WinIMECheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(WinIMEDestroyContext, req);
    req->reqType = info->codes->major_opcode;
    req->imeReqType = X_WinIMEDestroyContext;
    req->context = (context != -1)?context:nCurContext;
    UnlockDisplay(dpy);
    SyncHandle();
    TRACE("IMEDestroyContext... return True");
    return True;
}
// << Add Y.Arai
