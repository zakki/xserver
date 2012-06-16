/*
 * Copyright (c) 1990  Software Research Associates, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Software Research
 * Associates makes no representations about the suitability of this software
 * for any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * Author:  Makoto Ishisone, Software Research Associates, Inc., Japan
 */

/* Copyright 1991 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of NEC Corporation
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  NEC 
 * Corporation makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 *
 * Author: Akira Kon, NEC Corporation.  (kon@d1.bs2.mt.nec.co.jp)
 */

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

#ifndef _MB_CAPABLE
#define _MB_CAPABLE 1
#endif

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Atoms.h>
#define XK_KATAKANA
#include <X11/keysym.h>
#if XtSpecificationRelease > 4
#include <X11/Xfuncs.h>
#endif

#include "WinIMMDefs.h"

#include "WinIMM32P.h"
#include "DebugPrint.h"

#include <X11/extensions/winime.h>

#include <wchar.h>

#ifndef _WCHAR_T
#define _WCHAR_T /* $B$3$NDj5A$O(B jrkanji.h $B$G(B wcKanjiStatus $B$J$I$rDj5A$9$k$?$a(B */
#endif
//#define wchar_t wchar
//typedef unsigned short wchar_t;

static XtResource resources[] = {
#define offset(field) XtOffset(WinIMM32Object, winimm32.field)
    { XtNwinimm32file, XtCWinimm32file, XtRString, sizeof(String),
	offset(winimm32conf), XtRString, NULL },
    { XtNwinimm32mode, XtCWinimm32mode,
	XtRBoolean, sizeof(Boolean),
	offset(winimm32mode), XtRBoolean, False },
#undef offset
};

// >> $BI,?\(B
static void ClassInitialize();
static void Initialize();
static void Destroy();
static Boolean SetValues();

static int InputEvent();
static ICString *GetMode();
static int CursorPos();
static int NumSegments();
static ICString *GetSegment();
static int CompareSegment();
static ICString *GetItemList();
static int SelectItem();
static int ConvertedString();
static int ClearConversion(Widget w, Boolean fin);
static ICString *GetAuxSegments();
static int GetTriggerKeys();
static int PreeditString();
static int StatusString();
static void convend(WinIMM32Object obj);
// << $BI,?\(B

#define uchar	unsigned char
#define ushort	unsigned short
#define ulong	unsigned long

// >> helper
static WCHAR *Xwinimm32GetModeStr();
//static int sjis2euc(uchar *sjis, int sjislen, uchar *euc, int euclen);
//static int euc2cannawc(char *src, int srclen, ushort *dest, int destlen);
//static ushort *mw_ucs2wcs(LPCWSTR src, int srclen);
// << helper

#define STRING_BUFFER_SIZE 1024

WinIMM32ClassRec winimm32ClassRec = {
  { /* object fields */
    /* superclass		*/	(WidgetClass) &inputConvClassRec,
    /* class_name		*/	"WinIMM32",
    /* widget_size		*/	sizeof(WinIMM32Rec),
    /* class_initialize		*/	ClassInitialize,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* obj1			*/	NULL,
    /* obj2			*/	NULL,
    /* obj3			*/	0,
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* obj4			*/	FALSE,
    /* obj5			*/	FALSE,
    /* obj6			*/	FALSE,
    /* obj7			*/	FALSE,
    /* destroy			*/	Destroy,
    /* obj8			*/	NULL,
    /* obj9			*/	NULL,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* obj10			*/	NULL,
    /* get_values_hook		*/	NULL,
    /* obj11			*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* obj12			*/	NULL,
    /* obj13			*/	NULL,
    /* obj14			*/	NULL,
    /* extension		*/	NULL
  },
  { /* inputConv fields */
    /* InputEvent		*/	InputEvent,
    /* GetMode			*/	GetMode,
    /* CursorPos		*/	CursorPos,
    /* NumSegments		*/	NumSegments,
    /* GetSegment		*/	GetSegment,
    /* CompareSegment		*/	CompareSegment,
    /* GetItemList		*/	GetItemList,
    /* SelectItem		*/	SelectItem,
    /* GetConvertedString	*/	ConvertedString,
    /* ClearConversion		*/	ClearConversion,
    /* GetAuxSegments		*/	GetAuxSegments,
    /* SupportMultipleObjects	*/	True,
    /* GetTriggerKeys		*/	GetTriggerKeys,
    /* num_trigger_keys		*/	0,
    /* trigger_keys		*/	NULL,
    /* GetPreeditString		*/	PreeditString,
    /* GetStatusString		*/	StatusString,
    /* NoMoreObjects		*/	False,
  },
  { /* WinIMM32 fields */
    /* foo			*/	0,
  }
};

WidgetClass winimm32ObjectClass = (WidgetClass)&winimm32ClassRec;

static void fix();

//static ICString *SymbolList;
//static int NumSymbols;

//static void addObject();
static void deleteObject();

//static int ignoreListfunc = 0;

static Display *displaybell = (Display *)0;

//static int nWinIMMContexts = 0;

#define MODE_MAX_NUM 4

