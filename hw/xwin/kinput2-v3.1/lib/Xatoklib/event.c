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
static  char    rcsid[] = "$Id: event.c,v 1.4 1999/08/24 09:05:26 ishisone Exp $" ;
#endif  /* !lint */

#include	<stdio.h>
#include	<X11/IntrinsicP.h>
#include	<X11/StringDefs.h>
#include	<X11/Xmu/Atoms.h>
#include	<X11/keysym.h>
#include	<InputConv.h>

#include	<Xatoklib.h>
#include	<key_def.h>


/* Page */
/*
 * [$B4X?tL>(B]
 *              WordDelEvent( )
 * [$BI=Bj(B]
 *              $BC18l:o=|%&%#%s%I%&I=<(;~$N%$%Y%s%H=hM}(B
 * [$B8F=P7A<0(B]
 *      int WordDelEvent( _XatokRecPtr acomm, WORD *aajcode, WORD *ceCmd )
 *
 * [$B0z?t(B]
 *              $B7?(B        : $BL>(B       $B>N(B   : IO : $B@b(B      $BL@(B
 *		_XatokRecPtr : acomm    : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *		WORD      :  *aajcode   : i/o : $BF~NO%-!<%3!<%I(B
 *              WORD      :  *ceCmd     :   o : ATOK12CE $B$N%3%^%s%IHV9f(B
 *
 * [$BJV$jCM(B]
 *		0 : $B%-!<F~NO%$%Y%s%H$r(BCE$B$XEO$5$J$$(B
 *		1 : $B%-!<F~NO%$%Y%s%H$r(BCE$B$XEO$9(B
 *
 * [$B;HMQ4X?t(B]
 *		$B$J$7(B
 * [$B5!G=(B]
 *		$BC18l:o=|3NG'%@%$%"%m%0$rI=<($7$F$$$k;~$KM-8z$J%-!<$N(B
 *		$B@)8f$r9T$J$&!#M-8z$J%-!<$O(B 'Y','y', 'N', 'n' $B$@$1$G$"$k!#(B
 *		$B$=$l0J30$N%-!<$,F~NO$5$l$?;~$O2?$b$7$J$$!#(B
 *		$BM-8z%-!<$N;~$@$1!"%(%s%8%s$K%-!<$rEO$9$N$G#1$rJV$9!#(B
 */
static int WordDelEvent(acomm, aajcode, ceCmd)
_XatokRecPtr	acomm;
WORD		*aajcode;
WORD		*ceCmd;
{
	int	status = 0 ;

	switch( *aajcode ) {
		case 'y' :
		case 'Y' :
		case 'n' :
		case 'N' :
			/* 
			 * $BM-8z%-!<$N>l9g(B
			 */
			acomm->convStatus ^= M_WDEL ;	/* 623 */
			status = 1 ;
			break ;
		default :
			status = 0 ;
	}
	return status ;
}
/* Page */
/*
 * [$B4X?tL>(B]
 *		AuxEvent( )
 * [$BI=Bj(B]
*		AUX$B%&%#%s%I%&I=<(;~$N%$%Y%s%H=hM}(B
 * [$B8F=P7A<0(B]
 *      	int AuxEvent( _XatokRecPtr acomm, WORD *aajcode, 
 *			WORD *ceCmd, int *length )
 *
 * [$B0z?t(B]
 *              $B7?(B        : $BL>(B       $B>N(B   : IO : $B@b(B      $BL@(B
 *		_XatokRecPtr : acomm    : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *		WORD      :  *aajcode   : i/o : $BF~NO%-!<%3!<%I(B
 *              WORD      :  *ceCmd     :   o : ATOK12CE $B$N%3%^%s%IHV9f(B
 *		int       : *length     :   o : $BF~NO$5$l$?J8;zNs$ND9$5(B
 *
 *
 * [$BJV$jCM(B]
 *		-1 : $BAw?.%(%i!<(B
 *		 N : $BAw?.%P%$%H?t(B
 *
 * [$B;HMQ4X?t(B]
 *		MenuCtrl( )
 * [$B5!G=(B]
 *		AUX $B%&%#%s%I%&I=<(;~$N%$%Y%s%H=hM}$r9T$J$&!#(B
 *		AUX $B%&%#%s%I%&$K$O(B MenuClass $B$,@_Dj$5$l$F$$$F!"(B
 *		MenuClass $B$KDj5A$5$l$?2hLL$NA+0\$r9T$J$&!#(B
 */
