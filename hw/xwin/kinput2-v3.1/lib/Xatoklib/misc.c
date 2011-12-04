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
 * [$B4X?tL>(B]
 *		wcs2euc( )
 * [$BI=Bj(B]
 *              WCHAR $B7?$+$i(BEUC$BJ8;zNs$X$NJQ49(B
 * [$B8F=P7A<0(B]
 *	int	wcs2euc( wchar *wbuf, int wlen, unsigned char *euc )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		wchar        : *wbuf   : i   : WCHAR $B7?J8;zNs(B
 *		int          : wlen    : i   : wchar $B7?J8;zNs$ND9$5(B
 *		unsigned char : *euc   :   o : EUC $BJ8;zNs3JG<NN0h(B
 *
 * [$BJV$jCM(B]
 *		EUC $BJ8;zNs$ND9$5(B
 *
 * [$B;HMQ4X?t(B]
 *		$B$J$7(B
 * [$B5!G=(B]
 *		wchar $B7?J8;zNs$+$i(B unsigined char  $B$N(BEUC$BJ8;zNs$KJQ49$9$k!#(B
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
 * [$B4X?tL>(B]
 *		euc2wcs( )
 * [$BI=Bj(B]
 *              EUC$BJ8;zNs$+$i(B wchar $B7?J8;zNs$X$NJQ49(B
 * [$B8F=P7A<0(B]
 *		int	euc2wcs( unsigned char *euc, int elen, wchar *wbuf )
 *
 * [$B0z?t(B]
 *              $B7?(B            : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		unsigned char : *euc   : i   : EUC $BJ8;zNs(B
 *		int           : elen   : i   : EUC $BJ8;zNs$ND9$5(B
 *		wchar         : *wbuf  :   o : wchar $B7?J8;zNs3JG<NN0h(B
 *
 * [$BJV$jCM(B]
 *		1 : $B>o$K#1(B
 *
 * [$B;HMQ4X?t(B]
 *		$B$J$7(B
 * [$B5!G=(B]
 *		unsigined char $B7?$N(BEUC $BJ8;zNs$r(Bwchar $B7?$KJQ49$9$k!#(B
 *		EUC $BJ8;zNs$K$O!"(B0x8f $B$NFCJL$J%3!<%I$,4^$^$l$F$$$k$N$G(B
 *		wchar $B$KJQ49$9$k;~$K8DJL=hM}$r$9$k!#(B
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