#if 1
// Wide Char $B$G5-=R$9$k$3$H(B
static wchar_t
_wcsModeNames[MODE_MAX_NUM][10] = 
{   /* $B$3$3$K@_Dj$9$k(BWideChar$B$O:GBg(B10$BJ8;z(B(20byte)$B$K$7$F$/$@$5$$(B */
    {0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0000},
    {0x005B, 0x0020, 0xA4A2, 0x0020, 0x005D, 0x0000},		/*[ $B$"(B ]*/
    {0x005B, 0xB4C1, 0xBBFA, 0x005D, 0x0000},			/*[$B4A;z(B]*/
    {0x005B, 0xB0EC, 0xCDF7, 0x005D, 0x0000}			/*[$B0lMw(B]*/
};
#else
// Compound Text $B$G5-=R$9$k$3$H(B
static
char *
_sModeNames[MODE_MAX_NUM] = 
{   /* $B$3$3$K@_Dj$9$k(BCompound-Text$B$O:GBg(B20$BJ8;z$K$7$F$/$@$5$$(B */
    "      ",			        /* AlphaMode */
    "\033(B[ \033$(B\044\042\033(B ]"	/*[ $B$"(B ]*/  /* EmptyMode */ 
};

/*
0x1B,0x28,0x42,
0x5B,0x20,
0x1B,0x24,0x28,0x42,
0x24,0x22,
0x1B,0x28,0x42,
0x20,0x5D,
*/

static wchar_t *
_wcsModeNames[MODE_MAX_NUM] = { NULL, NULL };
static int fAlreadyInitModeStrings = 0;
#endif

// in winglobals.c
extern DWORD	g_TriggerKeycode;
extern long	g_TriggerModifier;
extern BOOL g_ignore_key;

// in kinput2.c
BOOL findProcessKey(Time time, unsigned int keycode);

static void
ClassInitialize()
{
  TRACE(("WinIMM32ObjectClass initialized\n"));
  /* $B2?$b$7$J$$(B */
}

static BOOL checkIMEKey(XKeyEvent* ev)
{
    if ((ev->keycode == g_TriggerKeycode) &&
    	((ev->state & g_TriggerModifier) == g_TriggerModifier))
    {
	TRACE(("  XIM end trigger key found.\n"));
    	return TRUE;
    }

    return FALSE;
}

/*
 * int InputEvent(Widget ojbect, XEvent *event)
 *	$B%*%V%8%'%/%H$KF~NO%$%Y%s%H$rM?$($k(B
 *	$B%(%i!<$,5/$3$C$?>l9g$O(B -1 $B$,JV$k(B
 *	$B%(%i!<$G$J$1$l$P(B 0 $B$+(B 1 $B$,JV$k(B
 *	$BDL>o$O(B 0 $B$,JV$k$,!"$b$7(B 1 $B$,JV$C$F$-$?$i!"$=$l$O$G$-$k$J$i$P(B
 *	$B$=$NF~NO%$%Y%s%H$r%"%W%j%1!<%7%g%s$KAw$jJV$7$?J}$,$h$$$H$$$&(B
 *	$B%*%V%8%'%/%H$+$i$N%R%s%H$G$"$k(B
 */
static int
InputEvent(Widget w, XEvent *event)
{
    WinIMM32Object obj = (WinIMM32Object)w;
    XKeyEvent* ev = (XKeyEvent*)event;
    Bool fopen;
    DWORD conversion;
    DWORD sentence;
    Bool fmodify;
//    int i;

    TRACE(("    WinIMM32: InputEvent\n"));	/*YA*/
    /* KeyPress$B0J30$O<N$F$k(B */
    TRACE(("Key Event(type = %d, time = %ld, keycode = %d ignore = %d).\n", event->type, ev->time, ev->keycode, g_ignore_key));
    if (event->type != KeyPress /*&& event->type != KeyRelease*/)
    {
	TRACE(("      Not KeyPress Event(type = %d, time = %ld, keycode = %d).\n", event->type, ev->time, ev->keycode));
	return 0;
    }

    if (ev->time > LOCALEVENT_MAX)
    {
	if (ev->keycode == 255)
	{
	    TRACE(("      IME Proccessed Event.\n"));
	    return 0;
	} else
	{
	    // dummy$B$N%$%Y%s%H$G$O$J$$$N$G%/%i%$%"%s%H$KLa$9(B
	    TRACE(("      Not Dummy Event.\n"));

	    // $B=*N;$N%-!<%7!<%1%s%9$+$I$&$+D4$Y$k(B
	    if (checkIMEKey(ev) == TRUE)
	    {
		convend(obj);
		return 0;
	    }

	    return 1;
	}
    }

TRACE(("      time: %d\n", ev->time));

    /* $B%Y%k$rLD$i$9%G%#%9%W%l%$$N@_Dj(B */
    displaybell = XtDisplayOfObject((Widget)obj);

    switch (ev->time)
    {
	case 1:		// $B=*$o$j(B
	    fix(obj);
	    // fallthrough
	case 0:		// $BJ8;zF~NO$"$j(B
	    {
TRACE(("*** XtCallCallbackList ***: textchangecallback\n"));
		XtCallCallbackList(w, obj->inputConv.textchangecallback, (XtPointer)NULL);
	    }

	    /* $BF~NO%b!<%I$r%A%'%C%/$9$k(B */
TRACE(("    Call XWinIMEGetConversionStatus(1)...\n"));	/*YA*/
	    XWinIMEGetConversionStatus(obj->winimm32.dpy, -1, &fopen, &conversion, &sentence, &fmodify);
	    if (fmodify)
	    {
TRACE(("*** XtCallCallbackList ***: modechangecallback\n"));
		XtCallCallbackList(w, obj->inputConv.modechangecallback, (XtPointer)NULL);
	    }
// >> $B8uJd%&%#%s%I%&$N:BI8$r$"$i$+$8$a@_Dj$9$k(B
	    {
		ICSelectionControlArg arg;

		arg.command = ICSelectionCalc;
		arg.u.selection_kind = 0;	// 0 $B$O%@%_!<(B
TRACE(("*** XtCallCallbackList ***: selectioncallback, ICSelectionCalc\n"));
		XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback, (XtPointer)&arg);
	    }
// << $B8uJd%&%#%s%I%&$N:BI8$r$"$i$+$8$a@_Dj$9$k(B
	    break;
	case 2:		// $B=*N;%A%'%C%/(B
	    convend(obj);
	    break;
	case 3:		// $B8uJd%&%#%s%I%&(B
	    {
		ICSelectionControlArg arg;

		if (ev->subwindow == -1)
		{
		    arg.command = ICSelectionEnd;
		    arg.u.current_item = -1;
TRACE(("*** XtCallCallbackList ***: selectioncallback, ICSelectionEnd\n"));
		    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback, (XtPointer)&arg);
		} else
		{
		    arg.command = ICSelectionStart;
		    arg.u.selection_kind = ev->subwindow;
TRACE(("*** XtCallCallbackList ***: selectioncallback, ICSelectionStart\n"));
		    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback, (XtPointer)&arg);
		}
	    }
	    break;
