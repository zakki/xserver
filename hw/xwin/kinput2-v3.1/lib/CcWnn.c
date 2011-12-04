#ifndef lint
static char *rcsid = "$Id: CcWnn.c,v 1.59 2002/01/10 15:51:47 ishisone Exp $";
#endif
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

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Atoms.h>
#if XtSpecificationRelease > 4
#include <X11/Xfuncs.h>
#endif
#include "CcWnnP.h"
#include "CachedAtom.h"
#include "IOECall.h"

#define DEBUG_VAR debug_CcWnn
#include "DebugPrint.h"

static XtResource resources[] = {
#define offset(field) XtOffset(CcWnnObject, ccWnn.field)
    { XtNconfirmFunc, XtCFunction, XtRPointer, sizeof(int (*)()),
       offset(confirmfunc), XtRPointer, (XtPointer)NULL },
    { XtNconfirmData, XtCConfirmData, XtRPointer, sizeof(XtPointer),
       offset(confirmdata), XtRPointer, (XtPointer)NULL },
    { XtNjserver, XtCJserver, XtRString, sizeof(String),
	offset(jservername), XtRString, NULL },
    { XtNjserver2nd, XtCJserver, XtRString, sizeof(String),
	offset(jservername2), XtRString, NULL },
    { XtNwnnEnvname, XtCWnnEnvname, XtRString, sizeof(String),
	offset(wnnenvname), XtRString, NULL },
    { XtNwnnEnvrc, XtCWnnEnvrc, XtRString, sizeof(String),
	offset(wnnenvrcfile), XtRString, NULL },
    { XtNwnnEnvrc4, XtCWnnEnvrc, XtRString, sizeof(String),
	offset(wnnenvrcfile4), XtRString, NULL },
    { XtNwnnEnvrc6, XtCWnnEnvrc, XtRString, sizeof(String),
	offset(wnnenvrcfile6), XtRString, NULL },
    { XtNwnnOverrideEnv, XtCWnnOverrideEnv, XtRBoolean, sizeof(Boolean),
	offset(wnnoverrideenv), XtRString, "false" },
    { XtNccdef, XtCCcdef, XtRString, sizeof(String),
	offset(ccdeffile), XtRString, NULL },
    { XtNwnnEnv, XtCWnnEnv, XtRWnnEnv, sizeof(struct wnn_buf *),
	offset(wnnbuf), XtRWnnEnv, NULL},
    { XtNccRule, XtCCcRule, XtRCcRule, sizeof(ccRule),
	offset(ccrule), XtRCcRule, NULL},
    { XtNsaveInterval, XtCSaveInterval, XtRInt, sizeof(int),
	offset(saveinterval), XtRImmediate, 0 },
#undef offset
};

static void ClassInitialize();
static int buildSymbolList();
static void Initialize(), Destroy();
static Boolean SetValues();
static int InputEvent();
static ICString *GetMode();
static int CursorPos();
static int NumSegments();
static ICString *GetSegment();
static int CompareSegment();
static ICString *GetItemList();
static int SelectItem();
static int ConvertedString();
static int ClearConversion();
static ICString *GetAuxSegments();
static int PreeditString();
static int StatusString();

CcWnnClassRec ccWnnClassRec = {
  { /* object fields */
    /* superclass		*/	(WidgetClass) &inputConvClassRec,
    /* class_name		*/	"CcWnn",
    /* widget_size		*/	sizeof(CcWnnRec),
    /* class_initialize		*/	ClassInitialize,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* obj1			*/	NULL,
    /* obj2			*/	NULL,
    /* obj3			*/	0,
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* obj4			*/	FALSE,
    /* obj5			*/	FALSE,
    /* obj6			*/	FALSE,
    /* obj7			*/	FALSE,
    /* destroy			*/	Destroy,
    /* obj8			*/	NULL,
    /* obj9			*/	NULL,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* obj10			*/	NULL,
    /* get_values_hook		*/	NULL,
    /* obj11			*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* obj12			*/	NULL,
    /* obj13			*/	NULL,
    /* obj14			*/	NULL,
    /* extension		*/	NULL
  },
  { /* inputConv fields */
    /* InputEvent		*/	InputEvent,
    /* GetMode			*/	GetMode,
    /* CursorPos		*/	CursorPos,
    /* NumSegments		*/	NumSegments,
    /* GetSegment		*/	GetSegment,
    /* CompareSegment		*/	CompareSegment,
    /* GetItemList		*/	GetItemList,
    /* SelectItem		*/	SelectItem,
    /* GetConvetedString	*/	ConvertedString,
    /* ClearConversion		*/	ClearConversion,
    /* GetAuxSegments		*/	GetAuxSegments,
    /* SupportMultipleObjects	*/	True,
    /* GetTriggerKeys		*/	XtInheritGetTriggerKeys,
    /* num_trigger_keys		*/	0,
    /* trigger_keys		*/	NULL,
    /* GetPreeditString		*/	PreeditString,
    /* GetStatusString		*/	StatusString,
    /* NoMoreObjects		*/	False,
  },
  { /* ccWnn fields */
    /* foo			*/	0,
  }
};

WidgetClass ccWnnObjectClass = (WidgetClass)&ccWnnClassRec;

/* cconv function table */
static char *fepfunctbl[] = {
	"convert",
	"convert-or-fix1",
	"convert-or-space",
 	"convert-or-sendback",
	"convert-s",
	"unconvert",
	"next",
	"next-s",
	"previous",
	"previous-s",
	"forward",
	"backward",
	"move-top",
	"move-bottom",
	"clear",
	"expand",
	"expand-s",
	"shrink",
	"shrink-s",
	"expand-noconv",
	"expand-noconv-s",
	"shrink-noconv",
	"shrink-noconv-s",
	"fix",
	"fix2",
	"fix-or-cr",
 	"fix-or-sendback",
	"to-hankaku",
	"to-zenkaku",
	"to-hiragana",
	"to-katakana",
	"backspace",
	"delete",
	"kill-line",
	"carriage-return",
	"fix-and-write",
	"beep",
	"jiscode-begin",
	"jiscode-end",
	"kutencode-begin",
	"kutencode-end",
	"symbol-input",
	"end-conversion",
	"send-back",
 	"convert-move-top-or-sendback",
	"convert-move-top-or-space",
	"clear-or-cancel",
	"backspace-or-cancel",
	"delete-or-cancel",
 	"convert-next-or-move-top-or-sendback",
	"convert-next-or-move-top-or-space",
	"select",
	"select-s",
	"register",
};
#define FTSIZE	(sizeof(fepfunctbl) / sizeof(char *))

static void	convert();
static void	convert_f1();
static void	convert_sb();
static void	convert_sp();
static void	convert_s();
static void	convert_mt_sb();
static void	convert_mt_sp();
static void	unconvert();
static void	egg_select();
static void	egg_select_s();

static void	move_forward();
static void	move_backward();
static void	move_top();
static void	move_bottom();

static void	cand_next();
static void	cand_next_s();
static void	cand_next_mt_sb();
static void	cand_next_mt_sp();
static void	cand_prev();
static void	cand_prev_s();

static void	expand_cl();
static void	expand_cl_s();
static void	shrink_cl();
static void	shrink_cl_s();
static void	expand_cl2();
static void	expand_cl2_s();
static void	shrink_cl2();
static void	shrink_cl2_s();

static void	clear_buffer();
static void	clear_c();

static void	sel_top();
static void	sel_bottom();
static void	sel_forward();
static void	sel_backward();
static void	sel_next();
static void	sel_prev();
static void	sel_select();
static void	sel_abort();

static void	fix();
static void	fix1();
static void	fix_cr();
static void	fix_sb();

static void	hankaku();
static void	zenkaku();

static void	hiragana();
static void	katakana();

static void	backspace();
static void	backspace_c();
static void	delete();
static void	delete_c();
static void	kill_line();

static void	bell();
static void	beep();
static void	carriageret();
static void	jiscode_begin();
static void	jiscode_end();
static void	kuten_begin();
static void	kuten_end();

static void	sym_input();
static void	convend();
static void	send_back();
static void	register_word();

/* cconv function dispatch table */
static void (*functable[][3])() = {
/* Function Name	Normal-mode	selection-mode	symbol-mode */
/* convert */		convert,	sel_forward,    beep,
/* convert-or-fix1 */	convert_f1,	sel_forward,    beep,
/* convert-or-sendback*/convert_sb,	sel_forward,    beep,
/* convert-or-space */	convert_sp,	sel_forward,    beep,
/* convert-s */		convert_s,	sel_forward,    beep,
/* unconvert */		unconvert,	beep,           beep,
/* next */		cand_next,	sel_next,       sel_next,
/* next-s */		cand_next_s,	sel_next,       sel_next,
/* previous */		cand_prev,	sel_prev,       sel_prev,
/* previous-s */	cand_prev_s,	sel_prev,       sel_prev,
/* forward */		move_forward,	sel_forward,    sel_forward,
/* backward */		move_backward,	sel_backward,   sel_backward,
/* move-top */		move_top,	sel_top,        sel_top,
/* move-bottom */	move_bottom,	sel_bottom,     sel_bottom,
/* clear */		clear_buffer,	clear_buffer,   clear_buffer,
/* expand */		expand_cl,	expand_cl,  	beep,
/* expand-s */		expand_cl_s,	expand_cl_s,	beep,
/* shrink */		shrink_cl,	shrink_cl,  	beep,
/* shrink-s */		shrink_cl_s,	shrink_cl_s,	beep,
/* expand-noconv */	expand_cl2,	expand_cl2, 	beep,
/* expand-noconv-s */	expand_cl2_s,	expand_cl2_s,	beep,
/* shrink-noconv */	shrink_cl2,	shrink_cl2, 	beep,
/* shrink-noconv-s */	shrink_cl2_s,	shrink_cl2_s,	beep,
/* fix */		fix,		fix,            fix,
/* fix2 */		fix,		fix,            fix,
/* fix-or-cr */		fix_cr,		sel_select,     sel_select,
/* fix-or-sendback */	fix_sb,		sel_select,     sel_select,
/* to-hankaku */	hankaku,	hankaku,        beep,
/* to-zenkaku */	zenkaku,	zenkaku,        beep,
/* to-hiragana */	hiragana,	hiragana,       beep,
/* to-katakana */	katakana,	katakana,       beep,
/* backspace */		backspace,	backspace,      backspace,
/* delete */		delete,		delete,         delete,
/* kill-line */		kill_line,	kill_line,	kill_line,
/* carriage-return */	carriageret,	sel_select,     sel_select,
/* fix-and-write */	fix,		beep,           beep,
/* beep */		bell,		bell,           bell,
/* jiscode-begin */	jiscode_begin,	beep,		beep,
/* jiscode-end */	jiscode_end,	beep,		beep,
/* kutencode-begin */	kuten_begin,	beep,		beep,
/* kutencode-end */	kuten_end,	beep,		beep,
/* symbol-input */	sym_input,	beep,           sel_abort,
/* end-conversion */	convend,	convend,	convend,
/* send-back */		send_back,	send_back,	send_back,
/* convert-move-..sb*/	convert_mt_sb,	sel_forward,	beep,
/* convert-move-..sp*/	convert_mt_sp,	sel_forward,	beep,
/* clear-or-cancel */	clear_c,	clear_c,	clear_c,
/* backspace-or-cancel */ backspace_c,	backspace_c,	backspace_c,
/* delete-or-cancel */	delete_c,	delete_c,	delete_c,
/* convert-next-..sb */	cand_next_mt_sb, sel_forward,	beep,
/* convert-next-..sp */	cand_next_mt_sp, sel_forward,	beep,
/* select */		egg_select,	beep,		beep,
/* select-s */		egg_select_s,	beep,		beep,
/* register */		register_word,	beep,		beep,
};

