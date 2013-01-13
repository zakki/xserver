/*
 *	xwstr.c
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

/*
 * wide character string を用いてテキストを表示するためのライブラリ
 *
 * wide character の特長を生かし、最大同時に 4 種類のフォントを
 * 扱うことができる。
 *
 *	GSet	bit 表現		UJIS での割り当て
 *	----------------------------------------------
 *	G0	0xxxxxxx0xxxxxxx	ASCII
 *	G1	1xxxxxxx1xxxxxxx	漢字
 *	G2	0xxxxxxx1xxxxxxx	半角かな
 *	G3	1xxxxxxx0xxxxxxx	外字
 *
 * XWSGC XWSSetGCSet(GC gc0, GC gc1, GC gc2, GC gc3)
 *	指定された 4つの GC を組み合わせて XWSGC を作る。
 *	G0, G1, G2, G3 の文字に対してそれぞれ gc0, gc1, gc2, gc3 が
 *	使われる。引数 gc? には NULL を指定してもよい。その場合には
 *	対応するキャラクタセットの文字は書かれない。
 *	なお Xt の中で使用する時には、xtwstr.c の中の XtWSGetGCSet()
 *	を使う方がよいだろう。
 *
 * void XWSSetMapping(XWSGC gcset, int g0map, int g1map, int g2map, int g3map)
 *	G0, G1, G2, G3 の文字を描画する時にフォントの GL / GR のどちらを
 *	使用するかを設定する。0 を指定すると GL が使用され、1 を指定すると
 *	GR が使用される。-1 が指定された場合には前の設定がそのまま使われる。
 *	デフォルトの設定 (つまり XWSSetGCSet() または XtWSGetGCSet() で
 *	gcset が作られた時) はすべて GL を使用するようになっている。
 *
 * int XWSDrawString(Display *d, Drawable w, XWSGC gcset,
 *		      int x, int y, wchar *wstr, int len)
 * int XWSDrawImageString(Display *d, Drawable w, XWSGC gcset,
 *		   int x, int y, wchar *wstr, int len)
 *	それぞれ、Xlib の XDrawString() / XDrawImageString() に
 *	対応する関数。指定された位置に指定された GC セットを用いて
 *	テキストを書く。引数 len は文字列 wstr の文字数である。
 *	バイト数ではないことに注意。
 *	なお、XDrawString() などとは異なり、リターンバリューとして
 *	書いた文字の幅を返す。
 *
 * int XWSTextWidth(XWSGC gcset, wchar *wstr, int len)
 * void XWSTextExtents(XWSGC gcset, wchar *wstr, int len,
 *			int *ascent, int *descent, XCharStruct *overall)
 *	それぞれ、Xlib の XTextWidth() / XTextExtents() に対応する
 *	関数。指定された文字列の幅・大きさを返す。
 *
 * void XWSFontHeight(XWSGC gcset, wchar *wstr, int len,
 *			int *ascent, int *descent)
 *	引数 wstr で指定された文字に対応するフォントの ascent/descent の
 *	最大値を返す。wstr に NULL を指定すると、XWSGC にセットされた
 *	すべてのフォントの ascent/descent の最大値を返す。
 */

#ifndef lint
static char	*rcsid = "$Id: xwstr.c,v 2.9 1999/01/07 03:13:03 ishisone Exp $";
#endif

#include <X11/Xlib.h>
#if defined(XlibSpecificationRelease) && XlibSpecificationRelease > 4
#include <X11/Xfuncs.h>
#endif
#include "WStr.h"
#include "XWStr.h"

#ifdef __STDC__
#include <stdlib.h>
#else
extern char *malloc();
#endif

/* #define G0MASK	0x0000 */
/* #define G1MASK	0x8080 */
/* #define G2MASK	0x0080 */
/* #define G3MASK	0x8000 */