//	case 4:		// $B$+$J4A3+;O(B
//	    break;
    }

    return 0;
}

/*
 * ICString *GetMode(Widget object)
 *	$B8=:_$NF~NO%b!<%I$r(B ICString $B$N7A<0$GJV$9(B
 *	$B%G!<%?$O(B InputConvObject $B$N$b$N$J$N$G>!<j$KCM$rJQ99$7$?$j(B
 *	free() $B$7$F$O$J$i$J$$(B
 */
static ICString *
GetMode(Widget w)
{
//    WinIMM32Object obj = (WinIMM32Object)w;
//    wchar *mode;
    int len;
    static ICString icstr;

TRACE(("    WinIMM32: GetMode\n"));	/*YA*/
    icstr.data = (char *)Xwinimm32GetModeStr(w, &len);

    icstr.nchars = len;
    icstr.nbytes = icstr.nchars * sizeof(wchar);
    icstr.attr = ICAttrNormalString;

    return &icstr;
}

/*
 * int CursorPos(Widget object, Cardinal *segidx, Cardinal *offset)
 *	$B8=:_$N%$%s%5!<%H%+!<%=%k0LCV$rJV$9(B
 *	$B8=:_%+!<%=%k$,$J$$;~$K$O(B 0 $B$"$k;~$K$O(B 1 $B$,JV$k(B
 *	1 $B$,JV$5$l$?;~$K$O!"(Bsegidx $B$K%+!<%=%k$N$"$k%;%0%a%s%HHV9f!"(B
 *	offset $B$K%;%0%a%s%HFb$N0LCV(B($B@hF,$+$i2?J8;zL\$+(B) $B$,JV$5$l$k(B
 */
static int
CursorPos(Widget w, Cardinal *nsegp, Cardinal *ncharp)
{
    WinIMM32Object obj = (WinIMM32Object)w;
    int cursor, numClause, curClause, offset, context;
    Bool bResult;

TRACE(("    WinIMM32: CursorPos\n"));	/*YA*/
TRACE(("    Call XWinIMEGetCursorPosition...\n"));	/*YA*/
    bResult = XWinIMEGetCursorPosition (obj->winimm32.dpy, -1, &cursor, &numClause, &curClause, &offset);
    if (bResult == True)
    {
	if (nsegp != NULL)
	    *nsegp = curClause;
	if (ncharp != NULL)
	    *ncharp = offset;
TRACE(("    WinIMM32: CursorPos return: curClause = %d, offset = %d\n", curClause, offset));	/*YA*/
	return 1;
    } else
    {
	if (nsegp != NULL)
	    *nsegp = 0;
	if (ncharp != NULL)
	    *ncharp = 0;
TRACE(("    WinIMM32: CursorPos return: no cursor.\n"));	/*YA*/
	return 0;
    }
}

/*
 * int NumSegments(Widget object)
 *	$B%;%0%a%s%H?t$rJV$9(B
 */
static int
NumSegments(Widget w)
{
    WinIMM32Object obj = (WinIMM32Object)w;
    int cursor, numClause, curClause, offset;

    Bool bResult;

TRACE(("    WinIMM32: NumSegments\n"));	/*YA*/
TRACE(("    Call XWinIMEGetCursorPosition...\n"));	/*YA*/
    bResult = XWinIMEGetCursorPosition (obj->winimm32.dpy, -1, &cursor, &numClause, &curClause, &offset);
    if (bResult == True)
    {
TRACE(("    WinIMM32: Segments Num = %d\n", numClause));	/*YA*/
	return numClause;
    } else
    {
TRACE(("    WinIMM32: ERROR.\n"));	/*YA*/
	return 0;
    }
}

/*
 * ICString *GetSegment(Widget object, Cardinal n)
 *	n $BHVL\$N%;%0%a%s%H$rJV$9(B ($B@hF,$N%;%0%a%s%H$O(B n=0)
 *	$B;XDj$5$l$?%;%0%a%s%H$,B8:_$7$J$1$l$P(B NULL $B$rJV$9(B
 *	$B%G!<%?$O(B InputConvObject $B$N$b$N$J$N$G>!<j$KCM$rJQ99$7$?$j(B
 *	free() $B$7$F$O$J$i$J$$(B
 */