static int AuxEvent(acomm, aajcode, ceCmd, length)
_XatokRecPtr	acomm;
WORD		*aajcode;
WORD		*ceCmd;
int		*length;
{
	int	status ;

	*length = 0 ;

#ifdef	ATOK_DEBUG
	printf( "AUX KEY 0x%x\n", *aajcode ) ;
#endif	/* ATOK_DEBUG */
	status = MenuCtrl( acomm, &acomm->curAux, aajcode, 0, length ) ;
#ifdef	ATOK_DEBUG
	printf( "MENU Status = %d\n", status ) ;
#endif	/* ATOK_DEBUG */
	/*
	 * NULL $B$K$J$C$?>l9g$O(BAUX WINDOW$B$r%-%c%s%;%k$5$l$?>l9g(B
	 * status : -1 $BJQ99$J$7(B(DefaultProc $B$G=hM}(B)
	 * 	  :  0 $B%a%K%e!<$NA*BrJQ99(B
	 *	  :  1 $B%a%K%e!<JQ99(B
	 *	  :  2 $B%a%K%e!<$N3NDj(B
	 *
	 * MenuStatus : 1  AuxStart
	 *		2  AuxChange
	 *		3  AuxEnd
	 */
	switch( status ) {
		case 1 :
			/* $B%a%K%e!<JQ99$"$j(B */
			/* printf( "acomm->curAux 0x%x\n", acomm->curAux ) ; */
			if ( !acomm->curAux ) {
				acomm->curAux = acomm->menuAux ;
				acomm->menuStatus = ICAuxEnd ;	/* END */
			}
			break ;
		case 2 :
			/* $B%a%K%e!<3NDj(B */
			acomm->menuStatus = ICAuxChange ;
			break ;
		case -1 :
			acomm->menuStatus = ICAuxChange ;
			return *length ;
		case 0 :
		default :
			acomm->menuStatus = ICAuxChange ;
			return 0 ;
	}
	/* return 0 ; */
	return *length ;
}

/* Page */
/*
 * [$B4X?tL>(B]
 *              CandEvent( )
 * [$BI=Bj(B]
 *              $B8uJd%&%#%s%I%&I=<(;~$N%$%Y%s%H=hM}(B
 * [$B8F=P7A<0(B]
 *      int CandEvent( _XatokRecPtr acomm, WORD *aajcode, WORD *ceCmd )
 *
 * [$B0z?t(B]
 *              $B7?(B        : $BL>(B     $B>N(B   : IO  : $B@b(B      $BL@(B
 *		_XatokRecPtr : acomm    : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *		WORD      :  *aajcode   : i/o : $BF~NO%-!<%3!<%I(B
 *              WORD      :  *ceCmd     :   o : ATOK12CE $B$N%3%^%s%IHV9f(B
 *
 *
 * [$BJV$jCM(B]
 *		0 : $B%-!<F~NO%$%Y%s%H$r(BCE$B$XEO$5$J$$(B
 *		1 : $B%-!<F~NO%$%Y%s%H$r(BCE$B$XEO$9(B
 *
 * [$B;HMQ4X?t(B]
 *		atokCandidate( )
 * [$B5!G=(B]
 *		$B8uJd%&%#%s%I%&$,I=<($5$l$F$$$k;~$N%$%Y%s%H=hM}$r9T$J$&!#(B
 *		$B%$%Y%s%H=hM}$OF~NO$5$l$?(B aajcode $B$K$h$jBP1~$9$k=hM}$,(B
 *		$B7hDj$9$k!#$3$3$G%A%'%C%/$7$F$$$k%3!<%I$O(B
 *		kinput2 $B$N(BAtokClass $B$N%b%8%e!<%k$G$bI=<(@)8f$N$?$a(B
 *		$B%A%'%C%/$7$F$$$k!#(B
 *		CTR_DEL $B$,2!$5$l$?;~$O!"(Bkinput2 $BB&$GJQ99$5$l$?%+%l%s%H8uJd$r(B
 *		$B%(%s%8%s$KEA$($F$+$i!"(BCTR_DEL $B$N:o=|=hM}$r8F$S=P$9!#(B
 */
