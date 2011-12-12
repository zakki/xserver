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
static  char    rcsid[] = "$Id: buffer.c,v 1.3 1999/08/24 09:05:26 ishisone Exp $" ;
#endif  /* !lint */

#include        <stdio.h>
#include        <X11/IntrinsicP.h>
#include        <X11/StringDefs.h>
#include        <X11/Xmu/Atoms.h>
#include        <X11/keysym.h>


#include	<Xatoklib.h>


/* Page */
/*
 * [関数名]
 *		XatokClearBuffer( )
 * [表題]
 *              kinput2用変換バッファにクリア
 * [呼出形式]
 *	int	XatokClearBuffer( _XatokRecPtr  acomm )
 *
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		_XatokRecPtr : acomm   : i/o : kinput2用変換バッファ
 *
 * [返り値]
 *		 なし
 *
 * [使用関数]
 *		なし
 * [機能]
 *		確保された kinput2用変換バッファをクリアする。
 */
void XatokClearBuffer(abuf)
_XatokRecPtr	abuf;
{
	int	i, n ;

	for( i = 0 ; i < abuf->nbr_of_seg ; i++ ) {
		/* XatokFreeSegement( abuf->segments[i] ) ; */
	}
	abuf->cur_seg = 0 ;
	abuf->nbr_of_seg = 0 ;

}

/* Page */
/* 
 * [関数名]
 *		XatokBufferConvert( )
 * [表題]
 *		エンジン変換結果データのコンバート
 * [呼出形式]
 *		int XatokBufferConvert( _XatokRecPtr acomm,
 *			WORD ceCmd, AtokRsltPacket *result, int *status, 
 *			int *enable, int *change, int *elseEvent )
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		_XatokRecPtr : acomm   : i/o : kinput2用変換バッファ
 *		WORD		ceCmd,
 *		AtokRsltPacket	*result,
 *		int		*status,
 *		int		*enable,
 *		int		*change,
 *		int		*elseEvent
 *	
 *	
 * [返り値]
 *		確定文字列の長さ
 * [使用関数]
 *		ucs2euc( )
 *		es = euc2wcs( )
 *		s2e( )
 *		XatokCompStr( )
 *	
 * [機能]
 *		ATOK12X へのREQUESTより 戻って来たデータをチェックし、
 *		kinput2 の ATOK CLASSで使用できるデータ形式に変換する。
 *
 */