static ICString *
GetSegment(Widget w, Cardinal n)
{
    WinIMM32Object obj = (WinIMM32Object)w;
    int attr;
    int numChars;
    static wchar data[STRING_BUFFER_SIZE];

    static ICString seg;

TRACE(("    WinIMM32: GetSegment\n"));	/*YA*/
TRACE(("    Call XWinIMEGetTargetClause...\n"));	/*YA*/
    numChars = XWinIMEGetTargetClause(obj->winimm32.dpy, -1, n, data, &attr);

    if (numChars == -1)
    {
	TRACE(("    WinIMM32: GetSegment ERROR.\n"));
	return NULL;
    }

    data[numChars] = (wchar)0;

    seg.data = (char *)data;
    seg.nchars = numChars;
    seg.nbytes = numChars * sizeof(wchar);
    seg.attr = attr;

TRACE(("    WinIMM32: GetSegment return seg.nchars = %d\n", seg.nchars));	/*YA*/

{
	int i;
	for (i=0; i< numChars + 1; i++)
	{
		TRACE(("        0x%X\n", data[i]));
	}
	TRACE(("\n"));
}

    if (seg.nchars == 0)
    {
	TRACE(("    WinIMM32: GetSegment return NULL.\n"));
	return NULL;
    } else
	return &seg;
}

/*
 * int CompareSegment(Widget object, ICString *seg1, ICString *seg2,
 *			Cardinal *nchar)
 *	$BFs$D$N%;%0%a%s%H$rHf3S$9$k(B
 *	$BHf3S7k2L$O4X?t$NCM$H$7$FJV$5$l$k(B
 *	nchar $B$K%;%0%a%s%H$N@hF,$+$i$N0lCW$9$kJ8;z?t$,JV$5$l$k(B
 */
/* ARGSUSED */
// $B$?$V$s$3$l$O$3$N$^$^$G$h$$(B	Y.Arai
static int
CompareSegment(Widget w, ICString *seg1, ICString *seg2, Cardinal *n)
{
    wchar *p, *q;
    int len, nsame;
    int result = 0;

TRACE(("    WinIMM32: CompareSegment\n"));	/*YA*/
    if (seg1->attr != seg2->attr) result |= ICAttrChanged;
TRACE(("    a\n"));

    len = seg1->nchars > seg2->nchars ? seg2->nchars : seg1->nchars;
    nsame = 0;
    p = (wchar *)seg1->data;
    q = (wchar *)seg2->data;
TRACE(("      nsame = %d, len = %d\n",nsame, len));	/*YA*/
    while (nsame < len && *p++ == *q++) nsame++;
TRACE(("      nsame = %d, len = %d\n",nsame, len));	/*YA*/

    if (nsame != len || len != seg1->nchars || len != seg2->nchars)
	result |= ICStringChanged;

    if (n) *n = nsame;

TRACE(("      len = %d\n",len));	/*YA*/
    return result;
}

/*
 * ICString *GetItemList(Widget object, Cardinal *num_items)
 *	$BA*Br9`L\$N%j%9%H$rJV$9(B
 *	$B9`L\A*BrCf$G$J$1$l$P(B NULL $B$rJV$9(B
 *	num_items $B$K9`L\?t$,JV$5$l$k(B
 *	$B%G!<%?$O(B InputConvObject $B$N$b$N$J$N$G>!<j$KCM$rJQ99$7$?$j(B
 *	free() $B$7$F$O$J$i$J$$(B
 *	$B$3$N4X?t$NJV$9%j%9%H$OA*BrCf(B (ICSelectionStart $B$N%3!<%k%P%C%/$+$i(B
 *	ICSelectionEnd $B$N%3!<%k%P%C%/$^$?$O(B ICSelectItem() $B$,%3!<%k$5$l$k$^$G(B)
 *	$BM-8z$G$"$k(B
 */
/* $B$H$j$"$($:8F$P$l$J$$2DG=@-$,$"$k$N$G(BNULL$B$rJV$7$F$*$/(B */
static ICString *
GetItemList(Widget w, Cardinal *n)
{
//    WinIMM32Object obj = (WinIMM32Object)w;

TRACE(("    WinIMM32: GetItemList\n"));	/*YA*/
    return NULL;
}

#define SELECTBUFSIZE 1024

/*
 * int SelectItem(Widget object, int n)
 *	n $BHVL\$N9`L\$,A*Br$5$l$?$3$H$r%*%V%8%'%/%H$KCN$i$;$k(B
 *	n < 0 $B$J$i$I$N9`L\$bA*Br$5$l$J$+$C$?$3$H$r<($9(B
 *	$B9`L\A*BrCf$G$J$+$C$?$j;XDj$7$?9`L\$,B8:_$7$J$1$l$P(B -1 $B$rJV$9(B
 *	$B$=$l0J30$O(B 0 $B$rJV$9(B
 *	XtNselectionControl $B%3!<%k%P%C%/Cf$G$3$N4X?t$r8F$VI,MW$O$J$$(B
 *	(ICSelectionControlArg.u.current_item $B$K%+%l%s%H$N9`L\HV9f$r(B
 *	$BBeF~$9$k$@$1$G$h$$(B)
 */
static int
SelectItem(Widget w, int n)
{
// $B$3$J$$$h$&$K$7$?$D$b$j!J$-$?$i%P%0!K(B
TRACE(("    WinIMM32: SelectItem\n"));	/*YA*/
    return -1; /* Not connected */
}

