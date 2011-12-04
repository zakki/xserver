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
static  char    rcsid[] = "$Id: if.c,v 1.3 1999/08/24 09:05:26 ishisone Exp $" ;
#endif  /* !lint */

#include	<stdio.h>
#include	<X11/IntrinsicP.h>
#include	<X11/StringDefs.h>
#include	<X11/Xmu/Atoms.h>
#include	<X11/keysym.h>

#include	<Xatoklib.h>
#include	<key_def.h>

/* Page */
/*
 * [$B4X?tL>(B]
 *              atokModeSet( )
 * [$BI=Bj(B]
 *              $B%b!<%I$N@_Dj(B
 * [$B8F=P7A<0(B]
 *      	int atokModeSet( _XatokRecPtr acomm, WORD ceCmd, WORD value )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B   $B>N(B : IO : $B@b(B      $BL@(B
 *		_XatokRecPtr : acomm   : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *		WORD         : ceCmd   : i   : CE $B%3%^%s%IHV9f(B
 *		WORD         : value   : i   : $BCM(B
 *
 *
 * [$BJV$jCM(B]
 *		CE$B$N=hM}7k2L(B
 * [$B;HMQ4X?t(B]
 *		AtokClntCommunicate( )
 * [$B5!G=(B]
 *		CE$B$N%b!<%I$r9T$J$&(BAPI$B$r8F$S=P$9!#(B
 *		value $B$,(BATCEPRVAPI_READCMD $B$N;~$O!"(B
 *		$B8=:_$N@_DjCM$NFI$_9~$_$G$"$k!#(B
 */
WORD atokModeSet(acomm, ceCmd, value)
_XatokRecPtr	acomm;
WORD		ceCmd;
WORD		value;
{
	if ( value  == ATCEPRVAPI_READCMD ) {
		acomm->NETrqst.AtCEPrvAPIOutinf = ATCEPRVAPI_READCMD ;
	}
	else {
		acomm->NETrqst.AtCEPrvAPIOutinf = value ;
	}
	acomm->NETrqst.ceCmd = ceCmd ; ;
	AtokClntCommunicate( acomm->sock, &acomm->NETrqst, &acomm->NETrslt ) ;
		
	return acomm->NETrslt.header.CEOutinf ;
}

/* Page */
/*
 * [$B4X?tL>(B]
 *              atokConnect( )
 * [$BI=Bj(B]
 *              ATOK12X $B%5!<%P$H$N@\B3(B
 * [$B8F=P7A<0(B]
 *      int atokConnect( char *server, char *port, char *conf, char  *style,
 *				_XatokRecPtr acomm )
 *
 * [$B0z?t(B]
 *              $B7?(B        : $BL>(B       $B>N(B   : IO : $B@b(B      $BL@(B
 *		char         : *server : i   : ATOK12x $B2TF/%[%9%HL>(B
 *		char         : *port   : i   : $B@\B3%5!<%S%9L>(B
 *		char         : *conf   : i   : conf $B%U%!%$%kL>(B
 *		char         : *style  : i   : style $B%U%!%$%kL>(B
 *		_XatokRecPtr : acomm   : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *
 *
 * [$BJV$jCM(B]
 *		0 $BL$K~(B : $B@\B3%(%i!<(B
 *		 $B#00J>e(B :$B@\B3$7$?(BSOCKET$B5-=R;R(B
 *
 * [$B;HMQ4X?t(B]
 *		AtokClntConnect( )
 *		XatokBufferConvert( )
 * [$B5!G=(B]
 *		$B?75,$K(B ATO$B#1#2(Bx$B$H$N@\B3$r9T$J$&!#(B
 *		$B@\B3$O%W%m%;%9$G#1$D$G$h$$!#(B
 *		$B@\B3$,=PMh$?>l9g$K$O!"3F%b!<%I$N8=:_$N@_DjCM$r<hF@$9$k!#(B
 */
