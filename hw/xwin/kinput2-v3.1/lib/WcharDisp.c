#ifndef lint
static char *rcsid = "$Id: WcharDisp.c,v 1.23 1994/10/27 08:29:05 ishisone Exp $";
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
#include <X11/Xmu/CharSet.h>
#include "CachedAtom.h"
#include "WcharDispP.h"
#include "LocalAlloc.h"

#define DEBUG_VAR debug_wcharDisplay
#include "DebugPrint.h"

/*
 * R5 servers implicitly changes font properties when an alias name is
 * used.  The character set properties (CHARSET_REGISTRY and
 * CHARSET_ENCODING) are taken from the XLFD fontname.  So if the
 * fontname is represented in lower-case letters, for example:
 *    -jis-fixed-medium-r-normal--16-110-100-100-c-160-jisx0208.1983-0
 *       (this example is taken from R5 fonts/misc/fonts.alias file)
 * then, the value of CHARSET_REGISTRY becomes "jisx0208.1983",
 * instead of the registered charset name "JISX0208.1983".
 * The following flag forces kinput2 to accept these invalid lower
 * case charset names as well.
 */
#define ALLOW_LOWERCASE_CHARSET_NAME

static XtResource resources[] = {
#define offset(field) XtOffset(WcharDisplayObject, wcharDisplay.field)
    { XtNfontSet, XtCFontSet, XtRFontSet, sizeof (XFontSet),
	offset(defaultfont), XtRString, XtDefaultFontSet },
#undef offset
};

static void ClassInitialize(void);

static void Initialize(Widget req, Widget new, ArgList args, Cardinal *num_args);
static void Destroy(Widget w);
static Boolean SetValues(Widget cur, Widget req, Widget wid, ArgList args, Cardinal *num_args);

static void GetGC(WcharDisplayObject obj);

static int StringWidth(Widget w, ICString *seg, int start, int end);
static int LineHeight(Widget w, Position *ascentp);
static void DrawString(Widget w, Widget canvas, ICString *seg, int start, int end, int x, int y);
static int MaxChar(Widget w, ICString *seg, int start, int width);

static int countControlChars(const wchar *wstr, int len);
static void expandControlChars(const wchar *org, int orglen, wchar *res);
/* static int charWidth(int c, XWSGC gcset); */

