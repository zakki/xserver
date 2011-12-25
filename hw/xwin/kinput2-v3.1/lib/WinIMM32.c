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
#define _WCHAR_T /* この定義は jrkanji.h で wcKanjiStatus などを定義するため */
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

// >> 必須
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
// << 必須

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
// Wide Char で記述すること
static wchar_t
_wcsModeNames[MODE_MAX_NUM][10] = 
{   /* ここに設定するWideCharは最大10文字(20byte)にしてください */
    {0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0000},
    {0x005B, 0x0020, 0xA4A2, 0x0020, 0x005D, 0x0000},		/*[ あ ]*/
    {0x005B, 0xB4C1, 0xBBFA, 0x005D, 0x0000},			/*[漢字]*/
    {0x005B, 0xB0EC, 0xCDF7, 0x005D, 0x0000}			/*[一覧]*/
};
#else
// Compound Text で記述すること
static
char *
_sModeNames[MODE_MAX_NUM] = 
{   /* ここに設定するCompound-Textは最大20文字にしてください */
    "      ",			        /* AlphaMode */
    "\033(B[ \033$(B\044\042\033(B ]"	/*[ あ ]*/  /* EmptyMode */ 
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
  /* 何もしない */
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
 *	オブジェクトに入力イベントを与える
 *	エラーが起こった場合は -1 が返る
 *	エラーでなければ 0 か 1 が返る
 *	通常は 0 が返るが、もし 1 が返ってきたら、それはできるならば
 *	その入力イベントをアプリケーションに送り返した方がよいという
 *	オブジェクトからのヒントである
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
    /* KeyPress以外は捨てる */
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
	    // dummyのイベントではないのでクライアントに戻す
	    TRACE(("      Not Dummy Event.\n"));

	    // 終了のキーシーケンスかどうか調べる
	    if (checkIMEKey(ev) == TRUE)
	    {
		convend(obj);
		return 0;
	    }

	    return 1;
	}
    }

TRACE(("      time: %d\n", ev->time));

    /* ベルを鳴らすディスプレイの設定 */
    displaybell = XtDisplayOfObject((Widget)obj);

    switch (ev->time)
    {
	case 1:		// 終わり
	    fix(obj);
	    // fallthrough
	case 0:		// 文字入力あり
	    {
TRACE(("*** XtCallCallbackList ***: textchangecallback\n"));
		XtCallCallbackList(w, obj->inputConv.textchangecallback, (XtPointer)NULL);
	    }

	    /* 入力モードをチェックする */
TRACE(("    Call XWinIMEGetConversionStatus(1)...\n"));	/*YA*/
	    XWinIMEGetConversionStatus(obj->winimm32.dpy, -1, &fopen, &conversion, &sentence, &fmodify);
	    if (fmodify)
	    {
TRACE(("*** XtCallCallbackList ***: modechangecallback\n"));
		XtCallCallbackList(w, obj->inputConv.modechangecallback, (XtPointer)NULL);
	    }
// >> 候補ウィンドウの座標をあらかじめ設定する
	    {
		ICSelectionControlArg arg;

		arg.command = ICSelectionCalc;
		arg.u.selection_kind = 0;	// 0 はダミー
TRACE(("*** XtCallCallbackList ***: selectioncallback, ICSelectionCalc\n"));
		XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback, (XtPointer)&arg);
	    }
// << 候補ウィンドウの座標をあらかじめ設定する
	    break;
	case 2:		// 終了チェック
	    convend(obj);
	    break;
	case 3:		// 候補ウィンドウ
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
//	case 4:		// かな漢開始
//	    break;
    }

    return 0;
}

/*
 * ICString *GetMode(Widget object)
 *	現在の入力モードを ICString の形式で返す
 *	データは InputConvObject のものなので勝手に値を変更したり
 *	free() してはならない
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
 *	現在のインサートカーソル位置を返す
 *	現在カーソルがない時には 0 ある時には 1 が返る
 *	1 が返された時には、segidx にカーソルのあるセグメント番号、
 *	offset にセグメント内の位置(先頭から何文字目か) が返される
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
 *	セグメント数を返す
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
 *	n 番目のセグメントを返す (先頭のセグメントは n=0)
 *	指定されたセグメントが存在しなければ NULL を返す
 *	データは InputConvObject のものなので勝手に値を変更したり
 *	free() してはならない
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
 *	二つのセグメントを比較する
 *	比較結果は関数の値として返される
 *	nchar にセグメントの先頭からの一致する文字数が返される
 */
