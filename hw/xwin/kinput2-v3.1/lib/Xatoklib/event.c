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
static  char    rcsid[] = "$Id: event.c,v 1.4 1999/08/24 09:05:26 ishisone Exp $" ;
#endif  /* !lint */

#include	<stdio.h>
#include	<X11/IntrinsicP.h>
#include	<X11/StringDefs.h>
#include	<X11/Xmu/Atoms.h>
#include	<X11/keysym.h>
#include	<InputConv.h>

#include	<Xatoklib.h>
#include	<key_def.h>


/* Page */
/*
 * [関数名]
 *              WordDelEvent( )
 * [表題]
 *              単語削除ウィンドウ表示時のイベント処理
 * [呼出形式]
 *      int WordDelEvent( _XatokRecPtr acomm, WORD *aajcode, WORD *ceCmd )
 *
 * [引数]
 *              型        : 名       称   : IO : 説      明
 *		_XatokRecPtr : acomm    : i/o : kinput2用変換バッファ
 *		WORD      :  *aajcode   : i/o : 入力キーコード
 *              WORD      :  *ceCmd     :   o : ATOK12CE のコマンド番号
 *
 * [返り値]
 *		0 : キー入力イベントをCEへ渡さない
 *		1 : キー入力イベントをCEへ渡す
 *
 * [使用関数]
 *		なし
 * [機能]
 *		単語削除確認ダイアログを表示している時に有効なキーの
 *		制御を行なう。有効なキーは 'Y','y', 'N', 'n' だけである。
 *		それ以外のキーが入力された時は何もしない。
 *		有効キーの時だけ、エンジンにキーを渡すので１を返す。
 */
static int WordDelEvent(acomm, aajcode, ceCmd)
_XatokRecPtr	acomm;
WORD		*aajcode;
WORD		*ceCmd;
{
	int	status = 0 ;

	switch( *aajcode ) {
		case 'y' :
		case 'Y' :
		case 'n' :
		case 'N' :
			/* 
			 * 有効キーの場合
			 */
			acomm->convStatus ^= M_WDEL ;	/* 623 */
			status = 1 ;
			break ;
		default :
			status = 0 ;
	}
	return status ;
}
/* Page */
/*
 * [関数名]
 *		AuxEvent( )
 * [表題]
*		AUXウィンドウ表示時のイベント処理
 * [呼出形式]
 *      	int AuxEvent( _XatokRecPtr acomm, WORD *aajcode, 
 *			WORD *ceCmd, int *length )
 *
 * [引数]
 *              型        : 名       称   : IO : 説      明
 *		_XatokRecPtr : acomm    : i/o : kinput2用変換バッファ
 *		WORD      :  *aajcode   : i/o : 入力キーコード
 *              WORD      :  *ceCmd     :   o : ATOK12CE のコマンド番号
 *		int       : *length     :   o : 入力された文字列の長さ
 *
 *
 * [返り値]
 *		-1 : 送信エラー
 *		 N : 送信バイト数
 *
 * [使用関数]
 *		MenuCtrl( )
 * [機能]
 *		AUX ウィンドウ表示時のイベント処理を行なう。
 *		AUX ウィンドウには MenuClass が設定されていて、
 *		MenuClass に定義された画面の遷移を行なう。
 */
static int AuxEvent(acomm, aajcode, ceCmd, length)
_XatokRecPtr	acomm;
WORD		*aajcode;
WORD		*ceCmd;
int		*length;
{
	int	status ;

	*length = 0 ;

#ifdef	ATOK_DEBUG
	printf( "AUX KEY 0x%x\n", *aajcode ) ;
#endif	/* ATOK_DEBUG */
	status = MenuCtrl( acomm, &acomm->curAux, aajcode, 0, length ) ;
#ifdef	ATOK_DEBUG
	printf( "MENU Status = %d\n", status ) ;
#endif	/* ATOK_DEBUG */
	/*
	 * NULL になった場合はAUX WINDOWをキャンセルされた場合
	 * status : -1 変更なし(DefaultProc で処理)
	 * 	  :  0 メニューの選択変更
	 *	  :  1 メニュー変更
	 *	  :  2 メニューの確定
	 *
	 * MenuStatus : 1  AuxStart
	 *		2  AuxChange
	 *		3  AuxEnd
	 */
	switch( status ) {
		case 1 :
			/* メニュー変更あり */
			/* printf( "acomm->curAux 0x%x\n", acomm->curAux ) ; */
			if ( !acomm->curAux ) {
				acomm->curAux = acomm->menuAux ;
				acomm->menuStatus = ICAuxEnd ;	/* END */
			}
			break ;
		case 2 :
			/* メニュー確定 */
			acomm->menuStatus = ICAuxChange ;
			break ;
		case -1 :
			acomm->menuStatus = ICAuxChange ;
			return *length ;
		case 0 :
		default :
			acomm->menuStatus = ICAuxChange ;
			return 0 ;
	}
	/* return 0 ; */
	return *length ;
}