/*
 * int ConvertedString(Widget object, Atom *encoding, int *format,
 *			    int *length, XtPointer *string)
 *	$BJQ49$5$l$?%F%-%9%H$r(B string $B$KJV$9(B
 *	encoding $B$K$O!"%F%-%9%H$N%(%s%3!<%G%#%s%0$r;XDj$7$F$*$/(B
 *	$B$?$@$7$3$l$OC1$J$k%j%/%(%9%H$G$"$C$F!"JQ49%*%V%8%'%/%H$O(B
 *	$BJL$N%(%s%3!<%G%#%s%0$GJV$7$F$b$h$$(B
 *	encoding $B$K$O<B:]$N%(%s%3!<%G%#%s%0$,JV$5$l$k(B
 *	$BJQ49%*%V%8%'%/%H$O>/$J$/$H$b(B COMPOUND_TEXT $B%(%s%3!<%G%#%s%0$O(B
 *	$B%5%]!<%H$7$J$/$F$O$J$i$J$$(B
 *	format $B$K$O(B 8/16/32 $B$N$$$:$l$+!"(Blength $B$O(B string $B$N%(%l%a%s%H?t$,(B
 *	$B$=$l$>$lJV$5$l$k(B
 *	$B%F%-%9%H$NNN0h$O(B malloc $B$5$l$F$$$k$N$G$3$N4X?t$r8F$s$@B&$G(B
 *	free $B$7$J$1$l$P$J$i$J$$(B
 *	$BJQ49%F%-%9%H$,$J$$;~$d%(%i!<$N>l9g$K$O(B -1$B!"$=$&$G$J$1$l$P(B 0 $B$,(B
 *	$B4X?t$NCM$H$7$FJV$5$l$k(B
 *
 *	$B$3$N4X?t$O(B XtNfixNotify $B%3!<%k%P%C%/$NCf$G;H$o$l$k$3$H$rA[Dj$7$F$$$k(B
 */
static int
ConvertedString(Widget w, Atom *encoding, int *format, int *length, XtPointer *string)
{
    WinIMM32Object obj = (WinIMM32Object)w;
    static wchar szCompositionString[STRING_BUFFER_SIZE];
    int numChar;

TRACE(("    WinIMM32: ConvertedString\n"));	/*YA*/
    *encoding = XA_COMPOUND_TEXT(XtDisplayOfObject((Widget)obj));
    *format = 8;

TRACE(("    Call XWinIMEGetCompositionString...\n"));	/*YA*/
    numChar = XWinIMEGetCompositionString(obj->winimm32.dpy, -1, WinIMECMPResultStr, STRING_BUFFER_SIZE, szCompositionString);
    *length = numChar * sizeof(wchar);

    szCompositionString[numChar] = (wchar)0;
    if (*length < 0)
    {
	TRACE(("  XWinIMEGetCompositionString failed.\n"));
	string = NULL;
	return -1;
    }

    // $BLa$9$?$a$N%P%C%U%!$r3NJ](B
    *length = convJWStoCT(szCompositionString, (unsigned char *)NULL, 0);
    *string = XtMalloc(*length + 1);
    (void)convJWStoCT(szCompositionString, (unsigned char *)*string, 0);

    return 0;

//
//    shiftLeftAll(ib); $B$KAjEv$9$k:n6H$O$$$i$J$$!)(B
}

/*
 * int ClearConversion(Widget object)
 *	$B6/@)E*$K(B ($BJQ49ESCf$G$"$C$F$b(B) $BJQ49%F%-%9%H$r%/%j%"$9$k(B
 *	$B2?$i$+$NM}M3$G%/%j%"$G$-$J$+$C$?;~$K$O(B -1$B!"$=$&$G$J$1$l$P(B 0 $B$,(B
 *	$BJV$5$l$k(B
 */
static int
ClearConversion(Widget w, Boolean fin)
{
    WinIMM32Object obj = (WinIMM32Object)w;
    BOOL fModify;

TRACE(("    WinIMM32: ClearConversion\n"));	/*YA*/
    if (fin)
    {
	return 0;
    }
TRACE(("    Call XWinIMEClearContext...\n"));	/*YA*/
    XWinIMEClearContext(obj->winimm32.dpy, -1, &fModify);

    if (fModify == TRUE)
    {
TRACE(("*** XtCallCallbackList ***: textchangecallback\n"));
	XtCallCallbackList((Widget)obj, obj->inputConv.textchangecallback, (XtPointer)NULL);
    }
    return 0;
}

/*
 * ICString *GetAuxSegments(Widget object, Cardinal *n,
 *                            Cardinal *ns, Cardinal *nc)
 *	AUX$BNN0h$N%;%0%a%s%H$rJV$9!#(B
 *	$B;XDj$5$l$?%;%0%a%s%H$,B8:_$7$J$1$l$P(B NULL $B$rJV$9(B
 *	$B%G!<%?$O(B InputConvObject $B$N$b$N$J$N$G>!<j$KCM$rJQ99$7$?$j(B
 *	free() $B$7$F$O$J$i$J$$(B
 *      $BF@$i$l$?%;%0%a%s%H?t!"%+%l%s%H%;%0%a%s%H!"%+%l%s%H%;%0%a%s%H(B
 *      $BFb$N%+!<%=%k%]%8%7%g%s$,$=$l$>$l(B n, ns, nc $B$K$FJV$k!#(B
 */
static ICString *
GetAuxSegments(Widget w, Cardinal *n, Cardinal *ns, Cardinal *nc)
{
//  WinIMM32Object obj = (WinIMM32Object)w;

TRACE(("    WinIMM32: GetAuxSegments\n"));	/*YA*/

  return NULL;
}