static ICString *SymbolList;
static int NumSymbols;

static void ccInitialize();
static void jcInitialize();
static void createEnvError();
static int createConfirm();

static int funcDispatch();
static void defAction();
static void insChar();
static void delChar();
static void autoFix();

static void startSelection();
static void moveSelection();
static int endSelection();
static int insertSelection();

static int getSymbol();

static void normalState();

static void allocCandlist();
static void allocStrdata();
static void getAllCandidates();

static void addObject();
static void deleteObject();
static void serverDead();

static void saveData();
static void restoreData();

static void ioeCallback();

static CcWnnObject findSelectionObj();

static Boolean convertSelection();
static void saveYomiAndKanji();

static void
ClassInitialize()
{
    /* symbollist $B$r@_Dj(B */
    NumSymbols = buildSymbolList(&SymbolList);
    /* I/O error $B%3!<%k%P%C%/4X?t$N@_Dj(B */
    XIOESet(ioeCallback, (XPointer)NULL);
}

static int
buildSymbolList(listp)
ICString **listp;
{
    static struct symgroup {
	int	first;
	int last;
    } symgroups[] = {
	{ 0xa1a1, 0xa2ae },	/* '$B!!(B' - '$B".(B' */
	{ 0xa2ba, 0xa2c1 },	/* '$B":(B' - '$B"A(B' */
	{ 0xa2ca, 0xa2d0 },	/* '$B"J(B' - '$B"P(B' */
	{ 0xa2dc, 0xa2ea },	/* '$B"\(B' - '$B"j(B' */
	{ 0xa2f2, 0xa2f9 },	/* '$B"r(B' - '$B"y(B' */
	{ 0xa2fe, 0xa2fe },	/* '$B"~(B' */
	{ 0xa4ee, 0xa4ee },	/* '$B$n(B' */
	{ 0xa4f0, 0xa4f1 },	/* '$B$p(B', '$B$q(B' */
	{ 0xa5ee, 0xa5ee },	/* '$B%n(B' */
	{ 0xa5f0, 0xa5f1 },	/* '$B%p(B', '$B%q(B' */
	{ 0xa5f4, 0xa5f6 },	/* '$B%t(B', '$B%u(B', '$B%v(B' */
	{ 0xa6a1, 0xa6b8 },	/* '$B&!(B' - '$B&8(B' */
	{ 0xa6c1, 0xa6d8 },	/* '$B&A(B' - '$B&X(B' */
	{ 0xa7a1, 0xa7c1 },	/* '$B'!(B' - '$B'A(B' */
	{ 0xa7d1, 0xa7f1 },	/* '$B'Q(B' - '$B'q(B' */
	{ 0xa8a1, 0xa8c0 },	/* '$B(!(B' - '$B(@(B' */
	{ -1, -1 }
    };
    struct symgroup *sgp;
    Cardinal nsyms;
    ICString *symlist, *sp;
    wchar *buf, *p;

    for (nsyms = 0, sgp = symgroups; sgp->first > 0; sgp++) {
#define LINEAR_INDEX(c)	(((((c)>>8)&0x7f)*94)+((c)&0x7f))
	nsyms += LINEAR_INDEX(sgp->last) - LINEAR_INDEX(sgp->first) + 1;
    }

    symlist = (ICString *)XtMalloc(nsyms * sizeof(ICString));
    buf = (wchar *)XtMalloc(nsyms * sizeof(wchar));

    sp = symlist;
    p = buf;
    for (sgp = symgroups; sgp->first > 0; sgp++) {
	int i;
#define NEXT_CHAR(c) ((((c)&0xff)>0xfd)?(((c)&0xff00)+0x1a1):((c)+1))
	for (i = sgp->first; i <= sgp->last; i = NEXT_CHAR(i)) {
	    sp->nchars = 1;
	    sp->nbytes = sizeof(wchar);
	    sp->data = (char *)p;
	    sp->attr = ICAttrNormalString;
	    sp++;
	    *p++ = i;
	}
    }

    *listp = symlist;
    return nsyms;
}

static int
InputEvent(w, event)
Widget w;
XEvent *event;
{
    CcWnnObject obj = (CcWnnObject)w;
    int sendback;
    int ret = 0;
    wchar *curmode;

    if (event->type != KeyPress /*&& event->type != KeyRelease*/) return 0;

    /* $B%$%Y%s%H$r%/%i%$%"%s%H$KAw$jJV$9$+$I$&$+$NH=Dj$=$N(B 1 */
    sendback = (NumSegments(w) == 0 && obj->ccWnn.state == normal_state);

    obj->ccWnn.sendbackevent = False;
    obj->ccWnn.fixperformed = False;
    obj->ccWnn.textchanged = False;

    /*
     * $B$b$7!"(Bwnnbuf $B$,L$3MF@$G$"$k$+(B ($B$3$s$J$3$H$O!"$"$j$($J$$$H;W$&$,(B)$B!"(B
     * jserver $B$,;`$s$@$J$I$NM}M3$G(B jserver $B$H$N@\B3$,@Z$l$F$$$k$J$i$P!":F(B
     * $B=i4|2=!J:F@\B3!K$r9T$J$&!#(B
     */
    if (obj->ccWnn.wnnbuf == NULL || !jcIsConnect(obj->ccWnn.wnnbuf)) {
	jcInitialize(obj);
	/* $B:F@\B3$r;n$_$F$b!"$^$@@\B3$G$-$F$$$J$$$H$-$O!"%(%i!<$rJV$9(B */
	if (obj->ccWnn.wnnbuf == NULL || !jcIsConnect(obj->ccWnn.wnnbuf)) {
	    bell(obj);
	    return -1;
	}
    }

    wnn_errorno = 0;
    curmode = ccGetModePrompt(obj->ccWnn.ccbuf);

    (void)ccConvchar(obj->ccWnn.ccbuf, (XKeyPressedEvent *)event);

    /*
     * $B%(%i!<HV9f$r%A%'%C%/$7!"(Bjserver $B$,;`$s$G$$$l$P4D6-$r(B destroy $B$7(B
     * $B:F$S@\B3$r;n$_$k(B
     */
    if (wnn_errorno == WNN_JSERVER_DEAD) {
	XtAppWarning(XtWidgetToApplicationContext((Widget)w),
		     "ccWnn Object: jserver died");
	/* $B$b$7$bF~NOCf$NJ8;zNs$,$"$l$P$H$C$F$*$/(B */
	saveData(obj);
	serverDead();
	/* secondary jserver $B$,;XDj$5$l$F$$$l$P:F@\B3$r;n$_$k(B */
	if (obj->ccWnn.jservername2 != NULL) jcInitialize(obj);
	if (obj->ccWnn.wnnbuf == NULL || !jcIsConnect(obj->ccWnn.wnnbuf)) {
	    bell(obj);
	    ret = -1;
	}
    }

    /* $B%F%-%9%H$NJQ2=$r%A%'%C%/$9$k(B */
    if (obj->ccWnn.textchanged) {
	XtCallCallbackList(w, obj->inputConv.textchangecallback,
			   (XtPointer)NULL);
	obj->ccWnn.textchanged = False;
    }
    /* $BF~NO%b!<%I$r%A%'%C%/$9$k(B */
    if (wstrcmp(ccGetModePrompt(obj->ccWnn.ccbuf), curmode)) {
	sendback = 0;
	XtCallCallbackList(w, obj->inputConv.modechangecallback,
			   (XtPointer)NULL);
    }
    /* $B%$%Y%s%H$r%/%i%$%"%s%H$KAw$jJV$9$+$I$&$+$NH=Dj$=$N(B 2 */
    if (NumSegments(w) != 0 ||
	obj->ccWnn.state != normal_state ||
	obj->ccWnn.fixperformed) {
	sendback = 0;
    }
    if (ret == 0 && (obj->ccWnn.sendbackevent || sendback)) ret = 1;

    return ret;
}

static ICString *
GetMode(w)
Widget w;
{
    CcWnnObject obj = (CcWnnObject)w;
    wchar *mode;
    static ICString icstr;

    mode = ccGetModePrompt(obj->ccWnn.ccbuf);
    icstr.data = (char *)mode;
    icstr.nchars = wstrlen(mode);
    icstr.nbytes = icstr.nchars * sizeof(wchar);
    icstr.attr = ICAttrNormalString;

    return &icstr;
}

static int
CursorPos(w, nsegp, ncharp)
Widget w;
Cardinal *nsegp;
Cardinal *ncharp;
{
    CcWnnObject obj = (CcWnnObject)w;
    jcConvBuf *jcbuf = obj->ccWnn.jcbuf;
    Cardinal nseg, nchar;

    if (jcbuf == NULL || jcIsConverted(jcbuf, jcbuf->curClause)) return 0;

    nseg = jcbuf->curClause;
    nchar = jcDotOffset(jcbuf);

    if (nseg >= jcbuf->nClause) {
	if (nseg == 0) {
	    nchar = 0;
	} else {
	    jcClause *cinfo = jcbuf->clauseInfo;
	    nseg--;
	    nchar = cinfo[nseg + 1].dispp - cinfo[nseg].dispp;
	}
    }

    if (nsegp) *nsegp = nseg;
    if (ncharp) *ncharp = nchar;

    return 1;
}

static int
NumSegments(w)
Widget w;
{
    CcWnnObject obj = (CcWnnObject)w;

    return (obj->ccWnn.jcbuf != NULL) ? obj->ccWnn.jcbuf->nClause : 0;
}

static ICString *
GetSegment(w, n)
Widget w;
Cardinal n;
{
    CcWnnObject obj = (CcWnnObject)w;
    jcConvBuf *jcbuf = obj->ccWnn.jcbuf;
    jcClause *cinfo = jcbuf->clauseInfo;
    static ICString seg;

    if (jcbuf == NULL || n >= jcbuf->nClause) return NULL;
    seg.data = (char *)cinfo[n].dispp;
    seg.nchars = cinfo[n + 1].dispp - cinfo[n].dispp;
    seg.nbytes = seg.nchars * sizeof(wchar);
    seg.attr = cinfo[n].conv ? ICAttrConverted : ICAttrNotConverted;
    if (n == jcbuf->curClause) {
	seg.attr |= ICAttrCurrentSegment;
    } else if (jcbuf->curLCStart <= n && n < jcbuf->curLCEnd) {
	seg.attr |= ICAttrCurrentSubSegment;
    }
    return &seg;
}

