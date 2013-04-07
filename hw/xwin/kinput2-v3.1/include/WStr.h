/*
 *	WStr.h -- header file for Wide-Character String Library
 */

/*
 * Copyright (c) 1989  Software Research Associates, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Software Research
 * Associates makes no representations about the suitability of this software
 * for any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * Author:  Makoto Ishisone, Software Research Associates, Inc., Japan
 *		ishisone@sra.co.jp
 */

/* $Id: WStr.h,v 2.1 1991/09/23 05:32:48 ishisone Rel $ */

#ifndef _WStr_h
#define _WStr_h

#ifndef WCHAR_DEFINED
#define WCHAR_DEFINED
typedef unsigned short	wchar;
#endif

/* for backward compatibility... */
#define convWStoLatin1	convJWStoLatin1
#define convLatin1toWS	convLatin1toJWS
#define convWStoJIS	convJWStoJIS
#define convJIStoWS	convJIStoJWS
#define convWStoEUC	convJWStoEUC
#define convEUCtoWS	convEUCtoJWS
#define convWStoSJIS	convJWStoSJIS
#define convSJIStoWS	convSJIStoJWS

extern int convJWStoLatin1(wchar *, unsigned char *);
extern int convLatin1toJWS(unsigned char *, wchar *);
extern int convJWStoJIS(wchar *, unsigned char *);
extern int convJIStoJWS(unsigned char *, wchar *);
extern int convJWStoEUC(wchar *, unsigned char *);
extern int convEUCtoJWS(unsigned char *, wchar *);
extern int convJWStoSJIS(wchar *, unsigned char *);
extern int convSJIStoJWS(unsigned char *, wchar *);

#endif
