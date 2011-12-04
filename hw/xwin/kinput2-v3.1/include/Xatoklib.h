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

#ifndef	_xatoklib_h_
#define	_xatoklib_h_

#include <atok_typ.h>
#include <atokce.h>
#include <atok12net.h>
#include <key_def.h>

/* #include <stdio.h> /* for BUFSIZ? */
#ifdef BUFSIZ
#undef BUFSIZ
#endif
#define BUFSIZ	1024

/* #define CAND_PAGE */
#ifdef CAND_PAGE
#undef CAND_PAGE
#endif

#define	CAND_ROWS	(9)
#define	CAND_COLS	(1) 
#include "WStr.h"

typedef	struct	{
    int		length;
    int		size;
    wchar	*string;
    int		offset;
} _XatokSegment;

typedef enum {
    OP_NONE = 0,
    OP_NEXT,
    OP_PREV,
    OP_FIRST,
    OP_LAST,
    OP_SELECT,
    OP_CANCEL,
    OP_SPEC
} _opType;

typedef struct _menuItem MenuItem;
typedef struct _menu MenuAux;
typedef struct _menuItem {
    WORD	key;
    int		mode;
    _opType	op;
    MenuAux	*child;
    char	*numStr;
    char	*itemStr;
} menuItem;

typedef struct  _menu {
    int		mode;
    int		cur;
    int		(*startProc)();
    int		(*defaultProc)();
    int		(*endProc)();
    int		(*stringProc) ();
    char	*str;
    MenuAux	*parent;
    menuItem	*items;
    int		nitems;
} menuAux;

typedef	enum {
    M_NOKEYCONV = 0,		/* キースルー */
    M_KEYCONV   = (1L<<0),	/* 変換中 */
    M_AUX       = (1L<<1),	/* AUX WINDOW 単語登録  */
    M_CAND      = (1L<<2),	/* 候補表示中 */
    M_SYSLINE   = (1L<<3),	/* システム行 */
    M_WDEL      = (1L<<4)	/* 単語削除 */
} opMode;

typedef	enum {
    W_WORD	= (1L<<0),	/* 単語 */
    W_YOMI	= (1L<<1),	/* よみ入力中 */
    W_HINSHI	= (1L<<2),	/* 品詞選択中 */
    W_YORN	= (1L<<3),	/* 処理確認 */
    W_RESULT	= (1L<<4)	/*  結果表示 */
} wordMode;

typedef enum {
    M_TOP,			/* AUX トップメニュー */
    M_WORDREG,			/* 単語登録 */
    M_CODE,			/* AUX CODE メニュー */
    M_CODESJ,			/* コード入力 SJIS */
    M_CODEEUC,			/* コード入力 EUC */
    M_CODEJIS,			/* コード入力 JIS */
    M_CODEKUTEN,		/* コード入力区点 */
    M_KIGOU,			/* AUX 記号入力メニュー */
    M_NKIGOU,			/* 10 */
    M_SKIGOU,
    M_DICUTIL,			/* AUX 辞書メニュー */
    M_WORD,			/* AUX 単語登録 */
    M_WORDR,
    M_WORDD
} menuMode;

typedef	struct	{
    WCHAR	name[64];
    int		length;
    WORD	modeKANDSP;	/* IME_PRV_SETKANDSP 1 - 4 
					1 : 平仮名
					2 : 全角カタカナ
					3 : 全角無変換
					4 : 半角カタカナ
					5 : 半角無変換
				 */
    WORD	modeHEN;	/* IME_PRV_SETHENMODE 
					0 : 連文節
					1 : 複文節
					2 : 自動
				 */
    WORD	modeGAKU;	/* IME_PRV_GAKMODE
					0 : 学習OFF
					1 : 学習ON
					2 : メモリー学習
				 */
    WORD	modeKOTE;	/* IME_SETKOTEMODE
					0 : 固定入力解除
					1 : ひらがな固定
					2 : 全角カタカナ固定
					3 : 全角無変換固定
					4 : 半角カタカナ固定
					5 : 半角無変換固定
				*/
    WORD	modeOKURI;	/* IME_PRV_SETOKURIMODE
					0 : 本則
					1 : 送る
					2 : 省く
					3 : 全て
				 */
    WORD	modeROT;	/* IME_PRV_SETROTMODEF
					1 : 漢字入力
					2 : 半角入力
					3 : コード入力
					4 : 記号入力
				*/
    /*int	changes;*/
} _XatokMode;

#define	WORD_HYOKI_MAX	64
#define	WORD_YOMI_MAX	32

/* #define NBR_OF_PART	(3) */
#define	ATOK_NSEG	(3)