/* ARGSUSED */
static int
CompareSegment(w, seg1, seg2, n)
Widget w;
ICString *seg1;
ICString *seg2;
Cardinal *n;
{
    wchar *p, *q;
    int len, nsame;
    int result = 0;

    if (seg1->attr != seg2->attr) result |= ICAttrChanged;

    len = seg1->nchars > seg2->nchars ? seg2->nchars : seg1->nchars;
    nsame = 0;
    p = (wchar *)seg1->data;
    q = (wchar *)seg2->data;
    while (nsame < len && *p++ == *q++) nsame++;

    if (nsame != len || len != seg1->nchars || len != seg2->nchars)
	result |= ICStringChanged;

    if (n) *n = nsame;

    return result;
}

static ICString *
GetItemList(w, n)
Widget w;
Cardinal *n;
{
    CcWnnObject obj = (CcWnnObject)w;

    switch (obj->ccWnn.state) {
    case selection_l_state:
    case selection_s_state:
	*n = obj->ccWnn.numcand;
	return obj->ccWnn.candlist;
    case symbol_state:
	*n = obj->ccWnn.numsymbols;
	return obj->ccWnn.symbollist;
    default:
	*n = 0;
	return NULL;	/* no item available */
    }
    /* NOTREACHED */
}

static int
SelectItem(w, n)
Widget w;
int n;
{
    CcWnnObject obj = (CcWnnObject)w;
    int ret = 0;

    if (obj->ccWnn.state == normal_state) return -1;

    if (obj->ccWnn.jcbuf == NULL) {
	ret = -1;
    } else if (n >= 0) {
	ret = insertSelection(obj, n);
	if (obj->ccWnn.textchanged) {
	    XtCallCallbackList((Widget)obj,
			       obj->inputConv.textchangecallback,
			       (XtPointer)NULL);
	    obj->ccWnn.textchanged = False;
	}
    }

    obj->ccWnn.state = normal_state;
    return ret;
}

static int
ConvertedString(w, encoding, format, length, string)
Widget w;
Atom *encoding;
int *format;
int *length;
XtPointer *string;
{
    CcWnnObject obj = (CcWnnObject)w;
    jcConvBuf *jcbuf = obj->ccWnn.jcbuf;
    wchar *wbuf, *wp;
    int len, wlen;
    extern int convJWStoCT();

    if (jcbuf == NULL) return -1;

    wlen = jcbuf->displayEnd - jcbuf->displayBuf;
    if (wlen == 0) return -1;

    /*
     * jcbuf $B$KF~$C$F$$$kJQ49%F%-%9%H$O(B null $B%?!<%_%M!<%H$5$l$F$$$J$$$N$G(B
     * $B$^$:%3%T!<$7$F(B null $B%?!<%_%M!<%H$9$k(B
     */
    wbuf = (wchar *)XtMalloc((wlen + 1) * sizeof(wchar));
    (void)bcopy((char *)jcbuf->displayBuf, (char *)wbuf,
		sizeof(wchar) * wlen);
    wbuf[wlen] = 0;

    /*
     * CcWnn $B%*%V%8%'%/%H$O(B COMPOUND_TEXT $B%(%s%3!<%G%#%s%0$7$+%5%]!<%H$7$J$$(B
     * COMPOUND_TEXT $B$KJQ49$9$k(B
     */
    *encoding = XA_COMPOUND_TEXT(XtDisplayOfObject((Widget)obj));
    *format = 8;

    /* COMPOUND_TEXT $B$O(B \r $B$,Aw$l$J$$$N$G(B \n $B$KJQ49$7$F$*$/(B */
    for (wp = wbuf; *wp != 0; wp++) {
	if (*wp == '\r') *wp = '\n';
    }

    *length = len = convJWStoCT(wbuf, (unsigned char *)NULL, 0);
    *string = XtMalloc(len + 1);
    (void)convJWStoCT(wbuf, (unsigned char *)*string, 0);

    /* wbuf $B$r(B free $B$7$F$*$/(B */
    XtFree((char *)wbuf);

    return 0;
}

static int
ClearConversion(w)
Widget w;
{
    CcWnnObject obj = (CcWnnObject)w;

    if (obj->ccWnn.jcbuf == NULL) {
	return 0;	/* not -1, because it's already cleared */
    }
    clear_buffer(obj);
    XtCallCallbackList(w, obj->inputConv.textchangecallback, (XtPointer)NULL);
    return 0;
}

/* ARGSUSED */
static ICString *
GetAuxSegments(w, n, ns, nc)
Widget w;
Cardinal *n, *ns, *nc;
{
    /* CcWnn doesn't use AuxPanel */
    XtAppWarning(XtWidgetToApplicationContext(w),
		 "ccWnn Object: GetAuxSegments shouldn't be called");
    return NULL;
}

/* ARGSUSED */
static void
Initialize(req, new, args, num_args)
Widget req;
Widget new;
ArgList args;
Cardinal *num_args;
{
    CcWnnObject obj = (CcWnnObject)new;

    obj->ccWnn.ccrule = NULL;
    obj->ccWnn.jcbuf = NULL;
    obj->ccWnn.state = normal_state;
    obj->ccWnn.selectionending = False;
    obj->ccWnn.textchanged = False;
    obj->ccWnn.symbollist = SymbolList;
    obj->ccWnn.numsymbols = NumSymbols;
    obj->ccWnn.cursymbol = 0;
    obj->ccWnn.candlist = NULL;
    obj->ccWnn.candlistsize = 0;
    obj->ccWnn.numcand = 0;
    obj->ccWnn.strdata = NULL;
    obj->ccWnn.strdatasize = 0;
    obj->ccWnn.inputmode = OTHERS;
    obj->ccWnn.pendingdata = NULL;
    obj->ccWnn.fixcount = 0;
    obj->ccWnn.selwidget = NULL;
    obj->ccWnn.selyomi = obj->ccWnn.selkanji = NULL;

    /* $BJQ49$N=i4|2=(B */
    obj->ccWnn.createrule = False;
    obj->ccWnn.createenv = False;
    ccInitialize(obj);
    jcInitialize(obj);

    addObject(obj);
}

static void
ccInitialize(obj)
CcWnnObject obj;
{
    extern char *getenv();

    if (obj->ccWnn.createrule) {
	ccDestroyBuf(obj->ccWnn.ccbuf);
	obj->ccWnn.ccbuf = NULL;
    }
    if (obj->ccWnn.ccrule == NULL) {
	if (obj->ccWnn.ccdeffile == NULL) {
	    obj->ccWnn.ccdeffile = getenv("CC_DEF");
	    if (obj->ccWnn.ccdeffile == NULL) {
		obj->ccWnn.ccdeffile = DEF_CCDEF_FILE;
	    }
	}
	obj->ccWnn.ccrule = ccParseRule(obj->ccWnn.ccdeffile, XtWarning);
	obj->ccWnn.createrule = True;
    }

    if (obj->ccWnn.ccrule == NULL) {
	XtAppError(XtWidgetToApplicationContext((Widget)obj),
		   "CcWnn Object: cconv initialization failed.");
    }

    obj->ccWnn.ccbuf = ccCreateBuf(obj->ccWnn.ccrule, 16,
				   fepfunctbl, FTSIZE,
				   defAction, insChar, delChar,
				   funcDispatch, autoFix, NULL, (caddr_t)obj);
}

static CcWnnObject current_obj = NULL;

static void
jcInitialize(obj)
CcWnnObject obj;
{
    if (obj->ccWnn.createenv) {
	(void)jcDestroyBuffer(obj->ccWnn.jcbuf, 0);
	obj->ccWnn.jcbuf = NULL;
	obj->ccWnn.createenv = False;
    }

    if (obj->ccWnn.wnnbuf == NULL || !jcIsConnect(obj->ccWnn.wnnbuf)) {
	if (obj->ccWnn.wnnbuf != NULL) {
	    jcClose(obj->ccWnn.wnnbuf);
	    obj->ccWnn.wnnbuf = NULL;
	}
	if (obj->ccWnn.wnnenvname == NULL) obj->ccWnn.wnnenvname = "";
	if (obj->ccWnn.wnnenvrcfile == NULL) obj->ccWnn.wnnenvrcfile = "";
	if (obj->ccWnn.wnnenvrcfile4 == NULL) {
	    obj->ccWnn.wnnenvrcfile4 = obj->ccWnn.wnnenvrcfile;
	}
	if (obj->ccWnn.wnnenvrcfile6 == NULL) {
	    obj->ccWnn.wnnenvrcfile6 = obj->ccWnn.wnnenvrcfile;
	}

	/*
	 * jllib $B$N%3!<%k%P%C%/$G$O!"%/%i%$%"%s%H!&%G!<%?$rEO$;$J$$$N(B
	 * $B$G!"3P$($F$*$/!#(B
	 */
	current_obj = obj;
	obj->ccWnn.wnnbuf = jcOpen2(obj->ccWnn.jservername,
				    obj->ccWnn.wnnenvname,
				    obj->ccWnn.wnnoverrideenv,
				    obj->ccWnn.wnnenvrcfile4,
				    obj->ccWnn.wnnenvrcfile6,
				    createEnvError,
				    createConfirm,
				    30);
	/* wnnbuf $B$,3MF@$G$-$J$1$l$P!"$=$N$^$^%j%?!<%s(B */
	if (obj->ccWnn.wnnbuf == NULL) {
	    XtAppWarning(XtWidgetToApplicationContext((Widget)obj),
			 "ccWnn Object: can't open jserver");
	    return;
	}

	/* jserver $B$H@\B3$5$l$F$$$J$1$l$P!"%;%+%s%@%j$r;n$7$F$_$k(B */
	if (!jcIsConnect(obj->ccWnn.wnnbuf)
		&& obj->ccWnn.jservername2 != NULL) {
	    jcClose(obj->ccWnn.wnnbuf);
	    current_obj = obj;
	    obj->ccWnn.wnnbuf = jcOpen2(obj->ccWnn.jservername2,
					obj->ccWnn.wnnenvname,
					obj->ccWnn.wnnoverrideenv,
					obj->ccWnn.wnnenvrcfile4,
					obj->ccWnn.wnnenvrcfile6,
					createEnvError,
					createConfirm,
					30);
	    if (obj->ccWnn.wnnbuf == NULL) {
		XtAppWarning(XtWidgetToApplicationContext((Widget)obj),
			     "ccWnn Object: can't open jserver");
		return;
	    }
	}
	obj->ccWnn.createenv = True;
	if (!jcIsConnect(obj->ccWnn.wnnbuf)) {
		XtAppWarning(XtWidgetToApplicationContext((Widget)obj),
			     "ccWnn Object: can't connect to jserver");
	}
    }
    /* $B@\B3$G$-$J$/$F$b%P%C%U%!$O:n$C$F$*$/(B */
    obj->ccWnn.jcbuf = jcCreateBuffer(obj->ccWnn.wnnbuf, 10, 80);
    if (obj->ccWnn.pendingdata) restoreData(obj);
}

static void
createEnvError(s)
char *s;
{
    if (current_obj != NULL)
	XtAppWarning(XtWidgetToApplicationContext((Widget)current_obj), s);
}

