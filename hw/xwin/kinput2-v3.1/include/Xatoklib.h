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
    M_NOKEYCONV = 0,		/* $B%-!<%9%k!<(B */
    M_KEYCONV   = (1L<<0),	/* $BJQ49Cf(B */
    M_AUX       = (1L<<1),	/* AUX WINDOW $BC18lEPO?(B  */
    M_CAND      = (1L<<2),	/* $B8uJdI=<(Cf(B */
    M_SYSLINE   = (1L<<3),	/* $B%7%9%F%`9T(B */
    M_WDEL      = (1L<<4)	/* $BC18l:o=|(B */
} opMode;

typedef	enum {
    W_WORD	= (1L<<0),	/* $BC18l(B */
    W_YOMI	= (1L<<1),	/* $B$h$_F~NOCf(B */
    W_HINSHI	= (1L<<2),	/* $BIJ;lA*BrCf(B */
    W_YORN	= (1L<<3),	/* $B=hM}3NG'(B */
    W_RESULT	= (1L<<4)	/*  $B7k2LI=<((B */
} wordMode;

typedef enum {
    M_TOP,			/* AUX $B%H%C%W%a%K%e!<(B */
    M_WORDREG,			/* $BC18lEPO?(B */
    M_CODE,			/* AUX CODE $B%a%K%e!<(B */
    M_CODESJ,			/* $B%3!<%IF~NO(B SJIS */
    M_CODEEUC,			/* $B%3!<%IF~NO(B EUC */
    M_CODEJIS,			/* $B%3!<%IF~NO(B JIS */
    M_CODEKUTEN,		/* $B%3!<%IF~NO6hE@(B */
    M_KIGOU,			/* AUX $B5-9fF~NO%a%K%e!<(B */
    M_NKIGOU,			/* 10 */
    M_SKIGOU,
    M_DICUTIL,			/* AUX $B<-=q%a%K%e!<(B */
    M_WORD,			/* AUX $BC18lEPO?(B */
    M_WORDR,
    M_WORDD
} menuMode;

typedef	struct	{
    WCHAR	name[64];
    int		length;
    WORD	modeKANDSP;	/* IME_PRV_SETKANDSP 1 - 4 
					1 : $BJ?2>L>(B
					2 : $BA43Q%+%?%+%J(B
					3 : $BA43QL5JQ49(B
					4 : $BH>3Q%+%?%+%J(B
					5 : $BH>3QL5JQ49(B
				 */
    WORD	modeHEN;	/* IME_PRV_SETHENMODE 
					0 : $BO"J8@a(B
					1 : $BJ#J8@a(B
					2 : $B<+F0(B
				 */
    WORD	modeGAKU;	/* IME_PRV_GAKMODE
					0 : $B3X=,(BOFF
					1 : $B3X=,(BON
					2 : $B%a%b%j!<3X=,(B
				 */
    WORD	modeKOTE;	/* IME_SETKOTEMODE
					0 : $B8GDjF~NO2r=|(B
					1 : $B$R$i$,$J8GDj(B
					2 : $BA43Q%+%?%+%J8GDj(B
					3 : $BA43QL5JQ498GDj(B
					4 : $BH>3Q%+%?%+%J8GDj(B
					5 : $BH>3QL5JQ498GDj(B
				*/
    WORD	modeOKURI;	/* IME_PRV_SETOKURIMODE
					0 : $BK\B'(B
					1 : $BAw$k(B
					2 : $B>J$/(B
					3 : $BA4$F(B
				 */
    WORD	modeROT;	/* IME_PRV_SETROTMODEF
					1 : $B4A;zF~NO(B
					2 : $BH>3QF~NO(B
					3 : $B%3!<%IF~NO(B
					4 : $B5-9fF~NO(B
				*/
    /*int	changes;*/
} _XatokMode;

#define	WORD_HYOKI_MAX	64
#define	WORD_YOMI_MAX	32

/* #define NBR_OF_PART	(3) */
#define	ATOK_NSEG	(3)