/* ARGSUSED */
// たぶんこれはこのままでよい	Y.Arai
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
 *	選択項目のリストを返す
 *	項目選択中でなければ NULL を返す
 *	num_items に項目数が返される
 *	データは InputConvObject のものなので勝手に値を変更したり
 *	free() してはならない
 *	この関数の返すリストは選択中 (ICSelectionStart のコールバックから
 *	ICSelectionEnd のコールバックまたは ICSelectItem() がコールされるまで)
 *	有効である
 */
/* とりあえず呼ばれない可能性があるのでNULLを返しておく */
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
 *	n 番目の項目が選択されたことをオブジェクトに知らせる
 *	n < 0 ならどの項目も選択されなかったことを示す
 *	項目選択中でなかったり指定した項目が存在しなければ -1 を返す
 *	それ以外は 0 を返す
 *	XtNselectionControl コールバック中でこの関数を呼ぶ必要はない
 *	(ICSelectionControlArg.u.current_item にカレントの項目番号を
 *	代入するだけでよい)
 */
static int
SelectItem(Widget w, int n)
{
// こないようにしたつもり（きたらバグ）
TRACE(("    WinIMM32: SelectItem\n"));	/*YA*/
    return -1; /* Not connected */
}

/*
 * int ConvertedString(Widget object, Atom *encoding, int *format,
 *			    int *length, XtPointer *string)
 *	変換されたテキストを string に返す
 *	encoding には、テキストのエンコーディングを指定しておく
 *	ただしこれは単なるリクエストであって、変換オブジェクトは
 *	別のエンコーディングで返してもよい
 *	encoding には実際のエンコーディングが返される
 *	変換オブジェクトは少なくとも COMPOUND_TEXT エンコーディングは
 *	サポートしなくてはならない
 *	format には 8/16/32 のいずれか、length は string のエレメント数が
 *	それぞれ返される
 *	テキストの領域は malloc されているのでこの関数を呼んだ側で
 *	free しなければならない
 *	変換テキストがない時やエラーの場合には -1、そうでなければ 0 が
 *	関数の値として返される
 *
 *	この関数は XtNfixNotify コールバックの中で使われることを想定している
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

    // 戻すためのバッファを確保
    *length = convJWStoCT(szCompositionString, (unsigned char *)NULL, 0);
    *string = XtMalloc(*length + 1);
    (void)convJWStoCT(szCompositionString, (unsigned char *)*string, 0);

    return 0;

//
//    shiftLeftAll(ib); に相当する作業はいらない？
}

/*
 * int ClearConversion(Widget object)
 *	強制的に (変換途中であっても) 変換テキストをクリアする
 *	何らかの理由でクリアできなかった時には -1、そうでなければ 0 が
 *	返される
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
 *	AUX領域のセグメントを返す。
 *	指定されたセグメントが存在しなければ NULL を返す
 *	データは InputConvObject のものなので勝手に値を変更したり
 *	free() してはならない
 *      得られたセグメント数、カレントセグメント、カレントセグメント
 *      内のカーソルポジションがそれぞれ n, ns, nc にて返る。
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
 *	指定された入力オブジェクトクラスに登録されている日本語
 *	入力開始キーを取り出す。
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
 *	変換途中の segn 番目の セグメントの offset 文字からのテキストを
 *	string に返す
 *	encoding には、テキストのエンコーディングを指定しておく
 *	ただしこれは単なるリクエストであって、変換オブジェクトは
 *	別のエンコーディングで返してもよい
 *	encoding には実際のエンコーディングが返される
 *	変換オブジェクトは少なくとも COMPOUND_TEXT エンコーディングは
 *	サポートしなくてはならない
 *	format には 8/16/32 のいずれか、length は string のエレメント数が
 *	それぞれ返される
 *	テキストの領域は malloc されているのでこの関数を呼んだ側で
 *	free しなければならない
 *	変換テキストがない時やエラーの場合には -1、そうでなければ 0 が
 *	関数の値として返される
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

    // 戻すためのバッファを確保
    *length = convJWStoCT(szCompositionString, (unsigned char *)NULL, 0);
    *string = XtMalloc(*length + 1);
    (void)convJWStoCT(szCompositionString, (unsigned char *)*string, 0);

    return 0;
}

/*
 * int StatusString(Widget object, Atom *encoding, int *format,
 *			 int *length, XtPointer *string, int *nchars)
 *	ステータステキストを string に返す
 *	encoding には、テキストのエンコーディングを指定しておく
 *	ただしこれは単なるリクエストであって、変換オブジェクトは
 *	別のエンコーディングで返してもよい
 *	encoding には実際のエンコーディングが返される
 *	変換オブジェクトは少なくとも COMPOUND_TEXT エンコーディングは
 *	サポートしなくてはならない
 *	format には 8/16/32 のいずれか、length は string のエレメント数が
 *	それぞれ返される
 *	nchars には string の文字数が返される
 *	テキストの領域は malloc されているのでこの関数を呼んだ側で
 *	free しなければならない
 *	変換テキストがない時やエラーの場合には -1、そうでなければ 0 が
 *	関数の値として返される
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
     * data に入っている変換テキストは null ターミネートされていないかも
     * しれないので、まずコピーして null ターミネートする
     */
    wbuf = (wchar *)XtMalloc((wlen + 1) * sizeof(wchar));
    (void)bcopy(seg->data, (char *)wbuf, sizeof(wchar) * wlen);
    wbuf[wlen] = 0;

    /*
     * Canna オブジェクトは COMPOUND_TEXT エンコーディングしかサポートしない
     * COMPOUND_TEXT に変換する
     */
    *encoding = XA_COMPOUND_TEXT(XtDisplayOfObject(w));
    *format = 8;

    /* COMPOUND_TEXT は \r が送れないので \n に変換しておく */
    for (wp = wbuf; *wp != 0; wp++) {
	if (*wp == '\r') *wp = '\n';
    }

    *length = len = convJWStoCT(wbuf, (unsigned char *)NULL, 0);
    *string = XtMalloc(len + 1);
    (void)convJWStoCT(wbuf, (unsigned char *)*string, 0);
    *nchars = seg->nchars;

    /* wbuf を free しておく */
    XtFree((char *)wbuf);

    return 0;
}