typedef	struct	{
    /* サーバとの接続情報 */
    int		sock;				/* サーバ */
    int		clntid;				/* セッション */
    int		ctxtid;				/* ID */
    /* セグメントの管理 */
    _XatokSegment segments[ATOK_NSEG+3];	/* 未確定文字列のセグメント */
    int		nbr_of_seg;			/* 使用しているセグメント数 */
    int		cur_seg;			/* 現在のセグメント */
    int		offset;

    /* 現在の入力モードの管理 */
    _XatokMode	mode;				/* モード表示文字列 */
    opMode	convStatus;			/* M_XXXX を参照 */
    opMode	oldStatus;			/* M_XXXX を参照 */
    int		candStatus;			/* 0 :
						   1 : 候補表示開始
						   2 : 候補表示中
						   */
    menuAux	*menuAux;
    menuAux	*curAux;
    int		menuStatus;

    wchar	wbuf[BUFSIZ*4];			/* 確定文字列用のバッファ */
    char	wrbuf[BUFSIZ*4];		/* 確定文字列読み用のバッファ */
    int	wlen;					/* 確定文字列の長さ */

    /* 変換中文字列表示用データ領域 */
    wchar	echoStr[BUFSIZ*2];		/* 変換中用文字列 */
    int		echoLen;			/* 変換中文字列の長さ */
    int		revPos;				/* 反転を開始するポジション */
    int		revLen;				/* 反転文字列の長さ */

    /* AUX ウィンドウの表示文字列のデータ領域 */
    wchar	aux_echoStr[BUFSIZ*2];	
    int		aux_echoLen;
    int		aux_revPos;
    int		aux_revLen;			/* 反転文字列の長さ */
    int		aux_isopened; 

    /*
     * 単語登録のためのデータ
     * 単語 読み、表記、品詞情報、品詞総数、辞書情報
     */
    int		wordStatus;
    wchar	word_str[66+1];
    wchar	word_yomi[66+1];
    int		word_strLen;
    int		word_yomiLen;
    wordMode	word_mode;			/* 0 : 表記入力中
						   1 : 読み入力中
						   2 : 品詞選択中
						   3 : 辞書選択中
						   */
    int		word_hinshi;
    int		hinsiNum;
    ATOKDEHINSTR *hinsiPtr;
    int		dicNum;
    char	**dicPtr;

    /* 候補文字列の領域 */
    int		kohoNum;			/* 候補の数 */
    int		kohoNum2;
    int		kohoPos;			/* 選択されている候補番号 */
    int		kohoSize;			/* 候補データ用バッファサイズ */
    int		kohoStrLen;			/* 取得している候補文字列数 */
    WCHAR	*kohoStrPtr;			/* 候補文字列 */
    BYTE	*kohoLenPtr;			/* 各候補文字列の長さ */
    BYTE	*kohoSfxPtr;			/* ???? */
    int		kohoCurPage;			/* 現在表示しているページ */
    int		kohoPageNbr;			/* 全ページ数 */
    int		kohoMax;

    wchar	sysLineStr[BUFSIZ];
    WORD	*sysAttrPtr;
    int		sysLineLen;
    int		sysLineSize;
    DWORD	guide_openEventIndex;
    DWORD	guide_closeEventIndex;
    DWORD	guide_OpenIndex;

    /* サーバとの通信バッファ */
    AtokReqPacket	NETrqst;		/* リクエスト用の領域 */
    AtokRsltPacket	NETrslt;		/* 変換結果用の領域 */

} _XatokRec, *_XatokRecPtr;


typedef	enum {
    XKEY_START		= (1L<<0),	/* 変換キー */
    XKEY_END		= (1L<<1),	/* 変換終了 */
    XKEY_FIX		= (1L<<2),	/* 確定 */
    XKEY_LEFT		= (1L<<3),	/* 左 */
    XKEY_RIGHT		= (1L<<4),	/* 右 */
    XKEY_UP		= (1L<<5),	/* 上 */
    XKEY_DOWN		= (1L<<6),	/* 下 */
    XKEY_FIRST		= (1L<<7),	/* 先頭 */
    XKEY_LAST		= (1L<<8),	/* 最後 */
    XKEY_PREVP		= (1L<<9),	/* 前ページ */
    XKEY_NEXTP		= (1L<<10),	/* 次ページ */
    XKEY_CAND		= (1L<<11),	/* 候補 */
    XKEY_SELECTED	= (1L<<12),	/* 選択 */
    XKEY_CANCEL		= (1L<<13),	/* キャンセル */
    XKEY_START_SELECTION= (1L<<14),	/* 候補表示*/
    XKEY_DICT		= (1L<<15),	/* 辞書 */
} XKEY_TYPE;


#define	SetIsKakuteI(stat)	((stat) |= 1)
#define	SetIsMode(stat)		((stat) |= 2)
#define	IsKakutei(stat)		((stat) & 1)
#define	IsMode(stat)		((stat) & 2)

extern menuAux *getMenuInstance();
extern menuAux *auxMenuCopy();

#endif	/* !_xatoklib_h_ */