/*
 * int GetTriggerKeys(WidgetClass objectclass,
 *					  ICTriggerKey *keys_return)
 *	$B;XDj$5$l$?F~NO%*%V%8%'%/%H%/%i%9$KEPO?$5$l$F$$$kF|K\8l(B
 *	$BF~NO3+;O%-!<$r<h$j=P$9!#(B
 */
/* ARGSUSED */
static int
GetTriggerKeys(Widget w, ICTriggerKey **keys_return)
{
TRACE(("    WinIMM32: GetTriggerKeys\n"));	/*YA*/
//    *keys_return = NULL;
    return 0;
}

/*
 * int PreeditString(Widget object, int segn, int offset,
 *			  Atom *encoding, int *format,
 *			  int *length, XtPointer *string)
 *	$BJQ49ESCf$N(B segn $BHVL\$N(B $B%;%0%a%s%H$N(B offset $BJ8;z$+$i$N%F%-%9%H$r(B
 *	string $B$KJV$9(B
 *	encoding $B$K$O!"%F%-%9%H$N%(%s%3!<%G%#%s%0$r;XDj$7$F$*$/(B
 *	$B$?$@$7$3$l$OC1$J$k%j%/%(%9%H$G$"$C$F!"JQ49%*%V%8%'%/%H$O(B
 *	$BJL$N%(%s%3!<%G%#%s%0$GJV$7$F$b$h$$(B
 *	encoding $B$K$O<B:]$N%(%s%3!<%G%#%s%0$,JV$5$l$k(B
 *	$BJQ49%*%V%8%'%/%H$O>/$J$/$H$b(B COMPOUND_TEXT $B%(%s%3!<%G%#%s%0$O(B
 *	$B%5%]!<%H$7$J$/$F$O$J$i$J$$(B
 *	format $B$K$O(B 8/16/32 $B$N$$$:$l$+!"(Blength $B$O(B string $B$N%(%l%a%s%H?t$,(B
 *	$B$=$l$>$lJV$5$l$k(B
 *	$B%F%-%9%H$NNN0h$O(B malloc $B$5$l$F$$$k$N$G$3$N4X?t$r8F$s$@B&$G(B
 *	free $B$7$J$1$l$P$J$i$J$$(B
 *	$BJQ49%F%-%9%H$,$J$$;~$d%(%i!<$N>l9g$K$O(B -1$B!"$=$&$G$J$1$l$P(B 0 $B$,(B
 *	$B4X?t$NCM$H$7$FJV$5$l$k(B
 */
/* ARGSUSED */
static int
PreeditString(Widget w, int segn, int offset, Atom *encoding, int *format, int *length, XtPointer *string)
{
    WinIMM32Object obj = (WinIMM32Object)w;
    static wchar szCompositionString[STRING_BUFFER_SIZE];	// Compound Text
    int numChar;

TRACE(("    WinIMM32: PreeditString (segn = %d)\n", segn));	/*YA*/
    *encoding = XA_COMPOUND_TEXT(XtDisplayOfObject((Widget)obj));
    *format = 8;

TRACE(("    Call XWinIMEGetTargetString...\n"));	/*YA*/
//    numChar = XWinIMEGetTargetString(obj->winimm32.dpy, -1, WinIMECMPCompStr, STRING_BUFFER_SIZE, szCompositionString);
    numChar = XWinIMEGetTargetString(obj->winimm32.dpy, -1, segn, offset, szCompositionString);
    *length = numChar * sizeof(wchar);

    szCompositionString[numChar] = (wchar)0;
    if (*length < 0)
    {
	TRACE(("  XWinIMEGetTargetString failed.\n"));
	string = NULL;
	return -1;
    }

    // $BLa$9$?$a$N%P%C%U%!$r3NJ](B
    *length = convJWStoCT(szCompositionString, (unsigned char *)NULL, 0);
    *string = XtMalloc(*length + 1);
    (void)convJWStoCT(szCompositionString, (unsigned char *)*string, 0);

    return 0;
}

/*
 * int StatusString(Widget object, Atom *encoding, int *format,
 *			 int *length, XtPointer *string, int *nchars)
 *	$B%9%F!<%?%9%F%-%9%H$r(B string $B$KJV$9(B
 *	encoding $B$K$O!"%F%-%9%H$N%(%s%3!<%G%#%s%0$r;XDj$7$F$*$/(B
 *	$B$?$@$7$3$l$OC1$J$k%j%/%(%9%H$G$"$C$F!"JQ49%*%V%8%'%/%H$O(B
 *	$BJL$N%(%s%3!<%G%#%s%0$GJV$7$F$b$h$$(B
 *	encoding $B$K$O<B:]$N%(%s%3!<%G%#%s%0$,JV$5$l$k(B
 *	$BJQ49%*%V%8%'%/%H$O>/$J$/$H$b(B COMPOUND_TEXT $B%(%s%3!<%G%#%s%0$O(B
 *	$B%5%]!<%H$7$J$/$F$O$J$i$J$$(B
 *	format $B$K$O(B 8/16/32 $B$N$$$:$l$+!"(Blength $B$O(B string $B$N%(%l%a%s%H?t$,(B
 *	$B$=$l$>$lJV$5$l$k(B
 *	nchars $B$K$O(B string $B$NJ8;z?t$,JV$5$l$k(B
 *	$B%F%-%9%H$NNN0h$O(B malloc $B$5$l$F$$$k$N$G$3$N4X?t$r8F$s$@B&$G(B
 *	free $B$7$J$1$l$P$J$i$J$$(B
 *	$BJQ49%F%-%9%H$,$J$$;~$d%(%i!<$N>l9g$K$O(B -1$B!"$=$&$G$J$1$l$P(B 0 $B$,(B
 *	$B4X?t$NCM$H$7$FJV$5$l$k(B
 */
