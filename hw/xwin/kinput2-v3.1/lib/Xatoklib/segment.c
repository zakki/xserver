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
static  char    rcsid[] = "$Id: segment.c,v 1.3 1999/08/24 09:05:27 ishisone Exp $" ;
#endif  /* !lint */

#include	<ICtypes.h>
#include	<Xatoklib.h>
/* Page */
/*
 * [関数名]
 *		XatokGetModeStr( )
 * [表題]
 *              モード文字列の取得
 * [呼出形式]
 *		WCHAR	*XatokGetModeStr( _XatokRecPtr abuf, int *length )
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		_XatokRecPtr : abuf   : i/o : kinput2用変換バッファ
 *		int          : *length :  o : 文字列の長さ
 *
 * [返り値]
 *		モード文字列格納領域のアドレス
 * [使用関数]
 *		なし
 * [機能]
 *		モード表示用の文字列およびその長さを取得する。
 */
WCHAR *XatokGetModeStr(abuf, length)
_XatokRecPtr	abuf;
int		*length;
{
	int i, n ;
	
	*length = ( int )abuf->mode.length ;
	n = *length ;

#ifdef	ATOK_DEBUG
	printf( "GET \245\342\241\274\245\311 : "/*=GET モード : =*/ ) ;
	for ( i = 0 ; i < n ; i++ ) {
		printf( "0x%x ", abuf->mode.name[i] ) ;
	}
	printf( "\n" ) ;
#endif	/* ATOK_DEBUG */

	return abuf->mode.name ;
}

/* Page */
/*
 * [関数名]
 *		XatokGetSegmentPosition( )
 * [表題]
 *              カレントセグメントの取得
 * [呼出形式]
 *	int	wordStartFunc( _XatokRecPtr  acomm )
 *
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		_XatokRecPtr : acomm   : i/o : kinput2用変換バッファ
 *
 * [返り値]
 *		0 : 
 *		1 :
 *
 * [使用関数]
 *		なし
 * [機能]
 *		現在のカレントセグメントを取得する。
 * 		現在管理している カレントポシションが
 * 		セグメント数より小さければそのセグメントが、
 * 		呼ばれた時のセグメントとなる。
 * 		セグメントが減少して(文節を延ばした場合など)
 * 		現在のポジションがセグメント数より大きければ、
 * 		最後のセグメントが カレントポジションとなる。
 */
int XatokGetSegmentPosition(abuf, nsegp, ncharp)
_XatokRecPtr	abuf;
int		*nsegp;
int		*ncharp;
{
	int	ret = 0 ;

	if ( abuf->cur_seg < abuf->nbr_of_seg ) {
		*nsegp = abuf->cur_seg ;
		*ncharp = abuf->segments[abuf->cur_seg].offset ;
	}
	else if ( abuf->cur_seg > 0 ) {
		*nsegp = abuf->nbr_of_seg - 1 ;
		*ncharp = abuf->segments[abuf->cur_seg].offset ;
		ret = 1 ;
	}
	else {
		*nsegp = 0 ;
		*ncharp = 0 ;
		ret = 1 ;
	}

	return ret ;
}

/* Page */
/*
 * [関数名]
 *		XatokGetSegmentNumber( )
 * [表題]
 *              セグメント数の取得
 * [呼出形式]
 *		int	XatokGetSegmentNumber( _XatokRecPtr abuf )
 *
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		_XatokRecPtr : acomm   : i/o : kinput2用変換バッファ
 *
 * [返り値]
 *		N : セグメント数
 *
 * [使用関数]
 *		なし
 * [機能]
 *		現在のセグメント数を取得する。
 */
int XatokGetSegmentNumber(abuf)
_XatokRecPtr	abuf;
{
	int	nbr ;

	nbr = abuf->nbr_of_seg ;

	return nbr ;
}

/* Page */
/*
 * [関数名]
 *		XatokGetSegmentRec( )
 * [表題]
 *              指定セグメントのデータ取得
 * [呼出形式]
 *		wchar *XatokGetSegmentRec( _XatokRecPtr *abuf,
 *			int n, int *len, int *attr )
 *
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		_XatokRecPtr : acomm   : i   : kinput2用変換バッファ
 *		int          : n       : i   : 指定セグメント番号
 *		int          : *len    :  o  : 指定セグメントの文字列の長さ
 *              int          : *attr   :  o  : 指定セグメントの属性
 *
 * [返り値]
 *		指定セグメントの文字列
 *
 * [使用関数]
 *		なし
 * [機能]
 *		指定されたセグメントの文字列およびその文字列の長さ、
 *		表示属性を取得する。
 */
