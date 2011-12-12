/* $Id: CannaP.h,v 1.2 1994/09/21 04:57:02 kon Exp $ */
/*
 * Copyright (c) 1990  Software Research Associates, Inc.
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
 */

/* Copyright 1991 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of NEC Corporation
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  NEC 
 * Corporation makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 *
 * Author: Akira Kon, NEC Corporation. (kon@d1.bs2.mt.nec.co.jp)
 */
/* Copyright 2005 Media Lab. Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Media Lab. Inc.
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  
 * Media Lab. Inc. makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * Author: Yasuyuki ARAI
 */


#ifndef _WinIMM32P_h
#define _WinIMM32P_h

#include "InputConvP.h"

#include <imm.h>

#include "WinIMM32.h"

typedef struct {
    int foo;
} WinIMM32ClassPart;

typedef struct _WinIMM32ClassRec {
    ObjectClassPart object_class;
    InputConvClassPart inputConv_class;
    WinIMM32ClassPart winimm32_class;
} WinIMM32ClassRec;

typedef struct {
    /* resources */
    String	winimm32conf;
    Boolean	winimm32mode;
    /* private data */
    Display*	dpy;
    int		context;
} WinIMM32Part;

typedef struct _WinIMM32Rec {
    ObjectPart  object;
    InputConvPart inputConv;
    WinIMM32Part winimm32;
} WinIMM32Rec;

#endif	// #ifndef _WinIMM32P_h
