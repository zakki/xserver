#ifndef lint
static char *rcsid = "$Id: imdata.c,v 1.7 1994/06/02 02:20:23 ishisone Exp $";
#endif
/*
 * Copyright (c) 1994  Software Research Associates, Inc.
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

#include "im.h"

#define B_GET16(p)	(((p)[0]<<8) + (p)[1])
#define B_GET32(p)	(((p)[0]<<24) + ((p)[1]<<16) + ((p)[2]<<8) + (p)[3])
#define L_GET16(p)	((p)[0] + ((p)[1]<<8))
#define L_GET32(p)	((p)[0] + ((p)[1]<<8) + ((p)[2]<<16) + ((p)[3] << 24))

#define B_PUT16(x, p)	(p)[0] = ((x)>>8) & 0xff; (p)[1] = (x) & 0xff
#define B_PUT32(x, p)	(p)[0] = ((x)>>24) & 0xff; (p)[1] = ((x)>>16) & 0xff; \
			(p)[2] = ((x)>>8) & 0xff; (p)[3] = (x) & 0xff
#define L_PUT16(x, p)	(p)[0] = (x) & 0xff; (p)[1] = ((x)>>8) & 0xff
#define L_PUT32(x, p)	(p)[0] = (x) & 0xff; (p)[1] = ((x)>>8) & 0xff; \
			(p)[2] = ((x)>>16) & 0xff; (p)[3] = ((x)>>24) & 0xff;

int
IMGetC8(conn, offset)
IMConnection *conn;
int offset;
{
    IMBuffer *ibp = IM_INBUF(conn);
    int x;

    x = *((unsigned char *)IMBUFDATA(ibp) + offset);
    return x;
}

unsigned int
IMGetC16(conn, offset)
IMConnection *conn;
int offset;
{
    IMBuffer *ibp = IM_INBUF(conn);
    unsigned char *p = (unsigned char *)IMBUFDATA(ibp) + offset;
    unsigned int x;

    if (conn->byte_order == ORDER_BIG) {
	x = B_GET16(p);
    } else {
	x = L_GET16(p);
    }

    return x;
}

int
IMGetI16(conn, offset)
IMConnection *conn;
int offset;
{
    long x;

    x = (long)IMGetC16(conn, offset);
    return (x < 32768) ? (int)x : (int)(x - 65536L);
}

unsigned long
IMGetC32(conn, offset)
IMConnection *conn;
int offset;
{
    IMBuffer *ibp = IM_INBUF(conn);
    unsigned char *p = (unsigned char *)IMBUFDATA(ibp) + offset;
    unsigned long x;

    if (conn->byte_order == ORDER_BIG) {
	x = B_GET32(p);
    } else {
	x = L_GET32(p);
    }

    return x;
}

void
IMGetString(conn, offset, buf, len)
IMConnection *conn;
int offset;
char *buf;
int len;
{
    IMBuffer *ibp = IM_INBUF(conn);
    char *p = IMBUFDATA(ibp) + offset;

    bcopy(p, buf, len);
    buf[len] = '\0';
}

void
IMPutC8(conn, x)
IMConnection *conn;
int x;
{
    IMBuffer *ibp = IM_OUTBUF(conn);
    unsigned char c = (unsigned char)x;

    IMBufAdd(ibp, (char *)&c, 1);
}

void
IMPutC16(conn, x)
IMConnection *conn;
unsigned int x;
{
    IMBuffer *ibp = IM_OUTBUF(conn);
    unsigned char *p = (unsigned char *)IMBufAlloc(ibp, 2);

    if (conn->byte_order == ORDER_BIG) {
	B_PUT16(x, p);
    } else {
	L_PUT16(x, p);
    }
}

void
IMPutC32(conn, x)
IMConnection *conn;
unsigned long x;
{
    IMBuffer *ibp = IM_OUTBUF(conn);
    unsigned char *p = (unsigned char *)IMBufAlloc(ibp, 4);

    if (conn->byte_order == ORDER_BIG) {
	B_PUT32(x, p);
    } else {
	L_PUT32(x, p);
    }
}

void
IMPutI16(conn, x)
IMConnection *conn;
int x;
{
    IMBuffer *ibp = IM_OUTBUF(conn);
    unsigned char *p = (unsigned char *)IMBufAlloc(ibp, 2);

    if (conn->byte_order == ORDER_BIG) {
	B_PUT16(x, p);
    } else {
	L_PUT16(x, p);
    }
}

void
IMPutString(conn, s, len)
IMConnection *conn;
char *s;
int len;
{
    if (len < 0) len = strlen(s);
    IMBufAdd(&conn->out_buf, s, len);
}

void
IMPutPad(conn)
IMConnection *conn;
{
    int remainder;
    int npad;
    static char padding[] = { 0, 0, 0, 0 };

    if ((remainder = IMBUFLEN(&conn->out_buf) % 4) != 0) {
	npad = 4 - remainder;
	IMBufAdd(IM_OUTBUF(conn), padding, npad);
    }
}

void
IMRewriteC16(conn, pos, x)
IMConnection *conn;
int pos;
unsigned int x;
{
    IMBuffer *ibp = IM_OUTBUF(conn);
    unsigned char p[2];

    if (conn->byte_order == ORDER_BIG) {
	B_PUT16(x, p);
    } else {
	L_PUT16(x, p);
    }
    IMBufOverwrite(ibp, pos, (char *)p, 2);
}

int
IMWritePos(conn)
IMConnection *conn;
{
    IMBuffer *ibp = IM_OUTBUF(conn);

    return IMBUFLEN(ibp);
}

int
IMPutHeader(conn, major, minor, arglen)
IMConnection *conn;
int major;
int minor;
int arglen;
{
    int offset;

#ifdef USE_WINIME
{
char *ReqNameTbl[] = 
{
	"0",
	"XIM_CONNECT",
	"XIM_CONNECT_REPLY",
	"XIM_DISCONNECT",
	"XIM_DISCONNECT_REPLY",
	"5",
	"6",
	"7",
	"8",
	"9",
	"XIM_AUTH_REQUIRED",
	"XIM_AUTH_REPLY",
	"XIM_AUTH_NEXT",
	"XIM_AUTH_SETUP",
	"XIM_AUTH_NG",
	"15",
	"16",
	"17",
	"18",
	"19",
	"XIM_ERROR",
	"21",
	"22",
	"23",
	"24",
	"25",
	"26",
	"27",
	"28",
	"29",
	"XIM_OPEN",
	"XIM_OPEN_REPLY",
	"XIM_CLOSE",
	"XIM_CLOSE_REPLY",
	"XIM_REGISTER_TRIGGERKEYS",
	"XIM_TRIGGER_NOTIFY",
	"XIM_TRIGGER_NOTIFY_REPLY",
	"XIM_SET_EVENT_MASK",
	"XIM_ENCODING_NEGOTIATION",
	"XIM_ENCODING_NEGOTIATION_REPLY",
	"XIM_QUERY_EXTENSION",
	"XIM_QUERY_EXTENSION_REPLY",
	"XIM_SET_IM_VALUES",
	"XIM_SET_IM_VALUES_REPLY",
	"XIM_GET_IM_VALUES",
	"XIM_GET_IM_VALUES_REPLY",
	"46",
	"47",
	"48",
	"49",
	"XIM_CREATE_IC",
	"XIM_CREATE_IC_REPLY",
	"XIM_DESTROY_IC",
	"XIM_DESTROY_IC_REPLY",
	"XIM_SET_IC_VALUES",
	"XIM_SET_IC_VALUES_REPLY",
	"XIM_GET_IC_VALUES",
	"XIM_GET_IC_VALUES_REPLY",
	"XIM_SET_IC_FOCUS",
	"XIM_UNSET_IC_FOCUS",
	"XIM_FORWARD_EVENT",
	"XIM_SYNC",
	"XIM_SYNC_REPLY",
	"XIM_COMMIT",
	"XIM_RESET_IC",
	"XIM_RESET_IC_REPLY",
	"66",
	"67",
	"68",
	"69",
	"XIM_GEOMETRY",
	"XIM_STR_CONVERSION",
	"XIM_STR_CONVERSION_REPLY",
	"XIM_PREEDIT_START",
	"XIM_PREEDIT_START_REPLY",
	"XIM_PREEDIT_DRAW",
	"XIM_PREEDIT_CARET",
	"XIM_PREEDIT_CARET_REPLY",
	"XIM_PREEDIT_DONE",
	"XIM_STATUS_START",
	"XIM_STATUS_DRAW",
	"XIM_STATUS_DONE",
	"XIM_PREEDITSTATE",
	"83",
	"84",
	"85",
	"86",
	"87",
	"88",
	"89",
	"90",
	"91",
	"92",
	"93",
	"94",
	"95",
	"96",
	"97",
	"98",
	"99",
	"100",
	"101",
	"102",
	"103",
	"104",
	"105",
	"106",
	"107",
	"108",
	"109",
	"110",
	"111",
	"112",
	"113",
	"114",
	"115",
	"116",
	"117",
	"118",
	"119",
	"120",
	"121",
	"122",
	"123",
	"124",
	"125",
	"126",
	"127",
	"128",
	"XIM_EXT_SET_EVENT_MASK",
	"XIM_EXT_FORWARD_KEYEVENT",
	"XIM_EXT_MOVE"
};
if (major < 132)
{
	TRACE(("XIM Request ****: major=%s, minor=%d\n", ReqNameTbl[major], minor));
} else
{
	TRACE(("XIM Request ****: major=%d, minor=%d\n", major, minor));
}
}
#endif
    offset = IMBUFLEN(&conn->out_buf);
    arglen = (arglen + 3) / 4;
#ifdef XIM_BC
    if (conn->has_length_bug) arglen *= 4;
#endif
    IMPutC8(conn, major);
    IMPutC8(conn, minor);
    IMPutC16(conn, (unsigned int)arglen);
    return offset;
}

void
IMFinishRequest(conn, offset)
IMConnection *conn;
int offset;
{
    IMBuffer *ibp = IM_OUTBUF(conn);
    unsigned int arglen;

    /*
     * Append padding bytes, if needed.
     */
    IMPutPad(conn);

    /*
     * Offset points the beginning of the request.
     * Following is a Request header which is 4-byte long.
     */
    arglen = (unsigned int)((IMBUFLEN(ibp) - offset - 4) / 4);
