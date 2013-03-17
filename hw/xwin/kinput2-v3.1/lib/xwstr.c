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
 * UCS2を用いてテキストを表示するためのライブラリ
 */

#ifndef lint
static char	*rcsid = "$Id: xwstr.c,v 2.9 1999/01/07 03:13:03 ishisone Exp $";
#endif

#include <X11/Xlib.h>
#if defined(XlibSpecificationRelease) && XlibSpecificationRelease > 4
#include <X11/Xfuncs.h>
#include <X11/Xutil.h>
#endif
#include "WStr.h"
#include "XWStr.h"
#include <iconv.h>

#ifdef __STDC__
#include <stdlib.h>
#else
extern char *malloc();
#endif


#define MIN(a, b)	((a) > (b) ? (b) : (a))
#define MAX(a, b)	((a) > (b) ? (a) : (b))

#define STRING_BUFFER_SIZE 1024

static int flushstr(Display *, Drawable,  FontEnt *, int, int,
                    wchar *, wchar *, int);
static int wsdrawstring(Display *, Drawable, XWSGC, int, int,
                        wchar *, int, int);
int convUCS2toUTF8(const wchar *pszUnicodeStr, int nSize, char* pDest);
int convUTF8toCT(Display *display, const char *str, char *xstr);

XWSGC
XWSSetGCSet(Display *dpy, GC gc0)
{
    XWSGC	gcset;

    gcset = (XWSGC)malloc(sizeof(XWSGCSet));
    if (gcset == NULL)
        return (XWSGC)NULL;

    gcset->fe.gc = gc0;

    if (gcset->fe.gc == NULL) {
        gcset->fe.font = NULL;
    } else {
//		gcset->fe.font = font = XQueryFont(dpy, XGContextFromGC(gcset->fe[i].gc));
        gcset->fe.flag = FONTQUERY;
//		if (IS2B(font))
//			gcset->fe[i].flag = TWOB;
    }

    return gcset;
}

int
XWSDrawString(Display *d, Drawable w, XWSGC gcset, int x, int y,
              wchar *wstr, int len)
{
    return wsdrawstring(d, w, gcset, x, y, wstr, len, 0);
}

int
XWSDrawImageString(Display *d, Drawable w, XWSGC gcset, int x, int y,
                   wchar *wstr, int len)
{
    return wsdrawstring(d, w, gcset, x, y, wstr, len, 1);
}

void
XWSFontHeight(XWSGC gcset, wchar *wstr, int len, int *ascent, int *descent)
{
    FontEnt *fep = &(gcset->fe);
    int j;
    int asc = 0;
    int dsc = 0;
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
XWSTextWidth(XWSGC gcset, wchar *wstr, int len)
{
    int			width = 0;
    FontEnt			*fe;
    static char szString[STRING_BUFFER_SIZE];
    int ulen = convUCS2toUTF8(wstr, len, szString);
    fe = &gcset->fe;
    width = Xutf8TextEscapement(fe->font, szString, ulen);

    return width;
}

/*
 * private functions
 */

static int
wsdrawstring(Display *d, Drawable w, XWSGC gcset, int x, int y,
             wchar *wstr, int len, int image)
{
    int			sx = x;
    FontEnt			*fe;

    fe = &gcset->fe;
    x += flushstr(d, w, fe, x, y, wstr, wstr + len, image);

    return x - sx;
}

static int
flushstr(Display *d, Drawable w, FontEnt *fe, int x, int y,
         wchar *cp0, wchar *cp1, int image)
{
    if (cp0 >= cp1 || fe->gc == NULL)
        return 0;
    static char szString[STRING_BUFFER_SIZE];
    int len = convUCS2toUTF8(cp0, cp1 - cp0, szString);

    if (image)
        Xutf8DrawImageString(d, w, fe->font, fe->gc, x, y, szString, len);
    else
        Xutf8DrawString(d, w, fe->font, fe->gc, x, y, szString, len);
    return Xutf8TextEscapement(fe->font, szString, len);
}


static int
_Local_wcstombs(
    iconv_t cd,
    const char *str,
    int flen,
    char *ustr,
    int len)
{
    XPointer from, to;
    size_t from_left, to_left;
    int ret, status;

    from = (XPointer) str;
    from_left = flen;
    to = (XPointer) ustr;
    to_left = len;

    while(from_left > 0) {
        status = iconv(cd, (char **)&from, &from_left, &to, &to_left);
        if (status < 0) {
            /* 表現できない文字がきたので'？'で埋める */
            //*to++ = 0x00;
            from += 2;
            from_left -= 2;
            to_left -= 2;
        }
    }

    ret = len - to_left;
    if (ustr && to_left > 0)
        ustr[ret] = '\0';

    return ret;
}

int convUCS2toUTF8(const wchar *pszUnicodeStr, int nSize, char* pDest)
{
    iconv_t cd = (iconv_t)-1;
    int nLen = -1;  // 文字数

    // コード変換コンバータの準備
    cd = iconv_open("UTF-8", "UCS-2-INTERNAL");
    if (cd == (iconv_t)-1) {
        return nLen;
    }

    nLen = _Local_wcstombs(cd, pszUnicodeStr, nSize * 2, pDest, STRING_BUFFER_SIZE);
    iconv_close(cd);

    if (nLen == -1) {
        return nLen;
    } else {
        return nLen;
    }
}

/* convUTF8toCT -- UTF-8 String -> COMPOUND_TEXT */
int convUTF8toCT(Display *display, const char *str, char *xstr)
{
    XTextProperty prop;
    char* list[2];
    int ret;

    list[0] = str;
    list[1] = NULL;
    prop.value = NULL;
    prop.nitems = 0;

    ret = Xutf8TextListToTextProperty(display, list, 1, XCompoundTextStyle, &prop);
    if (ret != 0) {
        return 0;
    }

    if (xstr) {
        memcpy(xstr, prop.value, prop.nitems);
        xstr[prop.nitems] = '\0';
    }
    XFree(prop.value);

    return prop.nitems;
}