int atokConnect(server, port, conf, style, acomm)
char	*server;
char	*port;
char	*conf;
char	*style;
_XatokRecPtr	acomm;
{
	wchar	dummy[128] ;
	int	status ;
	int	sock ;

	sock = AtokClntConnect( server, port, conf, style,
		&acomm->NETrqst, &acomm->NETrslt ) ;
	if ( sock < 0 ) {
		return sock ;
	}

	/*
	 * $B@\B3$K@.8y$7$?$i8=:_$N%b!<%I$r<hF@$9$k!#(B
	 *$B%b!<%IJ8;zNs$K4X$7$F$O!"@\B3;~$K%j%?!<%s$5$l$k!#(B
	 */
	XatokBufferConvert( acomm, ( WORD ) -1,  &acomm->NETrslt, 
		dummy, &status, &status, &status, &status ) ;

	acomm->sock = sock ;

	/*
	 * $B8=:_$N@_DjCM$r<hF@$9$k!#(B
	 */
	acomm->mode.modeKANDSP = atokModeSet( acomm, 
			IME_PRV_SETKANDSP, ATCEPRVAPI_READCMD ) ;
	acomm->mode.modeHEN = atokModeSet( acomm, 
			IME_PRV_SETHENMODE, ATCEPRVAPI_READCMD ) ;
	acomm->mode.modeGAKU = atokModeSet( acomm, 
			IME_PRV_SETGAKUMODE, ATCEPRVAPI_READCMD ) ;
	acomm->mode.modeKOTE = atokModeSet( acomm, 
			IME_PRV_SETKOTEMODE, ATCEPRVAPI_READCMD ) ;
	acomm->mode.modeOKURI = atokModeSet( acomm, 
			IME_PRV_SETOKURIMODE, ATCEPRVAPI_READCMD ) ;
	acomm->mode.modeROT = atokModeSet( acomm, 
			IME_PRV_SETROTMODEF, ATCEPRVAPI_READCMD ) ;

	return sock ;
}
/* Page */
/*
 * [$B4X?tL>(B]
 *		atokCEOpen( )
 * [$BI=Bj(B]
 *              CONTEXT$B$N:n@.(B
 * [$B8F=P7A<0(B]
 *      int atokCEOpen( int sock, int clntid, _XatokRecPtr acomm )
 *
 * [$B0z?t(B]
 *              $B7?(B        : $BL>(B       $B>N(B   : IO : $B@b(B      $BL@(B
 * 		int          : sock    : i   : ATOK12X $B$H$N@\B3(BSOCKET
 *		int          : clntid  : i   : $B%/%i%$%"%s%H(BID
 *		_XatokRecPtr : acomm   : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *
 * [$BJV$jCM(B]
 *		-1 : CONTEXT$B:n@.%(%i!<(B
 *		 1 : CONTEXT $B:n@.@.8y(B
 *
 * [$B;HMQ4X?t(B]
 *		AtokClntCEOpen( )
 *		XatokBufferConvert( )
 * [$B5!G=(B]
 *		$B4{$K@\B3$5$l$F$$$F!"%/%i%$%"%s%H(BID$B$,3d$jEv$F$i$l$F$$$k(B
 *		$B%;%C%7%g%s$KBP$7!"(BATOK12CE $B$H$N(BCONTEXT$B$rDI2C:n@.$9$k!#(B
 */

int atokCEOpen(sock, clntid, acomm)
int	sock;
int	clntid;
_XatokRecPtr	acomm;
{
	wchar	dummy[128] ;	/* BufferConvert $B$N$?$a$N(B TMP */
	int	status ;	/* BufferConvert $B$N$?$a$N(B TMP */

	if ( AtokClntCEOpen( sock, clntid, &acomm->NETrqst, &acomm->NETrslt ) < 0 ) {
		return -1 ;
	}

	XatokBufferConvert( acomm, ( WORD ) -1,  &acomm->NETrslt, 
		dummy, &status, &status, &status, &status ) ;

	acomm->mode.modeKANDSP = atokModeSet( acomm, 
			IME_PRV_SETKANDSP, ATCEPRVAPI_READCMD ) ;
	acomm->mode.modeHEN = atokModeSet( acomm, 
			IME_PRV_SETHENMODE, ATCEPRVAPI_READCMD ) ;
	acomm->mode.modeGAKU = atokModeSet( acomm, 
			IME_PRV_SETGAKUMODE, ATCEPRVAPI_READCMD ) ;
	acomm->mode.modeKOTE = atokModeSet( acomm, 
			IME_PRV_SETKOTEMODE, ATCEPRVAPI_READCMD ) ;
	acomm->mode.modeOKURI = atokModeSet( acomm, 
			IME_PRV_SETOKURIMODE, ATCEPRVAPI_READCMD ) ;
	acomm->mode.modeROT = atokModeSet( acomm, 
			IME_PRV_SETROTMODEF, ATCEPRVAPI_READCMD ) ;

	return 1 ;
}