WcharDisplayClassRec wcharDisplayClassRec = {
  { /* object fields */
    /* superclass		*/	(WidgetClass)&convDisplayClassRec,
    /* class_name		*/	"WcharDisplay",
    /* widget_size		*/	sizeof(WcharDisplayRec),
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
  { /* convDisplay fields */
    /* StringWidth		*/	StringWidth,
    /* LineHeight		*/	LineHeight,
    /* DrawString		*/	DrawString,
    /* MaxChar			*/	MaxChar,
    /* DrawCursor		*/	XtInheritDrawCursor,
    /* GetCursorBounds		*/	XtInheritGetCursorBounds,
  }
};

WidgetClass wcharDisplayObjectClass = (WidgetClass)&wcharDisplayClassRec;

/* ARGSUSED */
static void
ClassInitialize(void)
{
}

/* ARGSUSED */
static void
Initialize(Widget req, Widget new, ArgList args, Cardinal *num_args)
{
    WcharDisplayObjectClass class = (WcharDisplayObjectClass)XtClass(new);
    WcharDisplayObject obj = (WcharDisplayObject)new;
    int i;
    static char stipple_bits[] = {
	0x55, 0x55, 0x00, 0x00, 0xaa, 0xaa, 0x00, 0x00, 0x55, 0x55, 0x00, 0x00,
	0xaa, 0xaa, 0x00, 0x00, 0x55, 0x55, 0x00, 0x00, 0xaa, 0xaa, 0x00, 0x00,
	0x55, 0x55, 0x00, 0x00, 0xaa, 0xaa, 0x00, 0x00
    };

    /* create a Stipple Bitmap (for drawing CurrentSubSegment) */
    obj->wcharDisplay.stipple =
      XCreateBitmapFromData(XtDisplayOfObject((Widget)obj),
			    RootWindowOfScreen(XtScreenOfObject((Widget)obj)),
			    stipple_bits, 16, 16);

    obj->wcharDisplay.font = obj->wcharDisplay.defaultfont;

    GetGC(obj);
}

static void
Destroy(Widget w)
{
    WcharDisplayObject obj = (WcharDisplayObject)w;

    XtWSReleaseGCSet(w, obj->wcharDisplay.gcset_normal);
    XtWSReleaseGCSet(w, obj->wcharDisplay.gcset_rev);
    XFreePixmap(XtDisplayOfObject((Widget)obj), obj->wcharDisplay.stipple);
}

/* ARGSUSED */
static Boolean
SetValues(Widget cur, Widget req, Widget wid, ArgList args, Cardinal *num_args)
{
    WcharDisplayObject old = (WcharDisplayObject)cur;
    WcharDisplayObject new = (WcharDisplayObject)wid;
    Boolean redraw = False;
    int i;

#define wd wcharDisplay 
    if (new->wd.defaultfont != old->wd.defaultfont &&
        new->wd.font == old->wd.defaultfont) {
        redraw = True;
    }
    if (redraw ||
	new->convDisplay.foreground != old->convDisplay.foreground ||
	new->convDisplay.background != old->convDisplay.background) {
	XtWSReleaseGCSet(wid, new->wcharDisplay.gcset_normal);
	XtWSReleaseGCSet(wid, new->wcharDisplay.gcset_rev);
	GetGC(new);
	redraw = True;
    }
#undef wd

    return redraw;
}

static void
GetGC(WcharDisplayObject obj)
{
    XtGCMask mask = GCForeground|GCBackground;
    XGCValues values;
    int ascent, descent;

    values.function = GXcopy;
    values.foreground = obj->convDisplay.foreground;
    values.background = obj->convDisplay.background;
    mask = GCFunction|GCForeground|GCBackground;
    obj->wcharDisplay.gc_normal = XtGetGC((Widget)obj, mask, &values);

    values.foreground = obj->convDisplay.background;
    values.fill_style = FillStippled;
    values.stipple = obj->wcharDisplay.stipple;
    mask = GCFunction|GCForeground|GCFillStyle|GCStipple;
    obj->wcharDisplay.gc_stipple = XtGetGC((Widget)obj, mask, &values);

    mask = GCFunction|GCForeground|GCBackground;
    values.function = GXcopy;
    values.foreground = obj->convDisplay.foreground;
    values.background = obj->convDisplay.background;
    obj->wcharDisplay.gcset_normal = XtWSGetGCSet((Widget)obj, mask, &values,
						  obj->wcharDisplay.font);
    values.foreground = obj->convDisplay.background;
    values.background = obj->convDisplay.foreground;
    obj->wcharDisplay.gcset_rev = XtWSGetGCSet((Widget)obj, mask, &values,
					       obj->wcharDisplay.font);

    XWSFontHeight(obj->wcharDisplay.gcset_normal, NULL, 0, &ascent, &descent);
    obj->wcharDisplay.ascent = ascent;
    obj->wcharDisplay.fontheight = ascent + descent;
}

static int
StringWidth(Widget w, ICString *seg, int start, int end)
{
    WcharDisplayObject obj = (WcharDisplayObject)w;
    wchar *wstr;
    int len = seg->nchars;
    int nctl;
    int width;

    if (end < 0 || len < end) end = len;
    if (start >= end || start >= len) return 0;

    wstr = (wchar *)seg->data + start;
    len = end - start;

    /*
     * searching for control characters -- if found, convert them
     * into '^?' format for readability.
     */
    if ((nctl = countControlChars(wstr, len)) == 0) {
	/* no control characters */
	width = XWSTextWidth(obj->wcharDisplay.gcset_normal, wstr, len);
    } else {
	wchar *s = (wchar *)LOCAL_ALLOC(sizeof(wchar) * (len + nctl));

	expandControlChars(wstr, len, s);
	width = XWSTextWidth(obj->wcharDisplay.gcset_normal, s, len + nctl);
	LOCAL_FREE(s);
    }
    return width;
}

static int
LineHeight(Widget w, Position *ascentp)
{
    WcharDisplayObject obj = (WcharDisplayObject)w;

    if (ascentp != NULL) *ascentp = obj->wcharDisplay.ascent;
    return obj->wcharDisplay.fontheight;
}

static void
DrawString(Widget w, Widget canvas, ICString *seg, int start, int end,
           int x, int y)
{
    WcharDisplayObject obj = (WcharDisplayObject)w;
    wchar *wstr;
    XWSGC gcset;
    int len = seg->nchars;
    int nctl;
    Display *dpy = XtDisplay(canvas);
    Window win = XtWindow(canvas);
    int width;
    int attr;

    if (end < 0 || len < end) end = len;
    if (start >= end || start >= len) return;

    wstr = (wchar *)seg->data + start;
    len = end - start;

#define STIPPLED	1
#define UNDERLINED	2

    if (seg->attr == ICAttrNormalString) {
	gcset = obj->wcharDisplay.gcset_normal;
	attr = 0;
    } else if (seg->attr & ICAttrConverted) {
	/* converted segment */
	if (seg->attr & ICAttrCurrentSegment) {
	    gcset = obj->wcharDisplay.gcset_rev;
	    attr = 0;
	} else if (seg->attr & ICAttrCurrentSubSegment) {
	    gcset = obj->wcharDisplay.gcset_rev;
	    attr = STIPPLED;
	} else {
	    gcset = obj->wcharDisplay.gcset_normal;
	    attr = 0;
	}
    } else {	/* ICAttrNotConverted */
	gcset = obj->wcharDisplay.gcset_normal;
	attr = UNDERLINED;
    }

    /* if ((nctl = countControlChars(wstr, len)) == 0) { */
	width = XWSDrawImageString(dpy, win, gcset,
				   x, y + obj->wcharDisplay.ascent,
				   wstr, len);
    /* } else { */
	/* wchar *s = (wchar *)LOCAL_ALLOC((len + nctl) * sizeof(wchar)); */
	/* expandControlChars(wstr, len, s); */
	/* width = XWSDrawImageString(dpy, win, gcset, */
	/* 			   x, y + obj->wcharDisplay.ascent, */
	/* 			   s, len + nctl); */
	/* LOCAL_FREE(s); */
    /* } */

    if (attr == UNDERLINED) {
	int uloffset = 1;
	int descent = obj->wcharDisplay.fontheight - obj->wcharDisplay.ascent;

	if (descent <= 1) {
	    /* font descent is 0 or 1 */
	    uloffset = descent - 1;
	}
	XDrawLine(dpy, win, obj->wcharDisplay.gc_normal,
		  x, y + obj->wcharDisplay.ascent + uloffset,
		  x + width - 1, y + obj->wcharDisplay.ascent + uloffset);
    } else if (attr == STIPPLED) {
	XFillRectangle(dpy, win, obj->wcharDisplay.gc_stipple, x, y,
		       (unsigned int)width,
		       (unsigned int)obj->wcharDisplay.fontheight);
    }
}

static int
MaxChar(Widget w, ICString *seg, int start, int width)
{
    int cwidth;
    int chars;

    chars = 1;
    while (chars <= width) {
        cwidth = StringWidth(w, seg, 0, chars);
        if (width < cwidth) break;
        chars++;
        if (width == cwidth) break;
    }
    /* while (sp < ep) { */
    /*     cwidth = charWidth(*sp++, gcset); */
    /*     if (width < cwidth) break; */
    /*     chars++; */
    /*     if ((width -= cwidth) == 0) break; */
    /* } */
    return chars;
}


/* countControlChars -- count number of control characters in a string */
static int
countControlChars(const wchar *wstr, int len)
{
    register wchar *end = wstr + len;
    register int n = 0;

    while (wstr < end) {
	if (*wstr < 0x20 || *wstr == 0x7f) n++;
	wstr++;
    }
    return n;
}

/* expandControlChars -- convert control characters into '^?' format */
static void
expandControlChars(const wchar *org, int orglen, wchar *res)
{
    wchar *end;

    for (end = org + orglen; org < end; org++) {
	if (*org < 0x20 || *org == 0x7f) {
	    *res++ = '^';
	    *res++ = *org ^ 0x40;
	} else {
	    *res++ = *org;
	}
    }
}

#define WITHIN_RANGE_2D(row, col, fs) \
    ((fs)->min_byte1 <= (row) && (row) <= (fs)->max_byte1 && \
     (fs)->min_char_or_byte2 <= (col) && (col) <= (fs)->max_char_or_byte2)

#define WITHIN_RANGE(c, fs) \
    ((fs)->min_char_or_byte2 <= (c) && (c) <= (fs)->max_char_or_byte2)

#define CHAR_INFO_2D(row, col, fs) \
    ((fs)->per_char + ((row) - (fs)->min_byte1) * \
     ((fs)->max_char_or_byte2 - (fs)->min_char_or_byte2 + 1) + \
     ((col) - (fs)->min_char_or_byte2))

#define CHAR_INFO(c, fs) \
    ((fs)->per_char + ((c) - (fs)->min_char_or_byte2))

#define CHAR_EXIST(csp) \
    ((csp)->width != 0 || ((csp)->rbearing != 0) || ((csp)->lbearing != 0))


/*
 * jpWcharDisplay definition
 *
 *	character set assignment for Japanese wchar:
 *	    G0: ascii (ISO8859/1 left-half)
 *	    G1: kanji (JISX0208)
 *	    G2: half-width kana (JISX0201 right-half)
 *	    G3: unused
 */

#define JPFONT_ASCII	"-Misc-Fixed-Medium-R-*--14-*-*-*-C-*-ISO8859-1,-Misc-Fixed-Medium-R-*--14-*-*-*-C-*-JISX0208.1983-0,-Misc-Fixed-Medium-R-*--14-*-*-*-C-*-JISX0201.1976-0"

static XtResource jpresources[] = {
    /* only override superclass's default */
#define offset(field) XtOffset(JpWcharDisplayObject, wcharDisplay.field)
    { XtNfontSet, XtCFontSet, XtRFontSet, sizeof (XFontSet),
	offset(defaultfont), XtRString, JPFONT_ASCII },
#undef offset
};

JpWcharDisplayClassRec jpWcharDisplayClassRec = {
  { /* object fields */
    /* superclass		*/	(WidgetClass)&wcharDisplayClassRec,
    /* class_name		*/	"JpWcharDisplay",
    /* widget_size		*/	sizeof(JpWcharDisplayRec),
    /* class_initialize		*/	NULL,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	NULL,
    /* initialize_hook		*/	NULL,
    /* obj1			*/	NULL,
    /* obj2			*/	NULL,
    /* obj3			*/	0,
    /* resources		*/	jpresources,
    /* num_resources		*/	XtNumber(jpresources),
    /* xrm_class		*/	NULLQUARK,
    /* obj4			*/	FALSE,
    /* obj5			*/	FALSE,
    /* obj6			*/	FALSE,
    /* obj7			*/	FALSE,
    /* destroy			*/	NULL,
    /* obj8			*/	NULL,
    /* obj9			*/	NULL,
    /* set_values		*/	NULL,
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
  { /* convDisplay fields */
    /* StringWidth		*/	XtInheritStringWidth,
    /* LineHeight		*/	XtInheritLineHeight,
    /* DrawString		*/	XtInheritDrawString,
    /* MaxChar			*/	XtInheritMaxChar,
    /* DrawCursor		*/	XtInheritDrawCursor,
    /* GetCursorBounds		*/	XtInheritGetCursorBounds,
  },
};

WidgetClass jpWcharDisplayObjectClass = (WidgetClass)&jpWcharDisplayClassRec;
