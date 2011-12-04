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

#ifndef lint
static  char    rcsid[] = "$Id: sysline.c,v 1.2 1999/08/24 09:01:10 ishisone Exp $" ;
#endif  /* !lint */

#include        <stdio.h>
#include        <X11/IntrinsicP.h>
#include        <X11/StringDefs.h>
#include        <X11/Xmu/Atoms.h>
#include        <X11/keysym.h>
#include	<InputConv.h>

#include	<Xatoklib.h>

/* Page */
/*
 * [関数名]
 *              auxSyslineCtrl( )
 * [表題]
 *              データの送信
 * [呼出形式]
 *      	int auxSyslineCtrl( _XatokRecPtr acomm, int isdecision )
 *
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		_XatokRecPtr : acomm   : i/o : kinput2用変換バッファ
 *		int          : isdecision : i : 確定データフラグ
 *
 *
 * [返り値]
 *		 0 : AUX ウィンドウ継続
 *		 1 : AUX ウィンドウ終了
 *
 * [使用関数]
 *		なし
 * [機能]
 *		SYSLINEデータのフラグが ATOK12CE エンジンで設定されている場合に
 *		システム行を新規に開く、システム行の文字列を変更する、
 *		システム行を閉じるなどの、表示をどのように行なうかを判断する
 *		ファンクションであり、表示を管理しているモジュールより
 *		呼び出される。
 */
int auxSyslineCtrl(acomm, isdecision)
_XatokRecPtr    acomm;
int		isdecision;		/* 確定文字があるかないか？ */
{
	int	status = 0 ;

#ifdef	ATOK_DEBUG
	printf( "GUIDE isdecision %d\n", isdecision ) ;
	printf( "GUIDE VAL %d\n", acomm->guide_OpenIndex ) ;
	printf( "GUIDE CLOSE VAL %d\n", acomm->guide_closeEventIndex ) ;
	printf( "GUIDE OPEN VAL %d\n", acomm->guide_openEventIndex ) ;
#endif
	if ( acomm->guide_openEventIndex & CEGUIDEINDX_ERROR ) {
		/* printf( "CEGUIDEINDX_ERROR\n" ) ; */
		acomm->convStatus |= M_SYSLINE ;
		if ( acomm->aux_isopened ) {
			acomm->menuStatus = ICAuxChange ;
		}
		else {
			acomm->menuStatus = ICAuxStart ;
		}
		acomm->aux_isopened = 1 ;
	}
	else if ( acomm->guide_OpenIndex & CEGUIDEINDX_CODE )  {
		/* printf( "CEGUIDEINDX_CODE\n" ) ; */
		acomm->convStatus |= M_SYSLINE ;
		if ( acomm->aux_isopened ) {
			acomm->menuStatus = ICAuxChange ;
		}
		else {
			acomm->menuStatus = ICAuxStart ;
		}
		acomm->aux_isopened = 1 ;
	}
	else if ( acomm->guide_OpenIndex & CEGUIDEINDX_KIGOU ) {
		/* printf( "CEGUIDEINDX_CODE\n" ) ; */
		acomm->convStatus |= M_SYSLINE ;
		if ( acomm->aux_isopened ) {
			acomm->menuStatus = ICAuxChange ;
		}
		else {
			acomm->menuStatus = ICAuxStart ;
		}
		acomm->aux_isopened = 1 ;
		status = isdecision ;
	}
	else {
		/* printf( "SYSLINE OTHER\n" ) ; */

		acomm->convStatus ^=  M_SYSLINE ;	/* 0616 */
		acomm->aux_isopened = 0 ;
		acomm->menuStatus = ICAuxEnd ;
		status = 1 ;
	}
	return status ;
}
/* Page */
/*
 * [関数名]
 *		auxSyslineString( )
 *              AtokNetSend( )
 * [表題]
 *              システム行文字列の作成。
 * [呼出形式]
 *      int auxSyslineString( _XatokRecPtr acomm, menuAux *mAux,
 *		wchar *echoStr, int *echoLen, int *revLen, int	*revPos )
 *
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		_XatokRecPtr : acomm   : i/o : kinput2用変換バッファ
 *		menuAux	     : *mAux:     : 現在未使用
 *		wchar	     : *echoStr:   o : 表示文字列
 *		int          : *echoLen:   o : 表示文字列の長さ
 *		int	     : *revLen :   o : 反転開始位置
 *		int	     : *revPos :   o : 反転文字列長さ
 *
 *
 * [返り値]
 *		-1 : 送信エラー
 *		 N : 送信バイト数
 *
 * [使用関数]
 *		なし
 * [機能]
 *		システム行に表示する文字列を作成し、反転表示位置、反転表示の
 *		長さを求める。
 */

int auxSyslineString(acomm, mAux, echoStr, echoLen, revLen, revPos)
_XatokRecPtr    acomm;
menuAux		*mAux;
wchar		*echoStr;
int		*echoLen;
int		*revLen;
int		*revPos;
{
	int	wsize = sizeof( wchar ) ;
	int	isreverse = 0 ;
	int	rev = 0 ;
	int	i ;

	*revPos = 0 ;
	*revLen = 0 ;

	memcpy( acomm->aux_echoStr, acomm->sysLineStr, acomm->sysLineLen * wsize ) ;

	*echoLen = acomm->sysLineLen ;
	for ( i = 0 ; i < acomm->sysLineLen ; i++ ) {
		switch( acomm->sysAttrPtr[i] ) {
			case ATCOLINDX_SYSLINEBACK :
			case ATCOLINDX_SYSLINEINPUT :
			case ATCOLINDX_SYSLINEINPUTCUR :
			case ATCOLINDX_SYSLINENOTSELECTITEM :
			case ATCOLINDX_SYSLINEHOSOKU :
			case ATCOLINDX_SYSLINEGUIDEMESSAGE :
				if ( isreverse ) {
					goto done;
				}
				rev++ ;
				break ;
			case ATCOLINDX_SYSLINESELECTITEM :
				isreverse = 1 ;
				( *revLen ) ++ ;
				break ;
		}
	}
done:
	if ( isreverse ) {
		*revPos = rev ;
	}
	return 1 ;
}
/* End of sysline.c */