static int
createConfirm(s)
char *s;
{
    if (current_obj != NULL && current_obj->ccWnn.confirmfunc != NULL)
	return (*current_obj->ccWnn.confirmfunc)((Widget)current_obj, s);
    return 1;
}

static void
Destroy(w)
Widget w;
{
    CcWnnObject obj = (CcWnnObject)w;

    /* $B%P%C%U%!$N2rJ|(B */
    if (obj->ccWnn.ccbuf) ccDestroyBuf(obj->ccWnn.ccbuf);
    if (obj->ccWnn.jcbuf) jcDestroyBuffer(obj->ccWnn.jcbuf, 1);

    /* $B$b$7(B Initialize() $BCf$G%k!<%k(B/$B4D6-$r:n$C$?$N$G$"$l$P2rJ|$9$k(B */
    if (obj->ccWnn.createrule) ccFreeRule(obj->ccWnn.ccrule);
    if (obj->ccWnn.createenv) jcClose(obj->ccWnn.wnnbuf);

    if (obj->ccWnn.candlist) XtFree((char *)obj->ccWnn.candlist);
    if (obj->ccWnn.strdata) XtFree((char *)obj->ccWnn.strdata);

    if (obj->ccWnn.pendingdata) XtFree((char *)obj->ccWnn.pendingdata);

    if (obj->ccWnn.selwidget) XtDestroyWidget(obj->ccWnn.selwidget);
    if (obj->ccWnn.selyomi) XtFree(obj->ccWnn.selyomi);
    if (obj->ccWnn.selkanji) XtFree(obj->ccWnn.selkanji);

    deleteObject(obj);
}

/* ARGSUSED */
static Boolean
SetValues(cur, req, wid, args, num_args)
Widget cur;
Widget req;
Widget wid;
ArgList args;
Cardinal *num_args;
{
    CcWnnObject old = (CcWnnObject)cur;
    CcWnnObject new = (CcWnnObject)wid;

    if (old->ccWnn.ccdeffile != new->ccWnn.ccdeffile ||
	old->ccWnn.wnnbuf != new->ccWnn.wnnbuf ||
	old->ccWnn.ccrule != new->ccWnn.ccrule) {
	XtAppWarning(XtWidgetToApplicationContext(wid),
		     "ccWnn Object: can't change resource by XtSetValues()");
    }
    return False;
}


/*
 *	cconv function dispatcher
 */

/* ARGSUSED */
static int
funcDispatch(func, str, nbytes, w)
int func;
unsigned char *str;
int nbytes;
caddr_t w;
{
    CcWnnObject obj = (CcWnnObject)w;

    obj->ccWnn.cont = True;
    if (func < 0 || func >= FTSIZE) return True;

    wnn_errorno = 0;

    switch (obj->ccWnn.state) {
    case selection_s_state:
    case selection_l_state:
	(*functable[func][1])(obj);
	break;
    case symbol_state:
	(*functable[func][2])(obj);
	break;
    default:
	(*functable[func][0])(obj);
	break;
    }

    return obj->ccWnn.cont;
}

/* ARGSUSED */
static void
defAction(str, nbytes, w)
unsigned char *str;
int nbytes;
caddr_t w;
{
    if (nbytes > 0) beep((CcWnnObject)w);
}

#define ZERO	0xa3b0
#define NINE	0xa3b9
#define SmallA	0xa3e1
#define SmallF	0xa3e6
#define LargeA	0xa3c1
#define LargeF	0xa3c6
static int
toHex(c)
int	c;
{
	if ('0' <= c && c <= '9')
		return c - '0';
	if ('a' <= c && c <= 'f')
		return c + 10 - 'a';
	if ('A' <= c && c <= 'F')
		return c + 10 - 'A';
	if (ZERO <= c && c <= NINE)
		return c - ZERO;
	if (SmallA <= c && c <= SmallF)
		return c + 10 - SmallA;
	if (LargeA <= c && c <= LargeF)
		return c + 10 - LargeA;
	return -1;
}

static int
toHex4(s)
wchar	*s;
{
	int	c, h, cnt, hex;

	hex = 0;
	cnt = 0;
	while (cnt < 4 && (c = *s++)) {
		if ((h = toHex(c)) < 0)
			return -1;
		hex = hex * 16 + h;
		cnt++;
	}
	if (cnt != 4)
		return -1;

	return hex;
}

static int
toKuten(s)
wchar *s;
{
	int i, c, d[4];

	for (i = 0; i < 4 && (c = *s++); i++) {
		if ((d[i] = toHex(c)) < 0 || d[i] >= 10)
			return(-1);
	}
	if (i != 4)
		return(-1);
	return((((d[0] * 10 + d[1]) << 8) | (d[2] * 10 + d[3])) + 0x2020);
}

static void
insChar(c, cldata)
int c;
caddr_t cldata;
{
    CcWnnObject obj = (CcWnnObject)cldata;
    jcConvBuf *jcbuf = obj->ccWnn.jcbuf;
    ccBuf ccbuf = obj->ccWnn.ccbuf;
    wchar	context[17];
    int h;

    normalState(obj);

    if (jcIsConverted(jcbuf, jcbuf->curClause)) {
	(void)jcBottom(jcbuf);
    }

    switch (obj->ccWnn.inputmode) {
    case KUTEN_MODE:
    case JIS_MODE:
	    /* $B%X%-%5$+$I$&$+$N%F%9%H(B */
	    if ((h = toHex(c)) < 0 || (obj->ccWnn.inputmode == KUTEN_MODE && h >= 10)) {
		    beep(obj);
		    ccContextDelete(ccbuf);
		    break;
	    }
	    ccContextGet(ccbuf, context);
	    if (wstrlen(context) == 4) {
		    /* convert to KANJI */
		    c = obj->ccWnn.inputmode == KUTEN_MODE ? toKuten(context): toHex4(context);
		    if (c < 0x2121 || 0x7e7e < c || (c & 0xff) < 0x21 ||
			0x7e < (c & 0xff)) {
			    beep(obj);
			    break;
		    }
		    /* $B#3J8;z:o=|(B -- $B#4J8;zL\$O$^$@A^F~$7$F$$$J$$(B */
		    jcDeleteChar(jcbuf, 1);
		    jcDeleteChar(jcbuf, 1);
		    jcDeleteChar(jcbuf, 1);
		    (void)jcInsertChar(jcbuf, c | 0x8080);
		    obj->ccWnn.textchanged = True;
		    /* $B%3%s%F%-%9%H$N%/%j%"(B */
		    ccContextClear(ccbuf);
		    break;
	    }
	    /* fall thru */
    case OTHERS:
	    (void)jcInsertChar(jcbuf, c);
	    obj->ccWnn.textchanged = True;
	    break;
    }
}

static void
delChar(cldata)
caddr_t cldata;
{
    CcWnnObject obj = (CcWnnObject)cldata;

    if (obj->ccWnn.state != normal_state) {
	beep(obj);
	return;
    }
    ccContextDelete(obj->ccWnn.ccbuf);
    jcDeleteChar(obj->ccWnn.jcbuf, 1);
    obj->ccWnn.textchanged = True;
}

static void
autoFix(cldata)
caddr_t cldata;
{
    CcWnnObject obj = (CcWnnObject)cldata;

    switch (obj->ccWnn.state)
    {
    case selection_s_state:
    case selection_l_state:
    case symbol_state:
	fix(obj);
	break;
    default:

	if (jcIsConverted(obj->ccWnn.jcbuf, 0))
	    fix(obj);
	break;
    }
}

/*
 *	cconv functions
 */

/* some convenient macros */
#define JCBUF(obj)	((obj)->ccWnn.jcbuf)
#define CCBUF(obj)	((obj)->ccWnn.ccbuf)
#define HINT(obj)	((obj)->ccWnn.textchanged)

/* $BJQ49%U%!%s%/%7%g%s72(B
 *	convert
 *	convert-sp
 *	convert-s
 *	unconvert
 */

static void
convert_general(obj, small)
CcWnnObject obj;
int small;
{
    jcConvBuf	*jcbuf = JCBUF(obj);

    if (jcbuf->curClause == jcbuf->nClause) {
	(void)jcMove(jcbuf, small, JC_BACKWARD);
	HINT(obj) = True;
    }

    if (jcIsConverted(jcbuf, jcbuf->curClause)) {
	startSelection(obj, small);
	return;
    }

    if (jcConvert(jcbuf, small, 0, 1) < 0) beep(obj);
    ccContextClear(CCBUF(obj));
    HINT(obj) = True;
}

static void
convert(obj)
CcWnnObject obj;
{
    convert_general(obj, 0);
}

static void
convert_sb(obj)
CcWnnObject obj;
{
    if (JCBUF(obj)->nClause == 0) {
	send_back(obj);
	fix(obj);
    } else {
	convert_general(obj, 0);
    }
}

static void
convert_sp(obj)
CcWnnObject obj;
{
    if (JCBUF(obj)->nClause == 0) {
	insChar(' ', (caddr_t)obj);
	fix(obj);
    } else {
	convert_general(obj, 0);
    }
}

static void
convert_mt(obj)
CcWnnObject obj;
{
    jcConvBuf *jcbuf = JCBUF(obj);
    if (jcbuf->nClause == 0) {
	send_back(obj);
	fix(obj);
    } else {
	int nc = jcbuf->nClause - 1;
	convert_general(obj, 0);
	if (!jcIsConverted(jcbuf, jcbuf->curClause)) {
	    int i;
	    move_top(obj);
	    if (nc < jcbuf->nClause)
		for (i = 0; i < nc; i++)
		    move_forward(obj);
	}
    }
}

static void
convert_f1(obj)
CcWnnObject obj;
{
    jcConvBuf	*jcbuf = JCBUF(obj);

    if (0 < jcbuf->nClause && jcIsConverted(jcbuf, 0)) {
	fix1(obj);
    } else {
	convert_general(obj, 0);
    }
}

static void
convert_mt_sb(obj)
CcWnnObject obj;
{
    jcConvBuf *jcbuf = JCBUF(obj);
    if (jcbuf->nClause == 0) {
	send_back(obj);
	fix(obj);
    } else {
	int nc = jcbuf->nClause - 1;
	convert_general(obj, 0);
	if (!jcIsConverted(jcbuf, jcbuf->curClause)) {
	    int i;
	    move_top(obj);
	    if (nc < jcbuf->nClause)
		for (i = 0; i < nc; i++)
		    move_forward(obj);
	}
    }
}

static void
convert_mt_sp(obj)
CcWnnObject obj;
{
    jcConvBuf *jcbuf = JCBUF(obj);
    if (jcbuf->nClause == 0) {
	insChar(' ', (caddr_t)obj);
	fix(obj);
    } else {
	int nc = jcbuf->nClause - 1;
	convert_general(obj, 0);
	if (!jcIsConverted(jcbuf, jcbuf->curClause)) {
	    int i;
	    move_top(obj);
	    if (nc < jcbuf->nClause)
		for (i = 0; i < nc; i++)
		    move_forward(obj);
	}
    }
}

static void
convert_s(obj)
CcWnnObject obj;
{
    convert_general(obj, 1);
}

