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

/*
 * Y.Arai
 * iconvを使うようにしたのでUCS2のままlibWinimeに戻すように変更
 */

#ifdef HAVE_XWIN_CONFIG_H
#include <xwin-config.h>
#endif
#include "win.h"

#define NEED_REPLIES
#define NEED_EVENTS
#include "misc.h"
#include "dixstruct.h"
#include "extnsionst.h"
#include "servermd.h"

#define _WINIME_SERVER_
#include <X11/extensions/winimestr.h>
#include "wmutil/scancodes.h"
#include <imm.h>
#include <wchar.h>

#define CYGIME_DEBUG TRUE

extern Bool g_fIME;
extern HWND g_hwndKeyboardFocus;
extern int g_iNumScreens;
extern HWND g_hwndLastKeyPress;

extern DWORD	g_TriggerKeycode;
extern long	g_TriggerModifier;
extern void SendImeKey (void);

static int WinIMEErrorBase;

static DISPATCH_PROC(ProcWinIMEDispatch);
static DISPATCH_PROC(SProcWinIMEDispatch);

static void WinIMEResetProc(ExtensionEntry* extEntry);

static unsigned char WinIMEReqCode = 0;
static int WinIMEEventBase = 0;

static RESTYPE ClientType, EventType; /* resource types for event masks */
static XID eventResource;

/* Currently selected events */
static unsigned int eventMask = 0;
// static HWND s_hAssociatedWnd = NULL;	// 代わりにpWIC->hWndを使う

static int WinIMEFreeClient (pointer data, XID id);
static int WinIMEFreeEvents (pointer data, XID id);
static void SNotifyEvent(xWinIMENotifyEvent *from, xWinIMENotifyEvent *to);

typedef struct _WinIMEEvent *WinIMEEventPtr;
typedef struct _WinIMEEvent {
  WinIMEEventPtr	next;
  ClientPtr		client;
  XID			clientResource;
  unsigned int		mask;
} WinIMEEventRec;

// とりあえずpszComposition、pszCompositionResultはUCS-2にしておく。
typedef struct _WIContext *WIContextPtr;
typedef struct _WIContext {
  WIContextPtr		pNext;
  int			nContext;
  HIMC			hIMC;
  HWND			hWnd;
  BOOL			fCompositionDraw;
  int			nCursor;
  DWORD			dwCompositionStyle;
  POINT			ptCompositionPos;
  RECT			rcCompositionArea;
  wchar_t		*pszComposition;		// char -> wchar_t	Y.Arai
  wchar_t		*pszCompositionResult;		// char -> wchar_t	Y.Arai
  char			*pAttr;
  int			nAttr;
// >> for mode change check
  char			*pLastAttr;
  int			nLastAttr;
// << for mode change check
  int			nNumClause;	// 文節数					Add Y.Arai
  int			nCurClause;	// 文節番号					Add Y.Arai
  int			nOffset;	// カレント文節でのカーソルのオフセット(文字数)	Add Y.Arai
  DWORD			*pClauseList;	// 中身はバイト数じゃなくて文字数	Add Y.Arai
  wchar_t		*pszClause;	// Add Y.Arai
  BOOL			fActiveStat;	// Y.Arai
  BOOL			fPreeditStart;	// Y.Arai

  int			nCandPosX;
  int			nCandPosY;
  int			nCandPage;
} WIContextRec;

typedef struct _WIClause
{
    int		nNumClause;	// 文節数
    int		nCurClause;	// カレント文節番号
    int		nOffset;	// カーソルのカレント文節内のオフセット
    DWORD	*pClauseList;	// 文政津位置の先頭からのオフセットのリスト＋最後に全体のバイト数
} WIClauseRec;

static int s_nContextMax = 0;
static WIContextPtr s_pContextList = NULL;

typedef struct
{
    HWND hwnd;
    Window window;
} WindowMatch;
//extern WindowPtr *WindowTable;

extern pthread_t			g_ptImServerProc;
extern volatile char dispatchException;

/*
 * Local helper functions
 *
 *
 *
 */
static wchar_t*
GetTargetClause(WIContextPtr pWIC, int target, int *attr, int *nNumChar)
{
//    DWORD from;
//    DWORD to;
    wchar_t *pszClause;

#if CYGIME_DEBUG
  winDebug ("%s\n", __FUNCTION__);
#endif

    if (((pWIC->nNumClause - 1) < target) || (pWIC->pClauseList == NULL))
    {
#if CYGIME_DEBUG
  winDebug ("TargetClause Not Found.\n");
#endif

#if 1	// issue-19対策：
	pszClause = (wchar_t*)calloc(2, 2);
	pszClause[0] = 0x0020;
	pszClause[1] = 0;
	*nNumChar = 1;
	*attr = 0;
	if (pWIC->pszClause != NULL)
	    free (pWIC->pszClause);
	pWIC->pszClause = pszClause;
	return pszClause;
#else	// #if 1	// issue-19対策
	*attr = 0;
	return NULL;
#endif	// #if 1	// issue-19対策
    }

    *nNumChar = (pWIC->pClauseList[target + 1] - pWIC->pClauseList[target]);
    pszClause = (wchar_t*)calloc(2, (*nNumChar) + 1);	// NULLワイド文字分を足す
winDebug("  Size = %ld, target = %ld, target+1 = %ld\n", (pWIC->pClauseList[target + 1] - pWIC->pClauseList[target] + 1) * 2, pWIC->pClauseList[target], pWIC->pClauseList[target + 1]);
    wchar_t* pSrc = &((pWIC->pszComposition)[(pWIC->pClauseList)[target]]);
    memcpy(pszClause, pSrc, (*nNumChar) * 2);

    if (pWIC->pszClause != NULL)
	free (pWIC->pszClause);
    pWIC->pszClause = pszClause;
//    *attr = pWIC->pAttr[target];	// pWIC->pAttrには一文字ごとに入っているのでこれではだめ
    *attr = pWIC->pAttr[pWIC->pClauseList[target]];
{
	int i;
	int num = (pWIC->pClauseList[target + 1] - pWIC->pClauseList[target]);
	wchar_t* pChar;

	pChar = pWIC->pszComposition;
	winDebug("  Top: 0x%X\n", pChar[0]);

	pChar = &((pWIC->pszComposition)[(pWIC->pClauseList)[target]]);

	winDebug("  From: ");
	for (i=0; i<num; i++)
	{
		winDebug("0x%X ", *pChar);
		pChar ++;
	}
	winDebug("\n");

	pChar = pszClause;

	winDebug("  To: ");
	for (i=0; i<num; i++)
	{
		winDebug("0x%X ", *pChar);
		pChar ++;
	}
	winDebug("\n");
}
    return pszClause;
}

// offsetは文字数
static wchar_t*
GetTargetString(WIContextPtr pWIC, int target, int offset)
{
    wchar_t *pszClause;
    int target_len;

#if CYGIME_DEBUG
  winDebug ("%s, target = %d, offset = %d\n", __FUNCTION__, target, offset);
#endif

    if (pWIC->pClauseList == NULL)
    {
#if CYGIME_DEBUG
  winDebug ("TargetString Not Found.\n");
#endif
	pszClause = (wchar_t*)calloc(2,1);
	return pszClause;
    }
    target_len = pWIC->pClauseList[target + 1] - pWIC->pClauseList[target];
    if (target < pWIC->nNumClause && offset >= target_len)
    {
winDebug("      Last segment.\n");	/*YA*/
	target ++;
	offset = 0;
	target_len = pWIC->pClauseList[target + 1] - pWIC->pClauseList[target];
    }
    if (((pWIC->nNumClause - 1) < target) || offset >= target_len)
    {
#if CYGIME_DEBUG
  winDebug ("TargetString Not Found.\n");
#endif
	pszClause = (wchar_t*)calloc(2,1);
	return pszClause;
    }

    int nSize = pWIC->pClauseList[pWIC->nNumClause] - pWIC->pClauseList[target] - offset;
    pszClause = (wchar_t*)calloc(2, nSize + 1);	// NULLワイド文字分を足す
    wchar_t* pSrc = &((pWIC->pszComposition)[(pWIC->pClauseList)[target] + offset]);
    memcpy(pszClause, pSrc, nSize * 2);

    if (pWIC->pszClause != NULL)
	free (pWIC->pszClause);
    pWIC->pszClause = pszClause;
    return pszClause;
}

static WIContextPtr
NewContext(void)
{
  WIContextPtr pWIC;

#if CYGIME_DEBUG
  winDebug ("%s\n", __FUNCTION__);
#endif

  pWIC = (WIContextPtr) malloc(sizeof(WIContextRec));

  if (pWIC)
    {
      /* Init member */
      pWIC->nContext = ++s_nContextMax;
      pWIC->hIMC = ImmCreateContext ();
      ImmSetOpenStatus(pWIC->hIMC, FALSE);	// とりあえずここではOFFにする
      pWIC->hWnd = NULL;	// Add
      pWIC->fCompositionDraw = FALSE;
      pWIC->dwCompositionStyle = CFS_DEFAULT;
      pWIC->pszComposition = NULL;
      pWIC->pszCompositionResult = NULL;
      pWIC->pAttr = NULL;
      pWIC->nAttr = 0;
      pWIC->pLastAttr = NULL;
      pWIC->nLastAttr = 0;
      pWIC->nNumClause = 0;	// Add Y.Arai
      pWIC->nCurClause = 0;	// Add Y.Arai
      pWIC->nOffset = 0;	// Add Y.Arai
      pWIC->pClauseList = NULL;	// Add Y.Arai
      pWIC->pszClause = NULL;	// Add Y.Arai

      pWIC->fActiveStat = FALSE;
      pWIC->fPreeditStart = FALSE;

      pWIC->nCandPosX = 0;
      pWIC->nCandPosY = 0;
      pWIC->nCandPage = 0;

      /* Add to list. */
      pWIC->pNext = s_pContextList;
      s_pContextList = pWIC;

#if CYGIME_DEBUG
      winDebug ("nContext:%d hIMC:0x%X\n", pWIC->nContext, pWIC->hIMC);
#endif
    }

  return pWIC;
}