wchar *XatokGetSegmentRec(abuf, n, len, attr)
_XatokRecPtr	abuf;
int		n;
int		*len;
int		*attr;
{
	wchar	*data ;

	if ( abuf->cur_seg == n ) {
		*attr = ICAttrConverted | ICAttrCurrentSegment ;
	}
	else if ( n < abuf->cur_seg ) {
		*attr = ICAttrConverted ;
	}
	else {
		*attr = ICAttrConverted ;
	}

	*len = abuf->segments[n].length ;
	data = abuf->segments[n].string ;

	return data ;
}
/* Page */
/*
 * [関数名]
 *		XatokGetConvertedLength( )
 * [表題]
 *              変換結果文字列の長さ取得
 * [呼出形式]
 *		int XatokGetConvertedLength( _XatokRecPtr abuf )
 *
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		_XatokRecPtr : abuf   : i   : kinput2用変換バッファ
 *
 * [返り値]
 *		N : 文字列の長さ
 *
 * [使用関数]
 *		なし
 * [機能]
 *		変換結果文字列の長さを取得する。
 *		変換結果文字列の長さとは、セグメントに保存されている
 *		文字列の長さの合計である。
 */
int XatokGetConvertedLength(abuf)
_XatokRecPtr	abuf;
{

	int	n = 0 ;
	int	i ;

	for( i = 0 ; i < abuf->nbr_of_seg ; i++ ) {
		n += abuf->segments[i].length ;
	}
	return n ;
}

/* Page */
/*
 * [関数名]
 *		XatokGetConvertedString( )
 * [表題]
 *              変換結果文字列の取得
 * [呼出形式]
 *		int XatokGetConvertedString( _XatokRecPtr abuf, wchar *wstr )
 *
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		_XatokRecPtr : abuf   : i   : kinput2用変換バッファ
 *		wchar        : *wstr  :  o  : 取得した文字列格納領域
 *
 * [返り値]
 *		N : 文字列の長さ
 *
 * [使用関数]
 *		なし
 * [機能]
 *		変換結果文字列の長さを取得する。
 */
int XatokGetConvertedString(abuf, wstr)
_XatokRecPtr	abuf;
wchar		*wstr;
{
	register	wchar *w1, *w2 ;
	int		i, n = 0 ;

	w1 = wstr ;

	for( i = 0 ; i < abuf->nbr_of_seg ; i++ ) {
		w2 = abuf->segments[i].string ;
		while( *w2 ) {
			*w1 = *w2 ;
			w1++ ;
			w2++ ;
			n++ ;
		}
	}
	*w1 = 0 ;
	return n ;
}
/* Page */
/*
 * [関数名]
 *		XatokShiftRight( )
 * [表題]
 *              セグメントデータの右ROTATE
 * [呼出形式]
 *		int	XatokShiftRight( _XatokRecPtr  abuf )
 *
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		_XatokRecPtr : abuf   : i/o : kinput2用変換バッファ
 *
 * [返り値]
 *		1 : 常に１
 *
 * [使用関数]
 *		なし
 * [機能]
 *		セグメントデータを右に ROTATEする。
 *		右のROTATEとは セグメントの配列のインデックスが小さいデータを
 *		１つ大きい所に移すという事である。
 *		１番大きい所は、先頭に移る。
 */
