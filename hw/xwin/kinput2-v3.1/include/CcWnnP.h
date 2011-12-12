/* $Id: CcWnnP.h,v 1.16 1999/04/12 08:53:38 ishisone Exp $ */
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

#ifndef _CcWnnP_h
#define _CcWnnP_h

#include "InputConvP.h"

#undef wchar
#include "WStr.h"
#include "CcWnn.h"
#include "wnnlib.h"
#include "cconv.h"

typedef struct {
    int foo;
} CcWnnClassPart;

typedef struct _CcWnnClassRec {
    ObjectClassPart object_class;
    InputConvClassPart inputConv_class;
    CcWnnClassPart ccWnn_class;
} CcWnnClassRec;

typedef enum {
    normal_state,
    selection_s_state,	/* 小文節候補選択モード */
    selection_l_state,	/* 大文節候補選択モード */
    symbol_state	/* 記号入力モード */
} CcWnnState;

typedef struct {
    /* resources */
    int		(*confirmfunc)();
    XtPointer	confirmdata;
    String	jservername;
    String	jservername2;		/* secondary jserver */
    String	wnnenvname;
    String	wnnenvrcfile;
    String	wnnenvrcfile4;		/* rcfile for Wnn4 */
    String	wnnenvrcfile6;		/* rcfile for Wnn6 */
    Boolean	wnnoverrideenv;
    String	ccdeffile;
    struct wnn_buf     *wnnbuf;
    ccRule	ccrule;
    int		saveinterval;
    /* private data */
    ccBuf	ccbuf;
    jcConvBuf	*jcbuf;
    Boolean	createrule;	/* ccrule を自分で作ったか外から与えられたか */
    Boolean	createenv;	/* wnnenv を自分で作ったか外から与えられたか */
    CcWnnState	state;		/* 変換の内部状態 */
    Boolean	textchanged;	/* 変換テキストが変わったか */
    Boolean	selectionending;/* 選択モードを終了しようとしているか */
    Boolean	sendbackevent;	/* イベントを送り返すか */
    Boolean	fixperformed;	/* 確定処理が行われたか */
    ICString	*symbollist;
    int		numsymbols;
    int		cursymbol;	/* 記号選択モードの時、現在選択されている記号 */
    ICString	*candlist;
    int		candlistsize;
    wchar	*strdata;
    int		strdatasize;
    int		numcand;	/* 選択モードの時、候補数 */
    int		curcand;	/* 候補選択モードの時、現在選択されている候補 */
    int		fixcount;
    enum {JIS_MODE, KUTEN_MODE, OTHERS} inputmode;
    wchar	*pendingdata;
    int		cont;		/* 後続のコマンドを実行するか */
    Widget	selwidget;	/* セレクション用の補助ウィジェット */
    char	*selyomi;	/* セレクションで返す「読み」 */
    char	*selkanji;	/* セレクションで返す「漢字」 */
} CcWnnPart;

typedef struct _CcWnnRec {
    ObjectPart  object;
    InputConvPart inputConv;
    CcWnnPart ccWnn;
} CcWnnRec;

#endif