#ifdef XIM_BC
    if (conn->has_length_bug) arglen *= 4;
#endif

    /*
     * rewrite the length field of the request header.
     */
    IMRewriteC16(conn, offset + 2, arglen);

    IMSchedule(conn, SCHED_WRITE);
}

void
IMCancelRequest(conn, offset)
IMConnection *conn;
int offset;
{
    IMBuffer *ibp = IM_OUTBUF(conn);

    IMBufDiscard(ibp, offset - IMBUFLEN(ibp));
}

void
IMSendSimpleRequest(conn, major, minor)
IMConnection *conn;
int major;
int minor;
{
    IMPutC8(conn, major);
    IMPutC8(conn, minor);
    /*
     * This function (IMSendSimpleRequest) might be called when
     * client's byte order is yet unknown, so usually it is unwise to
     * call IMPutC16().  But in this particular case where the value
     * to be put is zero, it is perfectly ok.
     */
    IMPutC16(conn, 0);
    IMSchedule(conn, SCHED_WRITE);
}

void
IMSendRequestWithIC(conn, major, minor, icp)
IMConnection *conn;
int major;
int minor;
IMIC *icp;
{
    (void)IMPutHeader(conn, major, minor, 4);
    IMPutC16(conn, icp->im->id);
    IMPutC16(conn, icp->id);
    IMSchedule(conn, SCHED_WRITE);
}

void
IMSendError(conn, code, imid, icid, msg)
IMConnection *conn;
int code;
unsigned int imid;
unsigned int icid;
char *msg;
{
    int offset;
    int msg_len;
    unsigned int valid_flag = 0;

    msg_len = strlen(msg);

    if (imid != 0) valid_flag |= 1;
    if (icid != 0) valid_flag |= 2;

    offset = IMPutHeader(conn, XIM_ERROR, 0, 0);
    IMPutC16(conn, imid);
    IMPutC16(conn, icid);
    IMPutC16(conn, valid_flag);
    IMPutC16(conn, (unsigned int)code);
    IMPutC16(conn, (unsigned int)msg_len);
    IMPutC16(conn, 0);
    IMPutString(conn, msg, msg_len);
    IMFinishRequest(conn, offset);
}

void
IMSendBadProtocol(conn, msg)
IMConnection *conn;
char *msg;
{
    IMSendError(conn, IMBadProtocol, 0, 0, msg);
}

void
IMSendBadLength(conn, imid, icid)
IMConnection *conn;
unsigned int imid;
unsigned int icid;
{
    IMSendError(conn, IMBadSomething, imid, icid, "Bad request length");
}
