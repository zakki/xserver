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

#ifndef lint
static  char    rcsid[] = "$Id: if.c,v 1.3 1999/08/24 09:05:26 ishisone Exp $" ;
#endif  /* !lint */

#include	<stdio.h>
#include	<X11/IntrinsicP.h>
#include	<X11/StringDefs.h>
#include	<X11/Xmu/Atoms.h>
#include	<X11/keysym.h>

#include	<Xatoklib.h>
#include	<key_def.h>

/* Page */
/*
 * [関数名]
 *              atokModeSet( )
 * [表題]
 *              モードの設定
 * [呼出形式]
 *      	int atokModeSet( _XatokRecPtr acomm, WORD ceCmd, WORD value )
 *
 * [引数]
 *              型           : 名   称 : IO : 説      明
 *		_XatokRecPtr : acomm   : i/o : kinput2用変換バッファ
 *		WORD         : ceCmd   : i   : CE コマンド番号
 *		WORD         : value   : i   : 値
 *
 *
 * [返り値]
 *		CEの処理結果
 * [使用関数]
 *		AtokClntCommunicate( )
 * [機能]
 *		CEのモードを行なうAPIを呼び出す。
 *		value がATCEPRVAPI_READCMD の時は、
 *		現在の設定値の読み込みである。
 */
WORD atokModeSet(acomm, ceCmd, value)
_XatokRecPtr	acomm;
WORD		ceCmd;
WORD		value;
{
	if ( value  == ATCEPRVAPI_READCMD ) {
		acomm->NETrqst.AtCEPrvAPIOutinf = ATCEPRVAPI_READCMD ;
	}
	else {
		acomm->NETrqst.AtCEPrvAPIOutinf = value ;
	}
	acomm->NETrqst.ceCmd = ceCmd ; ;
	AtokClntCommunicate( acomm->sock, &acomm->NETrqst, &acomm->NETrslt ) ;
		
	return acomm->NETrslt.header.CEOutinf ;
}

/* Page */
/*
 * [関数名]
 *              atokConnect( )
 * [表題]
 *              ATOK12X サーバとの接続
 * [呼出形式]
 *      int atokConnect( char *server, char *port, char *conf, char  *style,
 *				_XatokRecPtr acomm )
 *
 * [引数]
 *              型        : 名       称   : IO : 説      明
 *		char         : *server : i   : ATOK12x 稼働ホスト名
 *		char         : *port   : i   : 接続サービス名
 *		char         : *conf   : i   : conf ファイル名
 *		char         : *style  : i   : style ファイル名
 *		_XatokRecPtr : acomm   : i/o : kinput2用変換バッファ
 *
 *
 * [返り値]
 *		0 未満 : 接続エラー
 *		 ０以上 :接続したSOCKET記述子
 *
 * [使用関数]
 *		AtokClntConnect( )
 *		XatokBufferConvert( )
 * [機能]
 *		新規に ATO１２xとの接続を行なう。
 *		接続はプロセスで１つでよい。
 *		接続が出来た場合には、各モードの現在の設定値を取得する。
 */