/* ARGSUSED */
/* ＯＫ（仮） */
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
#if 0	// 変わらなかったので削除
// >> atoc.cのまね
TRACE(("*** XtCallCallbackList ***: modechangecallback\n"));
	XtCallCallbackList(obj, obj->inputConv.modechangecallback, (XtPointer)NULL);
// << atoc.cのまね
#endif
TRACE(("    WinIMM32: Initialize end(context = #%d).\n", context));	/*YA*/
}

/* ＯＫ（そのまま） */
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
    /* 確定の処理 */
TRACE(("*** XtCallCallbackList ***: fixcallback\n"));
    XtCallCallbackList((Widget)obj, obj->inputConv.fixcallback,
		       (XtPointer)NULL);	/* ？？？ */
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

#if 0	/// とりあえず使わない
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

#if 0	// たぶんいらない
/* cfuncdef

   changeTextForCanna -- ibuf の内容を kanji_status を用いて書き換える。


   基本的には候補文字列と候補一覧行文字列に関して以下の処理を行う。

   (1) いずれの文字列もテキストが反転している部分とそうでない部分
       が存在し、反転している部分は１カ所しか存在しない。
   (2) したがっていずれの文字列も反転しているところとその両端に反転
       していない部分とが存在する場合に３部分に分かれることになり、
       反転している部分の位置や、反転している箇所がない場合などを
       合わせて考えても３部分以上に分かれることはない。
   (3) したがって、いずれの文字列も最大３つのセグメントに分けて表示
       ウィジェットに渡すようにする。

 */

static void
changeTextForWinIMM32(caddr_t cldata, int *ksp)
{
/* TBD */
}
#endif

/* cfuncdef

   copyInWchar -- wchar をコピーする。

   ws, wlen で示された wchar 文字列を wsbuf のポイント先のバッファに格
   納する。wsbuf のサイズは wssize のポイント先に格納されている値で指
   定されるが、それでは小さい時は copyInWchar 内で XtRealloc され、新
   たにアロケートされたバッファが wsbuf のポイント先に格納される。また、
   バッファの新たなサイズが wssize のポイント先に格納される。得られた
   文字数がwslen のポイント先に格納される。

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

   fixProcForCanna -- 確定した文字の処理を行う

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
    {	// IMEがクローズ中
	*length = wcslen(_wcsModeNames[0]);
	return _wcsModeNames[0];
    } else
    {	// オープン中は何も考えずに"[ あ ]"
	*length = wcslen(_wcsModeNames[1]);
	return _wcsModeNames[1];
    }
}