static int CandEvent(acomm, aajcode, ceCmd)
_XatokRecPtr	acomm;
WORD		*aajcode;
WORD		*ceCmd;
{
	int	pgMax ;
	int	len = 0 ;

	pgMax = acomm->kohoMax ;

	switch( *aajcode ) {
		case EESC :
			*aajcode = FUNC_CNV_CANCL ;
			return 0 ;
		case LEFT :
		case RIGHT :
		case DOWN :
		case UP :
		case CTR_U :
		case CTR_N :
		case CTR_F :
		case CTR_B :
		case CTR_A :
			return 0 ;
		case CTR_DEL :
			/*
			 * $B8=:_(B $BI=<(>eA*Br$5$l$F$$$k8uJd$r%(%s%8%s$KEA$($k(B
			 * $BI,MW$,$"$k!#(B
			 */
			*aajcode = acomm->kohoPos ;
			atokCandidate( acomm, *aajcode ) ;
			*aajcode = CTR_DEL ;
			acomm->convStatus |= M_WDEL ;	/* 623 */
			return 1 ;
		case SPACE :
			*aajcode = SPACE ;
			/* *aajcode = XFER ; */
			return 0 ;	/* 617 */
			break ;
		case CR :
			/*
			 * kinput2 $BB&$GA*Br$5$l$F$$$k=j$H%(%s%8%sFb$G(B
			 * $BA*Br$5$l$F$$$k=j$rF14|$5$;$k!#(B
			 */
			*aajcode = acomm->kohoPos ;

			acomm->NETrqst.AtCEIncode = acomm->kohoPos ;
#ifdef	ATOK_DEBUG
	printf( "\301\252\302\362\244\265\244\354\244\277\270\365\312\344\310\326\271\346 (%d) Page (%d)\n"/*=$BA*Br$5$l$?8uJdHV9f(B (%d) Page (%d)\n=*/, 
				acomm->NETrqst.AtCEIncode,
			acomm->kohoCurPage ) ;
#endif	/* ATOK_DEBUG */
			len = 1 ;
			break ;
		/*
		 * $B?t;z%-!<$N>l9g$O8uJdHV9f$rA*Br$7$?;v$H$9$k!#(B
		 * $BM-8z$J?t;z$K$D$$$F$O%A%'%C%/$9$k;v(B
		 */

		default :
			len = 0 ;
#ifdef	CAND_PAGE
			if ( *aajcode >= '1' && *aajcode <= '9' ) {
				*aajcode -= '1' ;
				len = 1 ;
			}
			else if ( *aajcode >= 'a' && *aajcode <= 'z' ) {
				*aajcode -=  'a' ;
				*aajcode += 10 ;
				len = 1 ;
			}
			else if ( *aajcode >= 'A' && *aajcode <= 'Z' ) {
				*aajcode -=  'A' ;
				*aajcode += 10 ;
				len = 1 ;
			}
			*aajcode = acomm->kohoCurPage * pgMax + *aajcode ;
			*aajcode = acomm->kohoCurPage * pgMax + *aajcode ;
#else			
			len = 0 ;
#endif
	}
	if ( len > 0 ) {
#ifdef	ATOK_DEBUG
		Printf( "\270\365\312\344 \270\275\272\337\244\316\245\332\241\274\245\270 (%d)\n"/*=$B8uJd(B $B8=:_$N%Z!<%8(B (%d)\n=*/, acomm->kohoCurPage ) ;
		printf( "\270\365\312\344 \263\316\304\352\310\326\271\346 (%d)\n"/*=$B8uJd(B $B3NDjHV9f(B (%d)\n=*/, *aajcode ) ;
		printf( "\270\365\312\344 \263\316\304\352\310\326\271\346 (%d)\n"/*=$B8uJd(B $B3NDjHV9f(B (%d)\n=*/, acomm->NETrqst.AtCEIncode ) ;
#endif	/* ATOK_DEBUG */
		atokCandidate( acomm, *aajcode ) ;

		*aajcode = VKEY|FUNC_KAKU_BUBUN ;

	}
	return len ;
}
/* Page */
/*
 * [$B4X?tL>(B]
 *              KeyEvent( )
 * [$BI=Bj(B]
 *              $B%-!<F~NO%$%Y%s%H=hM}(B
 * [$B8F=P7A<0(B]
 *      	int KeyEvent( _XatokRecPtr acomm, WORD *aajcode, WORD *ceCmd )
 *
 * [$B0z?t(B]
 *              $B7?(B        : $BL>(B       $B>N(B   : IO : $B@b(B      $BL@(B
 *		_XatokRecPtr : acomm    : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *		WORD      :  *aajcode   : i/o : $BF~NO%-!<%3!<%I(B
 *              WORD      :  *ceCmd     :   o : ATOK12CE $B$N%3%^%s%IHV9f(B
 *
 * [$BJV$jCM(B]
 *		0 : $B%-!<F~NO%$%Y%s%H$r(BCE$B$XEO$5$J$$(B
 *		1 : $B%-!<F~NO%$%Y%s%H$r(BCE$B$XEO$9(B
 *
 * [$B;HMQ4X?t(B]
 *		atokCandidate( )
 *		XatokBufferConvert( )
 * [$B5!G=(B]
 *		$BDL>oF~NO>uBV$G$N%-!<F~NO%$%Y%s%H$N=hM}$r9T$J$&!#(B
 *		$BDL>o$N>uBV$G$O$J$/!"B>$N%$%Y%s%H=hM}$r9T$J$o$l$?8e$G$b(B
 *		$B%(%s%8%s$K%-!<$rEO$9I,MW$,$"$k>l9g$K$O8F$S=P$5$l$k!#(B
 */