int atokConnect(server, port, conf, style, acomm)
char	*server;
char	*port;
char	*conf;
char	*style;
_XatokRecPtr	acomm;
{
	wchar	dummy[128] ;
	int	status ;
	int	sock ;

	sock = AtokClntConnect( server, port, conf, style,
		&acomm->NETrqst, &acomm->NETrslt ) ;
	if ( sock < 0 ) {
		return sock ;
	}

	/*
	 * 接続に成功したら現在のモードを取得する。
	 *モード文字列に関しては、接続時にリターンされる。
	 */
	XatokBufferConvert( acomm, ( WORD ) -1,  &acomm->NETrslt, 
		dummy, &status, &status, &status, &status ) ;

	acomm->sock = sock ;

	/*
	 * 現在の設定値を取得する。
	 */
	acomm->mode.modeKANDSP = atokModeSet( acomm, 
			IME_PRV_SETKANDSP, ATCEPRVAPI_READCMD ) ;
	acomm->mode.modeHEN = atokModeSet( acomm, 
			IME_PRV_SETHENMODE, ATCEPRVAPI_READCMD ) ;
	acomm->mode.modeGAKU = atokModeSet( acomm, 
			IME_PRV_SETGAKUMODE, ATCEPRVAPI_READCMD ) ;
	acomm->mode.modeKOTE = atokModeSet( acomm, 
			IME_PRV_SETKOTEMODE, ATCEPRVAPI_READCMD ) ;
	acomm->mode.modeOKURI = atokModeSet( acomm, 
			IME_PRV_SETOKURIMODE, ATCEPRVAPI_READCMD ) ;
	acomm->mode.modeROT = atokModeSet( acomm, 
			IME_PRV_SETROTMODEF, ATCEPRVAPI_READCMD ) ;

	return sock ;
}
/* Page */
/*
 * [関数名]
 *		atokCEOpen( )
 * [表題]
 *              CONTEXTの作成
 * [呼出形式]
 *      int atokCEOpen( int sock, int clntid, _XatokRecPtr acomm )
 *
 * [引数]
 *              型        : 名       称   : IO : 説      明
 * 		int          : sock    : i   : ATOK12X との接続SOCKET
 *		int          : clntid  : i   : クライアントID
 *		_XatokRecPtr : acomm   : i/o : kinput2用変換バッファ
 *
 * [返り値]
 *		-1 : CONTEXT作成エラー
 *		 1 : CONTEXT 作成成功
 *
 * [使用関数]
 *		AtokClntCEOpen( )
 *		XatokBufferConvert( )
 * [機能]
 *		既に接続されていて、クライアントIDが割り当てられている
 *		セッションに対し、ATOK12CE とのCONTEXTを追加作成する。
 */

int atokCEOpen(sock, clntid, acomm)
int	sock;
int	clntid;
_XatokRecPtr	acomm;
{
	wchar	dummy[128] ;	/* BufferConvert のための TMP */
	int	status ;	/* BufferConvert のための TMP */

	if ( AtokClntCEOpen( sock, clntid, &acomm->NETrqst, &acomm->NETrslt ) < 0 ) {
		return -1 ;
	}

	XatokBufferConvert( acomm, ( WORD ) -1,  &acomm->NETrslt, 
		dummy, &status, &status, &status, &status ) ;

	acomm->mode.modeKANDSP = atokModeSet( acomm, 
			IME_PRV_SETKANDSP, ATCEPRVAPI_READCMD ) ;
	acomm->mode.modeHEN = atokModeSet( acomm, 
			IME_PRV_SETHENMODE, ATCEPRVAPI_READCMD ) ;
	acomm->mode.modeGAKU = atokModeSet( acomm, 
			IME_PRV_SETGAKUMODE, ATCEPRVAPI_READCMD ) ;
	acomm->mode.modeKOTE = atokModeSet( acomm, 
			IME_PRV_SETKOTEMODE, ATCEPRVAPI_READCMD ) ;
	acomm->mode.modeOKURI = atokModeSet( acomm, 
			IME_PRV_SETOKURIMODE, ATCEPRVAPI_READCMD ) ;
	acomm->mode.modeROT = atokModeSet( acomm, 
			IME_PRV_SETROTMODEF, ATCEPRVAPI_READCMD ) ;

	return 1 ;
}


/* Page */
/* 
 * [関数名]
 *		atokKanjiOn
 * [表題]
 *		漢字ON
 * [呼出形式]
 *		int	atokKanjiOn( _XatokRecPtr acomm )
 * [引数]
 *		型        : 名       称   : IO : 説      明
 *		_XatokRecPtr : acomm   : i/o : kinput2用変換バッファ
 *	
 * [返り値]
 *		0 未満 : エラー
 *		0      : OK
 *		
 * [使用関数]
 *		atokClntCommunicate( )
 * [機能]
 *		CE のモードを漢字ONとする。
 *
 */
int atokKanjiOn(acomm)
_XatokRecPtr	acomm;
{
	int	status ;

	acomm->convStatus = M_KEYCONV ;
	acomm->convStatus = acomm->oldStatus ;
	acomm->NETrqst.ceCmd = IME_PRV_KANJION ;
	acomm->curAux = acomm->menuAux ;

	status = AtokClntCommunicate( acomm->sock, 
		&acomm->NETrqst, &acomm->NETrslt ) ;
	
	return status ;
}