static BOOL
ClearContext(WIContextPtr pWIC, Bool fAll)
{
    BOOL bRet = FALSE;
#if CYGIME_DEBUG
  winDebug ("%s\n", __FUNCTION__);
#endif

    if (pWIC)
    {
#if CYGIME_DEBUG
	winDebug ("nContext:%d hIMC:%d\n", pWIC->nContext, pWIC->hIMC);
#endif
	// 文節がひとつ以上あれば、これからクリアするので変更ありにマークする
	if (pWIC->nNumClause != 0)
	    bRet = TRUE;
	/* Init member */
	pWIC->fCompositionDraw = FALSE;
	pWIC->dwCompositionStyle = CFS_DEFAULT;
	if (pWIC->pszComposition != NULL)
	{
	    free (pWIC->pszComposition);
	    pWIC->pszComposition = NULL;
	}
	if (fAll && (pWIC->pszCompositionResult != NULL))
	{
	    free (pWIC->pszCompositionResult);
	    pWIC->pszCompositionResult = NULL;
	}
	if (pWIC->pAttr != NULL)
	{
	    free (pWIC->pAttr);
	    pWIC->pAttr = NULL;
	}
	pWIC->nAttr = 0;
	if (pWIC->pLastAttr != NULL)
	{
	    free (pWIC->pLastAttr);
	    pWIC->pLastAttr = NULL;
	}
	pWIC->nLastAttr = 0;
	pWIC->nNumClause = 0;
	pWIC->nCurClause = 0;
	pWIC->nOffset = 0;
	if (pWIC->pClauseList != NULL)
	{
	    free (pWIC->pClauseList);
	    pWIC->pClauseList = NULL;
	}
	if (pWIC->pszClause != NULL)
	{
	    free (pWIC->pszClause);
	    pWIC->pszClause = NULL;
	}
//	pWIC->fActiveStat = FALSE;
    }

#if CYGIME_DEBUG
  winDebug ("%s exit\n", __FUNCTION__);
#endif
    return bRet;
}

static WIContextPtr
FindContext(int nContext)
{
  WIContextPtr pWIC;

#if CYGIME_DEBUG
  winDebug ("%s %d\n", __FUNCTION__, nContext);
#endif

  for (pWIC = s_pContextList; pWIC; pWIC = pWIC->pNext)
    {
      if (pWIC->nContext == nContext)
	{
#if CYGIME_DEBUG
	  winDebug ("found.\n");
#endif
	  return pWIC;
	}
    }

#if CYGIME_DEBUG
  winDebug ("not found.\n");
#endif

  return NULL;
}

// TODO: なぜかfree()のところで死ぬようになったので、とりあえずリストから外さずに
// DeleteAllContext()を頼る（ここではfree()しない）
static void
DeleteContext(int nContext)
{
  WIContextPtr pWIC, pPrev = NULL;

#if CYGIME_DEBUG
  winDebug ("%s %d\n", __FUNCTION__, nContext);
#endif

  for (pWIC = s_pContextList; pWIC; pPrev = pWIC, pWIC = pWIC->pNext)
    {
      if (pWIC->nContext == nContext)
	{
/*
	  if (pPrev)
	    pPrev->pNext = pWIC->pNext;
	  else
	    s_pContextList = pWIC->pNext;
*/

	  ClearContext(pWIC, TRUE);
	  ImmDestroyContext (pWIC->hIMC);
//	  free (pWIC);
	  pWIC->nContext = -1;
	}
    }
#if CYGIME_DEBUG
  winDebug ("%s exit.\n", __FUNCTION__);
#endif
}

static void
DeleteAllContext(void)
{
  WIContextPtr pWIC, pNext = NULL;
  HIMC hIMC;

  for (pWIC = s_pContextList; pWIC; pWIC = pNext)
    {
      pNext = pWIC->pNext;

      // すべてのコンテキストを削除するんだからかな漢もＯＦＦにしよう
      hIMC = ImmGetContext(pWIC->hWnd);
      if (hIMC != NULL)
      {
	if (ImmGetOpenStatus(hIMC) == TRUE)
	{
	  winDebug ("at %s, Close IME...\n", __FUNCTION__);
	  ImmSetOpenStatus(hIMC, FALSE);
	}
	ImmAssociateContext(pWIC->hWnd, (HIMC)0);
      }

      ClearContext(pWIC, TRUE);
      ImmDestroyContext (pWIC->hIMC);
      free (pWIC);
    }

  s_pContextList = NULL;
}

// >> Add Y.Arai
static int
GetLastContext(void)
{
    WIContextPtr pWIC, pPrev = NULL;

#if CYGIME_DEBUG
    winDebug ("%s\n", __FUNCTION__);
#endif

    for (pWIC = s_pContextList; pWIC; pPrev = pWIC, pWIC = pWIC->pNext)
    {
	;	// とりあえず最後までたどる
    }

    if (pPrev != NULL)
	return pPrev->nContext;
    else
	return 0;
}
// << Add Y.Arai

int
winHIMCtoContext(DWORD hIMC)
{
  WIContextPtr pWIC;

#if CYGIME_DEBUG
  winDebug ("%s hIMC:0x%X\n", __FUNCTION__, hIMC);
#endif

  for (pWIC = s_pContextList; pWIC; pWIC = pWIC->pNext)
    {
      if (pWIC->hIMC == hIMC)
	{
#if CYGIME_DEBUG
	  winDebug ("found.\n");
#endif
	  return pWIC->nContext;
	}
    }

#if CYGIME_DEBUG
  winDebug ("not found.\n");
#endif
  return 0;
}

static void patchCode(wchar_t* src, int srclen)
{
    int i;
    /* Windows固有のコードをパッチする */
    for (i=0; i<srclen; i++)
    {
	switch(src[i])
	{
	    case 0x005C:	/* \ */
		src[i] = 0x00A5;
		break;
	    case 0xFF5E:	/* 〜 */
		src[i] = 0x301C;
		break;
	    case 0x2225:	/* ‖ */
		src[i] = 0x2016;
		break;
	    case 0xFF0D:	/* − */
		src[i] = 0x2015;
		break;
	    case 0xFFE0:	/* ¢ */
		src[i] = 0x00A2;
		break;
	    case 0xFFE1:	/* £ */
		src[i] = 0x00A3;
		break;
	    case 0xFFE2:	/* ¬ */
		src[i] = 0x00AC;
		break;
	}
    }
}

void
winCommitCompositionResult (int nContext, int nIndex, void *pData, int nLen)
{
  WIContextPtr pWIC;
#if CYGIME_DEBUG
  winDebug ("%s: (context %d)\n", __FUNCTION__, nContext);
#endif

  if (!(pWIC = FindContext(nContext))) return;

  switch (nIndex)
    {
    case GCS_COMPSTR:
      if (pWIC->pszComposition)
	{
	  free (pWIC->pszComposition);
	}
      patchCode((wchar_t*)pData, nLen/2);	// ここでnLenはbyte数だから文字数に直す
      pWIC->pszComposition = (wchar_t*)pData;		// char -> wchar_t	Y.Arai

winDebug("  GCS_COMPSTR: first char: 0x%lX\n", pWIC->pszComposition[0]);

      break;

    case GCS_RESULTSTR:
      if (pWIC->pszCompositionResult)
	{
	  free (pWIC->pszCompositionResult);
	}
      patchCode((wchar_t*)pData, nLen/2);	// ここでnLenはbyte数だから文字数に直す
      pWIC->pszCompositionResult = (wchar_t*)pData;	// char -> wchar_t	Y.Arai

winDebug("  GCS_RESULTSTR: first char: 0x%lX\n", pWIC->pszCompositionResult[0]);

      break;

    case GCS_CURSORPOS:
      pWIC->nCursor = *(int*)pData;
      break;

    case GCS_COMPATTR:
      // 現在の値を保存
      if (pWIC->pLastAttr)
	free (pWIC->pLastAttr);
      pWIC->pLastAttr = pWIC->pAttr;
      pWIC->nLastAttr = pWIC->nAttr;

      pWIC->pAttr = (char*)pData;
      pWIC->nAttr = nLen;
      break;

// >> Y.Arai
    case GCS_COMPCLAUSE:
winDebug("  GCS_COMPCLAUSE:\n");
      if (pData != NULL)
      {
        pWIC->nNumClause = ((WIClauseRec*)pData)->nNumClause;
winDebug("    nNumClause: %d\n", pWIC->nNumClause);
        pWIC->nCurClause = ((WIClauseRec*)pData)->nCurClause;
winDebug("    nCurClause: %d\n", pWIC->nCurClause);
        pWIC->nOffset = ((WIClauseRec*)pData)->nOffset;
        if (pWIC->pClauseList)
          free (pWIC->pClauseList);
        pWIC->pClauseList = ((WIClauseRec*)pData)->pClauseList;
      }
      break;
// << Y.Arai
    default:
      break;
    }
}

#define IsRoot(pWin) \
    ((pWin) == (pWin)->drawable.pScreen->root)
/*
static Bool
IsRoot(WindowPtr pWin)
{
  return pWin == WindowTable[(pWin)->drawable.pScreen->myNum];
}
*/

