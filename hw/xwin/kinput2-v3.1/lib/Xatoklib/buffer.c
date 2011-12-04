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
static  char    rcsid[] = "$Id: buffer.c,v 1.3 1999/08/24 09:05:26 ishisone Exp $" ;
#endif  /* !lint */

#include        <stdio.h>
#include        <X11/IntrinsicP.h>
#include        <X11/StringDefs.h>
#include        <X11/Xmu/Atoms.h>
#include        <X11/keysym.h>


#include	<Xatoklib.h>


/* Page */
/*
 * [$B4X?tL>(B]
 *		XatokClearBuffer( )
 * [$BI=Bj(B]
 *              kinput2$BMQJQ49%P%C%U%!$K%/%j%"(B
 * [$B8F=P7A<0(B]
 *	int	XatokClearBuffer( _XatokRecPtr  acomm )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		_XatokRecPtr : acomm   : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *
 * [$BJV$jCM(B]
 *		 $B$J$7(B
 *
 * [$B;HMQ4X?t(B]
 *		$B$J$7(B
 * [$B5!G=(B]
 *		$B3NJ]$5$l$?(B kinput2$BMQJQ49%P%C%U%!$r%/%j%"$9$k!#(B
 */
void XatokClearBuffer(abuf)
_XatokRecPtr	abuf;
{
	int	i, n ;

	for( i = 0 ; i < abuf->nbr_of_seg ; i++ ) {
		/* XatokFreeSegement( abuf->segments[i] ) ; */
	}
	abuf->cur_seg = 0 ;
	abuf->nbr_of_seg = 0 ;

}

/* Page */
/* 
 * [$B4X?tL>(B]
 *		XatokBufferConvert( )
 * [$BI=Bj(B]
 *		$B%(%s%8%sJQ497k2L%G!<%?$N%3%s%P!<%H(B
 * [$B8F=P7A<0(B]
 *		int XatokBufferConvert( _XatokRecPtr acomm,
 *			WORD ceCmd, AtokRsltPacket *result, int *status, 
 *			int *enable, int *change, int *elseEvent )
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		_XatokRecPtr : acomm   : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *		WORD		ceCmd,
 *		AtokRsltPacket	*result,
 *		int		*status,
 *		int		*enable,
 *		int		*change,
 *		int		*elseEvent
 *	
 *	
 * [$BJV$jCM(B]
 *		$B3NDjJ8;zNs$ND9$5(B
 * [$B;HMQ4X?t(B]
 *		ucs2euc( )
 *		es = euc2wcs( )
 *		s2e( )
 *		XatokCompStr( )
 *	
 * [$B5!G=(B]
 *		ATOK12X $B$X$N(BREQUEST$B$h$j(B $BLa$C$FMh$?%G!<%?$r%A%'%C%/$7!"(B
 *		kinput2 $B$N(B ATOK CLASS$B$G;HMQ$G$-$k%G!<%?7A<0$KJQ49$9$k!#(B
 *
 */