static void
unconvert(obj)
CcWnnObject obj;
{
    if (jcUnconvert(JCBUF(obj)) < 0) beep(obj);
    ccContextClear(CCBUF(obj));
    HINT(obj) = True;
}

static void
select_general(obj, small)
CcWnnObject obj;
int small;
{
    jcConvBuf	*jcbuf = JCBUF(obj);

    if (jcbuf->curClause == jcbuf->nClause) {
	(void)jcMove(jcbuf, small, JC_BACKWARD);
	HINT(obj) = True;
    }

    if (jcIsConverted(jcbuf, jcbuf->curClause)) {
	startSelection(obj, small);
	obj->ccWnn.cont = False; /* $B<!$N4X?t$O8F$P$J$$(B */
    }
}


static void
egg_select(obj)
CcWnnObject obj;
{
    select_general(obj, 0);
}


static void
egg_select_s(obj)
CcWnnObject obj;
{
    select_general(obj, 1);
}

/* $B%+!<%=%k0\F0%U%!%s%/%7%g%s72(B
 *	move_forward
 *	move_backward
 *	move_top
 *	move_bottom
 */

static void
move_general(obj, direction)
CcWnnObject obj;
int direction;
{
    int status = -1;

    switch (direction) {
    case ICMoveLeftMost:
	status = jcTop(JCBUF(obj));
	break;
    case ICMoveRightMost:
	status = jcBottom(JCBUF(obj));
	break;
    case ICMoveRight:
	status = jcMove(JCBUF(obj), 1, JC_FORWARD);
	break;
    case ICMoveLeft:
	status = jcMove(JCBUF(obj), 1, JC_BACKWARD);
	break;
    }

    if (status < 0) beep(obj);

    ccContextClear(CCBUF(obj));

    HINT(obj) = True;
}

static void
move_forward(obj)
CcWnnObject obj;
{
    move_general(obj, ICMoveRight);
}

static void
move_backward(obj)
CcWnnObject obj;
{
    move_general(obj, ICMoveLeft);
}

static void
move_top(obj)
CcWnnObject obj;
{
    move_general(obj, ICMoveLeftMost);
}

static void
move_bottom(obj)
CcWnnObject obj;
{
    move_general(obj, ICMoveRightMost);
}


/* $B8uJd@ZBX$(%U%!%s%/%7%g%s72(B
 *	cand_next
 *	cand_next_s
 *	cand_prev
 *	cand_prev_s
 */

static void
cand_general(obj, small, type)
CcWnnObject obj;
int small;
int type;
{
    if (jcNext(JCBUF(obj), small, type) < 0) beep(obj);
    ccContextClear(CCBUF(obj));
    HINT(obj) = True;
}

static void
cand_next(obj)
CcWnnObject obj;
{
    cand_general(obj, 0, JC_NEXT);
}

static void
cand_next_mt(obj)
CcWnnObject obj;
{
    jcConvBuf *jcbuf = JCBUF(obj);
    if (jcIsConverted(jcbuf, jcbuf->curClause))
	cand_general(obj, 0, JC_NEXT);
    else
	convert_mt(obj);
}

static void
cand_next_mt_sb(obj)
CcWnnObject obj;
{
    jcConvBuf *jcbuf = JCBUF(obj);
    if (jcIsConverted(jcbuf, jcbuf->curClause))
	cand_general(obj, 0, JC_NEXT);
    else
	convert_mt_sb(obj);
}

static void
cand_next_mt_sp(obj)
CcWnnObject obj;
{
    jcConvBuf *jcbuf = JCBUF(obj);
    if (jcIsConverted(jcbuf, jcbuf->curClause))
	cand_general(obj, 0, JC_NEXT);
    else
	convert_mt_sp(obj);
}

static void
cand_next_s(obj)
CcWnnObject obj;
{
    cand_general(obj, 1, JC_NEXT);
}

static void
cand_prev(obj)
CcWnnObject obj;
{
    cand_general(obj, 0, JC_PREV);
}

static void
cand_prev_s(obj)
CcWnnObject obj;
{
    cand_general(obj, 1, JC_PREV);
}


/* $BJ8@aD9JQ99%U%!%s%/%7%g%s72(B
 *	expand_cl
 *	expand_cl_s
 *	expand_cl2
 *	expand_cl2_s
 *	shrink_cl
 *	shrink_cl_s
 *	shrink_cl2
 *	shrink_cl2_s
 */

static void
expand_shrink_general(obj, shrink, small, conv)
CcWnnObject obj;
int shrink;
int small;
int conv;
{
    int status;

    normalState(obj);

    if (shrink) {
	status = jcShrink(JCBUF(obj), small, conv);
    } else {
	status = jcExpand(JCBUF(obj), small, conv);
    }
    if (status < 0) beep(obj);

    ccContextClear(CCBUF(obj));
    HINT(obj) = True;
}

static void
expand_cl(obj)
CcWnnObject obj;
{
    expand_shrink_general(obj, 0, 0, 1);
}

static void
expand_cl_s(obj)
CcWnnObject obj;
{
    expand_shrink_general(obj, 0, 1, 1);
}

static void
shrink_cl(obj)
CcWnnObject obj;
{
    expand_shrink_general(obj, 1, 0, 1);
}

static void
shrink_cl_s(obj)
CcWnnObject obj;
{
    expand_shrink_general(obj, 1, 1, 1);
}

static void
expand_cl2(obj)
CcWnnObject obj;
{
    expand_shrink_general(obj, 0, 0, 0);
}

static void
expand_cl2_s(obj)
CcWnnObject obj;
{
    expand_shrink_general(obj, 0, 1, 0);
}

static void
shrink_cl2(obj)
CcWnnObject obj;
{
    expand_shrink_general(obj, 1, 0, 0);
}

static void
shrink_cl2_s(obj)
CcWnnObject obj;
{
    expand_shrink_general(obj, 1, 1, 0);
}


/* $BJQ49%P%C%U%!%/%j%"%U%!%s%/%7%g%s(B
 *	clear_buffer
 */

static void
clear_buffer(obj)
CcWnnObject obj;
{
    normalState(obj);
    if (jcClear(JCBUF(obj)) < 0) beep(obj);
    ccContextClear(CCBUF(obj));
    HINT(obj) = True;
}

static void
cancel(obj)
CcWnnObject obj;
{
    if (jcCancel(JCBUF(obj)) < 0) beep(obj);
    ccContextClear(CCBUF(obj));
    move_bottom(obj);
    HINT(obj) = True;
}


static void
clear_c(obj)
CcWnnObject obj;
{
    switch (obj->ccWnn.state)
    {
    case selection_s_state:
    case selection_l_state:
	endSelection(obj, False);
	cancel(obj);
	break;
    case symbol_state:
	clear_buffer(obj);
	break;
    default:
	if (jcIsConverted(JCBUF(obj), JCBUF(obj)->curClause))
	    cancel(obj);
	else
	    clear_buffer(obj);
	break;
    }
}

/* $B8uJd0\F0%U%!%s%/%7%g%s72(B
 *	sel_next
 *	sel_prev
 *	sel_top
 *	sel_bottom
 *	sel_forward
 *	sel_backward
 */

static void
sel_top(obj)
CcWnnObject obj;
{
    moveSelection(obj, ICMoveLeftMost);
}

static void
sel_bottom(obj)
CcWnnObject obj;
{
    moveSelection(obj, ICMoveRightMost);
}

static void
sel_forward(obj)
CcWnnObject obj;
{
    moveSelection(obj, ICMoveRight);
}

static void
sel_backward(obj)
CcWnnObject obj;
{
    moveSelection(obj, ICMoveLeft);
}

static void
sel_next(obj)
CcWnnObject obj;
{
    moveSelection(obj, ICMoveDown);
}

static void
sel_prev(obj)
CcWnnObject obj;
{
    moveSelection(obj, ICMoveUp);
}

static void
sel_select(obj)
CcWnnObject obj;
{
    endSelection(obj, False);
}

static void
sel_abort(obj)
CcWnnObject obj;
{
    endSelection(obj, True);
}

static void
fix(obj)
CcWnnObject obj;
{
    jcConvBuf *jcbuf = JCBUF(obj);

    normalState(obj);
    ccContextClear(CCBUF(obj));

    if (jcbuf->nClause > 0) {
	obj->ccWnn.fixperformed = True;

	if (jcFix(jcbuf) < 0) {
	    beep(obj);
	    return;
	}

	/* $B<-=q%;!<%V$N=hM}(B */
	obj->ccWnn.fixcount++;
	if (obj->ccWnn.saveinterval > 0 &&
	    obj->ccWnn.fixcount >= obj->ccWnn.saveinterval) {
	    jcSaveDic(jcbuf);
	    obj->ccWnn.fixcount = 0;
	}

	/* $B3NDj$N=hM}(B */
	XtCallCallbackList((Widget)obj, obj->inputConv.fixcallback,
			   (XtPointer)NULL);	/* ??? */

	HINT(obj) = True;
    }

    /* $B%P%C%U%!$r%/%j%"$9$k(B */
    jcClear(jcbuf);
}

static void
fix1(obj)
CcWnnObject obj;
{
    jcConvBuf *jcbuf = JCBUF(obj);

    normalState(obj);
    ccContextClear(CCBUF(obj));

    if (jcbuf->nClause > 0) {
	obj->ccWnn.fixperformed = True;

	if (jcFix1(jcbuf) < 0) { /* $B$3$3$@$1$,(B fix(obj) $B$H0c$&$H$3$m(B */
	    beep(obj);
	    return;
	}

	/* $B<-=q%;!<%V$N=hM}(B */
	obj->ccWnn.fixcount++;
	if (obj->ccWnn.saveinterval > 0 &&
	    obj->ccWnn.fixcount >= obj->ccWnn.saveinterval) {
	    jcSaveDic(jcbuf);
	    obj->ccWnn.fixcount = 0;
	}

	/* $B3NDj$N=hM}(B */
	XtCallCallbackList((Widget)obj, obj->inputConv.fixcallback,
			   (XtPointer)NULL);	/* ??? */

	HINT(obj) = True;
    }

    /* $B%P%C%U%!$r%/%j%"$9$k(B */
    jcClear(jcbuf);
}

static void
fix_cr(obj)
CcWnnObject obj;
{
    if (JCBUF(obj)->nClause == 0) {
	carriageret(obj);
    } else {
	fix(obj);
    }
}


static void
fix_sb(obj)
CcWnnObject obj;
{
    if (JCBUF(obj)->nClause == 0) {
	send_back(obj);
    } else {
	fix(obj);
    }
}