static Bool
IsTopLevel(WindowPtr pWin)
{
  return pWin && (pWin)->parent && IsRoot(pWin->parent);
}

static WindowPtr
GetTopLevelParent(WindowPtr pWindow)
{
  WindowPtr pWin = pWindow;

  if (!pWin || IsRoot(pWin)) return NULL;

  while (pWin && !IsTopLevel(pWin))
    {
      pWin = pWin->parent;
    }
  return pWin;
}

void
winWinIMEExtensionInit (void)
{
  ExtensionEntry* extEntry;

#if CYGIME_DEBUG
  winDebug ("%s\n", __FUNCTION__);
#endif

  ClientType = CreateNewResourceType(WinIMEFreeClient,
				     "WinIMEClient");
  EventType = CreateNewResourceType(WinIMEFreeEvents,
				    "WinIMEEvent");
  eventResource = FakeClientID(0);

  if (ClientType && EventType &&
      (extEntry = AddExtension(WINIMENAME,
			       WinIMENumberEvents,
			       WinIMENumberErrors,
			       ProcWinIMEDispatch,
			       SProcWinIMEDispatch,
			       WinIMEResetProc,
			       StandardMinorOpcode)))
    {
      WinIMEReqCode = (unsigned char)extEntry->base;
      WinIMEErrorBase = extEntry->errorBase;
      WinIMEEventBase = extEntry->eventBase;
      EventSwapVector[WinIMEEventBase] = (EventSwapPtr) SNotifyEvent;

      DeleteAllContext();
    }
}

/*ARGSUSED*/
static void
WinIMEResetProc (ExtensionEntry* extEntry)
{
#if CYGIME_DEBUG
  winDebug ("%s\n", __FUNCTION__);
#endif
  DeleteAllContext();
}

static int
ProcWinIMEQueryVersion(register ClientPtr client)
{
  xWinIMEQueryVersionReply rep;

#if CYGIME_DEBUG
  winDebug ("%s\n", __FUNCTION__);
#endif

  REQUEST_SIZE_MATCH(xWinIMEQueryVersionReq);
  rep.type = X_Reply;
  rep.length = 0;
  rep.sequenceNumber = client->sequence;
  rep.majorVersion = WIN_IME_MAJOR_VERSION;
  rep.minorVersion = WIN_IME_MINOR_VERSION;
  rep.patchVersion = WIN_IME_PATCH_VERSION;
  if (client->swapped)
    {
      swaps(&rep.sequenceNumber);
      swapl(&rep.length);
    }
  WriteToClient(client, sizeof(xWinIMEQueryVersionReply), (char *)&rep);
  return (client->noClientException);
}


/* events */

static inline void
updateEventMask (WinIMEEventPtr *pHead)
{
  WinIMEEventPtr pCur;

#if CYGIME_DEBUG
  winDebug ("%s\n", __FUNCTION__);
#endif

  eventMask = 0;
  for (pCur = *pHead; pCur != NULL; pCur = pCur->next)
    eventMask |= pCur->mask;
}

/*ARGSUSED*/
static int
WinIMEFreeClient (pointer data, XID id)
{
  WinIMEEventPtr   pEvent;
  WinIMEEventPtr   *pHead, pCur, pPrev;

#if CYGIME_DEBUG
  winDebug ("%s\n", __FUNCTION__);
#endif

  pEvent = (WinIMEEventPtr) data;
  dixLookupResourceByType((pointer)&pHead, eventResource, EventType,
			  NullClient, DixUnknownAccess);
  if (pHead)
    {
      pPrev = 0;
      for (pCur = *pHead; pCur && pCur != pEvent; pCur=pCur->next)
	pPrev = pCur;
      if (pCur)
	{
	  if (pPrev)
	    pPrev->next = pEvent->next;
	  else
	    *pHead = pEvent->next;
	}
      updateEventMask (pHead);
    }
  free ((pointer) pEvent);

  return 1;
}

/*ARGSUSED*/
static int
WinIMEFreeEvents (pointer data, XID id)
{
  WinIMEEventPtr   *pHead, pCur, pNext;

#if CYGIME_DEBUG
  winDebug ("%s\n", __FUNCTION__);
#endif

  pHead = (WinIMEEventPtr *) data;
  for (pCur = *pHead; pCur; pCur = pNext)
    {
      pNext = pCur->next;
      FreeResource (pCur->clientResource, ClientType);
      free ((pointer) pCur);
    }
  free ((pointer) pHead);
  eventMask = 0;

  return 1;
}

static int
ProcWinIMESelectInput (register ClientPtr client)
{
  REQUEST(xWinIMESelectInputReq);
  WinIMEEventPtr	pEvent, pNewEvent, *pHead;
  XID			clientResource;

#if CYGIME_DEBUG
  winDebug ("%s\n", __FUNCTION__);
#endif

  REQUEST_SIZE_MATCH (xWinIMESelectInputReq);
  dixLookupResourceByType((pointer)&pHead, eventResource, EventType,
			  NullClient, DixWriteAccess);
  if (stuff->mask != 0)
    {
      if (pHead)
	{
	  /* check for existing entry. */
	  for (pEvent = *pHead; pEvent; pEvent = pEvent->next)
	    {
	      if (pEvent->client == client)
		{
		  pEvent->mask = stuff->mask;
		  updateEventMask (pHead);
		  return Success;
		}
	    }
	}

      /* build the entry */
      pNewEvent = (WinIMEEventPtr) xalloc (sizeof (WinIMEEventRec));
      if (!pNewEvent)
	return BadAlloc;
      pNewEvent->next = 0;
      pNewEvent->client = client;
      pNewEvent->mask = stuff->mask;
      /*
       * add a resource that will be deleted when
       * the client goes away
       */
      clientResource = FakeClientID (client->index);
      pNewEvent->clientResource = clientResource;
      if (!AddResource (clientResource, ClientType, (pointer)pNewEvent))
	return BadAlloc;
      /*
       * create a resource to contain a pointer to the list
       * of clients selecting input.  This must be indirect as
       * the list may be arbitrarily rearranged which cannot be
       * done through the resource database.
       */
      if (!pHead)
	{
	  pHead = (WinIMEEventPtr *) xalloc (sizeof (WinIMEEventRec));
	  if (!pHead ||
	      !AddResource (eventResource, EventType, (pointer)pHead))
	    {
	      FreeResource (clientResource, RT_NONE);
	      return BadAlloc;
	    }
	  *pHead = 0;
	}
      pNewEvent->next = *pHead;
      *pHead = pNewEvent;
      updateEventMask (pHead);
    }
  else if (stuff->mask == 0)
    {
      /* delete the interest */
      if (pHead)
	{
	  pNewEvent = 0;
	  for (pEvent = *pHead; pEvent; pEvent = pEvent->next)
	    {
	      if (pEvent->client == client)
		break;
	      pNewEvent = pEvent;
	    }
	  if (pEvent)
	    {
	      FreeResource (pEvent->clientResource, ClientType);
	      if (pNewEvent)
		pNewEvent->next = pEvent->next;
	      else
		*pHead = pEvent->next;
	      free (pEvent);
	      updateEventMask (pHead);
	    }
	}
    }
  else
    {
      client->errorValue = stuff->mask;
      return BadValue;
    }
  return Success;
}

static int
ProcWinIMECreateContext(register ClientPtr client)
{
  xWinIMECreateContextReply rep;
  WIContextPtr pWIC;

#if CYGIME_DEBUG
  winDebug ("%s\n", __FUNCTION__);
#endif

  if (!(pWIC = NewContext()))
    {
      return BadValue;
    }

  REQUEST_SIZE_MATCH(xWinIMECreateContextReq);
  rep.type = X_Reply;
  rep.length = 0;
  rep.sequenceNumber = client->sequence;
  rep.context = pWIC->nContext;
  if (client->swapped)
    {
      swaps(&rep.sequenceNumber);
      swapl(&rep.length);
    }
  WriteToClient(client, sizeof(xWinIMECreateContextReply), (char *)&rep);
  return (client->noClientException);
}

static int
ProcWinIMESetOpenStatus (register ClientPtr client)
{
  REQUEST(xWinIMESetOpenStatusReq);
  WIContextPtr pWIC;

  REQUEST_SIZE_MATCH(xWinIMESetOpenStatusReq);

#if CYGIME_DEBUG
  winDebug ("%s\n", __FUNCTION__);
#endif

  if (!(pWIC = FindContext(stuff->context)))
    {
      return BadValue;
    }

  ImmSetOpenStatus (pWIC->hIMC, stuff->state);

  return (client->noClientException);
}

static int
ProcWinIMESetCompositionWindow (register ClientPtr client)
{
  REQUEST(xWinIMESetCompositionWindowReq);
  WIContextPtr pWIC;
  COMPOSITIONFORM cf;
  REQUEST_SIZE_MATCH(xWinIMESetCompositionWindowReq);

#if CYGIME_DEBUG
  winDebug ("%s\n", __FUNCTION__);
#endif

  if (!(pWIC = FindContext(stuff->context)))
    {
      return BadValue;
    }

  pWIC->dwCompositionStyle = stuff->style;
  pWIC->ptCompositionPos.x = stuff->ix;
  pWIC->ptCompositionPos.y = stuff->iy;
  pWIC->rcCompositionArea.left = stuff->ix;
  pWIC->rcCompositionArea.top = stuff->iy;
  pWIC->rcCompositionArea.right = stuff->ix + stuff->iw;
  pWIC->rcCompositionArea.bottom = stuff->iy + stuff->ih;

  cf.dwStyle = pWIC->dwCompositionStyle;
  cf.ptCurrentPos.x = pWIC->ptCompositionPos.x;
  cf.ptCurrentPos.y = pWIC->ptCompositionPos.y;
  cf.rcArea.left = pWIC->rcCompositionArea.left;
  cf.rcArea.top = pWIC->rcCompositionArea.top;
  cf.rcArea.right = pWIC->rcCompositionArea.right;
  cf.rcArea.bottom = pWIC->rcCompositionArea.bottom;
//  ImmSetCompositionWindow(pWIC->hIMC, &cf);	// とりあえずやめてみる	Y.Arai

  return (client->noClientException);
}