/* ARGSUSED */
static int
StatusString(Widget w, Atom *encoding, int *format, int *length, XtPointer *string, int *nchars)
{
    ICString *seg;
    wchar *wbuf, *wp;
    int len, wlen;
    extern int convJWStoCT();

TRACE(("    WinIMM32: StatusString\n"));	/*YA*/
    seg = GetMode(w);
    if (seg == NULL) {
	*length = *nchars = 0;
	return -1;
    }

    wlen = seg->nchars;
    if (wlen <= 0) {
	*length = *nchars = 0;
	return -1;
    }

    /*
     * data $B$KF~$C$F$$$kJQ49%F%-%9%H$O(B null $B%?!<%_%M!<%H$5$l$F$$$J$$$+$b(B
     * $B$7$l$J$$$N$G!"$^$:%3%T!<$7$F(B null $B%?!<%_%M!<%H$9$k(B
     */
    wbuf = (wchar *)XtMalloc((wlen + 1) * sizeof(wchar));
    (void)bcopy(seg->data, (char *)wbuf, sizeof(wchar) * wlen);
    wbuf[wlen] = 0;

    /*
     * Canna $B%*%V%8%'%/%H$O(B COMPOUND_TEXT $B%(%s%3!<%G%#%s%0$7$+%5%]!<%H$7$J$$(B
     * COMPOUND_TEXT $B$KJQ49$9$k(B
     */
    *encoding = XA_COMPOUND_TEXT(XtDisplayOfObject(w));
    *format = 8;

    /* COMPOUND_TEXT $B$O(B \r $B$,Aw$l$J$$$N$G(B \n $B$KJQ49$7$F$*$/(B */
    for (wp = wbuf; *wp != 0; wp++) {
	if (*wp == '\r') *wp = '\n';
    }

    *length = len = convJWStoCT(wbuf, (unsigned char *)NULL, 0);
    *string = XtMalloc(len + 1);
    (void)convJWStoCT(wbuf, (unsigned char *)*string, 0);
    *nchars = seg->nchars;

    /* wbuf $B$r(B free $B$7$F$*$/(B */
    XtFree((char *)wbuf);

    return 0;
}

/* ARGSUSED */
/* $B#O#K!J2>!K(B */
static void
Initialize(Widget req, Widget new, ArgList args, Cardinal *num_args)
{
    WinIMM32Object obj = (WinIMM32Object)new;
    int context = 0;
    Display *dpy;

TRACE(("    WinIMM32: Initialize\n"));	/*YA*/
    if (obj == NULL)
    {
	TRACE(("    WinIMM32: Target object is NULL.\n"));	/*YA*/
    }
    if (req == NULL)
    {
	TRACE(("    WinIMM32: Request object is NULL.\n"));	/*YA*/
    }
    TRACE(("    WinIMM32: XtDisplayOfObject...\n"));	/*YA*/
    dpy = XtDisplayOfObject(req);
    if (dpy == NULL)
    {
	TRACE(("    WinIMM32: Can't get display.\n"));	/*YA*/
    }
    obj->winimm32.dpy = dpy;
#if 0
TRACE(("    Call XWinIMEGetLastContext...\n"));	/*YA*/
    if (XWinIMEGetLastContext (dpy, &context))
    {
	obj->winimm32.context = context;
    } else
    {
	obj->winimm32.context = 0;
    }
#endif
#if 0	// $BJQ$o$i$J$+$C$?$N$G:o=|(B
// >> atoc.c$B$N$^$M(B
TRACE(("*** XtCallCallbackList ***: modechangecallback\n"));
	XtCallCallbackList(obj, obj->inputConv.modechangecallback, (XtPointer)NULL);
// << atoc.c$B$N$^$M(B
#endif
TRACE(("    WinIMM32: Initialize end(context = #%d).\n", context));	/*YA*/
}

/* $B#O#K!J$=$N$^$^!K(B */
static int
bell()
{
  if (displaybell) {
    XBell(displaybell, 0);
  }
  return 0;
}


static void
Destroy(Widget w)
{
    WinIMM32Object obj = (WinIMM32Object)w;

TRACE(("    WinIMM32: Destroy\n"));	/*YA*/
    deleteObject(obj);
}

static Boolean
SetValues(Widget cur, Widget req, Widget wid, ArgList args, Cardinal *num_args)
/* ARGSUSED */
{
TRACE(("    WinIMM32: SetValues\n"));	/*YA*/
    WinIMM32Object old = (WinIMM32Object)cur;
    WinIMM32Object new = (WinIMM32Object)wid;

    return False;
}

static void
fix(WinIMM32Object obj)
{
TRACE(("    WinIMM32: fix\n"));	/*YA*/
    /* $B3NDj$N=hM}(B */
TRACE(("*** XtCallCallbackList ***: fixcallback\n"));
    XtCallCallbackList((Widget)obj, obj->inputConv.fixcallback,
		       (XtPointer)NULL);	/* $B!)!)!)(B */
}

static void
convend(WinIMM32Object obj)
{
TRACE(("    WinIMM32: convend\n"));	/*YA*/
TRACE(("*** XtCallCallbackList ***: endcallback\n"));
    XtCallCallbackList((Widget)obj, obj->inputConv.endcallback,
		       (XtPointer)NULL);
}

/*
 * keeping list of objects
 */
typedef struct _oblist_ {
    WinIMM32Object obj;
    struct _oblist_ *next;
} ObjRec;

static ObjRec *ObjList = NULL;