/* Page */
/* 
 * [$B4X?tL>(B]
 *		atokKanjiOn
 * [$BI=Bj(B]
 *		$B4A;z(BON
 * [$B8F=P7A<0(B]
 *		int	atokKanjiOn( _XatokRecPtr acomm )
 * [$B0z?t(B]
 *		$B7?(B        : $BL>(B       $B>N(B   : IO : $B@b(B      $BL@(B
 *		_XatokRecPtr : acomm   : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *	
 * [$BJV$jCM(B]
 *		0 $BL$K~(B : $B%(%i!<(B
 *		0      : OK
 *		
 * [$B;HMQ4X?t(B]
 *		atokClntCommunicate( )
 * [$B5!G=(B]
 *		CE $B$N%b!<%I$r4A;z(BON$B$H$9$k!#(B
 *
 */
int atokKanjiOn(acomm)
_XatokRecPtr	acomm;
{
	int	status ;

	acomm->convStatus = M_KEYCONV ;
	acomm->convStatus = acomm->oldStatus ;
	acomm->NETrqst.ceCmd = IME_PRV_KANJION ;
	acomm->curAux = acomm->menuAux ;

	status = AtokClntCommunicate( acomm->sock, 
		&acomm->NETrqst, &acomm->NETrslt ) ;
	
	return status ;
}

int atokKanjiOff(acomm)
_XatokRecPtr	acomm;
{
	int	status ;

	acomm->oldStatus = acomm->convStatus ;
	acomm->convStatus = M_NOKEYCONV ;
	acomm->NETrqst.ceCmd = IME_PRV_KANJIOFF ;
	acomm->curAux = acomm->menuAux ;

	status = AtokClntCommunicate( acomm->sock, 
		&acomm->NETrqst, &acomm->NETrslt ) ;
	
	return status ;
}

/* Page */
/*
 * [$B4X?tL>(B]
 *		atokWordStyleList( )
 * [$BI=Bj(B]
 *              $BIJ;l%j%9%H$N<hF@(B
 * [$B8F=P7A<0(B]
 *		int atokWordStyleList( _XatokRecPtr acomm )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		_XatokRecPtr : acomm   : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *
 * [$BJV$jCM(B]
 *		-1 : $BAw?.%(%i!<(B
 *		 N : $BIJ;l$N8D?t(B
 *
 * [$B;HMQ4X?t(B]
 *		atokClntCommunicate( )
 * [$B5!G=(B]
 *		$B<-=q$KEPO?$9$k$?$a$NIJ;l%j%9%H$r<hF@$9$k!#(B
 *		$BIJ;l%j%9%H$O(B S-JIS $B$GAw$i$l$FMh$k!#(Bkinput2 $BFbIt$G$O(B
 *		EUC $B$G4IM}$7$F$$$k$?$a!"$3$3$G%3!<%IJQ49$r9T$J$&!#(B
 *		$BIJ;l%j%9%H$O!"8uJd%j%9%H$r4IM}$9$k$N$HF1$8%P%C%U%!$G(B
 *		$B4IM}$9$k!#$=$N%P%C%U%!$,<hF@$5$l$F$$$J$$>l9g$K$O!"(B
 *		$B?7$?$K<hF@$9$k!#(B
 */