static int
ProcWinIMEGetCompositionString (register ClientPtr client)
{
  REQUEST(xWinIMEGetCompositionStringReq);
  WIContextPtr pWIC;
  int len;
  xWinIMEGetCompositionStringReply rep;

#if CYGIME_DEBUG
  winDebug ("%s %d\n", __FUNCTION__, stuff->context);
#endif

  REQUEST_SIZE_MATCH(xWinIMEGetCompositionStringReq);

  if ((pWIC = FindContext(stuff->context)))
    {
      switch (stuff->index)
	{
	case WinIMECMPCompStr:
winDebug("  * WinIMECMPCompStr *\n");
	  {
            void* szStr;
            szStr = (void*)(pWIC->pszComposition);
	    if (pWIC->pszComposition)
	      {
		len = wcslen(pWIC->pszComposition) * 2;
	      }
	    else
	      {
#if CYGIME_DEBUG
		winDebug ("no composition result.\n");
#endif
		len = 0;
	      }
            rep.type = X_Reply;
            rep.length = (len + 3) >> 2;
            rep.sequenceNumber = client->sequence;
            rep.strLength = len;
            WriteReplyToClient(client, sizeof(xWinIMEGetCompositionStringReply), &rep);
            if (len > 0)
              {
                (void)WriteToClient(client, len, szStr);
              }
	  }
	  break;
	case WinIMECMPResultStr:
winDebug("  * WinIMECMPResultStr *\n");
	  {
            void* szStr = (void*)(pWIC->pszCompositionResult);
            if (pWIC->pszCompositionResult)
              {
                len = wcslen(pWIC->pszCompositionResult) * 2;
              }
            else
              {
                len = 0;
              }
            rep.type = X_Reply;
            rep.length = (len + 3) >> 2;
            rep.sequenceNumber = client->sequence;
            rep.strLength = len;
            WriteReplyToClient(client, sizeof(xWinIMEGetCompositionStringReply), &rep);
	    if (szStr)
	      {
                (void)WriteToClient(client, len, szStr);
	      }
	    else
	      {
#if CYGIME_DEBUG
		winDebug ("no composition result.\n");
#endif
	      }
	  }
	  break;
	case WinIMECMPCompAttr:
winDebug("  * WinIMECMPCompAttr *\n");
	  {
	    //if (pWIC->pszComposition)
            //{
            len = pWIC->nAttr;
            rep.type = X_Reply;
            rep.length = (len + 3) >> 2;
            rep.sequenceNumber = client->sequence;
            rep.strLength = len;
            WriteReplyToClient(client, sizeof(xWinIMEGetCompositionStringReply), &rep);
            (void)WriteToClient(client, len, pWIC->pAttr);
            //}
            //else
            //{
#if CYGIME_DEBUG
            //winDebug ("no composition result.\n");
#endif
            //return BadValue;
            //}
	  }
	  break;
	default:
	  {
#if CYGIME_DEBUG
	    winDebug ("bad index.\n");
#endif
	    return BadValue;
	  }
	}
    }
  else
    {
#if CYGIME_DEBUG
      winDebug ("context is not found.\n");
#endif
      return BadValue;
    }

  return (client->noClientException);
}

static int
ProcWinIMEGetConversionStatus (register ClientPtr client)
{
    REQUEST(xWinIMEGetConversionStatusReq);
    WIContextPtr pWIC;
    int fOpen = 0;
    DWORD fdwConversion = 0;
    DWORD fdwSentence = 0;
    DWORD fChange = 0;
    xWinIMEGetConversionStatusReply rep;
//    char statsString[10];

#if CYGIME_DEBUG
    winDebug ("%s %d\n", __FUNCTION__, stuff->context);
#endif

    REQUEST_SIZE_MATCH(xWinIMEGetConversionStatusReq);

    if ((pWIC = FindContext(stuff->context)))
    {
	BOOL fStatus = ImmGetOpenStatus(pWIC->hIMC);
	if (fStatus == FALSE)
	{
#if CYGIME_DEBUG
	    winDebug ("no composition result.\n");
#endif
	    fOpen = 0;
	} else
	{
	    if (ImmGetConversionStatus(pWIC->hIMC, &fdwConversion, &fdwSentence) != 0)
	    {
		fOpen = 1;
	    } else
	    {
		fOpen = 0;
		fdwConversion = 0;
		fdwSentence = 0;
	    }
	}
// >> check mode changed
#if 1
	if (pWIC->nLastAttr == 0)
	{   // 前回は何もない状態
	    if (pWIC->nAttr == 0)
		fChange = 0;
	    else
	    {
		if (pWIC->pAttr[0] == ATTR_INPUT)
		    fChange = 0;
		else
		    fChange = 1;
	    }
	} else
	{   // 前回との比較。最初だけでチェックすればＯＫなはず
	    if (pWIC->nAttr == 0)
	    {	// 現在の変換文字列がない
		if (pWIC->pLastAttr[0] == ATTR_INPUT)
		    fChange = 0;
		else
		    fChange = 1;
	    } else
	    {	// どっちもある
		int nCur = ATTR_INPUT, nLast = ATTR_INPUT;
		if (pWIC->pAttr[0] != ATTR_INPUT)
		    nCur = ATTR_CONVERTED;
		if (pWIC->pLastAttr[0] != ATTR_INPUT)
		    nLast = ATTR_CONVERTED;
		if (nCur == nLast)
		    fChange = 0;
		else
		    fChange = 1;
	    }
	}
#else
	if (pWIC->nAttr != pWIC->nLastAttr)
	{
winDebug("nAttr(%d) != nLastAttr(%d)\n", pWIC->nAttr, pWIC->nLastAttr);
	    fChange = 1;
	} else
	{
	    int i;
	    for (i=0; i<pWIC->nAttr; i++)
	    {
		if (pWIC->pAttr[i] != pWIC->pLastAttr[i])
		{
winDebug("pAttr[i](%d) != pLastAttr[i](%d)\n", i, pWIC->pAttr[i], i, pWIC->pLastAttr[i]);
		    fChange = 1;
		    break;
		}
	    }
	}
#endif
// << check mode changed
    } else
    {
#if CYGIME_DEBUG
        winDebug ("context is not found.\n");
#endif
        return BadValue;
    }

    rep.type = X_Reply;
    rep.length = 0;
    rep.sequenceNumber = client->sequence;
    rep.conversion = fdwConversion;
    rep.sentence = fdwSentence;
    rep.fopen = fOpen;
    rep.fmodechange = fChange;
    WriteReplyToClient(client, sizeof(xWinIMEGetCompositionStringReply), &rep);

    return (client->noClientException);
}

static int
ProcWinIMEGetOpenStatus (register ClientPtr client)
{
    REQUEST(xWinIMEGetOpenStatusReq);
    WIContextPtr pWIC;
    int fOpen = 0;
    DWORD fdwConversion = 0;
    DWORD fdwSentence = 0;
    xWinIMEGetOpenStatusReply rep;

#if CYGIME_DEBUG
    winDebug ("%s %d\n", __FUNCTION__, stuff->context);
#endif

    REQUEST_SIZE_MATCH(xWinIMEGetConversionStatusReq);

    if ((pWIC = FindContext(stuff->context)))
    {
	BOOL fStatus;
winDebug ("  call ImmGetOpenStatus()\n");
	fStatus = ImmGetOpenStatus(pWIC->hIMC);
	if (fStatus == FALSE)
	{
#if CYGIME_DEBUG
	    winDebug ("no composition result.\n");
#endif
	    fOpen = 0;
	} else
	{
#if 1
	    fOpen = 1;
#else
winDebug ("  call ImmGetConversionStatus()\n");
	    if (ImmGetConversionStatus(pWIC->hIMC, &fdwConversion, &fdwSentence) != 0)
	    {
		fOpen = 1;
	    } else
	    {
		fOpen = 0;
	    }
#endif
	}
    } else
    {
#if CYGIME_DEBUG
        winDebug ("context is not found.\n");
#endif
        return BadValue;
    }

    rep.type = X_Reply;
    rep.length = 0;
    rep.sequenceNumber = client->sequence;
    rep.fopen = fOpen;
    WriteReplyToClient(client, sizeof(xWinIMEGetCompositionStringReply), &rep);

    return (client->noClientException);
}

static int
ProcWinIMEGetTargetClause (register ClientPtr client)
{
    REQUEST(xWinIMEGetTargetClauseReq);
    WIContextPtr pWIC;
    int len;
    xWinIMEGetTargetClauseReply rep;

#if CYGIME_DEBUG
    winDebug ("%s %d (target = %d)\n", __FUNCTION__, stuff->context, stuff->target);
#endif

    REQUEST_SIZE_MATCH(xWinIMEGetTargetClauseReq);

    if ((pWIC = FindContext(stuff->context)))
    {
	if (pWIC->pszComposition)
	{
	    wchar_t *pszClause;
	    void* szStr;
	    int attr;
	    pszClause = GetTargetClause(pWIC, stuff->target, &attr, &len);
	    if (pszClause == NULL)
		return BadValue;

	    szStr = (void*)pszClause;
	    len = wcslen(pszClause) * 2;
	    rep.type = X_Reply;
	    rep.length = (len + 3) >> 2;
	    rep.sequenceNumber = client->sequence;
	    rep.bytes = len;
	    rep.numchar = len / 2;
	    rep.attr = attr;
	    rep.curClause = pWIC->nCurClause;
winDebug ("  bytes = %d, num = %d, cur = %d\n", rep.bytes, rep.numchar, rep.curClause);
	    WriteReplyToClient(client, sizeof(xWinIMEGetTargetClauseReply), &rep);
	    (void)WriteToClient(client, len, szStr);
	} else
	{
#if CYGIME_DEBUG
	    winDebug ("no composition result.\n");
#endif
	    return BadValue;
	}
    } else
    {
#if CYGIME_DEBUG
        winDebug ("context is not found.\n");
#endif
        return BadValue;
    }


    return (client->noClientException);
}

