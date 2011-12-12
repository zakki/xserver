/* $Id: AtokP.h,v 1.1 1999/08/13 06:26:59 ishisone Exp $ */
/*
 * Copyright 1999 Justsystem Corporation, Japan.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Justsystem Corporation
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  Justsystem
 * Corporation makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * Author: Atsushi Irisawa
 */

#ifndef _AtokP_h
#define _AtokP_h

#include "InputConvP.h"

#include "WStr.h"
#include "Atok.h"
#include "Xatoklib.h"

typedef struct {
    int foo;
} AtokClassPart;

typedef struct _AtokClassRec {
    ObjectClassPart 	object_class;
    InputConvClassPart	inputConv_class;
    AtokClassPart	atok_class;
} AtokClassRec;

#define	ATOK_NSEGMENTS	20 		/* 文節の最大数 ???? */

#define	NBR_OF_PART	3
#if 0
typedef	struct	{
    /* 表示は 全部(最大)で３つのパートになる。 */
    ICString	ics[NBR_OF_PART];
    wchar	*gline[NBR_OF_PART];
    int		glsize[NBR_OF_PART];
    int		gllen[NBR_OF_PART];
    /* キー入力されたデータの保存バッファ */
    wchar	echoStr[1024];
    int		echoLen;
    int		revPos;
    int		revLen;
} InputBuf;
#endif

typedef	struct	{
    wchar	name[64];
    int		length;
    int		size;
} AtokMode;

typedef struct {
    /* RESOURCES */
    String	atokserver;
    String	port;
    String	conffile;
    String	stylefile;

    /* private data */
    long	context;	/* ATOK Communication  のContext */
    /*AtokMode	mode;*/
    Boolean	textchanged;	/* 変換テキストが変わったか */
    /*int	convStatus;	/* 0 : 無変換
				/* 1 : 変換中
				 * 2 : 候補表示中
				 * 3 : コード入力中
				 * 4 : ユーティリティー
				 */
    /* 未確定文字列表示用の領域(for ki2) */
    ICString	ics[NBR_OF_PART];/* 未確定文字列表示 */

    /* セグメントの管理 */
    _XatokRecPtr comm;		/* ATOKCE とのコミュニケーション */

    /* 候補データ関係の定義 */
    int		nbr_of_cand;	/* 候補選択時の候補数 */
    ICString	*cand_lists_ics;/* 候補 */
    int		cur_cand;	/* 選択されている広報 */
    int		selected_cand;
    int		cand_list_size;	/* アロケーションされているサイズ */

    /* AUX ウィンドウのためのICS 管理領域 */
    /*wchar	aux_echoStr[BUFSIZ*2]; */
    /*int	aux_echoLen;*/
    /*int	aux_revLen;*/
    /*int	aux_revPos;*/
    ICString	aux_ics[NBR_OF_PART];
    wchar	*aux_string[NBR_OF_PART];
    int		aux_length[NBR_OF_PART];
    int		aux_size[NBR_OF_PART];
    int		aux_curseg;
    int		aux_nseg;
    /*wchar	**atok_cand;*/
} AtokPart;

typedef struct _AtokRec {
    ObjectPart  	object;
    InputConvPart	inputConv;
    AtokPart		atok;
} AtokRec;

#endif
