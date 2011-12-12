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
static  char    rcsid[] = "$Id: aux.c,v 1.3 1999/08/24 09:05:25 ishisone Exp $" ;
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
 *		wordStartFunc( )
 * [$BI=Bj(B]
 *              $BC18lEPO?%a%K%e!<$N=i4|2==hM}(B
 * [$B8F=P7A<0(B]
 *	int	wordStartFunc( _XatokRecPtr  acomm )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		_XatokRecPtr : acomm   : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *
 * [$BJV$jCM(B]
 *		1 : $B>o$K#1(B
 *
 * [$B;HMQ4X?t(B]
 *		$B$J$7(B
 * [$B5!G=(B]
 *		$BC18lEPO?%a%K%e!<$rI=<($9$k0Y$K!"%G!<%?$N=i4|2=$r9T$J$&!#(B
 */
int wordStartFunc(acomm)
_XatokRecPtr	acomm;
{

#ifdef	ATOK_DEBUG
	printf( "\303\261\270\354\305\320\317\277\263\253\273\317\n"/*=$BC18lEPO?3+;O(B\n=*/ ) ;
#endif	/* ATOK_DEBUG */
	bzero( acomm->word_str, sizeof( acomm->word_str )) ;
	bzero( acomm->word_yomi, sizeof( acomm->word_yomi )) ;
	acomm->word_strLen = 0 ;
	acomm->word_yomiLen = 0 ;
	acomm->word_mode = W_WORD ;

	return 1 ;
}

/* Page */
/*
 * [$B4X?tL>(B]
 *		wordFunc( )
 * [$BI=Bj(B]
 *              $BC18lEPO?;~$N%$%Y%s%H=hM}(B
 * [$B8F=P7A<0(B]
 *      int wordFunc( _XatokRecPtr acomm,  menuAux *mAux, 
 *			WORD *aajcode, int *op )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B   $B>N(B  : I/O : $B@b(B      $BL@(B
 *		_XatokRecPtr : acomm    : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *		menuAux      : *mAux    : i   :  $B8=:_I=<($7$F$$$k(BMENU
 *		WORD         : *aajcode : i/o : $BF~NO$5$l$?J8;z(B
 *		int          : *op      :   o : MENU OPERATION
 *
 *
 * [$BJV$jCM(B]
 *		0 : key $BF~NO$r%(%s%8%s$KAw$i$J$$!#(B
 *		1 : key$BF~NO$r%(%s%8%s$KAw$k!#(B
 *
 * [$B;HMQ4X?t(B]
 *		atokModeSet( )
 *		atokWordRegister( )
 *		atokWordStyleList( )
 * [$B5!G=(B]
 *		$BC18lEPO?MQ$N(BAUX WINDOW$BI=<(;~$N%$%Y%s%H4IM}$r9T$J$&!#(B
 *		$BC18lEPO?;~$O!"C18lI=5-F~NO!"FI$_F~NO!"IJ;lA*Br!"=hM}3NG'(B
 *		$B=hM}7k2LI=<($N%b!<%I$,B8:_$9$k!#(B
 *		$B3F%b!<%I$K$*$$$F!"F~NO$G$-$kJ8;zBN7O$r@_Dj$9$k!#(B
 *		CTR$B%-!<$J$I$N@)8f%-!<$NA`:n$b9T$J$&!#(B
 */
int wordFunc(acomm, mAux, aajcode, op)
_XatokRecPtr    acomm;
menuAux		*mAux;
WORD		*aajcode;
int		*op;
{
	ATOKDEHINSTR	*hptr ;
	char		euc[BUFSIZ] ;
	int		i ;
	int		pgMax = CAND_ROWS * CAND_COLS ;
	int		status ;

	char	*pp ;
#ifdef	ATOK_DEBUG
printf( "WORD FUNC OK (%x) word_mode (%d)\n", *aajcode, acomm->word_mode ) ;
printf( "word_strLen  %d, word_yomiLen %d\n", 
		acomm->word_strLen, acomm->word_yomiLen ) ;
#endif	/* ATOK_DEBUG */
	if ( *aajcode == CTR_H ) 	*aajcode = BS ;

	/*
	 * $BL$3NDjJ8;z$,L5$$;~$K@)8f%-!<$rF~NO$5$l$?>l9g$K$O(B
	 * $B%*%Z%l!<%7%g%s$rL58z$K$9$k!#(B
	 */
	if ( !acomm->echoLen  && 
		(( *aajcode & CTRL ) || *aajcode == DEL )) {
		return 0 ;
	}
	/*
	 * $BC18lEPO?<B;\Cf$K(B $B%3!<%IF~NO$J$I$N#A#U#X%&%#%s%I%&$O(B
	 * $BI=<($G$-$J$$$N$G%*%Z%l!<%7%g%s$rL58z$K$9$k!#(B
	 */
	if ( *aajcode == F10 ) {
		return 0 ;
	}
	/*
	 * $BC18lF~NO;~$N#O#P(B
	 */
	if ( acomm->word_mode == W_WORD ) {
		if ( *aajcode == CR && acomm->echoLen == 0 ) {
			if ( acomm->word_strLen ) {
				atokModeSet( acomm, IME_PRV_SETKOTEMODE, 0x0001 ) ;
				acomm->word_mode <<= 1 ;
				return 0 ;
			}
			return 0 ;
		}
		else if ( *aajcode == BS && !acomm->echoLen ) {
			if (  acomm->word_strLen ) {
				acomm->word_strLen-- ;
			}
			else {
				*op  = OP_CANCEL ; 
			}
			return 0 ;
		}

		else if ( *aajcode == EESC ) {
			if ( !( acomm->convStatus & M_CAND )) {	/* 616 */
				*op = OP_CANCEL ;
			}
			/*
			 * $BL$3NDjJ8;zNs$,$"$k;~$O!"L$3NDj%P%C%U%!$r%/%j%"$9$k(B
			 */
			if ( acomm->echoLen  ) {
				acomm->echoLen = 0 ;
				*aajcode = EESC ;
				return 1 ;
			}
			return 0 ;
		}
		else if ((( *aajcode & CTRL ) && !acomm->echoLen ) 
			|| *aajcode == TAB ){
			return 0 ;
		}
		return 1 ;
	}
	/*
	 * $BFI$_F~NO;~$N(BOP
	 */
	else if ( acomm->word_mode == W_YOMI ) {
		if ( *aajcode == CR  && acomm->echoLen == 0 ) {
			if ( acomm->word_yomiLen ) {
				atokModeSet( acomm, IME_PRV_SETKOTEMODE, 0x0000 ) ;
				acomm->word_mode <<= 1 ;
				acomm->convStatus |= M_CAND ;
				atokWordStyleList( acomm ) ;
				*aajcode = XFER ;
				return 0 ;
			}
			return 0 ;
		}
		else if ( *aajcode == BS && !acomm->echoLen ) {
			if (  acomm->word_yomiLen ) {
				acomm->word_yomiLen-- ;
			}
			else {
				atokModeSet( acomm, IME_PRV_SETKOTEMODE, 0x0000 ) ;
				acomm->word_mode >>= 1 ;
			}
			return 0 ;
		}
		else if ((( *aajcode & CTRL ) && !acomm->echoLen ) 
			|| *aajcode == TAB ){
			return 0 ;
		}
		else if ( *aajcode == EESC ) {
			atokModeSet( acomm, IME_PRV_SETKOTEMODE, 0x0000 ) ;
			/* acomm->word_mode >>= 1 ; */
			*op = OP_CANCEL ;
			return 0 ;
		}
		return 1 ;
	}
	/*
	 * $BIJ;l$rA*BrCf$N(BOP
	 */
	else if ( acomm->word_mode == W_HINSHI ) {
		if ( *aajcode > '0' && *aajcode <= '9' ) {
			acomm->kohoPos = *aajcode - '1' ;
			 *aajcode = CR ;
		}
		if ( *aajcode == CR ) {
			acomm->word_hinshi = acomm->kohoCurPage * pgMax + acomm->kohoPos + 1 ;
			acomm->word_mode <<= 1 ;
		}
		else if ( *aajcode == CTR_G ||
			*aajcode == EESC ) {
			acomm->word_mode >>= 1 ;
		}
		return 0 ;
	}
	/*
	 * $BEPO?3NG'$N%@%$%"%m%0I=<(Cf$N(BOP
	 */
	else if ( acomm->word_mode == W_YORN ) {
		if ( *aajcode == 'y' || *aajcode == 'Y'  ) {
			acomm->word_str[acomm->word_strLen] = 0x00 ;
			acomm->word_str[acomm->word_strLen+1] = 0x00 ;

			acomm->wordStatus = atokWordRegister(
				acomm,
				acomm->word_str,
				acomm->word_strLen,
				acomm->word_yomi,
				acomm->word_yomiLen,
				acomm->word_hinshi ) ;
			acomm->word_mode <<= 1 ;
			/* *op  = OP_CANCEL ; */
		}
		else if ( *aajcode == 'n' || *aajcode == 'N' || *aajcode == BS ) {
			atokWordStyleList( acomm ) ;
			acomm->convStatus |= M_CAND ;
			acomm->word_mode >>= 1 ;
			*aajcode = XFER ;
		}
		else if ( *aajcode == EESC ) {
			acomm->word_mode = 0 ;
			*op = OP_CANCEL ;
		}
		return 0 ;
	}
	/*
	 * $BEPO?7k2LI=<(Cf$N(BOP
	 */
	else if ( acomm->word_mode == W_RESULT ) {
		if ( *aajcode == CR || *aajcode == EESC ) {
			acomm->word_mode = 0 ;
			*op = OP_CANCEL ;
		}
		return 0 ;
	}
	return 0 ;
		
#ifdef	ATOK_DEBUG
	printf( "WORD STR [%s]\n", pp ) ;
#endif	/* ATOK_DEBUG */
}

/* Page */
/*
 * [$B4X?tL>(B]
 *		wordEndFunc( )
 * [$BI=Bj(B]
 *              $BC18lEPO?%a%K%e!<$N8e=hM}(B
 * [$B8F=P7A<0(B]
 *	int	wordEndFunc( _XatokRecPtr  acomm )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		_XatokRecPtr : acomm   : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *
 * [$BJV$jCM(B]
 *		1 : $B>o$K#1(B
 *
 * [$B;HMQ4X?t(B]
 *		$B$J$7(B
 * [$B5!G=(B]
 *		$BC18lEPO?%a%K%e!<$r:o=|$9$k0Y$K!"%G!<%?$N8e=hM}$r9T$J$&!#(B
 *		
 */
int wordEndFunc(acomm)
_XatokRecPtr    acomm;
{

	/* printf( "wordEndFunc\n" ) ; */
	return 1 ;
}
/*
 * Str
 * $BC18lEPO?(B : $BC18l(B [XXXXX] $BFI$_(B [XXXX] $BIJ;l(B :
 */
/* Page */
/*
 * [$B4X?tL>(B]
 *		wordStringFunc( )
 * [$BI=Bj(B]
 *              $BC18lEPO?%a%K%e!<$NJ8;zNs:n@.(B
 * [$B8F=P7A<0(B]
 *	int	wordStringFunc( _XatokRecPtr  acomm, menuAux *mAux,
 *			wchar *echoStr, int *echoLen, int *revLen,
 *			int *revPos )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		_XatokRecPtr : acomm   : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *		menuAux      : *mAux   : i  : $B8=:_$N(B MENU CLASS
 *		wchar        : *echoStr:   o : $BI=<(J8;zNs3JG<NN0h(B
 *		int          : *echoLen :  o : $BI=<(J8;zNs$ND9$5(B
 *		int          : *revLen :   o : $BI=<(J8;zNs$NH?E>$ND9$5(B
 *		int          : *revPos :   o : $BI=<(J8;z%F%k$NH?E>$N3+;O0LCV(B
 *
 * [$BJV$jCM(B]
 *		$BI=<(J8;zNs$ND9$5(B
 *
 * [$B;HMQ4X?t(B]
 *		euc2wcs( )
 *		s2e( )
 * [$B5!G=(B]
 *		$BC18lEPO?%a%K%e!<$rI=<(;~$NI=<(MQ$NJ8;zNs$r:n@.$9$k!#(B
 *		$BI=5-F~NO;~$O!"I=5-3NDjJ8;zNs$HL$3NDjJ8;zNs!"(B
 *		$B$h$_F~NO;~$O!"I=5-3NDjJ8;zNs$H!"$h$_3NDjJ8;zNs!"$h$_L$3NDjJ8;zNs(B
 *		$BIJ;lA*Br;~$O!"FI$_F~NO;~$HF1$8!"(B
 *		$BEPO?3NG';~$O!"I=5-3NDjJ8;zNs(B,$B$h$_J8;zNs!"IJ;l(B
 *		$BEPO?40N;;~$O!"%9%F!<%?%9J8;zNs(B
 *		$B$r:n@.$9$k!#(B
 */
int wordStringFunc(acomm, mAux, echoStr, echoLen, revLen, revPos)
_XatokRecPtr    acomm;
menuAux		*mAux;
wchar		*echoStr;
int		*echoLen;
int		*revLen;
int		*revPos;
{
	char	tbuf[BUFSIZ*2] ;
	int	len ;
	int	n ;
	int	i, j, k, es ;
	char	*ep ;
	int		status ;
	int	c ;

	*revPos = 0 ;
	*revLen = 0 ;

	ep = tbuf ;
	if ( mAux->str ) {
		strcpy( ep, mAux->str ) ;
		ep += strlen( ep ) ;
	}
#ifdef	ATOK_DEBUG
printf(  "WORD MODE %d\n", acomm->word_mode );
	printf( "  \303\261\270\354 [%s"/*=  $BC18l(B [%s=*/, acomm->word_str ) ;
printf( "\303\261\270\354\263\316\304\352\272\321\312\270\273\372\316\363(%d) [%s]\n"/*=$BC18l3NDj:QJ8;zNs(B(%d) [%s]\n=*/, acomm->word_strLen, acomm->word_str ) ;
printf( "\303\261\270\354  \263\316\304\352\244\267\244\277\312\270\273\372\316\363(%d) [%s]\n"/*=$BC18l(B  $B3NDj$7$?J8;zNs(B(%d) [%s]\n=*/, acomm->wlen, acomm->wbuf ) ;
printf( "\303\261\270\354 \314\244\263\316\304\352\312\270\273\372\316\363[%s]\n"/*=$BC18l(B $BL$3NDjJ8;zNs(B[%s]\n=*/, acomm->echoStr ) ;
for( i = 0 ; i < acomm->echoLen ; i++ ) {
	printf( "%x ", acomm->echoStr[i] ) ;
}
printf( "\n" );
#endif	/* ATOK_DEBUG */


	/*
	 * $BC18lEPO?$N%,%$%I$r@_Dj(B
	 */
	sprintf( ep, "  \303\261\270\354 ["/*=  $BC18l(B [=*/ ) ;
	es = strlen( tbuf ) ;
	len = euc2wcs( tbuf, es, echoStr ) ;
	for( i = 0 ; i < acomm->word_strLen ; i++ ) {
		echoStr[len++] = acomm->word_str[i] ;
	}
	/*
	 * $BC18lF~NOCf$NI=<((B
	 */
	if (  acomm->word_mode == W_WORD ) {
		/* 
		 * $B:#2s$NF~NO$G3NDj$7$?J8;zNs$,$"$k>l9g$K$O!"(B
		 * $B3NDjC18lJ8;zNs$H$7$F@_Dj$9$k!#(B
		 */
		for( i = 0 ; i < acomm->wlen ; i++ ) {
			if ( acomm->word_strLen+i >= WORD_HYOKI_MAX ) {
				/* printf( "$BC18l$,$J$,$9$.$k!#(B(%d)\n", i ) ; */
				break ;
			}
			acomm->word_str[acomm->word_strLen+i] = acomm->wbuf[i] ;
			echoStr[len++] = acomm->word_str[acomm->word_strLen+i] ;
/* printf( "0x%x ", acomm->wbuf[i] ) ; */
		}
/* printf( "\n" ) ; */
		acomm->word_strLen += i ;
		acomm->word_str[acomm->word_strLen] = 0x00 ;
		/*
		 * $BL$3NDjJ8;zNs$r@_Dj(B
		 */
		if ( acomm->echoLen > 0 ) {
			*revPos = len + acomm->revPos ;
			*revLen = acomm->revLen ;
			for( i = 0 ; i < acomm->echoLen ; i++ ) {
				echoStr[len++] = acomm->echoStr[i] ;
			}
		}
		else { 
			*revPos = len ;
			*revLen = 1 ;
		}
	}
	n = euc2wcs( "]", 1, &echoStr[len] ) ;
	len += n ;
	*echoLen = len ;
	if ( acomm->word_mode < W_YOMI ) {
		return ;
	}
	sprintf( tbuf, "  \306\311\244\337 ["/*=  $BFI$_(B [=*/ ) ;
	es = strlen( tbuf ) ;
	n = euc2wcs( tbuf, es, &echoStr[len] ) ;
	len += n ;

	/*
	 * $B$h$_$N$J$,$5$O(B $B4A;z$H(BANK$B$G$O(B $BF1$8$G$"$k!#(B
	 */
	for( i = 0 ; i < acomm->word_yomiLen ; i++ ) {
		echoStr[len++] = acomm->word_yomi[i] ;
	}
	if ( acomm->word_mode == W_YOMI ) {
		/* 
		 * $B:#2s$NF~NO$G3NDj$7$?J8;zNs$,$"$k>l9g$K$O!"(B
		 * $B3NDjC18lJ8;zNs$H$7$F@_Dj$9$k!#(B
		 */
		for( i = 0 ; i < acomm->wlen ; i++ ) {
			if ( acomm->word_yomiLen+i >= WORD_YOMI_MAX ) {
				/* printf( "$B$h$_$,$J$,$9$.$k!#(B(%d)\n", i ) ; */
				break ;
			}
			acomm->word_yomi[acomm->word_yomiLen+i] = acomm->wbuf[i] ;
			echoStr[len++] = acomm->word_yomi[acomm->word_yomiLen+i] ;
		}
		acomm->word_yomiLen += i ;
		/*
		 * $BL$3NDjJ8;zNs$r@_Dj(B
		 */
		if ( acomm->echoLen > 0 ) {
			*revPos = len + acomm->revPos ;
			*revLen = acomm->revLen ;
			for( i = 0 ; i < acomm->echoLen ; i++ ) {
				echoStr[len++] = acomm->echoStr[i] ;
			}
		}
		else { 
			*revPos = len ;
			*revLen = 1 ;
		}
	}

	n = euc2wcs( "]", 1, &echoStr[len] ) ;
	len += n ;

	*echoLen = len ;
	if ( acomm->word_mode < W_HINSHI ) {
		return len ;
	}
	if ( acomm->word_mode > W_HINSHI ) {
		sprintf( tbuf, "  \311\312\273\354  ["/*=  $BIJ;l(B  [=*/ ) ;
		es = strlen( tbuf ) ;
		n = euc2wcs( tbuf, es, &echoStr[len] ) ;
		len += n ;

		s2e( acomm->hinsiPtr[acomm->word_hinshi-1].szHinsiStr, 
			strlen( acomm->hinsiPtr[acomm->word_hinshi-1].szHinsiStr ),
			tbuf, sizeof( tbuf )) ;

		es = strlen( tbuf ) ;
		n = euc2wcs( tbuf, es, &echoStr[len] ) ;
		len += n ;
			
		sprintf( tbuf, "] \244\350\244\355\244\267\244\244\244\307\244\271\244\253 [y/n]"/*=] $B$h$m$7$$$G$9$+(B [y/n]=*/, tbuf ) ;
		es = strlen( tbuf ) ;
		n = euc2wcs( tbuf, es, &echoStr[len] ) ;
		len += n ;

		*revPos = len - 1 ;
		*revLen = 1 ;
	}
	if ( acomm->word_mode == W_RESULT ) {
		switch( acomm->wordStatus ) {
			case 0 :
				sprintf( tbuf, "\305\320\317\277\275\252\316\273\244\267\244\336\244\267\244\277\241\243"/*=$BEPO?=*N;$7$^$7$?!#(B=*/ ) ;
				break ;
			case 1 :
				sprintf( tbuf, "\244\263\244\316\306\311\244\337\244\307\244\317\244\263\244\354\260\312\276\345\305\320\317\277\244\307\244\255\244\336\244\273\244\363\241\243"/*=$B$3$NFI$_$G$O$3$l0J>eEPO?$G$-$^$;$s!#(B=*/ ) ;
				break ;
			case 2 :
				sprintf( tbuf, "\244\263\244\354\260\312\276\345\305\320\317\277\244\307\244\255\244\336\244\273\244\363\241\243"/*=$B$3$l0J>eEPO?$G$-$^$;$s!#(B=*/ ) ;
				break ;
			case 3 :
				sprintf( tbuf, "\274\255\275\361\300\337\304\352\244\254\311\324\305\254\300\332\244\307\244\271\241\243"/*=$B<-=q@_Dj$,ITE,@Z$G$9!#(B=*/ ) ;
				break ;
			case 4 :
				sprintf( tbuf, "\303\261\270\354\244\313\311\324\305\254\300\332\244\312\312\270\273\372\244\254\264\336\244\336\244\354\244\306\244\244\244\336\244\271\241\243"/*=$BC18l$KITE,@Z$JJ8;z$,4^$^$l$F$$$^$9!#(B=*/ ) ;
				break ;
			case 5 :
				sprintf( tbuf, "\305\320\317\277\244\307\244\255\244\336\244\273\244\363\241\243"/*=$BEPO?$G$-$^$;$s!#(B=*/ ) ;
				break ;
			case 6 :
				sprintf( tbuf, "\306\311\244\337\244\313\311\324\305\254\300\332\244\312\312\270\273\372\244\254\264\336\244\336\244\354\244\306\244\244\244\336\244\271\241\243"/*=$BFI$_$KITE,@Z$JJ8;z$,4^$^$l$F$$$^$9!#(B=*/ ) ;
				break ;
			case 7 :
				sprintf( tbuf, "\264\373\244\313\305\320\317\277\244\265\244\354\244\306\244\244\244\336\244\271\241\243"/*=$B4{$KEPO?$5$l$F$$$^$9!#(B=*/ ) ;
				break ;
			default:
				sprintf( tbuf, "\303\261\270\354\305\320\317\277\244\313\274\272\307\324\244\267\244\336\244\267\244\277\241\243"/*=$BC18lEPO?$K<:GT$7$^$7$?!#(B=*/ ) ;
				break;
		}
		strcat( tbuf, " " ) ;
		es = strlen( tbuf ) ;
		n = euc2wcs( tbuf, es, echoStr ) ;
		len = n ;
		*revPos = len - 1 ;
		*revLen = 1 ;
	}
	*echoLen = len ;

#ifdef	ATOK_DEBUG
printf( "\303\261\270\354 EUC STR [%s]\n"/*=$BC18l(B EUC STR [%s]\n=*/, tbuf ) ;
#endif	/* ATOK_DEBUG */
	return len ;
}


#define	OFF	0
#define	ON	1
#define	itemSize( item )	sizeof( item ) / sizeof( item[0] )

/*
 * $BC18lEPO?(B
 */
menuItem	auxWordRegItem[] = {
	  { CTR_G,   OFF, OP_CANCEL, NULL, NULL, NULL } 
} ;
menuAux		auxWordRegMenu[] = {
	M_WORD, -1, 
		wordStartFunc,
		wordFunc,
		NULL, 
		wordStringFunc,
		"\303\261\270\354\305\320\317\277\241\247"/*=$BC18lEPO?!'(B=*/, NULL,
		auxWordRegItem, itemSize( auxWordRegItem ) 
} ;

#define	REVERSE		printf( "\033[7m" )
#define	NORMAL		printf( "\033[m" )

menuAux	*getMenuInstance( )
{
	return auxWordRegMenu ;
}

/* Page */
/*
 * [$B4X?tL>(B]
 *		menuPrevOn( )
 * [$BI=Bj(B]
 *              $B%a%K%e!<9`L\$N0\F0(B
 * [$B8F=P7A<0(B]
 *	int	menuPrevOn( int cur, int mode, menuAux *mAux )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		int          : cur    : i    : $B8=:_$N9`L\(B
 *		int          : mode   : i    : $B0\F0%b!<%I(B
 *		menuAux      : *mAux  : i  : $B8=:_$N%a%K%e!<%G!<%?(B
 *
 * [$BJV$jCM(B]
 *		$B0\F08e$N9`L\HV9f(B
 *
 * [$B;HMQ4X?t(B]
 *		$B$J$7(B
 * [$B5!G=(B]
 *		mode $B$,#0$N;~$O!"8=:_A*Br$5$l$F$$$k9`L\$h$j#1$DA0$N(B
 *		$BA*Br2DG=$J9`L\$rC5$9!#(B
 *		mode $B$,#1$N;~$O!"8=:_$N%a%K%e!<$G#1HV8e$m$NA*Br2DG=$J(B
 *		$B9`L\$rC5$9!#(B
 *		$BA*Br2DG=$J9`L\$H$O(Bitem $B$N(B mode $B$,(BON$B$N;~$G$"$k!#(B
 */
int menuPrevOn(cur, mode, mAux)
int	cur;
int	mode;
menuAux	*mAux;
{
	register	menuItem	*item ;
	int		last = -1 ;
	int		target = -1 ;
	int		i ;

	item = &( mAux->items[mAux->nitems-1] ) ;
	for( i = mAux->nitems - 1 ; i >= 0 ; i-- ) {
#ifdef	ATOK_DEBUG
		printf( "item->mode = (%s) %s\n", 
			item->itemStr ?  item->itemStr : "NULL", 
			item->mode == ON ? "ON" : "OFF" ) ;
#endif	/* ATOK_DEBUG */
		if ( item->mode == ON ) {
			if ( last == -1 ) {
				last = i ;
				if ( mode ) {
					/* printf( "LAST FOUND %d\n", last ); */
					return last ;
				}
			}
			if ( target == -1 && i < cur ) {
				target = i ;
			}
		}
		item-- ;
	}
	if ( target == -1 ) {
		target = last ;
	}

	return target ;
}

/* Page */
/*
 * [$B4X?tL>(B]
 *		menuNextOn( )
 * [$BI=Bj(B]
 *              $B%a%K%e!<9`L\$N0\F0(B
 * [$B8F=P7A<0(B]
 *	int	menuNextOn( int cur, int mode, menuAux *mAux )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		int          : cur    : i    : $B8=:_$N9`L\(B
 *		int          : mode   : i    : $B0\F0%b!<%I(B
 *		menuAux      : *mAux : i  : $B8=:_$N%a%K%e!<%G!<%?(B
 *
 * [$BJV$jCM(B]
 *		$B0\F08e$N9`L\HV9f(B
 *
 * [$B;HMQ4X?t(B]
 *		$B$J$7(B
 * [$B5!G=(B]
 *		mode $B$,#0$N;~$O!"8=:_A*Br$5$l$F$$$k9`L\$h$j#1$D8e$m$N(B
 *		$BA*Br2DG=$J9`L\$rC5$9!#(B
 *		mode $B$,#1$N;~$O!"8=:_$N%a%K%e!<$G@hF,$NA*Br2DG=$J(B
 *		$B9`L\$rC5$9!#(B
 *		$BA*Br2DG=$J9`L\$H$O(Bitem $B$N(B mode $B$,(BON$B$N;~$G$"$k!#(B
 */
int menuNextOn(cur, mode, mAux)
int	cur;
int	mode;
menuAux	*mAux;
{
	register menuItem	*item ;
	int	first = -1 ;
	int	target = -1 ;
	int	i ;

	item = mAux->items ;
	for( i = 0 ; i < mAux->nitems ; i++ ) {
		if ( item->mode == ON ) {
			if ( first == -1 ) {
				first = i ;
				if ( mode ) {
					return first ;
				}
			}
			if ( target == -1 && i > cur ) {
				target = i ;
			}
		}
		item++ ;
	}
	if ( target == -1 ) {
		target = first ;
	}

	return target ;
}

/* Page */
/*
 * [$B4X?tL>(B]
 *		menuKey( )
 * [$BI=Bj(B]
 *              $B%a%K%e!<FbM-8z%-!<$N%A%'%C%/(B
 * [$B8F=P7A<0(B]
 *		int	menuKey( menuAux *mAux, WORD key, int *cur )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		menuAux      : *mAux : i  : $B8=:_$N%a%K%e!<%G!<%?(B
 *		WORD         : key      : i  : $BF~NO$5$l$?%-!<(B
 *		int          : *cur    :   o    : $BA*Br$5$l$?9`L\(B
 *
 * [$BJV$jCM(B]
 *		0 : $B9`L\$OA*Br$5$l$F$$$J$$!#(B 
 *		$B$=$NB>(B: $BA*Br$5$l$?9`L\$N%"%I%l%9(B
 *
 * [$B;HMQ4X?t(B]
 *		$B$J$7(B
 * [$B5!G=(B]
 *		$B;XDj$5$l$?%-!<$,!"(B $B8=:_I=<($7$F$$$k%a%K%e!<$h$j!"(B
 *		$B9`L\$rA*Br$9$k$?$a$NM-8z%-!<$+%A%'%C%/$7!"(B
 *		$BM-8z%-!<N)$C$?>l9g$K$O!"9`L\$,A*Br$5$l$?J*$H$7!"(B
 *		$B$=$N9`L\HV9f$r@_Dj$7!"9`L\$N%"%I%l%9$rJV$9!#(B
 */
menuItem *menuKey(mAux, key, cur)
menuAux	*mAux;
WORD	key;
int	*cur;
{
	register menuItem	*item ;
	register int		i ;

	*cur = 0 ;
	item = mAux->items ;
	for ( i = 0 ; i < mAux->nitems ; i++ ) {
		/*
		printf( "Check Keys for '%s'\n",
			item->itemStr ?  item->itemStr : "NULL" ) ;
		*/
		if ( item->key == key ) {
		/* printf( "found\n" ) ; */
			*cur = i ;
			return item ;
		}
		item++ ;
	}
	/* printf( "not found\n" ) ; */
	return ( menuItem * )NULL ;
}

/* Page */
/*
 * [$B4X?tL>(B]
 *		MenuCtrl( )
 * [$BI=Bj(B]
 *              $B%a%K%e!<I=<(;~$N%$%Y%s%H%3%s%H%m!<%k(B
 * [$B8F=P7A<0(B]
 *		int	MenuCtrl( _XatokRecPtr acomm, menuAux **curMenu,
 *			WORD *aajcode, int op, int *length )
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		_XatokRecPtr : acomm   : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *		menuAux      : **curMenu : i/o : $B8=:_$N%a%K%e!<%G!<%?(B
 *		WORD         : *aajcode : i/o : $BF~NO$5$l$?(BAAJCODE
 *		int          : op       : i   : $B3+;O%U%!%s%/%7%g%s$N8F$S=P$7(B
 *		int          : *length  : o   : $BF~NOJ8;zNs$ND9$5(B
 *
 * [$BJV$jCM(B]
 *		0 : $B%a%K%e!<$NJQ99L5$7(B
 *		1 : $B%a%K%e!<=hM}%-%c%s%;%k(B
 *		2 : $B%a%K%e!<A*Br9`L\$N=hM}7hDj(B
 *
 * [$B;HMQ4X?t(B]
 *		menuAux->startProc( )
 *		menuAux->defaultProc( )
 *		menuAux->endProc( )
 *		menuNextOn( )
 *		menuPrevOn( )
 * [$B5!G=(B]
 *		AUX$B%&%#%s%I%&$K%a%K%e!<$,I=<($5$l$F$$$k;~$N(B
 *		$B%$%Y%s%H4IM}$r9T$J$&!#(B
 *		$B%$%Y%s%H$K$h$j!"I=<($7$F$$$k%a%K%e!<%G!<%?$N(B
 *		$B@ZBX$($d!"(BAUX$B%&%#%s%I%&$N=*N;$r9T$J$&!#(B
 */
int MenuCtrl(acomm, curMenu, aajcode, op, length)
_XatokRecPtr    acomm;
menuAux		**curMenu;
WORD		*aajcode;
int		op;
int		*length;
{

	menuItem	*target ;
	menuItem	sel ;
	int		newNbr ;
	int		changes =-1 ;
	int		cur ;

	*length = 0 ;
	if ( op ) {	/* Start Proc */
		if (( *curMenu  )->startProc ) {
			( *curMenu )->startProc( acomm ) ;
		}
		return ;
	}

	target = menuKey( *curMenu, *aajcode, &newNbr ) ;
	/*
	 * $B>e0L$G(BDEFAULT PROC$B$r<B9T$9$k!#(B
	 */
	if ( !target ) {
#ifdef	ATOK_DEBUG
		printf( "AUX DEFAULT FUNC\n" ) ;
#endif	/* ATOK_DEBUG */

		if (( *curMenu )->defaultProc ) {
			sel.op = 0 ;
			*length = ( *curMenu )->defaultProc( acomm, *curMenu, aajcode, &sel.op ) ;
			if ( sel.op ) {
				target = &sel ;
				goto menu_op ;
			}
#ifdef	ATOK_DEBUG
		printf( "\274\302\271\324\n"/*=$B<B9T(B\n=*/ ) ;
#endif	/* ATOK_DEBUG */
		}
		else {
#ifdef	ATOK_DEBUG
			printf( "\244\312\244\267\n"/*=$B$J$7(B\n=*/ ) ;
#endif	/* ATOK_DEBUG */
		}
		return -1 ;
	}
	/*
	 *  $BI=<($7$F$$$k%a%K%e!<$h$j!"A*Br$7$F$$$k$b$N$,JQ$o$C$?!#(B
	 */
menu_op :
	switch( target->op ) {
		case OP_NEXT :
			( *curMenu )->cur = menuNextOn(( *curMenu )->cur, 0, *curMenu ) ;
			changes = 0 ;
			break ;
		case OP_FIRST :
			( *curMenu )->cur = menuNextOn(( *curMenu )->cur, 1, *curMenu ) ;
			changes = 0 ;
			break ;
		case OP_PREV :
			( *curMenu )->cur = menuPrevOn(( *curMenu )->cur, 0, *curMenu ) ;
			changes = 0 ;
			break ;
		case OP_LAST :
			( *curMenu )->cur = menuPrevOn(( *curMenu )->cur, 1, *curMenu ) ;
			changes = 0 ;
			break ;
		case OP_SPEC :
			/*
			 * $B;XDj$7$?HV9f(B
			 */
			( *curMenu )->cur = newNbr ;
			changes = 0 ;
			break ;
		case OP_CANCEL :
			/*
			 * $B%-%c%s%;%k$,2!$5$l$?$N$G>e0L%a%K%e!<$X(B
			 */
			if (( *curMenu )->endProc ) {
				( *curMenu )->endProc( acomm ) ;
			}
			*curMenu = ( *curMenu )->parent ;
			if ( *curMenu ) {
				if (( *curMenu )->startProc ) {
					( *curMenu )->startProc( acomm ) ;
				}
			}
			changes = 1 ;
			break ;
		case OP_SELECT :
			/*
			 * $B7hDj%-!<$,2!$5$l$?$N$G=hM}<B9T(B
			 * $B2<0L%a%K%e!<$,$"$l$P!"%a%K%e!<JQ99(B
			 */
			cur = ( *curMenu )->cur ;
			if (( *curMenu )->items[cur].child ) {
				( *curMenu )->items[cur].child->parent = ( *curMenu ) ;
				*curMenu = ( *curMenu )->items[cur].child ;
				if (( *curMenu )->startProc ) {
					( *curMenu )->startProc( acomm, *aajcode ) ;
				}
			}
			changes = 2 ;
			break ;
		default :
			changes = 0 ;
			break ;
	}

	return changes ;
}


/* Page */
/*
 * [$B4X?tL>(B]
 *		auxMenuCopy( )
 * [$BI=Bj(B]
 *              $B%a%K%e!<%G!<%?$N%3%T!<(B
 * [$B8F=P7A<0(B]
 *		menuAux *auxMenuCopy( menuAux *orgAux )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		menuAux      : *orgAux : i   : $B%3%T!<85%a%K%e!<%G!<%?(B
 *
 * [$BJV$jCM(B]
 *		$B%3%T!<$7$?%a%K%e!<%G!<%?(B
 *
 * [$B;HMQ4X?t(B]
 *		auxMenuCopy( )
 * [$B5!G=(B]
 *		$B;XDj$5$l$?%a%K%e!<%G!<%?$r?75,%P%C%U%!$K%3%T!<$7$F(B
 *		$B$=$N%"%I%l%9$rJV$9!#(B
 */
menuAux	*auxMenuCopy(orgAux)
menuAux	*orgAux;
{
	menuItem	*item ;
	int		nitem ;
	int		i ;
	menuAux		*newAux ;

	newAux = ( menuAux * )malloc( sizeof( menuAux )) ;
	if ( !newAux )	return 0 ;

	memcpy(( char * )newAux, ( char * )orgAux, sizeof( menuAux )) ;

	item = ( menuItem * )malloc( sizeof( menuItem ) * orgAux->nitems ) ;
	newAux->items = item ;
	for( i = 0 ; i < orgAux->nitems ; i++ ) {
		memcpy(( char * )item, ( char * )&orgAux->items[i], 
			sizeof( menuItem )) ;
		if ( item->child ) {
			item->child = auxMenuCopy( item->child ) ;
		}
		item++ ;
	}
	return newAux ;
}

/* Page */
/*
 * [$B4X?tL>(B]
 *		auxMenuString( )
 * [$BI=Bj(B]
 *              $B%a%K%e!<I=<(MQJ8;zNs$N:n@.(B
 * [$B8F=P7A<0(B]
 *	int	auxMenuString( _XatokRecPtr  acomm, menuAux *mAux,
 *			wchar *echoStr, int *echoLen, int *revLen,
 *			int *revPos )
 *
 * [$B0z?t(B]
 *              $B7?(B           : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		_XatokRecPtr : acomm   : i/o : kinput2$BMQJQ49%P%C%U%!(B
 *		menuAux      : *mAux : i  : $B8=:_$N(B MENU CLASS
 *		wchar        : *echoStr:   o : $BI=<(J8;zNs3JG<NN0h(B
 *		int          : *echoLen :  o : $BI=<(J8;zNs$ND9$5(B
 *
 * [$BJV$jCM(B]
 *		$BI=<(J8;zNs$ND9$5(B
 *
 * [$B;HMQ4X?t(B]
 *		menuAux->stringProc( )
 *		euc2wcs( )
 * [$B5!G=(B]
 *		$B%a%K%e!<$KI=<($9$k$?$a$NJ8;zNs$r:n@.$9$k!#(B
 *		$B8=:_I=<($7$F$$$k%a%K%e!<$KJ8;zNs$r:n@.$9$k$?$a$N(B
 *		$B4X?t(B(menuAux->stirngProc)$B$,;XDj$5$l$F$$$k>l9g$K$O!"(B
 *		$B$=$N4X?t$r8F$S=P$7$F!"J8;zNs$r:n@.$9$k!#(B
 */
int auxMenuString(acomm, mAux, echoStr, echoLen, revLen, revPos)
_XatokRecPtr    acomm;
menuAux		*mAux;
wchar		*echoStr;
int		*echoLen;
int		*revLen;
int		*revPos;
{
	register menuItem	*item ;
	char	tbuf[BUFSIZ*2] ;
	int	len ;
	int	i, es ;
	char	*ep ;
	int		status ;

	item = mAux->items ;

	*revPos = 0 ;
	*revLen = 0 ;
	len = 0 ;
	bzero( tbuf, sizeof( tbuf )) ;

#ifdef	ATOK_DEBUG
printf( "AUX ADDR (STR) 0x%x\n", mAux ) ;
#endif	/* ATOK_DEBUG */
	if ( mAux->stringProc ) {
#ifdef	ATOK_DEBUG
	printf( "STRING PROC\n" ) ;
#endif	/* ATOK_DEBUG */
		len = ( mAux->stringProc )( acomm, mAux,
			echoStr, echoLen, revLen, revPos ) ;
		return len ;
	}
	ep = tbuf ;
	if ( mAux->str ) {
		strcpy( ep, mAux->str ) ;
		ep += strlen( ep ) ;
	}

#ifdef	ATOK_DEBUG
	printf( "auxMenuString START(%d)\n", mAux->nitems ) ;
#endif	/* ATOK_DEBUG */

	for( i = 0 ; i < mAux->nitems ; i++ ) {

#ifdef	ATOK_DEBUG
	printf( "ITEMS %s\n", item->itemStr ? item->itemStr : "NULL" ) ;
#endif	/* ATOK_DEBUG */
		if ( item->mode == ON ) {
			if ( mAux->cur == i ) {
				es = strlen( tbuf ) ;
				*revPos = euc2wcs( tbuf, es, echoStr )  ;
#ifdef	ATOK_DEBUG
			printf( "SET ECHO POS %d\n", *revPos ) ;
#endif	/* ATOK_DEBUG */

				len += *revPos ;
				es = strlen( item->numStr ) ;

				*revLen = euc2wcs( item->numStr, es, 
						&echoStr[*revPos] ) ;
				len += *revLen ;
#ifdef	ATOK_DEBUG
			printf( "SET ECHO LEN %d\n", *revLen ) ;
#endif	/* ATOK_DEBUG */
				ep = tbuf ;
				sprintf( ep, " %s", item->itemStr ) ;
				ep += strlen( ep ) ;

				REVERSE ;
				printf( "%s", item->numStr ) ;
				NORMAL ;
				printf( " %s", item->itemStr ) ;
			}
			else {
				sprintf( ep, "%s %s", item->numStr, item->itemStr ) ;
				ep += strlen( ep ) ;

#ifdef	ATOK_DEBUG
				printf( "%s", item->numStr ) ;
				printf( " %s", item->itemStr ) ;
#endif	/* ATOK_DEBUG */
			}
			sprintf( ep, "    " ) ;
			ep += strlen( ep ) ;
		}
		item++ ;
	}

	if (( es = strlen( tbuf ))) {
		es = euc2wcs( tbuf, es, &echoStr[*revPos + *revLen] ) ;
#ifdef	ATOK_DEBUG
		printf( "STR [%s] set +len = %d\n", tbuf, es ) ;
#endif	/* ATOK_DEBUG */
		len += es ;
	}
	*echoLen = len ;
#ifdef	ATOK_DEBUG
	printf( "len = %d, echoLen = %d\n", len, *echoLen ) ;
#endif	/* ATOK_DEBUG */
	return len ;
}

/*
 * $B$3$3$+$i2<$O(BDEBUG$BMQ(B
 */

menuPrint(mAux)
menuAux	*mAux;
{
	int	i ;
	register menuItem	*item ;

	item = mAux->items ;
	if ( mAux->str ) {
		printf( "%s", mAux->str ) ;
	}
	for( i = 0 ; i < mAux->nitems ; i++ ) {
		if ( item->mode == ON ) {
			if ( mAux->cur == i ) {
				REVERSE ;
				printf( "%s", item->numStr ) ;
				NORMAL ;
				printf( " %s", item->itemStr ) ;
			}
			else {
				printf( "%s", item->numStr ) ;
				printf( " %s", item->itemStr ) ;
			}
			printf( "    " ) ;
		}
		item++ ;
	}
	printf( "\n" ) ;
}

#ifdef	STAND
void
main (argc, argv)
int argc;
char **argv;
{
	
	char	buf[BUFSIZ] ;
	menuAux *topMenu ;
	menuAux *cur ;

	int	status ;
	int	key ;

	topMenu = auxTopMenu ;
	cur = topMenu ;

	while( 1 ) {
		menuPrint( cur ) ;
		printf( "enter command => " ) ;
		gets( buf ) ;
		key = buf[0] ;
		status = MenuCtrl( &cur, key ) ;
		if ( status == 1 ) {
			if ( !cur ) {
				printf( "Program terminated\n" ) ;
				exit( 0 ) ;
			}
		}
		else if ( status == 2 ) {
			printf( "go to Chiled\n" ) ;
		}
		else if ( status != 0 ) {
			switch( cur->mode ) {
				case M_AUX :
					printf( "TOP MENU FUNC\n" ) ;
					break ;
				case 	M_CODE	:
					printf( "CODE MENU FUNC\n" ) ;
					break ;
				case 	M_CODESJ	:
					printf( "SJ FUNC\n" ) ;
					break ;
				case 	M_CODEEUC	:
					printf( "EUC FUNC\n" ) ;
					break ;
				case 	M_CODEJIS	:
					printf( "JIS FUNC\n" ) ;
					break ;
				case 	M_CODEKUTEN	:
					printf( "KUTEN FUNC\n" ) ;
					break ;
				case 	M_KIGOU		:
					printf( "KIGOU FUNC\n" ) ;
					break ;
				default: 
					printf( "UNKNOWN\n" ) ;
					break ;
			}
		}
	}
}

#endif	STAND
