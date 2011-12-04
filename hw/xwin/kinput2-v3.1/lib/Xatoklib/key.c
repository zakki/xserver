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
static  char    rcsid[] = "$Id: key.c,v 1.3 1999/08/24 09:01:09 ishisone Exp $" ;
#endif  /* !lint */


#include	<X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Atoms.h>
#include	<X11/keysym.h>

#include	<Xatoklib.h>
#include	<key_def.h>

const WORD XkeyAAJtable[]={
/*      0,8   ,1,9   ,2,a   ,3,b   ,4,c   ,5,d   ,6,e   ,7,f */
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,    /* 00-07 */
        0x0108,0x0109,0x0000,0x0000,0x0000,0x010d,0x0000,0x0000,    /* 08-0f */
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,    /* 10-17 */
        0x0000,0x0000,0x0000,0x011b,0x0000,0x0000,0x0000,0x0000,    /* 18-1f */
	/*          !      "      #      $      %      &      ' */
	/*     XK_Kanji XK_Muhenkan XK_                         */
        0x0000,XFER,NFER,XFER,EIJI,HIRAGANA,KATAKANA,HIRAGANA,      /* 20-27 */
	/*   (      )      *      +      ,      -               */
          ZORH,  ZORH,  ZORH,0x0341,0x033a,0x0015,0x0000,0x0000,    /* 28-2f */
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,    /* 30-37 */
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,    /* 38-3f */
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,    /* 40-47 */
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,    /* 48-4f */
	/*   P      Q      R      S      T      U      V      W */
	/* XK_Home XK_Left XK_Up XK_Right XK_PageUP XK_PageDown XK_End */
          HOME,  LEFT,    UP, RIGHT,  DOWN,  PGUP,  PGDN,  EEND,    /* 50-57 */
	/*   X      Y      Z      [      \      ]      ^      _ */
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,    /* 58-5f */
	/*   `      a      b      c      d      e      f      g */
        0x0000,0x0000,  NFER,   INS,0x0000,0x0000,0x0000,0x0000,    /* 60-67 */
	/*   h      i      j      k      l      m      n      o */
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,    /* 68-6f */
	/*   p      q      r      s      t      u      v      w */
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,    /* 70-77 */
	/*   x      y      z      {      |      }      ~    del */
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,    /* 78-7f */
        0x0020,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,    /* 80-87 */
        0x0000,   TAB,0x0000,0x0000,0x0000,    CR,0x0000,0x0000,    /* 88-8f */
        0x0000,    F1,    F2,    F3,    F4,  HOME,  LEFT,    UP,    /* 90-97 */
         RIGHT,  DOWN,  PGUP,  PGDN,  EEND,0x0000,   INS,   DEL,    /* 98-9f */
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,    /* a0-a7 */
        0x0000,0x0000,0x022a,0x022b,0x0000,0x022d,0x0000,0x022f,    /* a8-af */
        0x0230,0x0231,0x0232,0x0233,0x0234,0x0235,0x0236,0x0237,    /* b0-b7 */
	/*                                         XK_F1  XK_F2 */
        0x0238,0x0239,0x0000,0x0000,0x0000,0x0000,    F1,    F2,    /* b8-bf */
	/* XK_F3 XK_F4 XK_F5  XK_F6  XK_F7  XK_F8  XK_F9 XK_F10 */
            F3,    F4,    F5,    F6,    F7,    F8,    F9,   F10,    /* c0-c7 */
	/* XK_F11 XK_F12 XK_F13 XK_F14 XK_F15 XK_F16 XK_F17 XK_F18 */
           F11,   F12,   F13,   F14,   F15,   F16,   F17,   F18,    /* c8-cf */
	/* XK_F19 XK_F20                                        */
           F19,   F20,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,    /* d0-d7 */
	/*                                        XK_F33        */
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,  HELP,0x0000,    /* d8-df */
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,    /* e0-e7 */
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,    /* e8-ef */
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,    /* f0-f7 */
	/*                                              XK_Delete */
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,   DEL     /* f8-ff */
};