static int
ProcWinIMEGetTargetString (register ClientPtr client)
{
    REQUEST(xWinIMEGetTargetStringReq);
    WIContextPtr pWIC;
    int len;
    xWinIMEGetTargetStringReply rep;

#if CYGIME_DEBUG
    winDebug ("%s %d\n", __FUNCTION__, stuff->context);
#endif

    REQUEST_SIZE_MATCH(xWinIMEGetTargetStringReq);

    if ((pWIC = FindContext(stuff->context)))
    {
	if (pWIC->pszComposition)
	{
	    wchar_t *pszClause;
	    void* szStr;

	    pszClause = GetTargetString(pWIC, stuff->target, stuff->offset);
	    if (pszClause == NULL)
		return BadValue;

	    szStr = (void*)pszClause;
	    len = wcslen(pszClause) * 2;
	    rep.type = X_Reply;
	    rep.length = (len + 3) >> 2;
	    rep.sequenceNumber = client->sequence;
	    rep.bytes = len;
	    rep.numchar = len / 2;
	    WriteReplyToClient(client, sizeof(xWinIMEGetTargetStringReply), &rep);
	    (void)WriteToClient(client, len, szStr);
	} else
	{
#if CYGIME_DEBUG
	    winDebug ("no composition result. 1\n");
#endif
	    //return BadValue;
	    len = 0;
	    rep.type = X_Reply;
	    rep.length = (len + 3) >> 2;
	    rep.sequenceNumber = client->sequence;
	    rep.bytes = len;
	    rep.numchar = len / 2;
	    WriteReplyToClient(client, sizeof(xWinIMEGetTargetStringReply), &rep);
	}
    } else
    {
#if CYGIME_DEBUG
        winDebug ("context is not found.\n");
#endif
        return BadValue;
    }

    return (client->noClientException);
}

static int
ProcWinIMEGetLastContext(register ClientPtr client)
{
    xWinIMEGetLastContextReply rep;
    int nContext;

#if CYGIME_DEBUG
    winDebug ("%s\n", __FUNCTION__);
#endif

    nContext = GetLastContext();
    if (nContext == 0)
    {
	return BadValue;
    }

    REQUEST_SIZE_MATCH(xWinIMEGetLastContextReq);
    rep.type = X_Reply;
    rep.length = 0;
    rep.sequenceNumber = client->sequence;
    rep.context = nContext;
    if (client->swapped)
    {
	swaps(&rep.sequenceNumber);
	swapl(&rep.length);
    }
    WriteToClient(client, sizeof(xWinIMEGetLastContextReply), (char *)&rep);
    return (client->noClientException);
}

static int
ProcWinIMEClearContext (register ClientPtr client)
{
    xWinIMEClearContextReply rep;
    REQUEST(xWinIMEClearContextReq);
    WIContextPtr pWIC;
    BOOL bModify;

    REQUEST_SIZE_MATCH(xWinIMEClearContextReq);

#if CYGIME_DEBUG
    winDebug ("%s\n", __FUNCTION__);
#endif

    if (!(pWIC = FindContext(stuff->context)))
    {
	return BadValue;
    }

    bModify = ClearContext(pWIC, TRUE);

    REQUEST_SIZE_MATCH(xWinIMEClearContextReq);
    rep.type = X_Reply;
    rep.length = 0;
    rep.sequenceNumber = client->sequence;
    rep.modify = bModify;
    if (client->swapped)
    {
	swaps(&rep.sequenceNumber);
	swapl(&rep.length);
    }
    WriteToClient(client, sizeof(xWinIMEClearContextReply), (char *)&rep);
    return (client->noClientException);
}

static void CalcCandPos(HWND hWnd, CANDIDATEFORM *pForm)
{
    BOOL fRecalc = FALSE;
#ifdef XWIN_MULTIWINDOW
    WindowPtr pWin = NULL;
    winPrivWinPtr pWinPriv = NULL;
    winPrivScreenPtr pScreenPriv = NULL;
    winScreenInfo *pScreenInfo = NULL;

    /* Check if the Windows window property for our X window pointer is valid */
    if ((pWin = GetProp (hWnd, WIN_WINDOW_PROP)) != NULL)
    {
	/* Get a pointer to our window privates */
	pWinPriv = winGetWindowPriv(pWin);

	/* Get pointers to our screen privates and screen info */
	pScreenPriv = pWinPriv->pScreenPriv;
	pScreenInfo = pScreenPriv->pScreenInfo;

	if (pScreenInfo->fMultiWindow)
	    fRecalc = TRUE;
    }
#endif
#ifdef XWIN_MULTIWINDOWEXTWM
    /* Check if the Windows window property for our X window pointer is valid */
    if ((pWin = GetProp (hWnd, WIN_WINDOW_PROP)) != NULL)
    {
	/* Get a pointer to our window privates */
	pWinPriv = winGetWindowPriv(pWin);

	/* Get pointers to our screen privates and screen info */
	pScreenPriv = pWinPriv->pScreenPriv;
	pScreenInfo = pScreenPriv->pScreenInfo;

	if (pScreenInfo->fMWExtWM)
	    fRecalc = TRUE;
    }
#endif
    if (fRecalc == TRUE)
    {
	ScreenToClient(hWnd, &(pForm->ptCurrentPos));
    }
}

static int
ProcWinIMESetCandidateWindow (register ClientPtr client)
{
    REQUEST(xWinIMESetCandidateWindowReq);
    WIContextPtr pWIC;
    CANDIDATEFORM form;
    LRESULT result;
    int n;

    REQUEST_SIZE_MATCH(xWinIMESetCandidateWindowReq);

#if CYGIME_DEBUG
    winDebug ("%s\n", __FUNCTION__);
#endif

    if (!(pWIC = FindContext(stuff->context)))
    {
	return BadValue;
    }

winDebug("  x = %d, y = %d, n = %d\n", stuff->x, stuff->y, stuff->n);

    form.dwIndex = stuff->n;
    form.dwStyle = CFS_CANDIDATEPOS;
    form.ptCurrentPos.x = stuff->x;
    form.ptCurrentPos.y = stuff->y;
    form.rcArea.left = form.rcArea.top = form.rcArea.right = form.rcArea.bottom = 0;
    CalcCandPos(pWIC->hWnd, &form);

    POINT pt = {form.ptCurrentPos.x, form.ptCurrentPos.y};
    ClientToScreen(pWIC->hWnd, &pt);
    pWIC->nCandPosX = pt.x;
    pWIC->nCandPosY = pt.y;
    pWIC->nCandPage = stuff->n;
#if 1
    n = ImmSetCandidateWindow(pWIC->hIMC, &form);
#endif
#if 1	// 両方やらないといけない
    result = SendMessage(pWIC->hWnd, WM_IME_CONTROL, IMC_SETCANDIDATEPOS, (LPARAM)&form);
#endif

    return (client->noClientException);
}

static int
ProcWinIMEDestroyContext (register ClientPtr client)
{
    REQUEST(xWinIMEDestroyContextReq);
    WIContextPtr pWIC;
    CANDIDATEFORM form;
    LRESULT result;
    int n;

    REQUEST_SIZE_MATCH(xWinIMEDestroyContextReq);

#if CYGIME_DEBUG
    winDebug ("%s\n", __FUNCTION__);
#endif

    if (!(pWIC = FindContext(stuff->context)))
    {
	return BadValue;
    }

    DeleteContext(stuff->context);

#if CYGIME_DEBUG
    winDebug ("%s exit.\n", __FUNCTION__);
#endif
    return (client->noClientException);
}
// << Add Y.Arai

static void
setFocus(WIContextPtr pWIC)
{
    if (pWIC->hWnd)
    {
	ImmAssociateContext (pWIC->hWnd, (HIMC)0);
    }

    pWIC->hWnd = g_hwndLastKeyPress;
    if (pWIC->hWnd != NULL)
    {
	ImmAssociateContext (pWIC->hWnd, pWIC->hIMC);
winDebug("  target window: %lX\n", pWIC->hWnd);
    } else
    {
winDebug("  target window: NULL\n");
    }
}