static void
to_hankaku(start, end, res)
wchar *start;
wchar *end;
wchar *res;
{
    static unsigned short hiratohan[] = {	/* $BA43Q$+$J(B <-> $BH>3Q$+$J(B */
#define D	(0xde<<8)	/* $BBy2;(B */
#define H	(0xdf<<8)	/* $BH>By2;(B */
	/* a */ 0xa7, 0xb1, 0xa8, 0xb2, 0xa9, 0xb3, 0xaa, 0xb4, 0xab, 0xb5,
	/* k */ 0xb6, 0xb6|D, 0xb7, 0xb7|D, 0xb8, 0xb8|D,
		0xb9, 0xb9|D, 0xba, 0xba|D,
	/* s */ 0xbb, 0xbb|D, 0xbc, 0xbc|D, 0xbd, 0xbd|D,
		0xbe, 0xbe|D, 0xbf, 0xbf|D,
	/* t */ 0xc0, 0xc0|D, 0xc1, 0xc1|D, 0xaf, 0xc2, 0xc2|D,
		0xc3, 0xc3|D, 0xc4, 0xc4|D,
	/* n */	0xc5, 0xc6, 0xc7, 0xc8, 0xc9,
	/* h */	0xca, 0xca|D, 0xca|H, 0xcb, 0xcb|D, 0xcb|H, 0xcc, 0xcc|D,
		0xcc|H, 0xcd, 0xcd|D, 0xcd|H, 0xce, 0xce|D, 0xce|H,
	/* m */ 0xcf, 0xd0, 0xd1, 0xd2, 0xd3,
	/* y */ 0xac, 0xd4, 0xad, 0xd5, 0xae, 0xd6,
	/* r */ 0xd7, 0xd8, 0xd9, 0xda, 0xdb,
	/* w */ 0xdc, 0xdc, 0xb2, 0xb4, 0xa6,
	/* n */ 0xdd
#undef D
#undef H
    };
    static struct symzenhan {
	unsigned short	zen;
	unsigned char	han;
    } kigoutohan[] = {				/* $BA43Q5-9f(B -> $BH>3Q5-9f(B */
	0xa1a1, 0x20,	0xa1a2, 0xa4,	0xa1a3, 0xa1,	0xa1a4, 0x2c,
	0xa1a5, 0x2e,	0xa1a6, 0xa5,	0xa1a7, 0x3a,	0xa1a8, 0x3b,
	0xa1a9, 0x3f,	0xa1aa, 0x21,	0xa1ab, 0xde,	0xa1ac, 0xdf,
	0xa1b0, 0x5e,	0xa1b2, 0x5f,	0xa1bc, 0xb0,	0xa1bf, 0x2f,
	0xa1c1, 0x7e,	0xa1c3, 0x7c,	0xa1c6, 0x60,	0xa1c7, 0x27,
	0xa1c8, 0x22,	0xa1c9, 0x22,	0xa1ca, 0x28,	0xa1cb, 0x29,
	0xa1cc, 0x5b,	0xa1cd, 0x5d,	0xa1ce, 0x5b,	0xa1cf, 0x5d,
	0xa1d0, 0x7b,	0xa1d1, 0x7d,	0xa1d6, 0xa2,	0xa1d7, 0xa3,
	0xa1dc, 0x2b,	0xa1dd, 0x2d,	0xa1e1, 0x3d,	0xa1e3, 0x3c,
	0xa1e4, 0x3e,	0xa1ef, 0x5c,	0xa1f0, 0x24,	0xa1f3, 0x25,
	0xa1f4, 0x23,	0xa1f5, 0x26,	0xa1f6, 0x2a,	0xa1f7, 0x40,
    };
#define KIGOUSIZE	(sizeof(kigoutohan) / sizeof(struct symzenhan))
    register int c;

    while (start < end) {
	c = *start++;
	if (0xa1a1 <= c && c <= 0xa1fe) {		/* symbol */
	    register struct symzenhan *hi = kigoutohan + KIGOUSIZE;
	    register struct symzenhan *lo = kigoutohan;
	    register struct symzenhan *m;
	    register int dif;

	    while (lo <= hi) {
		m = lo + (hi - lo) / 2;
		if ((dif = c - m->zen) == 0) break;
		if (dif < 0) {
		    hi = m - 1;
		} else {
		    lo = m + 1;
		}
	    }
	    *res++ = (lo > hi) ? c : m->han;
	} else if (0xa3b0 <= c && c <= 0xa3b9) {	/* Numeric */
	    *res++ = c - 0xa3b0 + '0';
	} else if (0xa3c1 <= c && c <= 0xa3da) {	/* A-Z */
	    *res++ = c - 0xa3c1 + 'A';
	} else if (0xa3e1 <= c && c <= 0xa3fa) {	/* a-z */
	    *res++ = c - 0xa3e1 + 'a';
	} else if (0xa4a1 <= c && c <= 0xa4f3) {	/* $B$R$i$,$J(B */
	    c = hiratohan[c - 0xa4a1];
	    *res++ = c & 0xff;
	    if (c & 0xff00) *res++ = c >> 8;
	} else if (0xa5a1 <= c && c <= 0xa5f3) {	/* $B$+$?$+$J(B */
	    c = hiratohan[c - 0xa5a1];
	    *res++ = c & 0xff;
	    if (c & 0xff00) *res++ = c >> 8;
	} else {
	    *res++ = c;
	}
    }
    *res = 0;	/* NULL terminate */
}

static void
to_zenkaku(start, end, res)
wchar *start;
wchar *end;
wchar *res;
{
    static wchar hantozen[] = {	/* $BH>3Q(B $B"M(B $BA43QJQ49I=(B */
	/* C0 */
	0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
	0x0008, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000f,
	0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
	0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x001d, 0x001e, 0x001f,
	/* ASCII */
	0xa1a1, 0xa1aa, 0xa1c9, 0xa1f4, 0xa1f0, 0xa1f3, 0xa1f5, 0xa1c7,
	0xa1ca, 0xa1cb, 0xa1f6, 0xa1dc, 0xa1a4, 0xa1dd, 0xa1a5, 0xa1bf,
	0xa3b0, 0xa3b1, 0xa3b2, 0xa3b3, 0xa3b4, 0xa3b5, 0xa3b6, 0xa3b7,
	0xa3b8, 0xa3b9, 0xa1a7, 0xa1a8, 0xa1e3, 0xa1e1, 0xa1e4, 0xa1a9,
	0xa1f7, 0xa3c1, 0xa3c2, 0xa3c3, 0xa3c4, 0xa3c5, 0xa3c6, 0xa3c7,
	0xa3c8, 0xa3c9, 0xa3ca, 0xa3cb, 0xa3cc, 0xa3cd, 0xa3ce, 0xa3cf,
	0xa3d0, 0xa3d1, 0xa3d2, 0xa3d3, 0xa3d4, 0xa3d5, 0xa3d6, 0xa3d7,
	0xa3d8, 0xa3d9, 0xa3da, 0xa1ce, 0xa1ef, 0xa1cf, 0xa1b0, 0xa1b2,
	0xa1c6, 0xa3e1, 0xa3e2, 0xa3e3, 0xa3e4, 0xa3e5, 0xa3e6, 0xa3e7,
	0xa3e8, 0xa3e9, 0xa3ea, 0xa3eb, 0xa3ec, 0xa3ed, 0xa3ee, 0xa3ef,
	0xa3f0, 0xa3f1, 0xa3f2, 0xa3f3, 0xa3f4, 0xa3f5, 0xa3f6, 0xa3f7,
	0xa3f8, 0xa3f9, 0xa3fa, 0xa1d0, 0xa1c3, 0xa1d1, 0xa1c1, 0x007f,
	/* C1 */
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
	0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f,
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
	0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f,
	/* KANA */
	0xa1a1, 0xa1a3, 0xa1d6, 0xa1d7, 0xa1a2, 0xa1a6, 0xa5f2, 0xa5a1,
	0xa5a3, 0xa5a5, 0xa5a7, 0xa5a9, 0xa5e3, 0xa5e5, 0xa5e7, 0xa5c3,
	0xa1bc, 0xa5a2, 0xa5a4, 0xa5a6, 0xa5a8, 0xa5aa, 0xa5ab, 0xa5ad,
	0xa5af, 0xa5b1, 0xa5b3, 0xa5b5, 0xa5b7, 0xa5b9, 0xa5bb, 0xa5bd,
	0xa5bf, 0xa5c1, 0xa5c4, 0xa5c6, 0xa5c8, 0xa5ca, 0xa5cb, 0xa5cc,
	0xa5cd, 0xa5ce, 0xa5cf, 0xa5d2, 0xa5d5, 0xa5d8, 0xa5db, 0xa5de,
	0xa5df, 0xa5e0, 0xa5e1, 0xa5e2, 0xa5e4, 0xa5e6, 0xa5e8, 0xa5e9,
	0xa5ea, 0xa5eb, 0xa5ec, 0xa5ed, 0xa5ef, 0xa5f3, 0xa1ab, 0xa1ac,
	/* undefined */
	0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7,
	0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef,
	0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7,
	0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff,
    };
    register int c;

    while (start < end) {
	c = *start++;
	if ((0x20 <= c && c <= 0x7e) || (0xa1 <= c && c <= 0xdf)) {
	    *res++ = hantozen[c];
	} else {
	      *res++ = c;
	}
    }
    *res = 0;	/* NULL terminate */
}

static void
zenkaku_hankaku(obj, hankaku)
CcWnnObject obj;
int hankaku;
{
    jcConvBuf *jcbuf = JCBUF(obj);

    normalState(obj);

    if (jcbuf->curClause != jcbuf->nClause) {
	jcClause *cinfo = jcbuf->clauseInfo;
	wchar *ks = cinfo[jcbuf->curLCStart].kanap;
	wchar *ke = cinfo[jcbuf->curLCEnd].kanap;
	wchar buf[256];

	if (hankaku) {
	    to_hankaku(ks, ke, buf);
	} else {
	    to_zenkaku(ks, ke, buf);
	}
	if (jcChangeClause(jcbuf, buf) < 0) beep(obj);
    }
    ccContextClear(CCBUF(obj));
    HINT(obj) = True;
}

static void
zenkaku(obj)
CcWnnObject obj;
{
    zenkaku_hankaku(obj, 0);
}

static void
hankaku(obj)
CcWnnObject obj;
{
    zenkaku_hankaku(obj, 1);
}

static void
hiragana_katakana(obj, type)
CcWnnObject obj;
int type;
{
    normalState(obj);

    if (jcKana(JCBUF(obj), 0, type) < 0) beep(obj);
    ccContextClear(CCBUF(obj));
    HINT(obj) = True;
}

static void
hiragana(obj)
CcWnnObject obj;
{
    hiragana_katakana(obj, JC_HIRAGANA);
}

static void
katakana(obj)
CcWnnObject obj;
{
    hiragana_katakana(obj, JC_KATAKANA);
}


static void
backspace(obj)
CcWnnObject obj;
{
    switch (obj->ccWnn.state) {
    case selection_l_state:
	endSelection(obj, False);
	(void)jcMove(JCBUF(obj), 0, JC_FORWARD);
	break;
    case selection_s_state:
	endSelection(obj, False);
	(void)jcMove(JCBUF(obj), 1, JC_FORWARD);
	break;
    case symbol_state:
	endSelection(obj, False);
	break;
    }
    ccContextDelete(CCBUF(obj));
    if (jcDeleteChar(JCBUF(obj), 1) < 0) beep(obj);
    HINT(obj) = True;
}