static int KeyEvent(acomm, aajcode, ceCmd, wbuf, length, status, enable, change, elseEvent)
_XatokRecPtr	acomm;
WORD		*aajcode;
WORD		*ceCmd;
wchar		*wbuf;
int		*length;
int		*status;
int		*enable;
int		*change;
int		*elseEvent;
{
	int	call = 1 ;
	int	sRet = 0 ;
	WORD	key ;

	*length = 0 ;
	*status = 0 ;
	*enable = 0 ;
	*change = 0 ;
	acomm->NETrqst.AtCEIncode = 0 ;
	acomm->NETrqst.AtCEPrvAPIOutinf = 0 ;
	/*
	 * $BL$3NDjJ8;zNs$,$J$$$H$-$K@)8f%-!<$r2!$5$l$?>l9g$K$O(B
	 *  $B%-!<$r$=$N$^$^JV$9!#(B
	 * $B@)8f%-!<$K$D$$$F$N(B if $BJ8$O(B $B=$@5$9$kI,MW$"$j!#(B
	 */
#if	0
	if ( !acomm->echoLen && *aajcode == CR ) {
		/*
		( 
			 *aajcode == CR ||
			 *aajcode == SPACE   ||
			 *aajcode == BS
			)) {
		*/
		printf( "\245\271\245\353\241\274 KEY %x\n"/*=$B%9%k!<(B KEY %x\n=*/, *aajcode ) ;
#ifdef	ATOK_DEBUG
#endif	/* ATOK_DEBUG */
		wbuf[0] = *aajcode & 0xff ;
		*length = 1 ;
		return  1 ;
	}
#endif
	key =  *aajcode ;
	/*
	 * CTRL + W $B$O(B $BF~NO%b!<%I$N@Z$jBX$(%-!<(B
	 * $B%-!<F~NOCf$KBP1~$7$?(BAAJ CODE$B$N5!G=$G(B SWITCH$B$9$k$3$H!#(B
	 */
	switch( *aajcode ) {
		case CTR_F7 :
		case CTR_W :
			if ( acomm->echoLen > 0 ) {
				return 0 ;
			}
			acomm->convStatus = M_AUX ;
			MenuCtrl( acomm, &acomm->curAux, aajcode, 1, length ) ;
			acomm->menuStatus = ICAuxStart ;	
			return 0 ;
		case SPACE :
			*ceCmd = IME_PRV_KEYCONVERT ;
			/* *aajcode = XFER ; */
			/* key = XFER ; */
			/*
			 * $B$3$3$O(B $B%9%Z!<%9%-!<$N3dEv$r%A%'%C%/$9$k;v(B
			 */
			if ( !acomm->echoLen )	key = SPACE ;
			else 			key = XFER ;
			/* key = XFER ; */
			key = SPACE ;	 /* 616 */

			acomm->NETrqst.AtCEIncode = key & 0xffff ;
			sRet = 1 ;
			break ;
		/* KEYCONV */
		default :
			*ceCmd = IME_PRV_KEYCONVERT ;
			acomm->NETrqst.AtCEIncode = *aajcode & 0xffff ;
			sRet = 1 ;
			break ;
	}

	if ( call  ) {
		acomm->NETrqst.ceCmd = *ceCmd ;

#ifdef	ATOK_DEBUG
printf( "COMMUNICATE CMD(%d) AAJCODE(0x%04x)\n", *ceCmd, *aajcode & 0xffff ) ;
#endif	/* ATOK_DEBUG */
		AtokClntCommunicate( acomm->sock, &acomm->NETrqst, &acomm->NETrslt ) ;
#ifdef	ATOK_DEBUG
printf( "REQ OK\n" ) ;
#endif	/* ATOK_DEBUG */
	}

	*length = XatokBufferConvert( acomm, *ceCmd, &acomm->NETrslt, 
			status, enable, change, elseEvent ) ;

#ifdef	ATOK_DEBUG
	printf( "\270\365\312\344\244\316\277\364\244\362\245\301\245\247\245\303\245\257(%d)\241\243\n"/*=$B8uJd$N?t$r%A%'%C%/(B(%d)$B!#(B\n=*/, acomm->kohoNum ) ;
	printf( "CAND STAT (%d)\n", acomm->convStatus & M_CAND ) ;
#endif	/* ATOK_DEBUG */
	/*
	 * $B8uJd$,J#?tB8:_$7!"8=:_$^$@8uJd%&%#%s%I%&$rI=<($7$F$$$J$$$J$i(B
	 * $B8uJd%&%#%s%I%&$rI=<($9$k$?$a$K!"8uJd%j%9%H$r<hF@$9$k!#(B
	 */
	if ( acomm->kohoNum > 1 && 
		 !( acomm->convStatus & M_CAND ) ) {

		/*
		 * kohoNum $B$K$O(B $B8=:_$O8uJd?t$,F~$C$F$$$k$,!"(B
		 * Candidate $B$r$h$V$H%+%l%s%H8uJdHV9f$K$J$C$F$7$^$&$N$G(B
		 * kohoNum2 $B$K8uJd$NAm?t$rJ]B8$7$F$*$/(B
		 */
		acomm->kohoNum2 = acomm->kohoNum ;

#ifdef	ATOK_DEBUG
		printf( "\270\365\312\344\244\254\244\242\244\303\244\277\244\316\244\307 \270\365\312\344\311\275\274\250\245\342\241\274\245\311\244\313\260\334\244\353\241\243\n"/*=$B8uJd$,$"$C$?$N$G(B $B8uJdI=<(%b!<%I$K0\$k!#(B\n=*/ ) ;
#endif	/* ATOK_DEBUG */
		atokCandidate( acomm, ATCEPRVAPI_READCMD ) ;
		acomm->convStatus |= M_CAND ;
		*aajcode = XFER ;
		*length = XatokBufferConvert( acomm, *ceCmd, &acomm->NETrslt, 
			status, enable, change, elseEvent ) ;
	}

	return 1 ;
}