static int
ProcWinIMESetFocus (register ClientPtr client)
{
  REQUEST(xWinIMESetFocusReq);
  WIContextPtr pWIC;
  Window id;
  HIMC hOldIMC;

  REQUEST_SIZE_MATCH(xWinIMESetFocusReq);

#if CYGIME_DEBUG
  winDebug ("%s %d, g_hwndLastKeyPress = %lX\n", __FUNCTION__, stuff->context, g_hwndLastKeyPress);
#endif

  if (!(pWIC = FindContext(stuff->context)))
    {
winDebug("  bad context(%d)\n", stuff->context);
      return BadValue;
    }

  if (pWIC->hWnd)
    hOldIMC = ImmAssociateContext (pWIC->hWnd, (HIMC)0);
  else
    hOldIMC = NULL;

  if (stuff->focus)
    {
winDebug("  turn on(Context = %d, hIMC = %lX, hWnd = %lX)\n", pWIC->nContext, pWIC->hIMC, pWIC->hWnd);

/*
      if (hOldIMC == pWIC->hIMC)
      {
	winDebug("  already set %lX\n", pWIC->hIMC);
	return (client->noClientException);
      }
*/

      setFocus(pWIC);

    }
  else
    {
winDebug("  turn off(Context = %d, hIMC = %lX, hWnd = %lX)\n", pWIC->nContext, pWIC->hIMC, pWIC->hWnd);
      if (pWIC->hWnd)
	{
//	  hOldIMC = ImmAssociateContext (pWIC->hWnd, (HIMC)0);
#if 1
	  if (hOldIMC != NULL)
	  { // 前に別のが設定されていたので戻す
winDebug("  prev hIMC is not NULL(%lX).\n", hOldIMC);
#if 1	// from A.Yamanaka
	    if(hOldIMC!=pWIC->hIMC)
	      ImmAssociateContext (pWIC->hWnd, hOldIMC);
	    else
	      ImmAssociateContext (pWIC->hWnd, 0);
#else
	    ImmAssociateContext (pWIC->hWnd, hOldIMC);
#endif
	  }
#endif
	  pWIC->hWnd = NULL;
	}
    }

  return (client->noClientException);
}

static int
ProcWinIMESetCompositionDraw (register ClientPtr client)
{
  REQUEST(xWinIMESetCompositionDrawReq);
  WIContextPtr pWIC;

  REQUEST_SIZE_MATCH(xWinIMESetCompositionDrawReq);

#if CYGIME_DEBUG
  winDebug ("%s %d\n", __FUNCTION__, stuff->context);
#endif

  if (!(pWIC = FindContext(stuff->context)))
    {
      return BadValue;
    }

  pWIC->fCompositionDraw = stuff->draw;

  return (client->noClientException);
}

static int
ProcWinIMEGetCursorPosition(register ClientPtr client)
{
  REQUEST(xWinIMEGetCursorPositionReq);
  WIContextPtr pWIC;
  xWinIMEGetCursorPositionReply rep;

  REQUEST_SIZE_MATCH(xWinIMEGetCursorPositionReq);

#if CYGIME_DEBUG
  winDebug ("%s: (context %d)\n", __FUNCTION__, stuff->context);
#endif

  if (!(pWIC = FindContext(stuff->context)))
    {
      return BadValue;
    }

  REQUEST_SIZE_MATCH(xWinIMEGetCursorPositionReq);
  rep.type = X_Reply;
  rep.length = 0;
  rep.sequenceNumber = client->sequence;
  rep.cursor = pWIC->nCursor;
  rep.numClause = pWIC->nNumClause;
  rep.curClause = pWIC->nCurClause;
  rep.offset = pWIC->nOffset;

winDebug ("nCursor: %d, nNumClause: %d, nCurClause: %d, nOffset: %d\n", rep.cursor, rep.numClause, rep.curClause, rep.offset);

  WriteToClient(client, sizeof(xWinIMEGetCursorPositionReply), (char *)&rep);
  return (client->noClientException);
}

/*
 * deliver the event
 */

void
winWinIMESendEvent (int type, unsigned int mask, int kind, int arg, int context, HWND hwnd)
{
  WinIMEEventPtr	*pHead, pEvent;
  ClientPtr		client;
  xWinIMENotifyEvent se;
#if CYGIME_DEBUG
  winDebug ("%s %d %d %d %d #%d, hwnd = %lXd\n",
	  __FUNCTION__, type, mask, kind, arg, context, hwnd);
#endif
  dixLookupResourceByType((pointer)&pHead, eventResource, EventType,
			  NullClient, DixWriteAccess);
  if (!pHead)
  {
    winDebug("  no event found.\n");
    return;
  }
  for (pEvent = *pHead; pEvent; pEvent = pEvent->next)
    {
      client = pEvent->client;
#if CYGIME_DEBUG
      winDebug ("winWinIMESendEvent - x%08x\n", (int) client);
#endif
      if ((pEvent->mask & mask) == 0
	  || client == serverClient || client->clientGone)
	{
	  continue;
	}
#if CYGIME_DEBUG 
      winDebug ("winWinIMESendEvent - send\n");
#endif
      se.type = type + WinIMEEventBase;
      se.kind = kind;
      se.context = context;
      se.arg = arg;
      se.sequenceNumber = client->sequence;
      se.time = currentTime.milliseconds;
      se.hwnd = hwnd;
      WriteEventsToClient (client, 1, (xEvent *) &se);
    }
}

/* dispatch */

static int
ProcWinIMEDispatch (register ClientPtr client)
{
  REQUEST(xReq);

#if CYGIME_DEBUG
  winDebug ("%s\n", __FUNCTION__);
#endif

  switch (stuff->data)
    {
    case X_WinIMEQueryVersion:
      return ProcWinIMEQueryVersion (client);
    case X_WinIMESelectInput:
      return ProcWinIMESelectInput (client);
    case X_WinIMECreateContext:
      return ProcWinIMECreateContext (client);
    case X_WinIMESetFocus:
      return ProcWinIMESetFocus (client);
    case X_WinIMESetOpenStatus:
      return ProcWinIMESetOpenStatus (client);
    case X_WinIMESetCompositionWindow:
      return ProcWinIMESetCompositionWindow (client);
    case X_WinIMEGetCompositionString:
      return ProcWinIMEGetCompositionString (client);
    case X_WinIMESetCompositionDraw:
      return ProcWinIMESetCompositionDraw (client);
    case X_WinIMEGetCursorPosition:
      return ProcWinIMEGetCursorPosition (client);
    case X_WinIMEGetConversionStatus:
      return ProcWinIMEGetConversionStatus (client);
    case X_WinIMEGetOpenStatus:
      return ProcWinIMEGetOpenStatus (client);
    case X_WinIMEGetTargetClause:
      return ProcWinIMEGetTargetClause (client);
    case X_WinIMEGetLastContext:
      return ProcWinIMEGetLastContext (client);
    case X_WinIMEGetTargetString:
      return ProcWinIMEGetTargetString (client);
    case X_WinIMESetCandidateWindow:
      return ProcWinIMESetCandidateWindow (client);
    case X_WinIMEClearContext:
      return ProcWinIMEClearContext (client);
    case X_WinIMEDestroyContext:
      return ProcWinIMEDestroyContext (client);
/*
    case X_WinIME:
      return ProcWinIME (client);
    case X_WinIME:
      return ProcWinIME (client);
    case X_WinIME:
      return ProcWinIME (client);
*/
// << Add Y.Arai
    default:
      return BadRequest;
    }
}

static void
SNotifyEvent (xWinIMENotifyEvent *from, xWinIMENotifyEvent *to)
{
#if CYGIME_DEBUG
  winDebug ("%s\n", __FUNCTION__);
#endif

  to->type = from->type;
  to->kind = from->kind;
  cpswaps (from->sequenceNumber, to->sequenceNumber);
  cpswapl (from->context, to->context);
  cpswapl (from->time, to->time);
  cpswapl (from->arg, to->arg);
}

static int
SProcWinIMEQueryVersion (register ClientPtr client)
{
  REQUEST(xWinIMEQueryVersionReq);

#if CYGIME_DEBUG
  winDebug ("%s\n", __FUNCTION__);
#endif

  swaps(&stuff->length);
  return ProcWinIMEQueryVersion(client);
}

static int
SProcWinIMEDispatch (register ClientPtr client)
{
  REQUEST(xReq);

#if CYGIME_DEBUG
  winDebug ("%s\n", __FUNCTION__);
#endif

  /* It is bound to be non-local when there is byte swapping */
  if (!LocalClient(client))
    return WinIMEErrorBase + WinIMEClientNotLocal;

  /* only local clients are allowed access */
  switch (stuff->data)
    {
    case X_WinIMEQueryVersion:
      return SProcWinIMEQueryVersion(client);
    default:
      return BadRequest;
    }
}

LRESULT