static void
backspace_c(obj)
CcWnnObject obj;
{
    switch (obj->ccWnn.state) {
    case selection_s_state:
    case selection_l_state:
	endSelection(obj, False);
	cancel(obj);
	break;
    case symbol_state:
	backspace(obj);
	break;
    default:
        if (jcIsConverted(JCBUF(obj), 0))
	    cancel(obj);
	else
	    backspace(obj);
	break;
    }
}

static void
delete(obj)
CcWnnObject obj;
{
    normalState(obj);
    if (jcDeleteChar(JCBUF(obj), 0) < 0) beep(obj);
    ccContextClear(CCBUF(obj));
    HINT(obj) = True;
}

static void
delete_c(obj)
CcWnnObject obj;
{
    switch (obj->ccWnn.state) {
    case selection_s_state:
    case selection_l_state:
	endSelection(obj, False);
	cancel(obj);
	break;
    case symbol_state:
	delete(obj);
	break;
    default:
	if (jcIsConverted(JCBUF(obj), JCBUF(obj)->curClause))
	    cancel(obj);
	else
	    delete(obj);
	break;
    }
}

static void
kill_line(obj)
CcWnnObject obj;
{
    normalState(obj);
    if (jcKillLine(JCBUF(obj)) < 0) beep(obj);
    ccContextClear(CCBUF(obj));
    HINT(obj) = True;
}

static void
bell(obj)
CcWnnObject obj;
{
    XBell(XtDisplayOfObject((Widget)obj), 0);
}

static void
beep(obj)
CcWnnObject obj;
{
    if (JCBUF(obj)->nClause == 0) return;
    bell(obj);
}

static void
jiscode_begin(obj)
CcWnnObject obj;
{
    obj->ccWnn.inputmode = JIS_MODE;
}

static void
jiscode_end(obj)
CcWnnObject obj;
{
    obj->ccWnn.inputmode = OTHERS;
}

static void
kuten_begin(obj)
CcWnnObject obj;
{
    obj->ccWnn.inputmode = KUTEN_MODE;
}

static void
kuten_end(obj)
CcWnnObject obj;
{
    obj->ccWnn.inputmode = OTHERS;
}

static void
carriageret(obj)
CcWnnObject obj;
{
    insChar('\r', (caddr_t)obj);
    fix(obj);
}


static void
convend(obj)
CcWnnObject obj;
{
    fix(obj);
    /* $B%$%Y%s%H$rAw$jJV$5$J$$$h$&$K(B fixperformed $B$r%;%C%H$7$F$*$/(B */
    obj->ccWnn.fixperformed = True;
    XtCallCallbackList((Widget)obj, obj->inputConv.endcallback,
		       (XtPointer)NULL);
}


static void
send_back(obj)
CcWnnObject obj;
{
    obj->ccWnn.sendbackevent = True;
}


static void
register_word(obj)
CcWnnObject obj;
{
    Widget w = obj->ccWnn.selwidget;
    Display *dpy;

    TRACE(("register_word()\n"));
    if (w == NULL) {
	/*
	 * $B%&%#%s%I%&$r;H$$$?$$$N$G!"<+J,$G(B widget $B$r0l$D:n$k!#(B
	 * nonwidget $B$r?F$K$7$F(B widget $B$r:n@.$9$k$3$H$O2DG=$J$h$&$K(B
	 * $B;W$($k$N$@$,!"$J$<$+$G$-$J$$$_$?$$$J$N$G!"$^$:$O(B widget $B$G$"$k(B
	 * $B?F$rC5$7$F!"$=$3$K:n$k$3$H$K$9$k!#(B
	 */
	Widget p = XtParent((Widget)obj);

	while (p != NULL) {
	    if (XtIsWidget(p)) break;
	    p = XtParent(p);
	}
	if (p == NULL) {
	    DPRINT(("register_word(): cannot find widget parent\n"));
	    return;
	}

	TRACE(("register_word(): creating core widget\n"));
	w = XtVaCreateWidget("for_selection", coreWidgetClass, p,
			     XtNwidth, 1, XtNheight, 1, NULL);
	XtRealizeWidget(w);
	obj->ccWnn.selwidget = w;
    }
    saveYomiAndKanji(obj);
    if (obj->ccWnn.selyomi == NULL && obj->ccWnn.selkanji == NULL) return;

    dpy = XtDisplay(w);
    XtOwnSelection(w, CachedInternAtom(dpy, CCWNN_REGISTER_ATOM, False),
		   XtLastTimestampProcessed(dpy),
		   convertSelection, NULL, NULL);
}

static void
sym_input(obj)
CcWnnObject obj;
{
    ICSelectionControlArg arg;

    if (obj->ccWnn.state != normal_state) {
	beep(obj);
	return;
    }
    obj->ccWnn.state = symbol_state;

    arg.command = ICSelectionStart;
    arg.u.selection_kind = ICSelectionSymbols;
    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback,
		       (XtPointer)&arg);

    arg.command = ICSelectionSet;
    arg.u.current_item = obj->ccWnn.cursymbol;
    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback,
		       (XtPointer)&arg);
}

static int
getSymbol(obj, n)
CcWnnObject obj;
int n;
{
    int c;

    if (n < 0 || n >= obj->ccWnn.numsymbols) return -1;

    c = *(wchar *)(obj->ccWnn.symbollist[n].data);

    return c;
}

static void
startSelection(obj, small)
CcWnnObject obj;
int small;
{
    ICSelectionControlArg arg;
    int ncand, curcand;

    if (obj->ccWnn.state != normal_state) {
	beep(obj);
	return;
    }

    if (jcCandidateInfo(JCBUF(obj), small, &ncand, &curcand) < 0) {
	beep(obj);
	return;
    }

    getAllCandidates(obj, ncand);

    obj->ccWnn.numcand = ncand;
    obj->ccWnn.curcand = curcand;
    obj->ccWnn.state = small ? selection_s_state : selection_l_state;

    arg.command = ICSelectionStart;
    arg.u.selection_kind = ICSelectionCandidates;
    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback,
		       (XtPointer)&arg);

    /* set current item */
    arg.command = ICSelectionSet;
    arg.u.current_item = curcand;
    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback,
		       (XtPointer)&arg);
}

static void
moveSelection(obj, dir)
CcWnnObject obj;
int dir;
{
    ICSelectionControlArg arg;

    if (obj->ccWnn.state == normal_state) return;
    arg.command = ICSelectionMove;
    arg.u.dir = dir;
    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback,
		       (XtPointer)&arg);
}

static int
endSelection(obj, abort)
CcWnnObject obj;
int abort;
{
    ICSelectionControlArg arg;
    int selected;
    int ret = 0;

    if (obj->ccWnn.selectionending) return 0;

    if (obj->ccWnn.state == normal_state) return -1;

    arg.command = ICSelectionEnd;
    arg.u.current_item = -1;
    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback,
		       (XtPointer)&arg);

    if (!abort && (selected = arg.u.current_item) >= 0) {
	ret = insertSelection(obj, selected);
    }
    obj->ccWnn.state = normal_state;

    return ret;
}

static int
insertSelection(obj, selected)
CcWnnObject obj;
int selected;
{
    int state = obj->ccWnn.state;
    int ret = 0;

    HINT(obj) = True;

    obj->ccWnn.selectionending = True;
    if (state == symbol_state) {
	int c = getSymbol(obj, selected);
	if (c < 0) return -1;
	obj->ccWnn.cursymbol = selected;
	ccContextAppend(CCBUF(obj), c);
	insChar(c, (caddr_t)obj);
    } else {
	obj->ccWnn.curcand = selected;
	ret = jcSelect(JCBUF(obj), selected);
    }
    obj->ccWnn.selectionending = False;

    return ret;
}

static void
normalState(obj)
CcWnnObject obj;
{
    switch (obj->ccWnn.state) {
    case selection_l_state:
    case selection_s_state:
	/* $B8uJdA*BrCf$G$"$l$P%+%l%s%H$N8uJd$rA*Br$7$FA*Br%b!<%I$+$iH4$1$k(B */
    case symbol_state:
	/* $B5-9fF~NOCf$G$"$l$P%+%l%s%H$N5-9f$rA*Br$7$F5-9f%b!<%I$+$iH4$1$k(B */
	endSelection(obj, False);
	break;
    }
}

static void
allocCandlist(obj, n)
CcWnnObject obj;
int n;
{
    ICString *p;

    if (n <= obj->ccWnn.candlistsize) return;

    if (obj->ccWnn.candlistsize == 0) {
	p = (ICString *)XtMalloc(n * sizeof(ICString));
    } else {
	p = (ICString *)XtRealloc((char *)obj->ccWnn.candlist,
				  n * sizeof(ICString));
    }

    obj->ccWnn.candlist = p;
    obj->ccWnn.candlistsize = n;
}

static void
allocStrdata(obj, nchars)
CcWnnObject obj;
Cardinal nchars;
{
    wchar *p;

    if (nchars <= obj->ccWnn.strdatasize) return;

    if (obj->ccWnn.strdatasize == 0) {
	if (nchars < 256) nchars = 256;
	p = (wchar *)XtMalloc(nchars * sizeof(wchar));
    } else {
	if (nchars - obj->ccWnn.strdatasize < 256)
	    nchars = obj->ccWnn.strdatasize + 256;
	p = (wchar *)XtRealloc((char *)obj->ccWnn.strdata,
				 nchars * sizeof(wchar));
    }

    obj->ccWnn.strdata = p;
    obj->ccWnn.strdatasize = nchars;
}

static void
getAllCandidates(obj, ncand)
CcWnnObject obj;
int ncand;
{
    ICString *strp;
    Cardinal nchars;
    wchar *p;
    int i;
    wchar buf[256];

    allocCandlist(obj, ncand);

    nchars = 0;
    for (i = 0, strp = obj->ccWnn.candlist; i < ncand; i++, strp++) {
	(void)jcGetCandidate(obj->ccWnn.jcbuf, i, buf);
	strp->nchars = wstrlen(buf);
	strp->nbytes = strp->nchars * sizeof(wchar);
	strp->attr = ICAttrNormalString;
	allocStrdata(obj, nchars + strp->nchars);
	(void)bcopy((char *)buf, (char *)(obj->ccWnn.strdata + nchars),
		    strp->nbytes);
	nchars += strp->nchars;
    }

    p = obj->ccWnn.strdata;
    for (i = 0, strp = obj->ccWnn.candlist; i < ncand; i++, strp++) {
	strp->data = (char *)p;
	p += strp->nchars;
    }
}

/*
 * keeping list of objects
 */
typedef struct _oblist_ {
    CcWnnObject obj;
    struct _oblist_ *next;
} ObjRec;

static ObjRec *ObjList = NULL;

static void
addObject(obj)
CcWnnObject obj;
{
    ObjRec *objp = XtNew(ObjRec);

    objp->obj = obj;
    objp->next = ObjList;
    ObjList = objp;
}

static void
deleteObject(obj)
CcWnnObject obj;
{
    ObjRec *objp, *objp0;

    for (objp0 = NULL, objp = ObjList;
	 objp != NULL;
	 objp0 = objp, objp = objp->next) {
	if (objp->obj == obj) {
	    if (objp0 == NULL) {
		ObjList = objp->next;
	    } else {
		objp0->next = objp->next;
	    }
	    XtFree((char *)objp);
	    return;
	}
    }
}

