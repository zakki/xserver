/* $Id: AtokP.h,v 1.1 1999/08/13 06:26:59 ishisone Exp $ */
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

#ifndef _AtokP_h
#define _AtokP_h

#include "InputConvP.h"

#include "WStr.h"
#include "Atok.h"
#include "Xatoklib.h"

typedef struct {
    int foo;
} AtokClassPart;

typedef struct _AtokClassRec {
    ObjectClassPart 	object_class;
    InputConvClassPart	inputConv_class;
    AtokClassPart	atok_class;
} AtokClassRec;

#define	ATOK_NSEGMENTS	20 		/* $BJ8@a$N:GBg?t(B ???? */

#define	NBR_OF_PART	3
#if 0
typedef	struct	{
    /* $BI=<($O(B $BA4It(B($B:GBg(B)$B$G#3$D$N%Q!<%H$K$J$k!#(B */
    ICString	ics[NBR_OF_PART];
    wchar	*gline[NBR_OF_PART];
    int		glsize[NBR_OF_PART];
    int		gllen[NBR_OF_PART];
    /* $B%-!<F~NO$5$l$?%G!<%?$NJ]B8%P%C%U%!(B */
    wchar	echoStr[1024];
    int		echoLen;
    int		revPos;
    int		revLen;
} InputBuf;
#endif

typedef	struct	{
    wchar	name[64];
    int		length;
    int		size;
} AtokMode;

typedef struct {
    /* RESOURCES */
    String	atokserver;
    String	port;
    String	conffile;
    String	stylefile;

    /* private data */
    long	context;	/* ATOK Communication  $B$N(BContext */
    /*AtokMode	mode;*/
    Boolean	textchanged;	/* $BJQ49%F%-%9%H$,JQ$o$C$?$+(B */
    /*int	convStatus;	/* 0 : $BL5JQ49(B
				/* 1 : $BJQ49Cf(B
				 * 2 : $B8uJdI=<(Cf(B
				 * 3 : $B%3!<%IF~NOCf(B
				 * 4 : $B%f!<%F%#%j%F%#!<(B
				 */
    /* $BL$3NDjJ8;zNsI=<(MQ$NNN0h(B(for ki2) */
    ICString	ics[NBR_OF_PART];/* $BL$3NDjJ8;zNsI=<((B */

    /* $B%;%0%a%s%H$N4IM}(B */
    _XatokRecPtr comm;		/* ATOKCE $B$H$N%3%_%e%K%1!<%7%g%s(B */

    /* $B8uJd%G!<%?4X78$NDj5A(B */
    int		nbr_of_cand;	/* $B8uJdA*Br;~$N8uJd?t(B */
    ICString	*cand_lists_ics;/* $B8uJd(B */
    int		cur_cand;	/* $BA*Br$5$l$F$$$k9-Js(B */
    int		selected_cand;
    int		cand_list_size;	/* $B%"%m%1!<%7%g%s$5$l$F$$$k%5%$%:(B */

    /* AUX $B%&%#%s%I%&$N$?$a$N(BICS $B4IM}NN0h(B */
    /*wchar	aux_echoStr[BUFSIZ*2]; */
    /*int	aux_echoLen;*/
    /*int	aux_revLen;*/
    /*int	aux_revPos;*/
    ICString	aux_ics[NBR_OF_PART];
    wchar	*aux_string[NBR_OF_PART];
    int		aux_length[NBR_OF_PART];
    int		aux_size[NBR_OF_PART];
    int		aux_curseg;
    int		aux_nseg;
    /*wchar	**atok_cand;*/
} AtokPart;

typedef struct _AtokRec {
    ObjectPart  	object;
    InputConvPart	inputConv;
    AtokPart		atok;
} AtokRec;

#endif
