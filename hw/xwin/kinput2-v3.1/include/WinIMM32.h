/* $Id: Canna.h,v 1.2 1994/09/21 04:56:56 kon Exp $ */
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
 * Author: Akira Kon, NEC Corporation.  (kon@d1.bs2.mt.nec.co.jp)
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

#ifndef _WinIMM32_h
#define _WinIMM32_h

#include "InputConv.h"
#include <imm.h>

/*
  WinIMM32 new resources:

  name			class		type		default		access
  ----------------------------------------------------------------------------
  winimm32conf		winimm32conf	String		*1		CG

  note:	*1) 
*/

#define XtNwinimm32file "winIMM32conf"
#define XtCWinimm32file "WinIMM32Conf"
#define XtNwinimm32mode "winIMM32mode"
#define XtCWinimm32mode "WinIMM32Mode"

typedef struct _WinIMM32ClassRec	*WinIMM32ObjectClass;
typedef struct _WinIMM32Rec		*WinIMM32Object;

extern WidgetClass	winimm32ObjectClass;

#endif /* _WinIMM32_h */