/* Page */
/* 
 * [$B4X?tL>(B]
 *		XatokEventControl( )
 * [$BI=Bj(B]
 *		$B%-!<F~NO$N<u$1IU$1(B
 * [$B8F=P7A<0(B]
 *		int	XatokEventControl( AtokPart
 * [$B0z?t(B]
 *		$B7?(B        : $BL>(B       $B>N(B   : IO : $B@b(B      $BL@(B
 *	
 *	
 * [$BJV$jCM(B]
 *		
 * [$B;HMQ4X?t(B]
 *	
 * [$B5!G=(B]
 *		X $B$+$i$N%$%Y%s%H$r<u$1<h$j(B ATOK $B$N(B AAJCODE$B$KJQ49$7$F!"(B
 *		ATOK CE $B$KJQ49$r0MMj$9$k!#(B
 *		$BJQ497k2L$r<u$1<h$C$?8e$K!"(BlibXatok $BMQ$KJQ497k2L$r(B
 *		$B%P%C%U%!$K@_Dj$9$k!#(B
 *
 */
int XatokEventControl(acomm, event, xkey, kbuf, nbytes, aajcode, status, enable, change, elseEvent)
/* Fix Length */
_XatokRecPtr	acomm;
XKeyEvent	*event;
KeySym		xkey;
char		*kbuf;
int		nbytes;
WORD		*aajcode;
int		*status;
int		*enable;
int		*change;
int		*elseEvent;
{
	char 		euc[BUFSIZ*4] ;	/* $BFI$_I=<($N$?$a$N%P%C%U%!(B */
	int		es ;			/* euc $BJ8;zNs$ND9$5(B */
	unsigned char	*sp ;
	wchar		*wbuf ;	

	WORD		ceCmd = -1 ;		/* CE $B$XAw$k%3%^%s%I(B */
	int		length ;		/* $B3NDjJ8;zNs$ND9$5(B */
	extern WORD	getAAJCode( )  ;	
	int		i ;			/* WORK */
	int		sRet ;
		
	wbuf = acomm->wbuf ;
	*status = 0 ;
	wbuf[0] = 0 ;
	length = 0 ;
	acomm->wlen = 0 ;

	/*
	 * $BK\Ev$O$3$3$G(B ATOK CE$B$r8F$S=P$7(B $BJQ497k2L$r8+$F=hM}$r$9$k!#(B
	 * CE $B$r8F$S=P$9(BAPI$B$O(B X$B$N%-!<%$%Y%s%H$r8+$F(B AAJCODE$B$K$9$k!#(B
	 * $B3NDj$7$?J8;z$,$"$k>l9g$K$O!"(B wbuf $B$K3NDjJ8;z$r@_Dj$9$k!#(B
	 * $B3NDj$7$J$+$C$?>l9g$K$O!"F~NOJ8;z$H$7$F(Bprebuf $B$K%;%C%H$9$k!#(B
	 */
	/*
	 * $B$3$3$G(B $B%-!<F~NO$r(BATOK$B$N%3!<%I(B(AAJCODE)$B$KJQ49$7$F!"(B
	 * $B3F%b!<%I;~$KBP1~$7$?%3%^%s%I$NE83+$9$k!#(B
	 */
	*aajcode = getAAJCode( event, xkey, kbuf ) ;
#ifdef	ATOK_DEBUG
printf( "aajcode  0x%x = len = %d\n", *aajcode, acomm->echoLen ) ;
printf( "convStatus  %d\n",  acomm->convStatus ) ;
#endif	/* ATOK_DEBUG */
	if ( *aajcode == 0 ) {
		return 0 ;
	}

	/*
	 * $B%-!<%$%Y%s%H$,H/@8$7$F$3$N4X?t$K$H$s$GMh$?;~$K(B
	 * $B%b!<%I$,@_Dj$5$l$F$$$J$$>l9g$K$O!"JQ493+;O$N%-!<$r(B
	 * $B2!$5$l$?;~$G$"$k$N$G!"%-!<JQ49%b!<%I$K@_Dj$9$k!#(B
	 */
	if ( acomm->convStatus == M_NOKEYCONV ) {
		atokKanjiOn( acomm ) ;
/* printf( "KANJI ON MODE (%d)\n", acomm->convStatus ) ; */
	}

	/*
	 * CTRL_SPACE $B$O(B $B4A;z(BON$B!?(BOFF$B$N@Z$jBX$(%-!<(B
	 * CTRL_SPACE $B$O$I$s$J>l9g$G$b4A;z(BOFF
	 */	
	if ( *aajcode == SFT_SP ) {
#ifdef	ATOK_DEBUG
	printf( "AAJ SFT_SP -> KANJIOFF\n" ) ;
#endif	/* ATOK_DEBUG */
		if  ( acomm->convStatus == M_KEYCONV ) {
			atokKanjiOff( acomm ) ;
/* printf( "KANJI ON OFF (%d)\n", acomm->oldStatus ) ; */
		}
		return 0 ;
	}

	if ( acomm->convStatus & M_AUX ) {
		/*
		 * AUX $B%&%#%s%I%&(B $BI=<(Cf$N%$%Y%s%H=hM}(B
		 */

		sRet = AuxEvent( acomm, aajcode, &ceCmd, &length ) ;
		/* printf( "AUX %d RETURN LENGTH = %d\n", sRet, length ) ; */
		if ( sRet < 1 ) {
			return length ;
		}
	}
	/* if ( acomm->convStatus & M_CAND ) { */
	/*
	 * $B8uJd%&%#%s%I%&$rI=<(Cf$G$+$D%7%9%F%`9T$rI=<($7$F$$$J$$>l9g$N(B
	 * $B%$%Y%s%H=hM}(B
	 */
	if ( acomm->convStatus & M_WDEL ) {
		sRet = WordDelEvent( acomm, aajcode, &ceCmd ) ;
		if ( sRet == 0 ) {
			return 0 ;
		}
	}
	if ( acomm->convStatus & M_CAND &&
		!( acomm->convStatus & M_SYSLINE )) { /* 616 */
		/*
		 * $B8uJd%&%#%s%I%&$rI=<(Cf$N%$%Y%s%H=hM}(B
		 */
		sRet = CandEvent( acomm, aajcode, &ceCmd ) ;
		/* 
		 * $B%9%F!<%?%9$,#00J30$N;~$O!"8uJd$,3NDj$5$l$?;~(B
		 */
		if ( sRet == 0 ) {
			return 0 ;
		}
	}
	/*
	 * $B$=$NB>$O(B $BDL>o$N%-!<F~NO%$%Y%s%H(B
	 */
/* printf( "KEY CONV AAJ 0x%x\n", *aajcode ) ; */
	 KeyEvent( acomm, aajcode, &ceCmd, wbuf, &length,
			status, enable, change, elseEvent ) ;

/* printf( "$B8uJd$NHV9f(B(%d)$B!#(B\n", acomm->kohoPos ) ; */
#ifdef	ATOK_DEBUG
	printf( "\263\316\304\352\312\270\273\372\316\363EVENT[%s]\n"/*=$B3NDjJ8;zNs(BEVENT[%s]\n=*/, acomm->wbuf ) ;
#endif	/* ATOK_DEBUG */
	acomm->wlen = length ;

	return length ;
}

/* End of event.c */
