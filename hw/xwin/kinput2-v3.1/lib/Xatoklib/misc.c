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
static  char    rcsid[] = "$Id: misc.c,v 1.2 1999/08/24 09:01:09 ishisone Exp $" ;
#endif  /* !lint */

#include	<Xatoklib.h>

/* Page */
/*
 * [関数名]
 *		wcs2euc( )
 * [表題]
 *              WCHAR 型からEUC文字列への変換
 * [呼出形式]
 *	int	wcs2euc( wchar *wbuf, int wlen, unsigned char *euc )
 *
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		wchar        : *wbuf   : i   : WCHAR 型文字列
 *		int          : wlen    : i   : wchar 型文字列の長さ
 *		unsigned char : *euc   :   o : EUC 文字列格納領域
 *
 * [返り値]
 *		EUC 文字列の長さ
 *
 * [使用関数]
 *		なし
 * [機能]
 *		wchar 型文字列から unsigined char  のEUC文字列に変換する。
 *
 */

int wcs2euc(wbuf, wlen, euc)
wchar	*wbuf;
int	wlen;
unsigned char *euc;
{
	int	i ;
	int	n = 0 ;
	unsigned char	c ;
	for( i = 0 ; i < wlen ; i++ ) {
		c =  ( *wbuf & 0xff00 ) >> 8 ;
		if ( c ) {
			*euc++ = c ;
			n++ ;
		}
		else if (( *wbuf & 0xff ) & 0x80 ) {
			*euc++ = 0x8e ;
			n++ ;
		}
		*euc++ =  *wbuf & 0xff ;
		wbuf++ ;
		n++ ;
	}
	*euc = 0 ;

	return n ;
}
/* Page */
/*
 * [関数名]
 *		euc2wcs( )
 * [表題]
 *              EUC文字列から wchar 型文字列への変換
 * [呼出形式]
 *		int	euc2wcs( unsigned char *euc, int elen, wchar *wbuf )
 *
 * [引数]
 *              型            : 名  称  : I O : 説      明
 *		unsigned char : *euc   : i   : EUC 文字列
 *		int           : elen   : i   : EUC 文字列の長さ
 *		wchar         : *wbuf  :   o : wchar 型文字列格納領域
 *
 * [返り値]
 *		1 : 常に１
 *
 * [使用関数]
 *		なし
 * [機能]
 *		unsigined char 型のEUC 文字列をwchar 型に変換する。
 *		EUC 文字列には、0x8f の特別なコードが含まれているので
 *		wchar に変換する時に個別処理をする。
 */
int euc2wcs(euc, elen, wbuf)
unsigned char	*euc;
int		elen;
wchar		*wbuf;
{
	int	lb = 0, hb = 0 ;
	int	i ;
	int	n = 0 ;
	int	isSkip ;

	for( i = 0 ; i < elen ; i++ ) {
		isSkip = 0 ;
		if (  *euc == 0x8e ) {
			euc++ ;
			hb = *euc ;
			lb = 0 ;
			i++ ;
		}
		else if (  *euc & 0x80 ) {
			if ( *euc == 0x8f ) {
				isSkip=1 ;
			}
			else {
				lb = *euc ;
				euc++ ;
				hb = *euc ;
				i++ ;
			}
		}
		else {
			hb = *euc ;
			lb = 0 ;
		}
		euc++ ;
		if ( !isSkip ) {
			*wbuf = (( lb << 8 ) | hb ) & 0xffff ;
			wbuf++ ;
			n++ ;
		}
	}

	*wbuf = 0 ;
	return n ;
}
/* End of misc.c */