int XatokShiftRight(abuf)
_XatokRecPtr	abuf;
{
	_XatokSegment	tmpseg ;
	_XatokSegment	*sp ;
	int		offset ;
	int		nseg ;
	int		i ;

	nseg = abuf->nbr_of_seg ;
	offset = abuf->offset ;
	sp = abuf->segments ;

	tmpseg.string = sp[nseg].string ;
	tmpseg.length = sp[nseg].length ;
	tmpseg.size = sp[nseg].size ;
	tmpseg.offset = sp[nseg].offset ;

	sp = &abuf->segments[nseg] ;
	for( i = nseg ; i > offset ; i-- ) {
		sp->string = ( sp - 1 )->string ;
		sp->length = ( sp - 1 )->length ;
		sp->size = ( sp - 1 )->size ;
		sp->offset = ( sp - 1 )->offset ;
		sp-- ;
	}
	sp = &abuf->segments[offset] ;
	sp->string = tmpseg.string ;
	sp->length = tmpseg.length ;
	sp->size = tmpseg.size ;
	sp->offset = tmpseg.offset ;

	abuf->offset++ ;
	abuf->nbr_of_seg++ ;
	abuf->cur_seg++ ;

	return 1 ;
}


/* Page */
/*
 * [関数名]
 *		XatokShiftLeft( )
 * [表題]
 *              セグメントデータの左ROTATE
 * [呼出形式]
 *		int	XatokShiftLeft( _XatokRecPtr  abuf )
 *
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		_XatokRecPtr : abuf   : i/o : kinput2用変換バッファ
 *
 * [返り値]
 *		1 : 常に１
 *
 * [使用関数]
 *		なし
 * [機能]
 *		セグメントデータを左に ROTATEする。
 *		左のROTATEとは セグメントの配列のインデックスが大きいデータを
 *		１つ小さい所に移すという事である。
 *		１番小さい所は、OFFSETに移る。
 */
int XatokShiftLeft(abuf)
_XatokRecPtr	abuf;
{
	_XatokSegment	tmpseg ;
	_XatokSegment	*sp ;
	int		offset ;
	int		i ;

	offset = abuf->offset ;

	sp = abuf->segments ;
	tmpseg.string = sp[0].string ;
	tmpseg.length = sp[0].length ;
	tmpseg.size = sp[0].size ;
	tmpseg.offset = sp[0].offset ;

	for( i = 0 ; i < offset - 1 ; i++ ) {
		sp->string = ( sp + 1 )->string ;
		sp->length = ( sp + 1 )->length ;
		sp->size = ( sp + 1 )->size ;
		sp->offset = ( sp + 1 )->offset ;
		sp++ ;
	}
	abuf->segments[offset-1].string = tmpseg.string ;
	abuf->segments[offset-1].length = tmpseg.length ;
	abuf->segments[offset-1].size = tmpseg.size ;
	abuf->segments[offset-1].offset = tmpseg.offset ;

	return 1 ;
}
/* Page */
/*
 * [関数名]
 *		XatokShiftLeftAll( )
 * [表題]
 *              全セグメントデータの左ROTATE
 * [呼出形式]
 *		int	XatokShiftLeftAll( _XatokRecPtr  abuf )
 *
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		_XatokRecPtr : abuf   : i/o : kinput2用変換バッファ
 *
 * [返り値]
 *		1 : 常に１
 *
 * [使用関数]
 *		なし
 * [機能]
 *		セグメントデータを左に ROTATEする。
 *		左のROTATEとは セグメントの配列のインデックスが大きいデータを
 *		１つ小さい所に移すという事である。
 */
int XatokShiftLeftAll(abuf)
_XatokRecPtr	abuf;
{
	_XatokSegment	tmpseg ;
	_XatokSegment	*sp ;
	int		offset ;
	int		nseg ;
	int		i ;

	nseg = abuf->nbr_of_seg ;
	sp = abuf->segments ;
	tmpseg.string = sp->string ;
	tmpseg.length = sp->length ;
	tmpseg.size = sp->size ;
	tmpseg.offset = sp->offset ;
	for( i = 0 ; i < nseg - 1 ; i++ ) {
		sp->string = ( sp + 1 )->string ;
		sp->length = ( sp + 1 )->length ;
		sp->size = ( sp + 1 )->size ;
		sp->offset = ( sp + 1 )->offset ;
		sp++ ;
	}
	sp = &abuf->segments[nseg-1] ;
	sp->string = tmpseg.string ;
	sp->length = tmpseg.length ;
	sp->size = tmpseg.size ;
	sp->offset = tmpseg.offset ;

	abuf->offset-- ;
	abuf->nbr_of_seg-- ;
	abuf->cur_seg-- ;
}

/* End of segment.c */