int atokWordStyleList(acomm)
_XatokRecPtr	acomm;
{
	int		size ;
	int		i, j, n ;
	int		len ;
	ATOKDEHINSTR	*sptr ;
	char		euc[BUFSIZ] ;

	if ( acomm->hinsiPtr ) {
		goto make_cand ;
	}

	acomm->NETrqst.ceCmd = IME_PRV_WORDSYLECOUNT ;
	if ( AtokClntCommunicate( acomm->sock, &acomm->NETrqst, &acomm->NETrslt ) < 0 ) {
		return -1 ;
	}

#ifdef	ATOK_DEBUG
	printf( "CHECK %d\n", acomm->NETrslt.header.CEOutinf ) ;
#endif	/* ATOK_DEBUG */
	acomm->hinsiNum = acomm->NETrslt.header.CEOutinf ;
	acomm->NETrqst.ceCmd = IME_PRV_WORDSTYLELIST ;
	acomm->NETrqst.AtCEIncode = acomm->hinsiNum ;
#ifdef	ATOK_DEBUG
printf( "\311\312\273\354\301\355\277\364 %d\n"/*=$BIJ;lAm?t(B %d\n=*/, acomm->hinsiNum ) ;
#endif	/* ATOK_DEBUG */
	AtokClntCommunicate( acomm->sock, &acomm->NETrqst, &acomm->NETrslt ) ;

	size = sizeof( ATOKDEHINSTR ) * acomm->hinsiNum ;
	acomm->hinsiPtr = ( ATOKDEHINSTR * )malloc( size ) ;
	sptr = acomm->NETrslt.header.wordPtr ;
	memcpy( acomm->hinsiPtr, sptr, size ) ;
make_cand:
	size = 0 ;
	/*
	 * S-JIS $B$GAw$i$l$F$-$?IJ;l%j%9%H%G!<%?$r(B kinput2$BB&$G07$($k$h$&$K(B
	 * EUC $B$KJQ49$9$k!#(B
	 */
	for( i = 0 ; i < acomm->hinsiNum ; i++ ) {
		size += s2e( acomm->hinsiPtr[i].szHinsiStr, 
			strlen( acomm->hinsiPtr[i].szHinsiStr ),
			euc, sizeof( euc )) ;
	}
	/*
	 * $BIJ;l%j%9%H$r4IM}$9$k%P%C%U%!$O8uJd%j%9%H$r4IM}$9$k%P%C%U%!$H(B
	 * $BF1$8NN0h$H$7!"4{$K3NJ]$5$l$F$$$k>l9g$K$O$=$NNN0h$O3NJ]$7$J$$!#(B
	 * $B%P%C%U%!$r6&M-=PMh$k$N$O!"8uJd$r=PNOCf$K!"IJ;l%j%9%H$r(B
	 * $B=PNO$9$k;v$,$J$$$+$i$G$"$j!"F1;~$K=PNO$9$k;v$,$"$k>l9g$K$O(B
	 * $BNN0h$rJL$K$H$k$h$&$K=$@5$9$kI,MW$,$"$k!#(B
	 */
	if ( acomm->kohoSize == 0 ) {
		acomm->kohoStrPtr =  ( WCHAR * )malloc( size * sizeof( WCHAR )) ;
		acomm->kohoLenPtr = ( BYTE * )malloc( acomm->hinsiNum * sizeof( BYTE )) ;
		acomm->kohoSfxPtr = ( BYTE * )malloc( acomm->hinsiNum * sizeof( BYTE )) ;
		acomm->kohoSize = acomm->hinsiNum ;
		acomm->kohoStrLen = size ;
	}
	if ( acomm->kohoSize < acomm->hinsiNum ) {
		acomm->kohoLenPtr = ( BYTE * )realloc( acomm->kohoLenPtr, 
			acomm->hinsiNum  * sizeof( BYTE )) ;
		acomm->kohoSfxPtr = ( BYTE * )realloc( acomm->kohoSfxPtr, 
			acomm->hinsiNum * sizeof( BYTE )) ;
		acomm->kohoSize = n ;
	}
	if ( acomm->kohoStrLen < size ) {
		acomm->kohoStrPtr = ( WCHAR * )realloc( acomm->kohoStrPtr,
			size * sizeof( WCHAR )) ;
		acomm->kohoStrLen = size ;
	}
	len = 0 ;
	for( i = 0 ; i < acomm->hinsiNum ; i++ ) {
		n = strlen( acomm->hinsiPtr[i].szHinsiStr ) ;
		n = sjis2ucs( acomm->hinsiPtr[i].szHinsiStr, n, 
			&acomm->kohoStrPtr[len], size -len, 0 ) ;
		acomm->kohoLenPtr[i] = n ;
		len += n ;
	}
	acomm->kohoNum2 = acomm->hinsiNum ;
	acomm->kohoPos = 0 ;
		
	return acomm->hinsiNum ;
}

