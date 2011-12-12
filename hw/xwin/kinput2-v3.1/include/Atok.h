/* $Id: Atok.h,v 1.1 1999/08/13 06:26:59 ishisone Exp $ */
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

#ifndef _Atok_h
#define _Atok_h

#include "InputConv.h"

/*
  Atok new resources:

  name			class		type		default		access
  ----------------------------------------------------------------------------
  server		Server		String		*1		CG
  conffile		Conffile	String		*2		CG

  note:	*1) if not specified, use value of an environment variable "ATOKHOST"
	*2) if not specified, use value of an environment variable "ATOKCONF"
*/

#define XtNAtokServer	"server"
#define XtCAtokServer	"Server"
#define XtNConfFile	"conf"
#define XtCConfFile	"Conf"
#define XtNStyleFile	"style"
#define XtCStyleFile	"StyleFile"
#define XtNPort		"port"
#define XtCPort		"Port"

typedef struct _AtokClassRec	*AtokObjectClass;
typedef struct _AtokRec		*AtokObject;

extern WidgetClass	atokObjectClass;

#endif /* _Atok_h */