int atokKanjiOff(acomm)
_XatokRecPtr	acomm;
{
	int	status ;

	acomm->oldStatus = acomm->convStatus ;
	acomm->convStatus = M_NOKEYCONV ;
	acomm->NETrqst.ceCmd = IME_PRV_KANJIOFF ;
	acomm->curAux = acomm->menuAux ;

	status = AtokClntCommunicate( acomm->sock, 
		&acomm->NETrqst, &acomm->NETrslt ) ;
	
	return status ;
}

/* Page */
/*
 * [関数名]
 *		atokWordStyleList( )
 * [表題]
 *              品詞リストの取得
 * [呼出形式]
 *		int atokWordStyleList( _XatokRecPtr acomm )
 *
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		_XatokRecPtr : acomm   : i/o : kinput2用変換バッファ
 *
 * [返り値]
 *		-1 : 送信エラー
 *		 N : 品詞の個数
 *
 * [使用関数]
 *		atokClntCommunicate( )
 * [機能]
 *		辞書に登録するための品詞リストを取得する。
 *		品詞リストは S-JIS で送られて来る。kinput2 内部では
 *		EUC で管理しているため、ここでコード変換を行なう。
 *		品詞リストは、候補リストを管理するのと同じバッファで
 *		管理する。そのバッファが取得されていない場合には、
 *		新たに取得する。
 */
int atokWordStyleList(acomm)
_XatokRecPtr	acomm;
{
	int		size ;
	int		i, j, n ;
	int		len ;
	ATOKDEHINSTR	*sptr ;
	char		euc[BUFSIZ] ;

	if ( acomm->hinsiPtr ) {
		goto make_cand ;
	}

	acomm->NETrqst.ceCmd = IME_PRV_WORDSYLECOUNT ;
	if ( AtokClntCommunicate( acomm->sock, &acomm->NETrqst, &acomm->NETrslt ) < 0 ) {
		return -1 ;
	}

#ifdef	ATOK_DEBUG
	printf( "CHECK %d\n", acomm->NETrslt.header.CEOutinf ) ;
#endif	/* ATOK_DEBUG */
	acomm->hinsiNum = acomm->NETrslt.header.CEOutinf ;
	acomm->NETrqst.ceCmd = IME_PRV_WORDSTYLELIST ;
	acomm->NETrqst.AtCEIncode = acomm->hinsiNum ;
#ifdef	ATOK_DEBUG
printf( "\311\312\273\354\301\355\277\364 %d\n"/*=品詞総数 %d\n=*/, acomm->hinsiNum ) ;
#endif	/* ATOK_DEBUG */
	AtokClntCommunicate( acomm->sock, &acomm->NETrqst, &acomm->NETrslt ) ;

	size = sizeof( ATOKDEHINSTR ) * acomm->hinsiNum ;
	acomm->hinsiPtr = ( ATOKDEHINSTR * )malloc( size ) ;
	sptr = acomm->NETrslt.header.wordPtr ;
	memcpy( acomm->hinsiPtr, sptr, size ) ;
make_cand:
	size = 0 ;
	/*
	 * S-JIS で送られてきた品詞リストデータを kinput2側で扱えるように
	 * EUC に変換する。
	 */
	for( i = 0 ; i < acomm->hinsiNum ; i++ ) {
		size += s2e( acomm->hinsiPtr[i].szHinsiStr, 
			strlen( acomm->hinsiPtr[i].szHinsiStr ),
			euc, sizeof( euc )) ;
	}
	/*
	 * 品詞リストを管理するバッファは候補リストを管理するバッファと
	 * 同じ領域とし、既に確保されている場合にはその領域は確保しない。
	 * バッファを共有出来るのは、候補を出力中に、品詞リストを
	 * 出力する事がないからであり、同時に出力する事がある場合には
	 * 領域を別にとるように修正する必要がある。
	 */
	if ( acomm->kohoSize == 0 ) {
		acomm->kohoStrPtr =  ( WCHAR * )malloc( size * sizeof( WCHAR )) ;
		acomm->kohoLenPtr = ( BYTE * )malloc( acomm->hinsiNum * sizeof( BYTE )) ;
		acomm->kohoSfxPtr = ( BYTE * )malloc( acomm->hinsiNum * sizeof( BYTE )) ;
		acomm->kohoSize = acomm->hinsiNum ;
		acomm->kohoStrLen = size ;
	}
	if ( acomm->kohoSize < acomm->hinsiNum ) {
		acomm->kohoLenPtr = ( BYTE * )realloc( acomm->kohoLenPtr, 
			acomm->hinsiNum  * sizeof( BYTE )) ;
		acomm->kohoSfxPtr = ( BYTE * )realloc( acomm->kohoSfxPtr, 
			acomm->hinsiNum * sizeof( BYTE )) ;
		acomm->kohoSize = n ;
	}
	if ( acomm->kohoStrLen < size ) {
		acomm->kohoStrPtr = ( WCHAR * )realloc( acomm->kohoStrPtr,
			size * sizeof( WCHAR )) ;
		acomm->kohoStrLen = size ;
	}
	len = 0 ;
	for( i = 0 ; i < acomm->hinsiNum ; i++ ) {
		n = strlen( acomm->hinsiPtr[i].szHinsiStr ) ;
		n = sjis2ucs( acomm->hinsiPtr[i].szHinsiStr, n, 
			&acomm->kohoStrPtr[len], size -len, 0 ) ;
		acomm->kohoLenPtr[i] = n ;
		len += n ;
	}
	acomm->kohoNum2 = acomm->hinsiNum ;
	acomm->kohoPos = 0 ;
		
	return acomm->hinsiNum ;
}