/* Page */
/*
 * [$B4X?tL>(B]
 *              atokCandidate( )
 * [$BI=Bj(B]
 *              $B8uJd%j%9%H$N<hF@!?@_Dj(B
 * [$B8F=P7A<0(B]
 *      	int atokCandidate( _XatokRecPtr acomm, int nbr )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		_XatokRecPtr : acomm   : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *		int          :   nbr   : i   : $B@_Dj$9$kHV9f(B
 *
 * [$BJV$jCM(B]
 *		0 $BL$K~(B : $BAw?.%(%i!<(B
 *		 0 : OK
 *
 * [$B;HMQ4X?t(B]
 *		atokClntCommunicate( )
 * [$B5!G=(B]
 *		$B8uJd%j%9%H$h$j!"8=:_A*Br$5$l$F$$$k8uJdHV9f$r@_Dj$9$k!#(B
 */
int atokCandidate(acomm, nbr)
_XatokRecPtr	acomm;
int		nbr;
{
	int	status ;
	/*
	 * if $BJ8I,MW$J$7(B
	 */
	if ( nbr != ATCEPRVAPI_READCMD ) {
		acomm->NETrqst.AtCEIncode = nbr ;
	}
	else {
		acomm->NETrqst.AtCEIncode = ATCEPRVAPI_READCMD ;
	}
	acomm->NETrqst.ceCmd = IME_PRV_CANDLIST ;

	status = AtokClntCommunicate( acomm->sock, &acomm->NETrqst, &acomm->NETrslt ) ;

	return status ;
}

/* Page */
/*
 * [$B4X?tL>(B]
 *		atokWordRegister( )
 * [$BI=Bj(B]
 *              $BC18l$NEPO?(B
 * [$B8F=P7A<0(B]
 *		atokWordRegister( _XatokRecPtr acomm, wchar *word,
 *			int wlen, wchar	*yomi, int ylen, int hinshi )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		_XatokRecPtr : acomm   : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *		wchar        : *word   : i   : $BEPO?C18l(B
 *		int          : wlen    : i   : $BEPO?C18lD9$5(B
 *		wchar        : *yomi   : i   : $BC18l$h$_(B
 *		int          :  ylen   : i   : $BC18l$h$_$J$,$5(B
 *		int          : hinshi  : i   : $BIJ;lHV9f(B
 *
 *
 * [$BJV$jCM(B]
 *		-1 : $BAw?.$G$-$J$$!#(B
 *	 	 0 : $B@5>o=*N;(B
 *		 1 : $B$3$NFI$_$G$O$3$l0J>eEPO?IT2D(B
 *		 2 : $B$3$l0J>eEPO?IT2D(B
 *		 3 : $B<-=q@_DjIT@5(B
 *		 4 : $BI=5-$KITE,@Z$JJ8;z$r4^$`(B
 *		 5 : $BEPO?$G$-$J$$!#(B
 *		 6 : $BFI$_$KITE,@Z$JJ8;z$r4^$`(B
 *		 7 : $B4{$KEPO?$5$l$F$$$k!#(B
 *
 * [$B;HMQ4X?t(B]
 *		atokClntCommunicate( )
 *		wcs2euc( )
 *		euc2ucs( )
 * [$B5!G=(B]
 *		$BC18lEPO?$r9T$J$&!#(B
 *		$BC18lI=5-!"$h$_$O(B $B$=$l$>$F(B UCS$B%3!<%I$GEPO?$9$k!#(B
 *		kinput2 $B$G4IM}$7$F$$$k;~$O(B
 *		$BC18lI=5-$O(B WCHAR, $BFI$_%3!<%I$O(BEUC$B$G$"$k$N$G$=$l$>$l(B
 *		UCS $B$X$NJQ49$r0lEY9T$J$&!#(B
 *		$BIJ;lHV9f$O!"#1$+$i;O$^$kJ*$G$"$k!#(B
 */
	