/* Page */
/*
 * [関数名]
 *              CandEvent( )
 * [表題]
 *              候補ウィンドウ表示時のイベント処理
 * [呼出形式]
 *      int CandEvent( _XatokRecPtr acomm, WORD *aajcode, WORD *ceCmd )
 *
 * [引数]
 *              型        : 名     称   : IO  : 説      明
 *		_XatokRecPtr : acomm    : i/o : kinput2用変換バッファ
 *		WORD      :  *aajcode   : i/o : 入力キーコード
 *              WORD      :  *ceCmd     :   o : ATOK12CE のコマンド番号
 *
 *
 * [返り値]
 *		0 : キー入力イベントをCEへ渡さない
 *		1 : キー入力イベントをCEへ渡す
 *
 * [使用関数]
 *		atokCandidate( )
 * [機能]
 *		候補ウィンドウが表示されている時のイベント処理を行なう。
 *		イベント処理は入力された aajcode により対応する処理が
 *		決定する。ここでチェックしているコードは
 *		kinput2 のAtokClass のモジュールでも表示制御のため
 *		チェックしている。
 *		CTR_DEL が押された時は、kinput2 側で変更されたカレント候補を
 *		エンジンに伝えてから、CTR_DEL の削除処理を呼び出す。
 */
static int CandEvent(acomm, aajcode, ceCmd)
_XatokRecPtr	acomm;
WORD		*aajcode;
WORD		*ceCmd;
{
	int	pgMax ;
	int	len = 0 ;

	pgMax = acomm->kohoMax ;

	switch( *aajcode ) {
		case EESC :
			*aajcode = FUNC_CNV_CANCL ;
			return 0 ;
		case LEFT :
		case RIGHT :
		case DOWN :
		case UP :
		case CTR_U :
		case CTR_N :
		case CTR_F :
		case CTR_B :
		case CTR_A :
			return 0 ;
		case CTR_DEL :
			/*
			 * 現在 表示上選択されている候補をエンジンに伝える
			 * 必要がある。
			 */
			*aajcode = acomm->kohoPos ;
			atokCandidate( acomm, *aajcode ) ;
			*aajcode = CTR_DEL ;
			acomm->convStatus |= M_WDEL ;	/* 623 */
			return 1 ;
		case SPACE :
			*aajcode = SPACE ;
			/* *aajcode = XFER ; */
			return 0 ;	/* 617 */
			break ;
		case CR :
			/*
			 * kinput2 側で選択されている所とエンジン内で
			 * 選択されている所を同期させる。
			 */
			*aajcode = acomm->kohoPos ;

			acomm->NETrqst.AtCEIncode = acomm->kohoPos ;
#ifdef	ATOK_DEBUG
	printf( "\301\252\302\362\244\265\244\354\244\277\270\365\312\344\310\326\271\346 (%d) Page (%d)\n"/*=選択された候補番号 (%d) Page (%d)\n=*/, 
				acomm->NETrqst.AtCEIncode,
			acomm->kohoCurPage ) ;
#endif	/* ATOK_DEBUG */
			len = 1 ;
			break ;
		/*
		 * 数字キーの場合は候補番号を選択した事とする。
		 * 有効な数字についてはチェックする事
		 */

		default :
			len = 0 ;
#ifdef	CAND_PAGE
			if ( *aajcode >= '1' && *aajcode <= '9' ) {
				*aajcode -= '1' ;
				len = 1 ;
			}
			else if ( *aajcode >= 'a' && *aajcode <= 'z' ) {
				*aajcode -=  'a' ;
				*aajcode += 10 ;
				len = 1 ;
			}
			else if ( *aajcode >= 'A' && *aajcode <= 'Z' ) {
				*aajcode -=  'A' ;
				*aajcode += 10 ;
				len = 1 ;
			}
			*aajcode = acomm->kohoCurPage * pgMax + *aajcode ;
			*aajcode = acomm->kohoCurPage * pgMax + *aajcode ;
#else			
			len = 0 ;
#endif
	}
	if ( len > 0 ) {
#ifdef	ATOK_DEBUG
		Printf( "\270\365\312\344 \270\275\272\337\244\316\245\332\241\274\245\270 (%d)\n"/*=候補 現在のページ (%d)\n=*/, acomm->kohoCurPage ) ;
		printf( "\270\365\312\344 \263\316\304\352\310\326\271\346 (%d)\n"/*=候補 確定番号 (%d)\n=*/, *aajcode ) ;
		printf( "\270\365\312\344 \263\316\304\352\310\326\271\346 (%d)\n"/*=候補 確定番号 (%d)\n=*/, acomm->NETrqst.AtCEIncode ) ;
#endif	/* ATOK_DEBUG */
		atokCandidate( acomm, *aajcode ) ;

		*aajcode = VKEY|FUNC_KAKU_BUBUN ;

	}
	return len ;
}
/* Page */
/*
 * [関数名]
 *              KeyEvent( )
 * [表題]
 *              キー入力イベント処理
 * [呼出形式]
 *      	int KeyEvent( _XatokRecPtr acomm, WORD *aajcode, WORD *ceCmd )
 *
 * [引数]
 *              型        : 名       称   : IO : 説      明
 *		_XatokRecPtr : acomm    : i/o : kinput2用変換バッファ
 *		WORD      :  *aajcode   : i/o : 入力キーコード
 *              WORD      :  *ceCmd     :   o : ATOK12CE のコマンド番号
 *
 * [返り値]
 *		0 : キー入力イベントをCEへ渡さない
 *		1 : キー入力イベントをCEへ渡す
 *
 * [使用関数]
 *		atokCandidate( )
 *		XatokBufferConvert( )
 * [機能]
 *		通常入力状態でのキー入力イベントの処理を行なう。
 *		通常の状態ではなく、他のイベント処理を行なわれた後でも
 *		エンジンにキーを渡す必要がある場合には呼び出される。
 */