/* #define IS2B(f)	(((f)->max_byte1 > 0) || ((f)->max_char_or_byte2 > 255)) */
#define MIN(a, b)	((a) > (b) ? (b) : (a))
#define MAX(a, b)	((a) > (b) ? (a) : (b))

#ifndef NULL
#define NULL	0
#endif

#define bufsize	256

#ifdef __STDC__
/* static function prototype */
static int flushstr(Display *, Drawable,  FontEnt *, int, int,
		    wchar *, wchar *, int);
static int wsdrawstring(Display *, Drawable, XWSGC, int, int,
			wchar *, int, int);
#else
static int flushstr();
static int wsdrawstring();
#endif

XWSGC
XWSSetGCSet(Display *dpy, GC gc0)
{
	XWSGC	gcset;
	int	i;

	gcset = (XWSGC)malloc(sizeof(XWSGCSet));
	if (gcset == NULL)
		return (XWSGC)NULL;

	gcset->fe.gc = gc0;

	if (gcset->fe.gc == NULL) {
		gcset->fe.font = NULL;
	} else {
		XFontSet font;
//		gcset->fe.font = font = XQueryFont(dpy, XGContextFromGC(gcset->fe[i].gc));
		gcset->fe.flag = FONTQUERY;
//		if (IS2B(font))
//			gcset->fe[i].flag = TWOB;
	}

	return gcset;
}

int
XWSDrawString(d, w, gcset, x, y, wstr, len)
Display *d;
Drawable w;
XWSGC gcset;
int x;
int y;
wchar *wstr;
int len;
{
	return wsdrawstring(d, w, gcset, x, y, wstr, len, 0);
}

int
XWSDrawImageString(d, w, gcset, x, y, wstr, len)
Display *d;
Drawable w;
XWSGC gcset;
int x;
int y;
wchar *wstr;
int len;
{
	return wsdrawstring(d, w, gcset, x, y, wstr, len, 1);
}

void
XWSFontHeight(gcset, wstr, len, ascent, descent)
XWSGC gcset;
wchar *wstr;
int len;
int *ascent;
int *descent;
{
	FontEnt	*fep = &(gcset->fe);
	int	i, j;
	int	asc = 0;
	int	dsc = 0;
	int font_cnt;
	XFontStruct **fs;
	char **fn;

	if (fep->font) {
		font_cnt = XFontsOfFontSet(fep->font, &fs, &fn);
        for (j = 0; j < font_cnt; j++) {
            if (fs[j]->ascent > asc)
                asc = fs[j]->ascent;
            if (fs[j]->descent > dsc)
                dsc = fs[j]->descent;
        }
    }
	*ascent = asc;
	*descent = dsc;
}

int
XWSTextWidth(gcset, wstr, len)
XWSGC gcset;
wchar *wstr;
int len;
{
	int			width = 0;
	FontEnt			*fe;
    fe = &gcset->fe;
    width = XwcTextEscapement(fe->font, wstr, len);

	return width;
}

/*
 * private functions
 */

static int
wsdrawstring(d, w, gcset, x, y, wstr, len, image)
Display *d;
Drawable w;
XWSGC gcset;
int x;
int y;
wchar *wstr;
int len;
int image;
{
	int			sx = x;
	FontEnt			*fe;

    fe = &gcset->fe;
    x += flushstr(d, w, fe, x, y, wstr, wstr + len, image);

	return x - sx;
}

static int
flushstr(d, w, fe, x, y, cp0, cp1, image)
Display *d;
Drawable w;
FontEnt *fe;
int x;
int y;
wchar *cp0;
wchar *cp1;
int image;
{
	if (cp0 >= cp1 || fe->gc == NULL)
		return 0;

	if (image)
		XwcDrawImageString(d, w, fe->font, fe->gc, x, y, cp0, cp1 - cp0);
	else
		XwcDrawString(d, w, fe->font, fe->gc, x, y, cp0, cp1 - cp0);
	return XwcTextEscapement(fe->font, cp0, cp1 - cp0);
}