winIMEMessageHandler (HWND hwnd, UINT message,
		      WPARAM wParam, LPARAM lParam)
{
    long BufLen = 0;		// Y.Arai
    LPDWORD dwCompCls = NULL;	// Y.Arai
//    int nClause = 0;		// Y.Arai
//    int nOffset = 0;		// Y.Arai
    int nCursor = 0;		// Y.Arai
    WIClauseRec ClauseInf;
    WIContextPtr pWIC;
    int nAttrLen = 0;
    char *pAttr = NULL;

    ClauseInf.pClauseList = NULL;

winDebug ("winIMEMessageHandler 0x%X %d\n", message, wParam);

  if (dispatchException != 0)
  {
    winDebug ("  in shutdown process...\n");
    return DefWindowProc (hwnd, message, wParam, lParam);
  }

  switch (message)
    {
    case WM_IME_NOTIFY:
      winDebug ("winIMEMessageHandler - WM_IME_NOTIFY(wParam = %d)\n", wParam);
      {
	HIMC hIMC = ImmGetContext(hwnd);
#if 1
	if (wParam == IMN_SETOPENSTATUS)
	  {
	    BOOL fStatus = ImmGetOpenStatus(hIMC);
	    BOOL fActive = FALSE;
	    pWIC = FindContext(winHIMCtoContext(hIMC));
	    if (pWIC != NULL)
	    {
	      fActive = (pWIC->fActiveStat == 1)?TRUE:FALSE;
winDebug ("  Send WinIMEOpenStatus Message... hIMC = %lX, stat = %s, active = %s\n", hIMC, fStatus?"TRUE":"FALSE",fActive?"TRUE":"FALSE");
//	      ImmReleaseContext(hwnd, hIMC);
/*
	      if (fStatus != fActive)
	        break;
*/
	      if (fStatus == pWIC->fPreeditStart)
	      {
		ImmReleaseContext(hwnd, hIMC);
	        break;
	      } else
	        pWIC->fPreeditStart = fStatus;

#if 1
	      SendImeKey();	// 開始も終了もXIM（とkinput2）の仕掛けで行う
winDebug (".\n");
#else
	      if (fStatus == TRUE)
	      {
	        SendImeKey();
	      } else
	      {
	        winWinIMESendEvent (WinIMEControllerNotify,
				    WinIMENotifyMask,
				    WinIMEOpenStatus,
				    fStatus,
				    winHIMCtoContext(hIMC),
				    hwnd);
	      }
#endif
	    } else {
winDebug ("### context not found. hIMC = %lX\n", hIMC);
            }
	  } else
#endif
	  if (wParam == IMN_OPENCANDIDATE)
	  {
#if 1
winDebug ("  Send WinIMEOpenCand Message (Open)...\n");
//	    HIMC hIMC = ImmGetContext(hwnd);
	    int nIndex = 0;
	    if (lParam && 0x01)
		nIndex = 0;
	    else if (lParam && 0x02)
		nIndex = 1;
	    else if (lParam && 0x04)
		nIndex = 2;
	    winWinIMESendEvent (WinIMEControllerNotify,
				WinIMENotifyMask,
				WinIMEOpenCand,
				nIndex,
				winHIMCtoContext(hIMC),
				hwnd);
//	    ImmReleaseContext(hwnd, hIMC);
// >> for Miscrosoft IME Standard 2003
//	    return 0;
	    break;
// << for Miscrosoft IME Standard 2003
#endif
	  } else if (wParam == IMN_CLOSECANDIDATE)
	  {
winDebug ("  Send WinIMEOpenCand Message (Close)...\n");
//	    HIMC hIMC = ImmGetContext(hwnd);
	    winWinIMESendEvent (WinIMEControllerNotify,
				WinIMENotifyMask,
				WinIMEOpenCand,
				-1,
				winHIMCtoContext(hIMC),
				hwnd);
//	    ImmReleaseContext(hwnd, hIMC);
	  }
	ImmReleaseContext(hwnd, hIMC);
      }
      break;

    case WM_IME_STARTCOMPOSITION:
      winDebug ("winIMEMessageHandler - WM_IME_STARTCOMPOSITION\n");
      {
	HIMC hIMC = ImmGetContext(hwnd);
	pWIC = FindContext(winHIMCtoContext(hIMC));
	if (pWIC != NULL)
	    ClearContext(pWIC, TRUE);

	winWinIMESendEvent (WinIMEControllerNotify,
			    WinIMENotifyMask,
			    WinIMEStartComposition,
			    0,
			    winHIMCtoContext(hIMC),
			    hwnd);
	ImmReleaseContext(hwnd, hIMC);
      }
      return 0;

    case WM_IME_COMPOSITION:
      winDebug ("winIMEMessageHandler - WM_IME_COMPOSITION (%d)\n", lParam);

if (lParam & GCS_COMPREADSTR)
	winDebug ("    GCS_COMPREADSTR\n");
if (lParam & GCS_COMPREADATTR)
	winDebug ("    GCS_COMPREADATTR\n");
if (lParam & GCS_COMPREADCLAUSE)
	winDebug ("    GCS_COMPREADCLAUSE\n");
if (lParam & GCS_COMPSTR)
	winDebug ("    GCS_COMPSTR\n");
if (lParam & GCS_COMPATTR)
	winDebug ("    GCS_COMPATTR\n");
if (lParam & GCS_COMPCLAUSE)
	winDebug ("    GCS_COMPCLAUSE\n");
if (lParam & GCS_CURSORPOS)
	winDebug ("    GCS_CURSORPOS\n");
if (lParam & GCS_DELTASTART)
	winDebug ("    GCS_DELTASTART\n");
if (lParam & GCS_RESULTREADSTR)
	winDebug ("    GCS_RESULTREADSTR\n");
if (lParam & GCS_RESULTREADCLAUSE)
	winDebug ("    GCS_RESULTREADCLAUSE\n");
if (lParam & GCS_RESULTSTR)
	winDebug ("    GCS_RESULTSTR\n");
if (lParam & GCS_RESULTCLAUSE)
	winDebug ("    GCS_RESULTCLAUSE\n");

      {
	HIMC hIMC = ImmGetContext(hwnd);

	if (lParam == 0)
	{   // 変換中の文字列が無くなった（BSで全部消したときとか）
	    pWIC = FindContext(winHIMCtoContext(hIMC));
	    if (pWIC != NULL)
		ClearContext(pWIC, TRUE);

	    ImmReleaseContext(hwnd, hIMC);
	    winWinIMESendEvent (WinIMEControllerNotify,
				WinIMENotifyMask,
				WinIMEComposition,
				WinIMECMPCompStr,
				winHIMCtoContext(hIMC),
				hwnd);
	    return 0;
	}

	if (lParam & GCS_RESULTSTR)
	  {
	    int			nUnicodeSize = 0;
	    wchar_t		*pwszUnicodeStr = NULL;

	    winDebug ("    GCS_RESULTSTR\n");
	    /* Get result */
	    nUnicodeSize = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, NULL, 0) + 2;
	    pwszUnicodeStr = (wchar_t*) calloc (1, nUnicodeSize);
	    ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, pwszUnicodeStr, nUnicodeSize);

	    winCommitCompositionResult (winHIMCtoContext(hIMC), GCS_RESULTSTR, pwszUnicodeStr, nUnicodeSize);
	    ClauseInf.nNumClause = 0;	// 変換が確定したんだから0
	    ClauseInf.nCurClause = 0;
	    ClauseInf.nOffset = 0;
	    ClauseInf.pClauseList = NULL;
winDebug ("      nNumClause: %d, nCurClause: %d\n", ClauseInf.nNumClause, ClauseInf.nCurClause);
	    winCommitCompositionResult (winHIMCtoContext(hIMC), GCS_COMPCLAUSE, &ClauseInf, 0);
	    winWinIMESendEvent (WinIMEControllerNotify,
				WinIMENotifyMask,
				WinIMEComposition,
				WinIMECMPResultStr,
				winHIMCtoContext(hIMC),
				hwnd);
	  }

#if 1	// 今までGCS_CURSORPOSを対象文節の区切りとして使用していたが、MS-IME系は必ず全体の最後になることが
	// わかったので、その対応
	if (lParam & GCS_COMPATTR)
	  {
//	    int			nAttrLen = 0;
//	    char		*pAttr = NULL;

	    winDebug ("    GCS_COMPATTR\n");
	    /* Get result */
	    nAttrLen = ImmGetCompositionStringW(hIMC, GCS_COMPATTR, NULL, 0);
	    pAttr = (char*)  malloc (nAttrLen);
	    ImmGetCompositionStringW(hIMC, GCS_COMPATTR, pAttr, nAttrLen);

	    winCommitCompositionResult (winHIMCtoContext(hIMC), GCS_COMPATTR, pAttr, nAttrLen);
	  }

	if (lParam & GCS_COMPCLAUSE)
	{
	    winDebug ("    GCS_COMPCLAUSE\n");
	    BufLen = ImmGetCompositionStringW(hIMC, GCS_COMPCLAUSE, NULL, 0);
	    if ((BufLen == IMM_ERROR_NODATA) || (BufLen == IMM_ERROR_GENERAL) || (BufLen == 0))
	    {
winDebug ("      NODATA.\n");
	    } else
	    {
		int i;
		char LastAttr, CurAttr;
		dwCompCls = (LPDWORD)calloc(1, BufLen);
		BufLen = ImmGetCompositionStringW(hIMC, GCS_COMPCLAUSE, dwCompCls, BufLen);
		ClauseInf.nNumClause = BufLen / sizeof(DWORD) - 1;	// 全体のサイズが入っている最後のDWORDはのぞく

		// 現在の文節情報が集まったので現在の対象文節の位置を探す
		if (pAttr == NULL)
		{	// 現在のアトリビュート情報がないので直接とってくるしかない（どうせ変換していないので困らないはず）
		    nCursor = ImmGetCompositionStringW(hIMC, GCS_CURSORPOS, NULL, 0);
		    nCursor = LOWORD(nCursor);
		} else
		{
		  nCursor = 0;	// ATTR_TARGET_XXXがどこにもないときは先頭

		  LastAttr = pAttr[0];
		  for (i=1; i<nAttrLen; i++)
		  {
		    CurAttr = pAttr[i];
		    if (((CurAttr != ATTR_TARGET_CONVERTED) && (CurAttr != ATTR_TARGET_NOTCONVERTED)) &&
		        ((LastAttr == ATTR_TARGET_CONVERTED) || (LastAttr == ATTR_TARGET_NOTCONVERTED)))
		    {
		      nCursor = i;
		    }
		    LastAttr = CurAttr;
		  }
		  if ((nCursor == 0) &&
		      ((LastAttr == ATTR_TARGET_CONVERTED) || (LastAttr == ATTR_TARGET_NOTCONVERTED)))
		  {
		    nCursor = i;
		  }
		}
		winCommitCompositionResult (winHIMCtoContext(hIMC), GCS_CURSORPOS, (int*)&nCursor, 0);

		for (i=ClauseInf.nNumClause - 1; i >= 0; i--)
		{
winDebug ("      check: dwCompCls[%d] = %d, nCursor = %d\n", i, dwCompCls[i], nCursor);
		    if (dwCompCls[i] <= nCursor)
		    {
			ClauseInf.nCurClause = i;
			ClauseInf.nOffset = nCursor - dwCompCls[i];	// nOffsetは文字数
			break;
		    }
		}
		ClauseInf.pClauseList = dwCompCls;
winDebug ("      nNumClause: %d, nCurClause: %d\n", ClauseInf.nNumClause, ClauseInf.nCurClause);
		winCommitCompositionResult (winHIMCtoContext(hIMC), GCS_COMPCLAUSE, &ClauseInf, 0);
	    }
	}

#else
	if (lParam & GCS_CURSORPOS)
	  {
	    winDebug ("    GCS_CURSORPOS\n");
	    nCursor = ImmGetCompositionStringW(hIMC, GCS_CURSORPOS, NULL, 0);
	    nCursor = LOWORD(nCursor);
winDebug("      nCursor = %d\n", nCursor);
	    winCommitCompositionResult (winHIMCtoContext(hIMC), GCS_CURSORPOS, (int*)&nCursor, 0);
	  }

	if (lParam & GCS_COMPCLAUSE)
	{
	    winDebug ("    GCS_COMPCLAUSE\n");
	    BufLen = ImmGetCompositionStringW(hIMC, GCS_COMPCLAUSE, NULL, 0);
	    if ((BufLen == IMM_ERROR_NODATA) || (BufLen == IMM_ERROR_GENERAL) || (BufLen == 0))
	    {
winDebug ("      NODATA.\n");
	    } else
	    {
		int i;
		dwCompCls = (LPDWORD)calloc(1, BufLen);
		BufLen = ImmGetCompositionStringW(hIMC, GCS_COMPCLAUSE, dwCompCls, BufLen);
		ClauseInf.nNumClause = BufLen / sizeof(DWORD) - 1;	// 全体のサイズが入っている最後のDWORDはのぞく

		for (i=ClauseInf.nNumClause - 1; i >= 0; i--)
		{
winDebug ("      check: dwCompCls[%d] = %d, nCursor = %d\n", i, dwCompCls[i], nCursor);
		    if (dwCompCls[i] <= nCursor)
		    {
			ClauseInf.nCurClause = i;
			ClauseInf.nOffset = nCursor - dwCompCls[i];	// nOffsetは文字数
			break;
		    }
		}
		ClauseInf.pClauseList = dwCompCls;
winDebug ("      nNumClause: %d, nCurClause: %d\n", ClauseInf.nNumClause, ClauseInf.nCurClause);
		winCommitCompositionResult (winHIMCtoContext(hIMC), GCS_COMPCLAUSE, &ClauseInf, 0);
	    }
	}

	if (lParam & GCS_COMPATTR)
	  {
	    int			nLen = 0;
	    char		*pAttr = NULL;

	    winDebug ("    GCS_COMPATTR\n");
	    /* Get result */
	    nLen = ImmGetCompositionStringW(hIMC, GCS_COMPATTR, NULL, 0);
	    pAttr = (char*)  malloc (nLen);
	    ImmGetCompositionStringW(hIMC, GCS_COMPATTR, pAttr, nLen);

{	// for debug
	int i;
	for (i=0; i<nLen; i++)
	{
		winDebug ("      ATTR: No. %02d is 0x%02x\n", i, pAttr[i]);
	}
}

	    winCommitCompositionResult (winHIMCtoContext(hIMC), GCS_COMPATTR, pAttr, nLen);
	  }
#endif

	if (lParam & GCS_COMPSTR)
	  {
	    int			nUnicodeSize = 0;
	    wchar_t		*pwszUnicodeStr = NULL;

	    winDebug ("    GCS_COMPSTR\n");
	    /* Get result */
	    nUnicodeSize = ImmGetCompositionStringW(hIMC, GCS_COMPSTR, NULL, 0) + 2;
winDebug("      nUnicodeSize = %d\n", nUnicodeSize);
	    pwszUnicodeStr = (wchar_t*) calloc (1, nUnicodeSize);
	    ImmGetCompositionStringW(hIMC, GCS_COMPSTR, pwszUnicodeStr, nUnicodeSize);

	    winCommitCompositionResult (winHIMCtoContext(hIMC), GCS_COMPSTR, pwszUnicodeStr, nUnicodeSize);

	    winWinIMESendEvent (WinIMEControllerNotify,
				WinIMENotifyMask,
				WinIMEComposition,
				WinIMECMPCompStr,
				winHIMCtoContext(hIMC),
				hwnd);
	  }

	ImmReleaseContext(hwnd, hIMC);
      }