typedef	struct	{
    /* $B%5!<%P$H$N@\B3>pJs(B */
    int		sock;				/* $B%5!<%P(B */
    int		clntid;				/* $B%;%C%7%g%s(B */
    int		ctxtid;				/* ID */
    /* $B%;%0%a%s%H$N4IM}(B */
    _XatokSegment segments[ATOK_NSEG+3];	/* $BL$3NDjJ8;zNs$N%;%0%a%s%H(B */
    int		nbr_of_seg;			/* $B;HMQ$7$F$$$k%;%0%a%s%H?t(B */
    int		cur_seg;			/* $B8=:_$N%;%0%a%s%H(B */
    int		offset;

    /* $B8=:_$NF~NO%b!<%I$N4IM}(B */
    _XatokMode	mode;				/* $B%b!<%II=<(J8;zNs(B */
    opMode	convStatus;			/* M_XXXX $B$r;2>H(B */
    opMode	oldStatus;			/* M_XXXX $B$r;2>H(B */
    int		candStatus;			/* 0 :
						   1 : $B8uJdI=<(3+;O(B
						   2 : $B8uJdI=<(Cf(B
						   */
    menuAux	*menuAux;
    menuAux	*curAux;
    int		menuStatus;

    wchar	wbuf[BUFSIZ*4];			/* $B3NDjJ8;zNsMQ$N%P%C%U%!(B */
    char	wrbuf[BUFSIZ*4];		/* $B3NDjJ8;zNsFI$_MQ$N%P%C%U%!(B */
    int	wlen;					/* $B3NDjJ8;zNs$ND9$5(B */

    /* $BJQ49CfJ8;zNsI=<(MQ%G!<%?NN0h(B */
    wchar	echoStr[BUFSIZ*2];		/* $BJQ49CfMQJ8;zNs(B */
    int		echoLen;			/* $BJQ49CfJ8;zNs$ND9$5(B */
    int		revPos;				/* $BH?E>$r3+;O$9$k%]%8%7%g%s(B */
    int		revLen;				/* $BH?E>J8;zNs$ND9$5(B */

    /* AUX $B%&%#%s%I%&$NI=<(J8;zNs$N%G!<%?NN0h(B */
    wchar	aux_echoStr[BUFSIZ*2];	
    int		aux_echoLen;
    int		aux_revPos;
    int		aux_revLen;			/* $BH?E>J8;zNs$ND9$5(B */
    int		aux_isopened; 

    /*
     * $BC18lEPO?$N$?$a$N%G!<%?(B
     * $BC18l(B $BFI$_!"I=5-!"IJ;l>pJs!"IJ;lAm?t!"<-=q>pJs(B
     */
    int		wordStatus;
    wchar	word_str[66+1];
    wchar	word_yomi[66+1];
    int		word_strLen;
    int		word_yomiLen;
    wordMode	word_mode;			/* 0 : $BI=5-F~NOCf(B
						   1 : $BFI$_F~NOCf(B
						   2 : $BIJ;lA*BrCf(B
						   3 : $B<-=qA*BrCf(B
						   */
    int		word_hinshi;
    int		hinsiNum;
    ATOKDEHINSTR *hinsiPtr;
    int		dicNum;
    char	**dicPtr;

    /* $B8uJdJ8;zNs$NNN0h(B */
    int		kohoNum;			/* $B8uJd$N?t(B */
    int		kohoNum2;
    int		kohoPos;			/* $BA*Br$5$l$F$$$k8uJdHV9f(B */
    int		kohoSize;			/* $B8uJd%G!<%?MQ%P%C%U%!%5%$%:(B */
    int		kohoStrLen;			/* $B<hF@$7$F$$$k8uJdJ8;zNs?t(B */
    WCHAR	*kohoStrPtr;			/* $B8uJdJ8;zNs(B */
    BYTE	*kohoLenPtr;			/* $B3F8uJdJ8;zNs$ND9$5(B */
    BYTE	*kohoSfxPtr;			/* ???? */
    int		kohoCurPage;			/* $B8=:_I=<($7$F$$$k%Z!<%8(B */
    int		kohoPageNbr;			/* $BA4%Z!<%8?t(B */
    int		kohoMax;

    wchar	sysLineStr[BUFSIZ];
    WORD	*sysAttrPtr;
    int		sysLineLen;
    int		sysLineSize;
    DWORD	guide_openEventIndex;
    DWORD	guide_closeEventIndex;
    DWORD	guide_OpenIndex;

    /* $B%5!<%P$H$NDL?.%P%C%U%!(B */
    AtokReqPacket	NETrqst;		/* $B%j%/%(%9%HMQ$NNN0h(B */
    AtokRsltPacket	NETrslt;		/* $BJQ497k2LMQ$NNN0h(B */

} _XatokRec, *_XatokRecPtr;


typedef	enum {
    XKEY_START		= (1L<<0),	/* $BJQ49%-!<(B */
    XKEY_END		= (1L<<1),	/* $BJQ49=*N;(B */
    XKEY_FIX		= (1L<<2),	/* $B3NDj(B */
    XKEY_LEFT		= (1L<<3),	/* $B:8(B */
    XKEY_RIGHT		= (1L<<4),	/* $B1&(B */
    XKEY_UP		= (1L<<5),	/* $B>e(B */
    XKEY_DOWN		= (1L<<6),	/* $B2<(B */
    XKEY_FIRST		= (1L<<7),	/* $B@hF,(B */
    XKEY_LAST		= (1L<<8),	/* $B:G8e(B */
    XKEY_PREVP		= (1L<<9),	/* $BA0%Z!<%8(B */
    XKEY_NEXTP		= (1L<<10),	/* $B<!%Z!<%8(B */
    XKEY_CAND		= (1L<<11),	/* $B8uJd(B */
    XKEY_SELECTED	= (1L<<12),	/* $BA*Br(B */
    XKEY_CANCEL		= (1L<<13),	/* $B%-%c%s%;%k(B */
    XKEY_START_SELECTION= (1L<<14),	/* $B8uJdI=<((B*/
    XKEY_DICT		= (1L<<15),	/* $B<-=q(B */
} XKEY_TYPE;


#define	SetIsKakuteI(stat)	((stat) |= 1)
#define	SetIsMode(stat)		((stat) |= 2)
#define	IsKakutei(stat)		((stat) & 1)
#define	IsMode(stat)		((stat) & 2)

extern menuAux *getMenuInstance();
extern menuAux *auxMenuCopy();

#endif	/* !_xatoklib_h_ */
