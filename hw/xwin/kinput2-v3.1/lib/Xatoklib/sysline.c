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
 * [$B4X?tL>(B]
 *              auxSyslineCtrl( )
 * [$BI=Bj(B]
 *              $B%G!<%?$NAw?.(B
 * [$B8F=P7A<0(B]
 *      	int auxSyslineCtrl( _XatokRecPtr acomm, int isdecision )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		_XatokRecPtr : acomm   : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *		int          : isdecision : i : $B3NDj%G!<%?%U%i%0(B
 *
 *
 * [$BJV$jCM(B]
 *		 0 : AUX $B%&%#%s%I%&7QB3(B
 *		 1 : AUX $B%&%#%s%I%&=*N;(B
 *
 * [$B;HMQ4X?t(B]
 *		$B$J$7(B
 * [$B5!G=(B]
 *		SYSLINE$B%G!<%?$N%U%i%0$,(B ATOK12CE $B%(%s%8%s$G@_Dj$5$l$F$$$k>l9g$K(B
 *		$B%7%9%F%`9T$r?75,$K3+$/!"%7%9%F%`9T$NJ8;zNs$rJQ99$9$k!"(B
 *		$B%7%9%F%`9T$rJD$8$k$J$I$N!"I=<($r$I$N$h$&$K9T$J$&$+$rH=CG$9$k(B
 *		$B%U%!%s%/%7%g%s$G$"$j!"I=<($r4IM}$7$F$$$k%b%8%e!<%k$h$j(B
 *		$B8F$S=P$5$l$k!#(B
 */
int auxSyslineCtrl(acomm, isdecision)
_XatokRecPtr    acomm;
int		isdecision;		/* $B3NDjJ8;z$,$"$k$+$J$$$+!)(B */
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
 * [$B4X?tL>(B]
 *		auxSyslineString( )
 *              AtokNetSend( )
 * [$BI=Bj(B]
 *              $B%7%9%F%`9TJ8;zNs$N:n@.!#(B
 * [$B8F=P7A<0(B]
 *      int auxSyslineString( _XatokRecPtr acomm, menuAux *mAux,
 *		wchar *echoStr, int *echoLen, int *revLen, int	*revPos )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		_XatokRecPtr : acomm   : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *		menuAux	     : *mAux:     : $B8=:_L$;HMQ(B
 *		wchar	     : *echoStr:   o : $BI=<(J8;zNs(B
 *		int          : *echoLen:   o : $BI=<(J8;zNs$ND9$5(B
 *		int	     : *revLen :   o : $BH?E>3+;O0LCV(B
 *		int	     : *revPos :   o : $BH?E>J8;zNsD9$5(B
 *
 *
 * [$BJV$jCM(B]
 *		-1 : $BAw?.%(%i!<(B
 *		 N : $BAw?.%P%$%H?t(B
 *
 * [$B;HMQ4X?t(B]
 *		$B$J$7(B
 * [$B5!G=(B]
 *		$B%7%9%F%`9T$KI=<($9$kJ8;zNs$r:n@.$7!"H?E>I=<(0LCV!"H?E>I=<($N(B
 *		$BD9$5$r5a$a$k!#(B
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