//      break;
      return 0;

    case WM_IME_ENDCOMPOSITION:
      winDebug ("winIMEMessageHandler - WM_IME_ENDCOMPOSITION\n");
      {
	HIMC hIMC = ImmGetContext(hwnd);
	winWinIMESendEvent (WinIMEControllerNotify,
			    WinIMENotifyMask,
			    WinIMEEndComposition,
			    0,
			    winHIMCtoContext(hIMC),
			    hwnd);
	ImmReleaseContext(hwnd, hIMC);
      }
      return 0;

    case WM_IME_CHAR:
      winDebug ("winIMEMessageHandler - WM_IME_CHAR\n");
//      break;
      return 0;
    case WM_CHAR:
      winDebug ("winIMEMessageHandler - WM_CHAR\n");
      break;
//      return 0;


    case WM_IME_SETCONTEXT:
      winDebug ("winIMEMessageHandler - WM_IME_SETCONTEXT\n");
#if 0
    {
	BOOL fStatus = (wParam == 1)?TRUE:FALSE;
	HIMC hIMC = ImmGetContext(hwnd);
	ImmReleaseContext(hwnd, hIMC);
	pWIC = FindContext(winHIMCtoContext(hIMC));
	if (pWIC != NULL)
	{
	    BOOL fOpen = ImmGetOpenStatus(hIMC);
	    if (fOpen != fStatus)
	 	break;

	    winWinIMESendEvent (WinIMEControllerNotify,
				WinIMENotifyMask,
				WinIMEOpenStatus,
				fStatus,
				winHIMCtoContext(hIMC),
				hwnd);
	}
    }
#else
      {
	HIMC hIMC = ImmGetContext(hwnd);
winDebug("  hIMC:0x%X\n", hIMC);
	pWIC = FindContext(winHIMCtoContext(hIMC));
	if (pWIC != NULL)
{
	  pWIC->fActiveStat = (wParam == 1)?TRUE:FALSE;
winDebug("  active = %s\n", pWIC->fActiveStat?"TRUE":"FALSE");
}
      }
#endif
      break;

    case WM_IME_CONTROL:
      winDebug ("winIMEMessageHandler - WM_IME_CONTROL(wParam = %d)\n", wParam);
      if (wParam == IMC_SETCANDIDATEPOS)
      {
winDebug ("  IMC_SETCANDIDATEPOS\n");
/*
winDebug ("  Send WinIMEOpenCand Message...\n");
	    HIMC hIMC = ImmGetContext(hwnd);
	    int nIndex = 0;
	    if (lParam && 0x01)
		nIndex = 0;
	    else if (lParam && 0x02)
		nIndex = 1;
	    else if (lParam && 0x04)
		nIndex = 2;
	    winWinIMESendEvent (WinIMEControllerNotify,
				WinIMENotifyMask,
				WinIMEOpenCand,
				nIndex,
				winHIMCtoContext(hIMC),
				hwnd);
	    ImmReleaseContext(hwnd, hIMC);
*/
      }
/*
      if (wParam == IMC_SETCANDIDATEPOS)
      {
	HIMC hIMC = ImmGetContext(hwnd);
	ImmNotifyIME(hIMC, NI_OPENCANDIDATE, 0, 0);
	ImmReleaseContext(hwnd, hIMC);
      }
*/
      break;

    case WM_IME_COMPOSITIONFULL:
      winDebug ("winIMEMessageHandler - WM_IME_COMPOSITIONFULL\n");
      break;

    case WM_IME_SELECT:
      winDebug ("winIMEMessageHandler - WM_IME_SELECT\n");
      break;

    case WM_IME_REQUEST:
      winDebug ("winIMEMessageHandler - WM_IME_REQUEST(wParam = %d)\n", wParam);
#if 1	// 実験
      if (wParam == IMR_QUERYCHARPOSITION)
      {
	IMECHARPOSITION* pPos = (IMECHARPOSITION*)lParam;
	HIMC hIMC = ImmGetContext(hwnd);
	pWIC = FindContext(winHIMCtoContext(hIMC));
	if (pWIC != NULL)
	{

	    winDebug ("  IMR_QUERYCHARPOSITION\n");
	    winDebug("  ** CANDPOS ** x = %d, y = %d(, n = %d)\n", pWIC->nCandPosX, pWIC->nCandPosY, pWIC->nCandPage);
	    pPos->dwSize = sizeof(IMECHARPOSITION);
	    pPos->dwCharPos = 0;
	    pPos->pt.x = pWIC->nCandPosX;
	    pPos->pt.y = pWIC->nCandPosY;
	    pPos->cLineHeight = 10;
	    pPos->rcDocument.left = 10;
	    pPos->rcDocument.top = 10;
	    pPos->rcDocument.bottom = 20;
	    pPos->rcDocument.right = 20;
	    return 1;	// If the application filled the IMECHARPOSITION structure in lParam, return non-zero.
	}
      }
#endif
      break;

    case WM_IME_KEYDOWN:
      winDebug ("winIMEMessageHandler - WM_IME_KEYDOWN\n");
      break;

    case WM_IME_KEYUP:
      winDebug ("winIMEMessageHandler - WM_IME_KEYUP\n");
      break;

    default:
      break;
    }
//  return 0;
  return DefWindowProc (hwnd, message, wParam, lParam);
}