/* Page */
/*
 * [$B4X?tL>(B]
 *		getAAJCode( )
 * [$BI=Bj(B]
 *              AAJCODE $B$N<hF@(B
 * [$B8F=P7A<0(B]
 *	WORD	getAAJCode( XKeyEvent *event, KeySym xkeysym,
 *			unsigned char *kbuf )
 *
 * [$B0z?t(B]
 *              $B7?(B            : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		XKeyEvent     : *event  : i   : X $B$N%$%Y%s%H%3!<%I(B
 *		KeySym        : xkeysym : i   : X$B$N%-!<%7%s%\%k(B
 *	        unsigned char : *kbuf   : i   : $BF~NO$5$l$?J8;z%3!<%I(B
 *
 * [$BJV$jCM(B]
 *		AAJCODE : $BJQ49$5$l$?(BAAJCODE
 *
 * [$B;HMQ4X?t(B]
 *		$B$J$7(B
 * [$B5!G=(B]
 *		X$B$N%-!<%$%Y%s%H$h$j!"(B AAJCODE$B$KJQ49$9$k!#(B
 *
 */
WORD getAAJCode(event, xkeysym, kbuf)
/* Fix Length */
XKeyEvent	*event;
KeySym		xkeysym;
unsigned char	*kbuf;
{

	WORD	aajcode = 0 ;
	unsigned char	ch ;

	ch = kbuf[0] ;

#ifdef	ATOK_DEBUG
printf( "KEY CODE CH = 0x%02x\n", ch ) ;
printf( "KEY SYM ORG 0x%x, FF00 4 = 0x%04x\n", xkeysym, xkeysym & 0xff00  ) ;
printf( "KEY SYM CODE 4 = 0x%04x\n", xkeysym  ) ;
printf( "SHIFT STAT %d\n",  event->state & ShiftMask )  ;
printf( "CTRL STAT %d\n",  event->state & ControlMask )  ;

#endif	/* ATOK_DEBUG */

	/*
 	 * X $B$N5!G=%-!<$@$C$?>l9g(B,AAJTABLE $B$KDj5A$7$F$"$k%3!<%I$KJQ49(B
	 */
	if (( xkeysym & 0xFF00 ) == 0xFF00 ) {
		int	index ;
		index = xkeysym & 0xff ;
#ifdef	ATOK_DEBUG
		printf( "FUNCTIONAL KEY INDEX = 0x%02x\n", index  ) ;
#endif	/* ATOK_DEBUG */
		aajcode = XkeyAAJtable[index] ;
		if ( !aajcode ) {
			return 0 ;
		}
	}
	/*
 	 * $BF~NO$5$l$?J8;z$H%-!<%3!<%I$,F1$8>l9g$K$O(B
	 * $B$=$N%3!<%I$r(BAAJ$B%3!<%I$H$9$k!#(B
 	 * $BF~NOJ8;z$,%3%s%H%m!<%k%3!<%I$N>l9g$OF~NOJ8;z$r(B
	 * AAJ$B%3!<%I$H$9$k!#(B
	 */
	else if ( ch == 0x8e ) {
		aajcode = ( WORD )kbuf[1] ;
	}
	else if ( ch == xkeysym & 0xff ) {
		aajcode = ( WORD )xkeysym ;
	}
	else if ( ch > 0 && ch < 0x20 ) {
		aajcode = ch ;
	}
/* 990614 */
	else if ( ch == 0 && xkeysym & 0xff ) {
		aajcode = xkeysym & 0xff ;
		if (( xkeysym & 0xff ) == '@' && ( event->state & ControlMask )) {
			aajcode = 0x1000 ;
			return aajcode ;
		}
	}
/* 990614 */
	else {
	/* printf( "unknonw key\n" ) ; */
		return 0 ;
	}

	if ( event->state & ShiftMask ) {
		aajcode |= SHIFT ;
	}
	if ( event->state & ControlMask ) {
		 aajcode |= CTRL ;
	}
	if( event->state & 0x08 ) { /* ALT Mask */
                 aajcode |= ALT ;
	}
	switch( aajcode ) {
		case 0x101b :	/* CTR + 3 */
		case 0x101c :	/* CTR + 4 */
		case 0x101d :	/* CTR + 5 */
		case 0x101e :	/* CTR + 6 */
		case 0x101f :	/* CTR + 7 */
			aajcode = 0x1033 + aajcode - 0x101b ;
			break ;
	}


	return aajcode ;
}
/* End of  key.c */