int XatokBufferConvert(acomm, ceCmd, result, status, enable, change, elseEvent)
_XatokRecPtr	acomm;
WORD		ceCmd;
AtokRsltPacket	*result;
int		*status;
int		*enable;
int		*change;
int		*elseEvent;
{
	char 		euc[BUFSIZ*4] ;	/* 読み表示のためのバッファ */
	int		es ;			/* euc 文字列の長さ */
	unsigned char	*sp, *sp1, *uni2euc( ) ;
	wchar		*wbuf ;		/* 確定文字列 */
	char		*wrbuf ;	/* 確定読み文字列 */

	int		length = 0 ;		/* 確定文字列の長さ */
	int		i, n, ksize ;			/* WORK */
	AtokRsltHdr	*hdr ;			/* WORK */
	int		wsize ; 		/* WORK */
	static	int euclen = 1 ;
		
	wsize = sizeof( wchar ) ;
	wbuf = acomm->wbuf ;
	wrbuf = acomm->wrbuf ;

	hdr = &acomm->NETrslt.header ;

	/*
	 * モード文字列の設定
	 * モード文字列の先頭に空白が存在するので削除する。
	 */
#ifdef	ATOK_DEBUG
	printf( "buffer conv \245\342\241\274\245\311LEN %d\n"/*=buffer conv モードLEN %d\n=*/, hdr->modeLen ) ;
#endif	/* ATOK_DEBUG */
	if ( hdr->modeLen > 0 ) {
		es = hdr->modeLen * 3 ;
		sp = ( unsigned char * )malloc( es ) ;
		bzero( sp, es ) ;
		ucs2euc( hdr->modePtr, hdr->modeLen, sp, es, 0x0000a2ae ) ;
		sp1 = sp ;
		while( *sp1 == 0x20 ) sp1++ ;	/* SPACE */
		es = strlen( sp1 ) ;
		es = euc2wcs( sp1, es, acomm->mode.name ) ;
#ifdef	ATOK_DEBUG
		printf( "\245\342\241\274\245\311\312\270\273\372\316\363 [%s]\n"/*=モード文字列 [%s]\n=*/, sp ) ;
#endif	/* ATOK_DEBUG */
		acomm->mode.length = hdr->modeLen ;
		acomm->mode.length = es ;
		free( sp ) ;
		/* acomm->mode.changes = 1 ; */
	}

	/*
	 * 確定文字列が存在する場合には、wbuf に確定文字列を設定し、
	 * リターン値である確定文字列の長さを設定する。
	 * wrbuf には読み文字列を設定する。
	 */
	if ( hdr->resultStrLen ) {
		es = hdr->resultStrLen * 3 ;
		sp = ( unsigned char * )malloc( es ) ;
		bzero( sp, es ) ;
		ucs2euc( hdr->resultStrPtr, hdr->resultStrLen, sp, es, 0x0000a2ae ) ;

		es = strlen( sp ) ;
		es = euc2wcs( sp, es, wbuf ) ;
		length = hdr->resultStrLen ;
		free( sp ) ;

		/*
		 * よみ文字列
		 */
		bzero( acomm->wrbuf, sizeof( acomm->wrbuf )) ;
		es = hdr->resultReadLen ;
		s2e( hdr->resultReadPtr, es, wrbuf, sizeof( acomm->wrbuf )) ;

#ifdef	ATOK_DEBUG
	printf( "=== buffer.c \263\316\304\352EUC [%s] === \n"/*==== buffer.c 確定EUC [%s] === \n=*/, sp ) ;
	printf( "=== buffer.c \263\316\304\352 READ EUC [%s] === \n"/*==== buffer.c 確定 READ EUC [%s] === \n=*/, acomm->wrbuf ) ;
#endif	/* ATOK_DEBUG */

	}
	/*
	 * 未確定文字列があった場合には、
	 * 未確定文字列をUNICODEからEUCに変換し、
	 * 反転表示等の位置を求める。
	 * 未確定文字列の長さと未確定文字列属性の長さは
	 * 同じなので if 文はまとめて処理する。
	 */
	if ( hdr->compStrLen ) {
		es = hdr->compStrLen * wsize ;
		es = hdr->compStrLen * 3 ;
		sp = ( unsigned char * )malloc( es ) ;
		bzero( sp, es ) ;

		ucs2euc( hdr->compStrPtr, hdr->compStrLen, sp, es, 0x0000a2ae ) ;
		es = strlen( sp ) ;

		acomm->echoLen = euc2wcs( sp, es, acomm->echoStr ) ;
		/*
		 * ここで反転表示のコメントも作成する。
		 */
		XatokCompStr( hdr->compStrPtr, hdr->compAttrPtr, 
			hdr->compStrLen, &acomm->revPos, &acomm->revLen ) ;
#ifdef	ATOK_DEBUG
		/* printf( "未確定文字列[%s]\n", sp ) ; */
		printf( "EUC CODE\n" ) ;
		for( i = 0 ; sp[i] ; i++ ) {
			printf( "0x%x ", sp[i] ) ;
		}
		printf( "\n" ) ;

		printf( "== DEBUG \314\244\263\316\304\352\311\275\274\250:\n"/*=== DEBUG 未確定表示:\n=*/ ) ;
		printf( "==> [%s]\n", sp ) ;
		printf( "==> [----+----1----+----2----+----3]\n" ) ;
		printf( "==> [" ) ;
		for( i = 0 ; i < acomm->revPos ; i++ ) {
			printf( "\241\373"/*=○=*/ ) ;
		}
		for( i = 0 ; i < acomm->revLen ; i++ ) {
			printf( "\241\374"/*=●=*/ ) ;
		}
		for( i = acomm->revPos + acomm->revLen ; i < hdr->compStrLen ; i++ ) {
			printf( "\241\373"/*=○=*/ ) ;
		}
		printf( "-]\n" ) ;
		printf( "revPos %d, revLen %d\n", acomm->revPos, acomm->revLen ) ;
#endif	/* ATOK_DEBUG */

		free( sp ) ;
	}
	else {
		acomm->echoLen = 0 ;
		acomm->echoStr[0] = 0 ;
	}

	/*
	 * 候補文字列
	 */
#ifdef	ATOK_DEBUG
	printf( "\270\365\312\344\277\364\241\242 %d, \270\365\312\344\245\265\245\244\245\272 %d\n"/*=候補数、 %d, 候補サイズ %d\n=*/, hdr->kohoNum, hdr->kohoStrLen ) ;
#endif	/* ATOK_DEBUG */

	acomm->kohoNum = 0 ;
	if ( hdr->kohoNum > 0 && hdr->kohoStrLen > 0 ) {
		n = hdr->kohoNum ;
		ksize = hdr->kohoStrLen ;
		if ( acomm->kohoSize == 0 ) {
			acomm->kohoStrPtr = ( WCHAR * )malloc( ksize * sizeof( WCHAR )) ;
			acomm->kohoLenPtr = ( BYTE * )malloc( n * sizeof( BYTE )) ;
			acomm->kohoSfxPtr = ( BYTE * )malloc( n * sizeof( BYTE )) ;
			acomm->kohoSize = n ;
			acomm->kohoStrLen = ksize ;
		}
		/*
		 * 文字列長、属性は、候補数だけ確保
		 * 候補数は kohoSize に 設定してある。
		 */
		if ( acomm->kohoSize < n ) {

			acomm->kohoLenPtr = ( BYTE * )realloc( acomm->kohoLenPtr, n * sizeof( BYTE )) ;
			acomm->kohoSfxPtr = ( BYTE * )realloc( acomm->kohoSfxPtr, n * sizeof( BYTE )) ;
			acomm->kohoSize = n ;
		}
		if ( acomm->kohoStrLen < ksize ) {
			acomm->kohoStrPtr = ( WCHAR * )realloc( acomm->kohoStrPtr, ksize * sizeof( WCHAR )) ;
			acomm->kohoStrLen = ksize ;
		}

                memcpy( acomm->kohoStrPtr, hdr->kohoStrPtr, ksize * sizeof( WCHAR)) ;
		memcpy( acomm->kohoLenPtr, hdr->kohoLenPtr, n * sizeof( BYTE )) ;
		memcpy( acomm->kohoSfxPtr, hdr->kohoSfxPtr, n * sizeof( BYTE )) ;
		acomm->kohoNum = n ;
		acomm->kohoPos = hdr->kohoPos ;

#ifdef	ATOK_DEBUG
{
	int kk ;
	char	*eep ;
	kk = ksize * sizeof( WCHAR );
	eep = uni2euc( acomm->kohoStrPtr, &kk ) ;
	printf( "\270\365\312\344 [%s]\n"/*=候補 [%s]\n=*/, eep ) ;
	for ( i = 0 ; i < n ; i++ ) {
		printf( "%d ", acomm->kohoLenPtr[i] ) ;
	}
	printf( "\304\271\244\265\n"/*=長さ\n=*/ ) ;
}
#endif	/* ATOK_DEBUG */
	}

	/*
	 * システム行データ
	 */
	if ( hdr->sysStrLen ) {
		es = hdr->sysStrLen * 3 ;
		sp = ( unsigned char * )malloc( es ) ;
		bzero( sp, es ) ;
		ucs2euc( hdr->sysStrPtr, hdr->sysStrLen, sp, es, 0x0000a2ae ) ;
		es = strlen( sp ) ;
		es = euc2wcs( sp, es, acomm->sysLineStr ) ;
		acomm->sysLineLen = hdr->sysStrLen ;
#ifdef	ATOK_DEBUG
		printf( "SYS LINE LEN in buffer.c (%d)\n", hdr->sysStrLen ) ;
#endif	/* ATOK_DEBUG */
		if ( acomm->sysLineSize == 0 ) {
			acomm->sysAttrPtr = ( WORD * )malloc( sizeof( WORD ) *
				hdr->sysStrLen ) ;
			acomm->sysLineSize = hdr->sysStrLen ;
		}
		else if ( acomm->sysLineSize < hdr->sysStrLen ) {
			acomm->sysAttrPtr = ( WORD * )malloc( sizeof( WORD ) *
				hdr->sysStrLen ) ;
			acomm->sysLineSize = hdr->sysStrLen ;
		}
		memcpy( acomm->sysAttrPtr, hdr->sysAttrPtr, sizeof( WORD ) * hdr->sysStrLen ) ;

		free( sp ) ;
	}
	if ( hdr->guideStrLen ) {
		es = hdr->guideStrLen * 3 ;
		sp = ( unsigned char * )malloc( es ) ;
		bzero( sp, es ) ;
		ucs2euc( hdr->guideStrPtr, hdr->guideStrLen, sp, es, 0x0000a2ae ) ;
		free( sp ) ;
	}
	acomm->kohoNum = hdr->kohoNum ;

	acomm->guide_openEventIndex = hdr->CEGuideOpenEventIndx ;
	acomm->guide_closeEventIndex = hdr->CEGuideCloseEventIndx ;
	acomm->guide_OpenIndex = hdr->CEGuideOpenIndx ;

	*status = hdr->CEOutinf ;
	*enable = hdr->StatusEnableFlag ;
	*change = hdr->DispChangeStatus ;
	*elseEvent = hdr->StatusElseEvent ;

	return length ;
}
/* End of buffer.c */
