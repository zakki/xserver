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
 * [$B4X?tL>(B]
 *		XatokCompStr( )
 * [$BI=Bj(B]
 *		$BL$3NDjJ8;zNs>uBV<hF@(B
 * [$B8F=P7A<0(B]
 *		int	XatokCompStr( WCHAR *compStr, WORD *compAttr,
 *				WORD compLen, int *revPos, int *revLen )
 *		
 * [$B0z?t(B]
 *		$B7?(B        : $BL>(B       $B>N(B   : IO : $B@b(B      $BL@(B
 *		WCHAR : *compStr : i : $BL$3NDjJ8;zNs(B
 *		WORD : *compAttr : i : $BL$3NDjJ8;zNsB0@-(B
 *		WORD : compLen : i : $BL$3NDjJ8;zNsD9$5(B
 *		int : *revPos : o : $BH?E>I=<(3+;O0LCV(B
 *		int : *revLen : o : $BH?E>I=<(D9$5(B
 *	
 * [$BJV$jCM(B]
 *		$B%;%0%a%s%H$N8D?t(B(1-3)
 *		
 * [$B;HMQ4X?t(B]
 *		$B$J$7(B
 * [$B5!G=(B]
 *		$BL$3NDjJ8;zNs$NJ8;z>pJs!"B0@->pJs$h$j!"(B
 *		$BL$3NDjJ8;zNs$NI=<(>uBV$r<hF@$9$k!#(B
 *		$BI=<(>uBV$H$O!"(B $B%N!<%^%kI=<(!"H?E>I=<($+$i@.N)$A!"(B
 *		$B:GBg#3$D$N%;%0%a%s%H$KJ,3d$5$l$k!#(B
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