int atokWordRegister(acomm, word, wlen, yomi, ylen, hinshi)
_XatokRecPtr	acomm;
wchar		*word;
int		wlen;
wchar		*yomi;
int		ylen;
int		hinshi;
{
	AtokReqPacket	*req ;
	unsigned	char	*eucp ;
	WCHAR		*ucs_wp ;		/* $BC18l(B UCS $BJ8;zNs(B */
	WCHAR		*ucs_yp ;	/* $BFI$_(B UCS $BJ8;zNs(B */
	unsigned char	*sjp ;
	int		len ;
	int		alen ;
int i ;

	if ( wlen > ylen ) 	alen = wlen ;
	else			alen = ylen ;

	acomm->NETrqst.ceCmd = IME_PRV_REGISTERWORDEX ;
	req = &acomm->NETrqst ;
/* printf( "wlen %d, ylen %d\n", wlen, ylen ) ; */

	eucp = ( unsigned char * )malloc( alen * sizeof( WCHAR ) + 1 ) ;
	sjp = ( unsigned char * )malloc( ylen * sizeof( WCHAR ) + 1 ) ;

	/*
	 * $BC18lJ8;zNs$r(B WCS -> EUC -> UCS $BJQ49$9$k!#(B
	 */
	bzero( eucp, alen ) ;
	wcs2euc( word, wlen, eucp ) ;
	len = strlen( eucp ) ;
	ucs_wp = ( WCHAR * )malloc(( len  + 1 ) * sizeof( WCHAR )) ;
	euc2ucs( eucp, strlen( eucp ), ucs_wp, len, 0x3013 ) ;

#if 0
printf( "\303\261\270\354 WCS CODE -> "/*=$BC18l(B WCS CODE -> =*/ ) ;
for( i = 0 ; i <= wlen ; i++ ) {
	printf( "0x%x ", word[i] ) ;
}
printf( "\303\261\270\354 EUC CODE -> "/*=$BC18l(B EUC CODE -> =*/ ) ;
for( i = 0 ; i <= len ; i++ ) {
	printf( "0x%x ", eucp[i] ) ;
}
printf( "\n" ) ;
printf( "\303\261\270\354 EUC [%s]\n"/*=$BC18l(B EUC [%s]\n=*/, eucp ) ;
printf( "\303\261\270\354 UCS CODE -> "/*=$BC18l(B UCS CODE -> =*/ ) ;
for( i = 0 ; i <= wlen ; i++ ) {
	/* printf( "0x%x ", ucs_wp[i] ) ; */
}
printf( "\n" ) ;
#endif
	

	/*
	 * $BFI$_J8;zNs$r(B WCS -> EUC -> UCS $BJQ49$9$k!#(B
	 */
	bzero( eucp, alen ) ;
	wcs2euc( yomi, ylen, eucp ) ;
	len = strlen( eucp ) ;
	ucs_yp = ( WCHAR * )malloc(( len  + 1 ) * sizeof( WCHAR )) ;
	euc2ucs( eucp, strlen( eucp ), ucs_yp, len, 0x3013 ) ;

#if	0
printf( "\306\311\244\337 EUC CODE -->"/*=$BFI$_(B EUC CODE -->=*/ ) ;
for( i = 0 ; i <= len ; i++ ) {
	printf( "0x%x ", eucp[i] ) ;
}
printf( "\n" ) ;
printf( "\306\311\244\337 EUC [%s]\n"/*=$BFI$_(B EUC [%s]\n=*/, eucp ) ;
for( i = 0 ; i <= strlen( eucp )  ; i++ ) {
	printf( "0x%0x ", eucp[i] ) ;
}
printf( "\n" ) ;
printf( "\306\311\244\337 UCS CODE -> "/*=$BFI$_(B UCS CODE -> =*/ ) ;
for( i = 0 ; i <= ylen ; i++ ) {
	printf( "0x%x ", ucs_yp[i] ) ;
}
printf( "\n" ) ;
#endif

	/*
	 * $BAw?.%Q%i%a!<%?$N:n@.(B
	 */
	req->Kind[0] = T_WCHAR ;
	req->Size[0] = wlen ;
	req->ArgPtr[0] = ucs_wp ;

	req->Kind[1] = T_WCHAR ;
	req->Size[1] = ylen ;
	req->ArgPtr[1] = ucs_yp ;

	req->Ctr[0] = hinshi ;

	AtokClntCommunicate( acomm->sock, &acomm->NETrqst, &acomm->NETrslt ) ;

	free( ucs_wp ) ;
	free( ucs_yp ) ;
	free( eucp ) ;

/* printf( "$BEPO?7k2L(B %d\n", acomm->NETrslt.header.CEOutinf ) ; */
	return acomm->NETrslt.header.CEOutinf ;
}
/* End of if.c */