/* Page */
/*
 * [関数名]
 *              atokCandidate( )
 * [表題]
 *              候補リストの取得／設定
 * [呼出形式]
 *      	int atokCandidate( _XatokRecPtr acomm, int nbr )
 *
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		_XatokRecPtr : acomm   : i/o : kinput2用変換バッファ
 *		int          :   nbr   : i   : 設定する番号
 *
 * [返り値]
 *		0 未満 : 送信エラー
 *		 0 : OK
 *
 * [使用関数]
 *		atokClntCommunicate( )
 * [機能]
 *		候補リストより、現在選択されている候補番号を設定する。
 */
int atokCandidate(acomm, nbr)
_XatokRecPtr	acomm;
int		nbr;
{
	int	status ;
	/*
	 * if 文必要なし
	 */
	if ( nbr != ATCEPRVAPI_READCMD ) {
		acomm->NETrqst.AtCEIncode = nbr ;
	}
	else {
		acomm->NETrqst.AtCEIncode = ATCEPRVAPI_READCMD ;
	}
	acomm->NETrqst.ceCmd = IME_PRV_CANDLIST ;

	status = AtokClntCommunicate( acomm->sock, &acomm->NETrqst, &acomm->NETrslt ) ;

	return status ;
}

/* Page */
/*
 * [関数名]
 *		atokWordRegister( )
 * [表題]
 *              単語の登録
 * [呼出形式]
 *		atokWordRegister( _XatokRecPtr acomm, wchar *word,
 *			int wlen, wchar	*yomi, int ylen, int hinshi )
 *
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		_XatokRecPtr : acomm   : i/o : kinput2用変換バッファ
 *		wchar        : *word   : i   : 登録単語
 *		int          : wlen    : i   : 登録単語長さ
 *		wchar        : *yomi   : i   : 単語よみ
 *		int          :  ylen   : i   : 単語よみながさ
 *		int          : hinshi  : i   : 品詞番号
 *
 *
 * [返り値]
 *		-1 : 送信できない。
 *	 	 0 : 正常終了
 *		 1 : この読みではこれ以上登録不可
 *		 2 : これ以上登録不可
 *		 3 : 辞書設定不正
 *		 4 : 表記に不適切な文字を含む
 *		 5 : 登録できない。
 *		 6 : 読みに不適切な文字を含む
 *		 7 : 既に登録されている。
 *
 * [使用関数]
 *		atokClntCommunicate( )
 *		wcs2euc( )
 *		euc2ucs( )
 * [機能]
 *		単語登録を行なう。
 *		単語表記、よみは それぞて UCSコードで登録する。
 *		kinput2 で管理している時は
 *		単語表記は WCHAR, 読みコードはEUCであるのでそれぞれ
 *		UCS への変換を一度行なう。
 *		品詞番号は、１から始まる物である。
 */
	