int XatokBufferConvert(acomm, ceCmd, result, status, enable, change, elseEvent)
_XatokRecPtr	acomm;
WORD		ceCmd;
AtokRsltPacket	*result;
int		*status;
int		*enable;
int		*change;
int		*elseEvent;
{
	char 		euc[BUFSIZ*4] ;	/* $BFI$_I=<($N$?$a$N%P%C%U%!(B */
	int		es ;			/* euc $BJ8;zNs$ND9$5(B */
	unsigned char	*sp, *sp1, *uni2euc( ) ;
	wchar		*wbuf ;		/* $B3NDjJ8;zNs(B */
	char		*wrbuf ;	/* $B3NDjFI$_J8;zNs(B */

	int		length = 0 ;		/* $B3NDjJ8;zNs$ND9$5(B */
	int		i, n, ksize ;			/* WORK */
	AtokRsltHdr	*hdr ;			/* WORK */
	int		wsize ; 		/* WORK */
	static	int euclen = 1 ;
		
	wsize = sizeof( wchar ) ;
	wbuf = acomm->wbuf ;
	wrbuf = acomm->wrbuf ;

	hdr = &acomm->NETrslt.header ;

	/*
	 * $B%b!<%IJ8;zNs$N@_Dj(B
	 * $B%b!<%IJ8;zNs$N@hF,$K6uGr$,B8:_$9$k$N$G:o=|$9$k!#(B
	 */
#ifdef	ATOK_DEBUG
	printf( "buffer conv \245\342\241\274\245\311LEN %d\n"/*=buffer conv $B%b!<%I(BLEN %d\n=*/, hdr->modeLen ) ;
#endif	/* ATOK_DEBUG */
	if ( hdr->modeLen > 0 ) {
		es = hdr->modeLen * 3 ;
		sp = ( unsigned char * )malloc( es ) ;
		bzero( sp, es ) ;
		ucs2euc( hdr->modePtr, hdr->modeLen, sp, es, 0x0000a2ae ) ;
		sp1 = sp ;
		while( *sp1 == 0x20 ) sp1++ ;	/* SPACE */
		es = strlen( sp1 ) ;
		es = euc2wcs( sp1, es, acomm->mode.name ) ;
#ifdef	ATOK_DEBUG
		printf( "\245\342\241\274\245\311\312\270\273\372\316\363 [%s]\n"/*=$B%b!<%IJ8;zNs(B [%s]\n=*/, sp ) ;
#endif	/* ATOK_DEBUG */
		acomm->mode.length = hdr->modeLen ;
		acomm->mode.length = es ;
		free( sp ) ;
		/* acomm->mode.changes = 1 ; */
	}

	/*
	 * $B3NDjJ8;zNs$,B8:_$9$k>l9g$K$O!"(Bwbuf $B$K3NDjJ8;zNs$r@_Dj$7!"(B
	 * $B%j%?!<%sCM$G$"$k3NDjJ8;zNs$ND9$5$r@_Dj$9$k!#(B
	 * wrbuf $B$K$OFI$_J8;zNs$r@_Dj$9$k!#(B
	 */
	if ( hdr->resultStrLen ) {
		es = hdr->resultStrLen * 3 ;
		sp = ( unsigned char * )malloc( es ) ;
		bzero( sp, es ) ;
		ucs2euc( hdr->resultStrPtr, hdr->resultStrLen, sp, es, 0x0000a2ae ) ;

		es = strlen( sp ) ;
		es = euc2wcs( sp, es, wbuf ) ;
		length = hdr->resultStrLen ;
		free( sp ) ;

		/*
		 * $B$h$_J8;zNs(B
		 */
		bzero( acomm->wrbuf, sizeof( acomm->wrbuf )) ;
		es = hdr->resultReadLen ;
		s2e( hdr->resultReadPtr, es, wrbuf, sizeof( acomm->wrbuf )) ;

#ifdef	ATOK_DEBUG
	printf( "=== buffer.c \263\316\304\352EUC [%s] === \n"/*==== buffer.c $B3NDj(BEUC [%s] === \n=*/, sp ) ;
	printf( "=== buffer.c \263\316\304\352 READ EUC [%s] === \n"/*==== buffer.c $B3NDj(B READ EUC [%s] === \n=*/, acomm->wrbuf ) ;
#endif	/* ATOK_DEBUG */

	}
	/*
	 * $BL$3NDjJ8;zNs$,$"$C$?>l9g$K$O!"(B
	 * $BL$3NDjJ8;zNs$r(BUNICODE$B$+$i(BEUC$B$KJQ49$7!"(B
	 * $BH?E>I=<(Ey$N0LCV$r5a$a$k!#(B
	 * $BL$3NDjJ8;zNs$ND9$5$HL$3NDjJ8;zNsB0@-$ND9$5$O(B
	 * $BF1$8$J$N$G(B if $BJ8$O$^$H$a$F=hM}$9$k!#(B
	 */
	if ( hdr->compStrLen ) {
		es = hdr->compStrLen * wsize ;
		es = hdr->compStrLen * 3 ;
		sp = ( unsigned char * )malloc( es ) ;
		bzero( sp, es ) ;

		ucs2euc( hdr->compStrPtr, hdr->compStrLen, sp, es, 0x0000a2ae ) ;
		es = strlen( sp ) ;

		acomm->echoLen = euc2wcs( sp, es, acomm->echoStr ) ;
		/*
		 * $B$3$3$GH?E>I=<($N%3%a%s%H$b:n@.$9$k!#(B
		 */
		XatokCompStr( hdr->compStrPtr, hdr->compAttrPtr, 
			hdr->compStrLen, &acomm->revPos, &acomm->revLen ) ;
#ifdef	ATOK_DEBUG
		/* printf( "$BL$3NDjJ8;zNs(B[%s]\n", sp ) ; */
		printf( "EUC CODE\n" ) ;
		for( i = 0 ; sp[i] ; i++ ) {
			printf( "0x%x ", sp[i] ) ;
		}
		printf( "\n" ) ;

		printf( "== DEBUG \314\244\263\316\304\352\311\275\274\250:\n"/*=== DEBUG $BL$3NDjI=<((B:\n=*/ ) ;
		printf( "==> [%s]\n", sp ) ;
		printf( "==> [----+----1----+----2----+----3]\n" ) ;
		printf( "==> [" ) ;
		for( i = 0 ; i < acomm->revPos ; i++ ) {
			printf( "\241\373"/*=$B!{(B=*/ ) ;
		}
		for( i = 0 ; i < acomm->revLen ; i++ ) {
			printf( "\241\374"/*=$B!|(B=*/ ) ;
		}
		for( i = acomm->revPos + acomm->revLen ; i < hdr->compStrLen ; i++ ) {
			printf( "\241\373"/*=$B!{(B=*/ ) ;
		}
		printf( "-]\n" ) ;
		printf( "revPos %d, revLen %d\n", acomm->revPos, acomm->revLen ) ;
#endif	/* ATOK_DEBUG */

		free( sp ) ;
	}
	else {
		acomm->echoLen = 0 ;
		acomm->echoStr[0] = 0 ;
	}

	/*
	 * $B8uJdJ8;zNs(B
	 */
#ifdef	ATOK_DEBUG
	printf( "\270\365\312\344\277\364\241\242 %d, \270\365\312\344\245\265\245\244\245\272 %d\n"/*=$B8uJd?t!"(B %d, $B8uJd%5%$%:(B %d\n=*/, hdr->kohoNum, hdr->kohoStrLen ) ;
#endif	/* ATOK_DEBUG */

	acomm->kohoNum = 0 ;
	if ( hdr->kohoNum > 0 && hdr->kohoStrLen > 0 ) {
		n = hdr->kohoNum ;
		ksize = hdr->kohoStrLen ;
		if ( acomm->kohoSize == 0 ) {
			acomm->kohoStrPtr = ( WCHAR * )malloc( ksize * sizeof( WCHAR )) ;
			acomm->kohoLenPtr = ( BYTE * )malloc( n * sizeof( BYTE )) ;
			acomm->kohoSfxPtr = ( BYTE * )malloc( n * sizeof( BYTE )) ;
			acomm->kohoSize = n ;
			acomm->kohoStrLen = ksize ;
		}
		/*
		 * $BJ8;zNsD9!"B0@-$O!"8uJd?t$@$13NJ](B
		 * $B8uJd?t$O(B kohoSize $B$K(B $B@_Dj$7$F$"$k!#(B
		 */
		if ( acomm->kohoSize < n ) {

			acomm->kohoLenPtr = ( BYTE * )realloc( acomm->kohoLenPtr, n * sizeof( BYTE )) ;
			acomm->kohoSfxPtr = ( BYTE * )realloc( acomm->kohoSfxPtr, n * sizeof( BYTE )) ;
			acomm->kohoSize = n ;
		}
		if ( acomm->kohoStrLen < ksize ) {
			acomm->kohoStrPtr = ( WCHAR * )realloc( acomm->kohoStrPtr, ksize * sizeof( WCHAR )) ;
			acomm->kohoStrLen = ksize ;
		}

                memcpy( acomm->kohoStrPtr, hdr->kohoStrPtr, ksize * sizeof( WCHAR)) ;
		memcpy( acomm->kohoLenPtr, hdr->kohoLenPtr, n * sizeof( BYTE )) ;
		memcpy( acomm->kohoSfxPtr, hdr->kohoSfxPtr, n * sizeof( BYTE )) ;
		acomm->kohoNum = n ;
		acomm->kohoPos = hdr->kohoPos ;

#ifdef	ATOK_DEBUG
{
	int kk ;
	char	*eep ;
	kk = ksize * sizeof( WCHAR );
	eep = uni2euc( acomm->kohoStrPtr, &kk ) ;
	printf( "\270\365\312\344 [%s]\n"/*=$B8uJd(B [%s]\n=*/, eep ) ;
	for ( i = 0 ; i < n ; i++ ) {
		printf( "%d ", acomm->kohoLenPtr[i] ) ;
	}
	printf( "\304\271\244\265\n"/*=$BD9$5(B\n=*/ ) ;
}
#endif	/* ATOK_DEBUG */
	}

	/*
	 * $B%7%9%F%`9T%G!<%?(B
	 */
	if ( hdr->sysStrLen ) {
		es = hdr->sysStrLen * 3 ;
		sp = ( unsigned char * )malloc( es ) ;
		bzero( sp, es ) ;
		ucs2euc( hdr->sysStrPtr, hdr->sysStrLen, sp, es, 0x0000a2ae ) ;
		es = strlen( sp ) ;
		es = euc2wcs( sp, es, acomm->sysLineStr ) ;
		acomm->sysLineLen = hdr->sysStrLen ;
#ifdef	ATOK_DEBUG
		printf( "SYS LINE LEN in buffer.c (%d)\n", hdr->sysStrLen ) ;
#endif	/* ATOK_DEBUG */
		if ( acomm->sysLineSize == 0 ) {
			acomm->sysAttrPtr = ( WORD * )malloc( sizeof( WORD ) *
				hdr->sysStrLen ) ;
			acomm->sysLineSize = hdr->sysStrLen ;
		}
		else if ( acomm->sysLineSize < hdr->sysStrLen ) {
			acomm->sysAttrPtr = ( WORD * )malloc( sizeof( WORD ) *
				hdr->sysStrLen ) ;
			acomm->sysLineSize = hdr->sysStrLen ;
		}
		memcpy( acomm->sysAttrPtr, hdr->sysAttrPtr, sizeof( WORD ) * hdr->sysStrLen ) ;

		free( sp ) ;
	}
	if ( hdr->guideStrLen ) {
		es = hdr->guideStrLen * 3 ;
		sp = ( unsigned char * )malloc( es ) ;
		bzero( sp, es ) ;
		ucs2euc( hdr->guideStrPtr, hdr->guideStrLen, sp, es, 0x0000a2ae ) ;
		free( sp ) ;
	}
	acomm->kohoNum = hdr->kohoNum ;

	acomm->guide_openEventIndex = hdr->CEGuideOpenEventIndx ;
	acomm->guide_closeEventIndex = hdr->CEGuideCloseEventIndx ;
	acomm->guide_OpenIndex = hdr->CEGuideOpenIndx ;

	*status = hdr->CEOutinf ;
	*enable = hdr->StatusEnableFlag ;
	*change = hdr->DispChangeStatus ;
	*elseEvent = hdr->StatusElseEvent ;

	return length ;
}
/* End of buffer.c */
