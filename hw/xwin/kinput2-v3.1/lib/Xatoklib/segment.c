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
 * [$B4X?tL>(B]
 *		XatokGetModeStr( )
 * [$BI=Bj(B]
 *              $B%b!<%IJ8;zNs$N<hF@(B
 * [$B8F=P7A<0(B]
 *		WCHAR	*XatokGetModeStr( _XatokRecPtr abuf, int *length )
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		_XatokRecPtr : abuf   : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *		int          : *length :  o : $BJ8;zNs$ND9$5(B
 *
 * [$BJV$jCM(B]
 *		$B%b!<%IJ8;zNs3JG<NN0h$N%"%I%l%9(B
 * [$B;HMQ4X?t(B]
 *		$B$J$7(B
 * [$B5!G=(B]
 *		$B%b!<%II=<(MQ$NJ8;zNs$*$h$S$=$ND9$5$r<hF@$9$k!#(B
 */
WCHAR *XatokGetModeStr(abuf, length)
_XatokRecPtr	abuf;
int		*length;
{
	int i, n ;
	
	*length = ( int )abuf->mode.length ;
	n = *length ;

#ifdef	ATOK_DEBUG
	printf( "GET \245\342\241\274\245\311 : "/*=GET $B%b!<%I(B : =*/ ) ;
	for ( i = 0 ; i < n ; i++ ) {
		printf( "0x%x ", abuf->mode.name[i] ) ;
	}
	printf( "\n" ) ;
#endif	/* ATOK_DEBUG */

	return abuf->mode.name ;
}

/* Page */
/*
 * [$B4X?tL>(B]
 *		XatokGetSegmentPosition( )
 * [$BI=Bj(B]
 *              $B%+%l%s%H%;%0%a%s%H$N<hF@(B
 * [$B8F=P7A<0(B]
 *	int	wordStartFunc( _XatokRecPtr  acomm )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		_XatokRecPtr : acomm   : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *
 * [$BJV$jCM(B]
 *		0 : 
 *		1 :
 *
 * [$B;HMQ4X?t(B]
 *		$B$J$7(B
 * [$B5!G=(B]
 *		$B8=:_$N%+%l%s%H%;%0%a%s%H$r<hF@$9$k!#(B
 * 		$B8=:_4IM}$7$F$$$k(B $B%+%l%s%H%]%7%7%g%s$,(B
 * 		$B%;%0%a%s%H?t$h$j>.$5$1$l$P$=$N%;%0%a%s%H$,!"(B
 * 		$B8F$P$l$?;~$N%;%0%a%s%H$H$J$k!#(B
 * 		$B%;%0%a%s%H$,8:>/$7$F(B($BJ8@a$r1d$P$7$?>l9g$J$I(B)
 * 		$B8=:_$N%]%8%7%g%s$,%;%0%a%s%H?t$h$jBg$-$1$l$P!"(B
 * 		$B:G8e$N%;%0%a%s%H$,(B $B%+%l%s%H%]%8%7%g%s$H$J$k!#(B
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
 * [$B4X?tL>(B]
 *		XatokGetSegmentNumber( )
 * [$BI=Bj(B]
 *              $B%;%0%a%s%H?t$N<hF@(B
 * [$B8F=P7A<0(B]
 *		int	XatokGetSegmentNumber( _XatokRecPtr abuf )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		_XatokRecPtr : acomm   : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *
 * [$BJV$jCM(B]
 *		N : $B%;%0%a%s%H?t(B
 *
 * [$B;HMQ4X?t(B]
 *		$B$J$7(B
 * [$B5!G=(B]
 *		$B8=:_$N%;%0%a%s%H?t$r<hF@$9$k!#(B
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
 * [$B4X?tL>(B]
 *		XatokGetSegmentRec( )
 * [$BI=Bj(B]
 *              $B;XDj%;%0%a%s%H$N%G!<%?<hF@(B
 * [$B8F=P7A<0(B]
 *		wchar *XatokGetSegmentRec( _XatokRecPtr *abuf,
 *			int n, int *len, int *attr )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		_XatokRecPtr : acomm   : i   : kinput2$BMQJQ49%P%C%U%!(B
 *		int          : n       : i   : $B;XDj%;%0%a%s%HHV9f(B
 *		int          : *len    :  o  : $B;XDj%;%0%a%s%H$NJ8;zNs$ND9$5(B
 *              int          : *attr   :  o  : $B;XDj%;%0%a%s%H$NB0@-(B
 *
 * [$BJV$jCM(B]
 *		$B;XDj%;%0%a%s%H$NJ8;zNs(B
 *
 * [$B;HMQ4X?t(B]
 *		$B$J$7(B
 * [$B5!G=(B]
 *		$B;XDj$5$l$?%;%0%a%s%H$NJ8;zNs$*$h$S$=$NJ8;zNs$ND9$5!"(B
 *		$BI=<(B0@-$r<hF@$9$k!#(B
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
 * [$B4X?tL>(B]
 *		XatokGetConvertedLength( )
 * [$BI=Bj(B]
 *              $BJQ497k2LJ8;zNs$ND9$5<hF@(B
 * [$B8F=P7A<0(B]
 *		int XatokGetConvertedLength( _XatokRecPtr abuf )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		_XatokRecPtr : abuf   : i   : kinput2$BMQJQ49%P%C%U%!(B
 *
 * [$BJV$jCM(B]
 *		N : $BJ8;zNs$ND9$5(B
 *
 * [$B;HMQ4X?t(B]
 *		$B$J$7(B
 * [$B5!G=(B]
 *		$BJQ497k2LJ8;zNs$ND9$5$r<hF@$9$k!#(B
 *		$BJQ497k2LJ8;zNs$ND9$5$H$O!"%;%0%a%s%H$KJ]B8$5$l$F$$$k(B
 *		$BJ8;zNs$ND9$5$N9g7W$G$"$k!#(B
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
 * [$B4X?tL>(B]
 *		XatokGetConvertedString( )
 * [$BI=Bj(B]
 *              $BJQ497k2LJ8;zNs$N<hF@(B
 * [$B8F=P7A<0(B]
 *		int XatokGetConvertedString( _XatokRecPtr abuf, wchar *wstr )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		_XatokRecPtr : abuf   : i   : kinput2$BMQJQ49%P%C%U%!(B
 *		wchar        : *wstr  :  o  : $B<hF@$7$?J8;zNs3JG<NN0h(B
 *
 * [$BJV$jCM(B]
 *		N : $BJ8;zNs$ND9$5(B
 *
 * [$B;HMQ4X?t(B]
 *		$B$J$7(B
 * [$B5!G=(B]
 *		$BJQ497k2LJ8;zNs$ND9$5$r<hF@$9$k!#(B
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
 * [$B4X?tL>(B]
 *		XatokShiftRight( )
 * [$BI=Bj(B]
 *              $B%;%0%a%s%H%G!<%?$N1&(BROTATE
 * [$B8F=P7A<0(B]
 *		int	XatokShiftRight( _XatokRecPtr  abuf )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		_XatokRecPtr : abuf   : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *
 * [$BJV$jCM(B]
 *		1 : $B>o$K#1(B
 *
 * [$B;HMQ4X?t(B]
 *		$B$J$7(B
 * [$B5!G=(B]
 *		$B%;%0%a%s%H%G!<%?$r1&$K(B ROTATE$B$9$k!#(B
 *		$B1&$N(BROTATE$B$H$O(B $B%;%0%a%s%H$NG[Ns$N%$%s%G%C%/%9$,>.$5$$%G!<%?$r(B
 *		$B#1$DBg$-$$=j$K0\$9$H$$$&;v$G$"$k!#(B
 *		$B#1HVBg$-$$=j$O!"@hF,$K0\$k!#(B
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
 * [$B4X?tL>(B]
 *		XatokShiftLeft( )
 * [$BI=Bj(B]
 *              $B%;%0%a%s%H%G!<%?$N:8(BROTATE
 * [$B8F=P7A<0(B]
 *		int	XatokShiftLeft( _XatokRecPtr  abuf )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		_XatokRecPtr : abuf   : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *
 * [$BJV$jCM(B]
 *		1 : $B>o$K#1(B
 *
 * [$B;HMQ4X?t(B]
 *		$B$J$7(B
 * [$B5!G=(B]
 *		$B%;%0%a%s%H%G!<%?$r:8$K(B ROTATE$B$9$k!#(B
 *		$B:8$N(BROTATE$B$H$O(B $B%;%0%a%s%H$NG[Ns$N%$%s%G%C%/%9$,Bg$-$$%G!<%?$r(B
 *		$B#1$D>.$5$$=j$K0\$9$H$$$&;v$G$"$k!#(B
 *		$B#1HV>.$5$$=j$O!"(BOFFSET$B$K0\$k!#(B
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
 * [$B4X?tL>(B]
 *		XatokShiftLeftAll( )
 * [$BI=Bj(B]
 *              $BA4%;%0%a%s%H%G!<%?$N:8(BROTATE
 * [$B8F=P7A<0(B]
 *		int	XatokShiftLeftAll( _XatokRecPtr  abuf )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		_XatokRecPtr : abuf   : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *
 * [$BJV$jCM(B]
 *		1 : $B>o$K#1(B
 *
 * [$B;HMQ4X?t(B]
 *		$B$J$7(B
 * [$B5!G=(B]
 *		$B%;%0%a%s%H%G!<%?$r:8$K(B ROTATE$B$9$k!#(B
 *		$B:8$N(BROTATE$B$H$O(B $B%;%0%a%s%H$NG[Ns$N%$%s%G%C%/%9$,Bg$-$$%G!<%?$r(B
 *		$B#1$D>.$5$$=j$K0\$9$H$$$&;v$G$"$k!#(B
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