int atokWordRegister(acomm, word, wlen, yomi, ylen, hinshi)
_XatokRecPtr	acomm;
wchar		*word;
int		wlen;
wchar		*yomi;
int		ylen;
int		hinshi;
{
	AtokReqPacket	*req ;
	unsigned	char	*eucp ;
	WCHAR		*ucs_wp ;		/* 単語 UCS 文字列 */
	WCHAR		*ucs_yp ;	/* 読み UCS 文字列 */
	unsigned char	*sjp ;
	int		len ;
	int		alen ;
int i ;

	if ( wlen > ylen ) 	alen = wlen ;
	else			alen = ylen ;

	acomm->NETrqst.ceCmd = IME_PRV_REGISTERWORDEX ;
	req = &acomm->NETrqst ;
/* printf( "wlen %d, ylen %d\n", wlen, ylen ) ; */

	eucp = ( unsigned char * )malloc( alen * sizeof( WCHAR ) + 1 ) ;
	sjp = ( unsigned char * )malloc( ylen * sizeof( WCHAR ) + 1 ) ;

	/*
	 * 単語文字列を WCS -> EUC -> UCS 変換する。
	 */
	bzero( eucp, alen ) ;
	wcs2euc( word, wlen, eucp ) ;
	len = strlen( eucp ) ;
	ucs_wp = ( WCHAR * )malloc(( len  + 1 ) * sizeof( WCHAR )) ;
	euc2ucs( eucp, strlen( eucp ), ucs_wp, len, 0x3013 ) ;

#if 0
printf( "\303\261\270\354 WCS CODE -> "/*=単語 WCS CODE -> =*/ ) ;
for( i = 0 ; i <= wlen ; i++ ) {
	printf( "0x%x ", word[i] ) ;
}
printf( "\303\261\270\354 EUC CODE -> "/*=単語 EUC CODE -> =*/ ) ;
for( i = 0 ; i <= len ; i++ ) {
	printf( "0x%x ", eucp[i] ) ;
}
printf( "\n" ) ;
printf( "\303\261\270\354 EUC [%s]\n"/*=単語 EUC [%s]\n=*/, eucp ) ;
printf( "\303\261\270\354 UCS CODE -> "/*=単語 UCS CODE -> =*/ ) ;
for( i = 0 ; i <= wlen ; i++ ) {
	/* printf( "0x%x ", ucs_wp[i] ) ; */
}
printf( "\n" ) ;
#endif
	

	/*
	 * 読み文字列を WCS -> EUC -> UCS 変換する。
	 */
	bzero( eucp, alen ) ;
	wcs2euc( yomi, ylen, eucp ) ;
	len = strlen( eucp ) ;
	ucs_yp = ( WCHAR * )malloc(( len  + 1 ) * sizeof( WCHAR )) ;
	euc2ucs( eucp, strlen( eucp ), ucs_yp, len, 0x3013 ) ;

#if	0
printf( "\306\311\244\337 EUC CODE -->"/*=読み EUC CODE -->=*/ ) ;
for( i = 0 ; i <= len ; i++ ) {
	printf( "0x%x ", eucp[i] ) ;
}
printf( "\n" ) ;
printf( "\306\311\244\337 EUC [%s]\n"/*=読み EUC [%s]\n=*/, eucp ) ;
for( i = 0 ; i <= strlen( eucp )  ; i++ ) {
	printf( "0x%0x ", eucp[i] ) ;
}
printf( "\n" ) ;
printf( "\306\311\244\337 UCS CODE -> "/*=読み UCS CODE -> =*/ ) ;
for( i = 0 ; i <= ylen ; i++ ) {
	printf( "0x%x ", ucs_yp[i] ) ;
}
printf( "\n" ) ;
#endif

	/*
	 * 送信パラメータの作成
	 */
	req->Kind[0] = T_WCHAR ;
	req->Size[0] = wlen ;
	req->ArgPtr[0] = ucs_wp ;

	req->Kind[1] = T_WCHAR ;
	req->Size[1] = ylen ;
	req->ArgPtr[1] = ucs_yp ;

	req->Ctr[0] = hinshi ;

	AtokClntCommunicate( acomm->sock, &acomm->NETrqst, &acomm->NETrslt ) ;

	free( ucs_wp ) ;
	free( ucs_yp ) ;
	free( eucp ) ;

/* printf( "登録結果 %d\n", acomm->NETrslt.header.CEOutinf ) ; */
	return acomm->NETrslt.header.CEOutinf ;
}
/* End of if.c */
