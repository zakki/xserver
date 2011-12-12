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
static  char    rcsid[] = "$Id: compstr.c,v 1.2 1999/08/24 09:01:08 ishisone Exp $" ;
#endif  /* !lint */
#include	<Xatoklib.h>
#include	<key_def.h>

/* Page */
/* 
 * [関数名]
 *		XatokCompStr( )
 * [表題]
 *		未確定文字列状態取得
 * [呼出形式]
 *		int	XatokCompStr( WCHAR *compStr, WORD *compAttr,
 *				WORD compLen, int *revPos, int *revLen )
 *		
 * [引数]
 *		型        : 名       称   : IO : 説      明
 *		WCHAR : *compStr : i : 未確定文字列
 *		WORD : *compAttr : i : 未確定文字列属性
 *		WORD : compLen : i : 未確定文字列長さ
 *		int : *revPos : o : 反転表示開始位置
 *		int : *revLen : o : 反転表示長さ
 *	
 * [返り値]
 *		セグメントの個数(1-3)
 *		
 * [使用関数]
 *		なし
 * [機能]
 *		未確定文字列の文字情報、属性情報より、
 *		未確定文字列の表示状態を取得する。
 *		表示状態とは、 ノーマル表示、反転表示から成立ち、
 *		最大３つのセグメントに分割される。
 *
 */

int XatokCompStr(compStr, compAttr, compLen, revPos, revLen)
WCHAR		*compStr;
WORD		*compAttr;
WORD		compLen;
int		*revPos;
int		*revLen;
{
	int	i ;
	int	nseg = 0 ;
	int	prevAttr = -1 ;
	int	isReverse = 0 ;
	*revPos = 0 ;
	*revLen = 0 ;

	for( i = 0 ; i < ( int )compLen ; i++ ) {
		/* printf( "compAttr[%d] = %d\n", i , compAttr[i] ) ; */
		switch( compAttr[i] ) {
			case ATCOLINDX_TARGETCONVERT:
			case ATCOLINDX_TARGETNOTCONVERTED:
			case ATCOLINDX_INPUTCUR:
			case ATCOLINDX_INPUTCURKOTEI:
			case ATCOLINDX_TARGETNOTCONVERTEDKOTEI:
			case ATCOLINDX_TARGETCOMMENT :
				( *revLen )++ ;
				if ( !isReverse ) {
					*revPos = i ;
					isReverse = 1 ;
				}
				if ( prevAttr != 1 ) {
					prevAttr = 1 ;
					nseg++ ;
				}
				break ;
			default :
				if ( prevAttr != 0 ) {
					prevAttr = 0 ;
					nseg++ ;
				}
				break ;
		}
	}
	return nseg ;
}

/* End of compstr.c */