#if 0	/// $B$H$j$"$($:;H$o$J$$(B
static void
addObject(WinIMM32Object obj)
{
    ObjRec *objp = XtNew(ObjRec);

    objp->obj = obj;
    objp->next = ObjList;
    ObjList = objp;
}
#endif

static void
deleteObject(WinIMM32Object obj)
{
    ObjRec *objp, *objp0;

    for (objp0 = NULL, objp = ObjList;
	 objp != NULL;
	 objp0 = objp, objp = objp->next)
    {
	if (objp->obj == obj)
	{
	    if (objp0 == NULL)
	    {
		ObjList = objp->next;
	    } else
	    {
		objp0->next = objp->next;
	    }
	    XtFree((char *)objp);
	    return;
	}
    }
}

/*
 * Operations to canna.ibuf
 */

#if 0	// $B$?$V$s$$$i$J$$(B
/* cfuncdef

   changeTextForCanna -- ibuf $B$NFbMF$r(B kanji_status $B$rMQ$$$F=q$-49$($k!#(B


   $B4pK\E*$K$O8uJdJ8;zNs$H8uJd0lMw9TJ8;zNs$K4X$7$F0J2<$N=hM}$r9T$&!#(B

   (1) $B$$$:$l$NJ8;zNs$b%F%-%9%H$,H?E>$7$F$$$kItJ,$H$=$&$G$J$$ItJ,(B
       $B$,B8:_$7!"H?E>$7$F$$$kItJ,$O#1%+=j$7$+B8:_$7$J$$!#(B
   (2) $B$7$?$,$C$F$$$:$l$NJ8;zNs$bH?E>$7$F$$$k$H$3$m$H$=$NN>C<$KH?E>(B
       $B$7$F$$$J$$ItJ,$H$,B8:_$9$k>l9g$K#3ItJ,$KJ,$+$l$k$3$H$K$J$j!"(B
       $BH?E>$7$F$$$kItJ,$N0LCV$d!"H?E>$7$F$$$k2U=j$,$J$$>l9g$J$I$r(B
       $B9g$o$;$F9M$($F$b#3ItJ,0J>e$KJ,$+$l$k$3$H$O$J$$!#(B
   (3) $B$7$?$,$C$F!"$$$:$l$NJ8;zNs$b:GBg#3$D$N%;%0%a%s%H$KJ,$1$FI=<((B
       $B%&%#%8%'%C%H$KEO$9$h$&$K$9$k!#(B

 */

static void
changeTextForWinIMM32(caddr_t cldata, int *ksp)
{
/* TBD */
}
#endif

/* cfuncdef

   copyInWchar -- wchar $B$r%3%T!<$9$k!#(B

   ws, wlen $B$G<($5$l$?(B wchar $BJ8;zNs$r(B wsbuf $B$N%]%$%s%H@h$N%P%C%U%!$K3J(B
   $BG<$9$k!#(Bwsbuf $B$N%5%$%:$O(B wssize $B$N%]%$%s%H@h$K3JG<$5$l$F$$$kCM$G;X(B
   $BDj$5$l$k$,!"$=$l$G$O>.$5$$;~$O(B copyInWchar $BFb$G(B XtRealloc $B$5$l!"?7(B
   $B$?$K%"%m%1!<%H$5$l$?%P%C%U%!$,(B wsbuf $B$N%]%$%s%H@h$K3JG<$5$l$k!#$^$?!"(B
   $B%P%C%U%!$N?7$?$J%5%$%:$,(B wssize $B$N%]%$%s%H@h$K3JG<$5$l$k!#F@$i$l$?(B
   $BJ8;z?t$,(Bwslen $B$N%]%$%s%H@h$K3JG<$5$l$k!#(B

 */

static void
copyInWchar(wchar *ws, int wlen, wchar **wsbuf, int *wssize, int *wslen)
{
    int i;

    if (*wssize == 0) {
	*wsbuf = (wchar *)XtMalloc((wlen + 1) * sizeof(wchar));
	*wssize = wlen + 1;
    }
    if (wlen + 1 > *wssize) {
	*wsbuf = (wchar *)XtRealloc((char *)*wsbuf, (wlen + 1) * sizeof(wchar));
	*wssize = wlen + 1;
    }
    *wslen = wlen;
    (void)bcopy(ws, *wsbuf, wlen * sizeof(wchar));
    *(*wsbuf + wlen) = (wchar)0;
}

/* cfuncdef

   fixProcForCanna -- $B3NDj$7$?J8;z$N=hM}$r9T$&(B

 */

static void
fixProcForWinIMM32(caddr_t cldata, wchar *fixedstr, int fixedlen)
{
/* TBD */
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

/*
 */
static WCHAR *Xwinimm32GetModeStr(Widget w, int *length)
{
    WinIMM32Object obj = (WinIMM32Object)w;
    Bool fopen;
    DWORD conversion;
    DWORD sentence;
    Bool fmodify;

TRACE(("    Call XWinIMEGetConversionStatus(2)...\n"));	/*YA*/
    XWinIMEGetConversionStatus(obj->winimm32.dpy, -1, &fopen, &conversion, &sentence, &fmodify);

    if (fopen == 0)
    {	// IME$B$,%/%m!<%:Cf(B
	*length = wcslen(_wcsModeNames[0]);
	return _wcsModeNames[0];
    } else
    {	// $B%*!<%W%sCf$O2?$b9M$($:$K(B"[ $B$"(B ]"
	*length = wcslen(_wcsModeNames[1]);
	return _wcsModeNames[1];
    }
}