static int KeyEvent(acomm, aajcode, ceCmd, wbuf, length, status, enable, change, elseEvent)
_XatokRecPtr	acomm;
WORD		*aajcode;
WORD		*ceCmd;
wchar		*wbuf;
int		*length;
int		*status;
int		*enable;
int		*change;
int		*elseEvent;
{
	int	call = 1 ;
	int	sRet = 0 ;
	WORD	key ;

	*length = 0 ;
	*status = 0 ;
	*enable = 0 ;
	*change = 0 ;
	acomm->NETrqst.AtCEIncode = 0 ;
	acomm->NETrqst.AtCEPrvAPIOutinf = 0 ;
	/*
	 * 未確定文字列がないときに制御キーを押された場合には
	 *  キーをそのまま返す。
	 * 制御キーについての if 文は 修正する必要あり。
	 */
#if	0
	if ( !acomm->echoLen && *aajcode == CR ) {
		/*
		( 
			 *aajcode == CR ||
			 *aajcode == SPACE   ||
			 *aajcode == BS
			)) {
		*/
		printf( "\245\271\245\353\241\274 KEY %x\n"/*=スルー KEY %x\n=*/, *aajcode ) ;
#ifdef	ATOK_DEBUG
#endif	/* ATOK_DEBUG */
		wbuf[0] = *aajcode & 0xff ;
		*length = 1 ;
		return  1 ;
	}
#endif
	key =  *aajcode ;
	/*
	 * CTRL + W は 入力モードの切り替えキー
	 * キー入力中に対応したAAJ CODEの機能で SWITCHすること。
	 */
	switch( *aajcode ) {
		case CTR_F7 :
		case CTR_W :
			if ( acomm->echoLen > 0 ) {
				return 0 ;
			}
			acomm->convStatus = M_AUX ;
			MenuCtrl( acomm, &acomm->curAux, aajcode, 1, length ) ;
			acomm->menuStatus = ICAuxStart ;	
			return 0 ;
		case SPACE :
			*ceCmd = IME_PRV_KEYCONVERT ;
			/* *aajcode = XFER ; */
			/* key = XFER ; */
			/*
			 * ここは スペースキーの割当をチェックする事
			 */
			if ( !acomm->echoLen )	key = SPACE ;
			else 			key = XFER ;
			/* key = XFER ; */
			key = SPACE ;	 /* 616 */

			acomm->NETrqst.AtCEIncode = key & 0xffff ;
			sRet = 1 ;
			break ;
		/* KEYCONV */
		default :
			*ceCmd = IME_PRV_KEYCONVERT ;
			acomm->NETrqst.AtCEIncode = *aajcode & 0xffff ;
			sRet = 1 ;
			break ;
	}

	if ( call  ) {
		acomm->NETrqst.ceCmd = *ceCmd ;

#ifdef	ATOK_DEBUG
printf( "COMMUNICATE CMD(%d) AAJCODE(0x%04x)\n", *ceCmd, *aajcode & 0xffff ) ;
#endif	/* ATOK_DEBUG */
		AtokClntCommunicate( acomm->sock, &acomm->NETrqst, &acomm->NETrslt ) ;
#ifdef	ATOK_DEBUG
printf( "REQ OK\n" ) ;
#endif	/* ATOK_DEBUG */
	}

	*length = XatokBufferConvert( acomm, *ceCmd, &acomm->NETrslt, 
			status, enable, change, elseEvent ) ;

#ifdef	ATOK_DEBUG
	printf( "\270\365\312\344\244\316\277\364\244\362\245\301\245\247\245\303\245\257(%d)\241\243\n"/*=候補の数をチェック(%d)。\n=*/, acomm->kohoNum ) ;
	printf( "CAND STAT (%d)\n", acomm->convStatus & M_CAND ) ;
#endif	/* ATOK_DEBUG */
	/*
	 * 候補が複数存在し、現在まだ候補ウィンドウを表示していないなら
	 * 候補ウィンドウを表示するために、候補リストを取得する。
	 */
	if ( acomm->kohoNum > 1 && 
		 !( acomm->convStatus & M_CAND ) ) {

		/*
		 * kohoNum には 現在は候補数が入っているが、
		 * Candidate をよぶとカレント候補番号になってしまうので
		 * kohoNum2 に候補の総数を保存しておく
		 */
		acomm->kohoNum2 = acomm->kohoNum ;

#ifdef	ATOK_DEBUG
		printf( "\270\365\312\344\244\254\244\242\244\303\244\277\244\316\244\307 \270\365\312\344\311\275\274\250\245\342\241\274\245\311\244\313\260\334\244\353\241\243\n"/*=候補があったので 候補表示モードに移る。\n=*/ ) ;
#endif	/* ATOK_DEBUG */
		atokCandidate( acomm, ATCEPRVAPI_READCMD ) ;
		acomm->convStatus |= M_CAND ;
		*aajcode = XFER ;
		*length = XatokBufferConvert( acomm, *ceCmd, &acomm->NETrslt, 
			status, enable, change, elseEvent ) ;
	}

	return 1 ;
}