static void
serverDead()
{
    ObjRec *objp = ObjList;

    while (objp != NULL) {
	if (objp->obj->ccWnn.wnnbuf != NULL
		 && !jcIsConnect(objp->obj->ccWnn.wnnbuf)
		 && wnn_errorno == WNN_JSERVER_DEAD) {
	    if (objp->obj->ccWnn.jcbuf != NULL) {
		(void)jcDestroyBuffer(objp->obj->ccWnn.jcbuf, 0);
		objp->obj->ccWnn.jcbuf = NULL;
	    }
	    (void)jcClose(objp->obj->ccWnn.wnnbuf);
	    objp->obj->ccWnn.wnnbuf = NULL;
	    if (objp->obj->ccWnn.ccbuf != NULL) {
		ccContextClear(objp->obj->ccWnn.ccbuf);
	    }
	}
	objp = objp->next;
    }
}

static void
saveData(obj)
CcWnnObject obj;
{
    wchar *wbuf;
    int len;
    jcConvBuf *jcbuf = obj->ccWnn.jcbuf;

    len = jcbuf->kanaEnd - jcbuf->kanaBuf;
    if (len <= 0) return;

    wbuf = (wchar *)XtMalloc((len + 1) * sizeof(wchar));
    (void)bcopy((char *)jcbuf->kanaBuf, (char *)wbuf,
		sizeof(wchar) * (len + 1));
    wbuf[len] = 0;
    obj->ccWnn.pendingdata = wbuf;
}

static void
restoreData(obj)
CcWnnObject obj;
{
    wchar *wp = obj->ccWnn.pendingdata;

    if (wp == NULL) return;

    while (*wp != 0) {
	jcInsertChar(obj->ccWnn.jcbuf, (int)*wp++);
    }
    XtFree((char *)obj->ccWnn.pendingdata);

    obj->ccWnn.pendingdata = NULL;
    obj->ccWnn.textchanged = True;
}

/* ARGSUSED */
static void
ioeCallback(dummy)
XPointer dummy;
{
    ObjRec *objp = ObjList;

    /*
     * I/O Error callback function.
     * Does minimum cleanup -- i.e. saving dictionaries.
     */
    while (objp != NULL) {
	if (objp->obj->ccWnn.jcbuf != NULL) {
	    jcSaveDic(objp->obj->ccWnn.jcbuf);
	}
	objp = objp->next;
    }
}

static CcWnnObject
findSelectionObj(w)
Widget w;
{
    ObjRec *objp = ObjList;

    while (objp != NULL) {
	if (objp->obj->ccWnn.selwidget == w) return objp->obj;
	objp = objp->next;
    }
    return NULL;
}

/* ARGSUSED */
static int
PreeditString(w, segn, offset, encoding, format, length, string)
Widget w;
int segn;
int offset;
Atom *encoding;
int *format;
int *length;
XtPointer *string;
{
    CcWnnObject obj = (CcWnnObject)w;
    jcConvBuf *jcbuf = obj->ccWnn.jcbuf;
    jcClause *cinfo = jcbuf->clauseInfo;
    wchar *wbuf, *wp;
    int len, wlen;
    extern int convJWStoCT();

    if (jcbuf == NULL) return -1;
    if (segn < jcbuf->nClause &&
	offset >= (cinfo[segn + 1].dispp - cinfo[segn].dispp)) {
	/* $B%;%0%a%s%H$N:G8e(B */
	++segn;
	offset = 0;
    }
    if (segn >= jcbuf->nClause ||
	offset >= (cinfo[segn + 1].dispp - cinfo[segn].dispp)) {
	/* $B:o=|$5$l$?(B */
	*encoding = XA_COMPOUND_TEXT(XtDisplayOfObject(w));
	*format = 8;
	*length = 0;
	*string = (XtPointer)XtMalloc(1);
	return 0;
    }

    wlen = (cinfo[jcbuf->nClause].dispp - cinfo[segn].dispp) - offset;

    /*
     * jcbuf $B$KF~$C$F$$$kJQ49%F%-%9%H$O(B null $B%?!<%_%M!<%H$5$l$F$$$J$$$N$G(B
     * $B$^$:%3%T!<$7$F(B null $B%?!<%_%M!<%H$9$k(B
     */
    wbuf = (wchar *)XtMalloc((wlen + 1) * sizeof(wchar));
    (void)bcopy((char *)(cinfo[segn].dispp + offset), (char *)wbuf, sizeof(wchar) * wlen);
    wbuf[wlen] = 0;

    /*
     * CcWnn $B%*%V%8%'%/%H$O(B COMPOUND_TEXT $B%(%s%3!<%G%#%s%0$7$+%5%]!<%H$7$J$$(B
     * COMPOUND_TEXT $B$KJQ49$9$k(B
     */
    *encoding = XA_COMPOUND_TEXT(XtDisplayOfObject(w));
    *format = 8;

    /* COMPOUND_TEXT $B$O(B \r $B$,Aw$l$J$$$N$G(B \n $B$KJQ49$7$F$*$/(B */
    for (wp = wbuf; *wp != 0; wp++) {
	if (*wp == '\r') *wp = '\n';
    }

    *length = len = convJWStoCT(wbuf, (unsigned char *)NULL, 0);
    *string = (XtPointer)XtMalloc(len + 1);
    (void)convJWStoCT(wbuf, (unsigned char *)*string, 0);

    /* wbuf $B$r(B free $B$7$F$*$/(B */
    XtFree((char *)wbuf);

    return 0;
}

/* ARGSUSED */
static int
StatusString(w, encoding, format, length, string, nchars)
Widget w;
Atom *encoding;
int *format;
int *length;
XtPointer *string;
int *nchars;
{
    ICString *seg;
    wchar *wbuf, *wp;
    int len, wlen;
    extern int convJWStoCT();

    seg = GetMode(w);
    if (seg == NULL) {
	*length = *nchars = 0;
	return -1;
    }

    wlen = seg->nchars;
    if (wlen <= 0) {
	*length = *nchars = 0;
	return -1;
    }

    /*
     * data $B$KF~$C$F$$$kJQ49%F%-%9%H$O(B null $B%?!<%_%M!<%H$5$l$F$$$J$$$+$b(B
     * $B$7$l$J$$$N$G!"$^$:%3%T!<$7$F(B null $B%?!<%_%M!<%H$9$k(B
     */
    wbuf = (wchar *)XtMalloc((wlen + 1) * sizeof(wchar));
    (void)bcopy(seg->data, (char *)wbuf, sizeof(wchar) * wlen);
    wbuf[wlen] = 0;

    /*
     * CcWnn $B%*%V%8%'%/%H$O(B COMPOUND_TEXT $B%(%s%3!<%G%#%s%0$7$+%5%]!<%H$7$J$$(B
     * COMPOUND_TEXT $B$KJQ49$9$k(B
     */
    *encoding = XA_COMPOUND_TEXT(XtDisplayOfObject(w));
    *format = 8;

    /* COMPOUND_TEXT $B$O(B \r $B$,Aw$l$J$$$N$G(B \n $B$KJQ49$7$F$*$/(B */
    for (wp = wbuf; *wp != 0; wp++) {
	if (*wp == '\r') *wp = '\n';
    }

    *length = len = convJWStoCT(wbuf, (unsigned char *)NULL, 0);
    *string = XtMalloc(len + 1);
    (void)convJWStoCT(wbuf, (unsigned char *)*string, 0);
    *nchars = seg->nchars;

    /* wbuf $B$r(B free $B$7$F$*$/(B */
    XtFree((char *)wbuf);

    return 0;
}

static Boolean
convertSelection(w, selp, targetp, typep, valp, lenp, formatp)
Widget w;
Atom *selp;
Atom *targetp;
Atom *typep;
XtPointer *valp;
unsigned long *lenp;
int *formatp;
{
    CcWnnObject	obj = findSelectionObj(w);
    int len;
    char *s, *data;
    Atom t = *targetp;

    TRACE(("CcWnn:convertSelection()\n"));

    if (obj == NULL) {
	DPRINT(("CcWnn:convertSelection(): cannot find selection object\n"));
	return False;
    }

    /* $B%+%l%s%HJ8@a$NFI$_$^$?$O4A;z$rJV$9(B */
    if (t == CachedInternAtom(XtDisplay(w), CCWNN_YOMI_ATOM, True) ||
	t == CachedInternAtom(XtDisplay(w), "TEXT", True)) {
	data = obj->ccWnn.selyomi;
    } else if (t == CachedInternAtom(XtDisplay(w), CCWNN_KANJI_ATOM, True)) {
	data = obj->ccWnn.selkanji;
    } else {
	DPRINT(("CcWnn:convertSelection(): unknown target %s\n",
		CachedGetAtomName(XtDisplay(w), t)));
	return False;
    }

    if (data != NULL) {
	len = strlen(data);
	s = XtMalloc(len + 1);
	strcpy(s, data);
    } else {
	len = 0;
	s = XtMalloc(1);
	*s = '\0';
    }

    TRACE(("CcWnn:convertSelection(): sending yomi/kanji\n"));
    *typep = XA_COMPOUND_TEXT(XtDisplay(w));
    *valp = (XtPointer)s;
    *lenp = len;
    *formatp = 8;
    return True;
}

static void
saveYomiAndKanji(obj)
CcWnnObject obj;
{
    jcConvBuf	*jcbuf = JCBUF(obj);
    int clnum = jcbuf->curClause;
    jcClause *cinfo = jcbuf->clauseInfo;
    wchar wbuf[1024];
    int wlen;
    int len;

    TRACE(("CcWnn:saveYomiAndKanji()\n"));

    if (obj->ccWnn.selyomi != NULL) {
	XtFree(obj->ccWnn.selyomi);
	obj->ccWnn.selyomi = NULL;
    }
    if (obj->ccWnn.selkanji != NULL) {
	XtFree(obj->ccWnn.selkanji);
	obj->ccWnn.selkanji = NULL;
    }

    /* $B%+%l%s%HJ8@a$NFI$_$H4A;z$r<hF@(B */
    if (clnum == jcbuf->nClause) clnum--;
    if (clnum < 0) return;

    /* $BFI$_(B */
    wlen = cinfo[clnum + 1].kanap - cinfo[clnum].kanap;
    memcpy(wbuf, cinfo[clnum].kanap, wlen * sizeof(wchar));
    wbuf[wlen] = 0;
    len = convJWStoCT(wbuf, (unsigned char *)NULL, 0);
    obj->ccWnn.selyomi = XtMalloc(len + 1);
    convJWStoCT(wbuf, obj->ccWnn.selyomi, 0);

    /* $B4A;z(B */
    wlen = cinfo[clnum + 1].dispp - cinfo[clnum].dispp;
    memcpy(wbuf, cinfo[clnum].dispp, wlen * sizeof(wchar));
    wbuf[wlen] = 0;
    len = convJWStoCT(wbuf, (unsigned char *)NULL, 0);
    obj->ccWnn.selkanji = XtMalloc(len + 1);
    convJWStoCT(wbuf, obj->ccWnn.selkanji, 0);
}