/* Page */
/* 
 * [関数名]
 *		XatokEventControl( )
 * [表題]
 *		キー入力の受け付け
 * [呼出形式]
 *		int	XatokEventControl( AtokPart
 * [引数]
 *		型        : 名       称   : IO : 説      明
 *	
 *	
 * [返り値]
 *		
 * [使用関数]
 *	
 * [機能]
 *		X からのイベントを受け取り ATOK の AAJCODEに変換して、
 *		ATOK CE に変換を依頼する。
 *		変換結果を受け取った後に、libXatok 用に変換結果を
 *		バッファに設定する。
 *
 */
int XatokEventControl(acomm, event, xkey, kbuf, nbytes, aajcode, status, enable, change, elseEvent)
/* Fix Length */
_XatokRecPtr	acomm;
XKeyEvent	*event;
KeySym		xkey;
char		*kbuf;
int		nbytes;
WORD		*aajcode;
int		*status;
int		*enable;
int		*change;
int		*elseEvent;
{
	char 		euc[BUFSIZ*4] ;	/* 読み表示のためのバッファ */
	int		es ;			/* euc 文字列の長さ */
	unsigned char	*sp ;
	wchar		*wbuf ;	

	WORD		ceCmd = -1 ;		/* CE へ送るコマンド */
	int		length ;		/* 確定文字列の長さ */
	extern WORD	getAAJCode( )  ;	
	int		i ;			/* WORK */
	int		sRet ;
		
	wbuf = acomm->wbuf ;
	*status = 0 ;
	wbuf[0] = 0 ;
	length = 0 ;
	acomm->wlen = 0 ;

	/*
	 * 本当はここで ATOK CEを呼び出し 変換結果を見て処理をする。
	 * CE を呼び出すAPIは Xのキーイベントを見て AAJCODEにする。
	 * 確定した文字がある場合には、 wbuf に確定文字を設定する。
	 * 確定しなかった場合には、入力文字としてprebuf にセットする。
	 */
	/*
	 * ここで キー入力をATOKのコード(AAJCODE)に変換して、
	 * 各モード時に対応したコマンドの展開する。
	 */
	*aajcode = getAAJCode( event, xkey, kbuf ) ;
#ifdef	ATOK_DEBUG
printf( "aajcode  0x%x = len = %d\n", *aajcode, acomm->echoLen ) ;
printf( "convStatus  %d\n",  acomm->convStatus ) ;
#endif	/* ATOK_DEBUG */
	if ( *aajcode == 0 ) {
		return 0 ;
	}

	/*
	 * キーイベントが発生してこの関数にとんで来た時に
	 * モードが設定されていない場合には、変換開始のキーを
	 * 押された時であるので、キー変換モードに設定する。
	 */
	if ( acomm->convStatus == M_NOKEYCONV ) {
		atokKanjiOn( acomm ) ;
/* printf( "KANJI ON MODE (%d)\n", acomm->convStatus ) ; */
	}

	/*
	 * CTRL_SPACE は 漢字ON／OFFの切り替えキー
	 * CTRL_SPACE はどんな場合でも漢字OFF
	 */	
	if ( *aajcode == SFT_SP ) {
#ifdef	ATOK_DEBUG
	printf( "AAJ SFT_SP -> KANJIOFF\n" ) ;
#endif	/* ATOK_DEBUG */
		if  ( acomm->convStatus == M_KEYCONV ) {
			atokKanjiOff( acomm ) ;
/* printf( "KANJI ON OFF (%d)\n", acomm->oldStatus ) ; */
		}
		return 0 ;
	}

	if ( acomm->convStatus & M_AUX ) {
		/*
		 * AUX ウィンドウ 表示中のイベント処理
		 */

		sRet = AuxEvent( acomm, aajcode, &ceCmd, &length ) ;
		/* printf( "AUX %d RETURN LENGTH = %d\n", sRet, length ) ; */
		if ( sRet < 1 ) {
			return length ;
		}
	}
	/* if ( acomm->convStatus & M_CAND ) { */
	/*
	 * 候補ウィンドウを表示中でかつシステム行を表示していない場合の
	 * イベント処理
	 */
	if ( acomm->convStatus & M_WDEL ) {
		sRet = WordDelEvent( acomm, aajcode, &ceCmd ) ;
		if ( sRet == 0 ) {
			return 0 ;
		}
	}
	if ( acomm->convStatus & M_CAND &&
		!( acomm->convStatus & M_SYSLINE )) { /* 616 */
		/*
		 * 候補ウィンドウを表示中のイベント処理
		 */
		sRet = CandEvent( acomm, aajcode, &ceCmd ) ;
		/* 
		 * ステータスが０以外の時は、候補が確定された時
		 */
		if ( sRet == 0 ) {
			return 0 ;
		}
	}
	/*
	 * その他は 通常のキー入力イベント
	 */
/* printf( "KEY CONV AAJ 0x%x\n", *aajcode ) ; */
	 KeyEvent( acomm, aajcode, &ceCmd, wbuf, &length,
			status, enable, change, elseEvent ) ;

/* printf( "候補の番号(%d)。\n", acomm->kohoPos ) ; */
#ifdef	ATOK_DEBUG
	printf( "\263\316\304\352\312\270\273\372\316\363EVENT[%s]\n"/*=確定文字列EVENT[%s]\n=*/, acomm->wbuf ) ;
#endif	/* ATOK_DEBUG */
	acomm->wlen = length ;

	return length ;
}

/* End of event.c */
