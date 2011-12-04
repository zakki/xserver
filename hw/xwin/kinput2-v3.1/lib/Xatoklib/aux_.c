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
static  char    rcsid[] = "$Id: aux.c,v 1.3 1999/08/24 09:05:25 ishisone Exp $" ;
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
 *		wordStartFunc( )
 * [表題]
 *              単語登録メニューの初期化処理
 * [呼出形式]
 *	int	wordStartFunc( _XatokRecPtr  acomm )
 *
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		_XatokRecPtr : acomm   : i/o : kinput2用変換バッファ
 *
 * [返り値]
 *		1 : 常に１
 *
 * [使用関数]
 *		なし
 * [機能]
 *		単語登録メニューを表示する為に、データの初期化を行なう。
 */
int wordStartFunc(acomm)
_XatokRecPtr	acomm;
{

#ifdef	ATOK_DEBUG
	printf( "\303\261\270\354\305\320\317\277\263\253\273\317\n"/*=単語登録開始\n=*/ ) ;
#endif	/* ATOK_DEBUG */
	bzero( acomm->word_str, sizeof( acomm->word_str )) ;
	bzero( acomm->word_yomi, sizeof( acomm->word_yomi )) ;
	acomm->word_strLen = 0 ;
	acomm->word_yomiLen = 0 ;
	acomm->word_mode = W_WORD ;

	return 1 ;
}

/* Page */
/*
 * [関数名]
 *		wordFunc( )
 * [表題]
 *              単語登録時のイベント処理
 * [呼出形式]
 *      int wordFunc( _XatokRecPtr acomm,  menuAux *mAux, 
 *			WORD *aajcode, int *op )
 *
 * [引数]
 *              型           : 名   称  : I/O : 説      明
 *		_XatokRecPtr : acomm    : i/o : kinput2用変換バッファ
 *		menuAux      : *mAux    : i   :  現在表示しているMENU
 *		WORD         : *aajcode : i/o : 入力された文字
 *		int          : *op      :   o : MENU OPERATION
 *
 *
 * [返り値]
 *		0 : key 入力をエンジンに送らない。
 *		1 : key入力をエンジンに送る。
 *
 * [使用関数]
 *		atokModeSet( )
 *		atokWordRegister( )
 *		atokWordStyleList( )
 * [機能]
 *		単語登録用のAUX WINDOW表示時のイベント管理を行なう。
 *		単語登録時は、単語表記入力、読み入力、品詞選択、処理確認
 *		処理結果表示のモードが存在する。
 *		各モードにおいて、入力できる文字体系を設定する。
 *		CTRキーなどの制御キーの操作も行なう。
 */
int wordFunc(acomm, mAux, aajcode, op)
_XatokRecPtr    acomm;
menuAux		*mAux;
WORD		*aajcode;
int		*op;
{
	ATOKDEHINSTR	*hptr ;
	char		euc[BUFSIZ] ;
	int		i ;
	int		pgMax = CAND_ROWS * CAND_COLS ;
	int		status ;

	char	*pp ;
#ifdef	ATOK_DEBUG
printf( "WORD FUNC OK (%x) word_mode (%d)\n", *aajcode, acomm->word_mode ) ;
printf( "word_strLen  %d, word_yomiLen %d\n", 
		acomm->word_strLen, acomm->word_yomiLen ) ;
#endif	/* ATOK_DEBUG */
	if ( *aajcode == CTR_H ) 	*aajcode = BS ;

	/*
	 * 未確定文字が無い時に制御キーを入力された場合には
	 * オペレーションを無効にする。
	 */
	if ( !acomm->echoLen  && 
		(( *aajcode & CTRL ) || *aajcode == DEL )) {
		return 0 ;
	}
	/*
	 * 単語登録実施中に コード入力などのＡＵＸウィンドウは
	 * 表示できないのでオペレーションを無効にする。
	 */
	if ( *aajcode == F10 ) {
		return 0 ;
	}
	/*
	 * 単語入力時のＯＰ
	 */
	if ( acomm->word_mode == W_WORD ) {
		if ( *aajcode == CR && acomm->echoLen == 0 ) {
			if ( acomm->word_strLen ) {
				atokModeSet( acomm, IME_PRV_SETKOTEMODE, 0x0001 ) ;
				acomm->word_mode <<= 1 ;
				return 0 ;
			}
			return 0 ;
		}
		else if ( *aajcode == BS && !acomm->echoLen ) {
			if (  acomm->word_strLen ) {
				acomm->word_strLen-- ;
			}
			else {
				*op  = OP_CANCEL ; 
			}
			return 0 ;
		}

		else if ( *aajcode == EESC ) {
			if ( !( acomm->convStatus & M_CAND )) {	/* 616 */
				*op = OP_CANCEL ;
			}
			/*
			 * 未確定文字列がある時は、未確定バッファをクリアする
			 */
			if ( acomm->echoLen  ) {
				acomm->echoLen = 0 ;
				*aajcode = EESC ;
				return 1 ;
			}
			return 0 ;
		}
		else if ((( *aajcode & CTRL ) && !acomm->echoLen ) 
			|| *aajcode == TAB ){
			return 0 ;
		}
		return 1 ;
	}
	/*
	 * 読み入力時のOP
	 */
	else if ( acomm->word_mode == W_YOMI ) {
		if ( *aajcode == CR  && acomm->echoLen == 0 ) {
			if ( acomm->word_yomiLen ) {
				atokModeSet( acomm, IME_PRV_SETKOTEMODE, 0x0000 ) ;
				acomm->word_mode <<= 1 ;
				acomm->convStatus |= M_CAND ;
				atokWordStyleList( acomm ) ;
				*aajcode = XFER ;
				return 0 ;
			}
			return 0 ;
		}
		else if ( *aajcode == BS && !acomm->echoLen ) {
			if (  acomm->word_yomiLen ) {
				acomm->word_yomiLen-- ;
			}
			else {
				atokModeSet( acomm, IME_PRV_SETKOTEMODE, 0x0000 ) ;
				acomm->word_mode >>= 1 ;
			}
			return 0 ;
		}
		else if ((( *aajcode & CTRL ) && !acomm->echoLen ) 
			|| *aajcode == TAB ){
			return 0 ;
		}
		else if ( *aajcode == EESC ) {
			atokModeSet( acomm, IME_PRV_SETKOTEMODE, 0x0000 ) ;
			/* acomm->word_mode >>= 1 ; */
			*op = OP_CANCEL ;
			return 0 ;
		}
		return 1 ;
	}
	/*
	 * 品詞を選択中のOP
	 */
	else if ( acomm->word_mode == W_HINSHI ) {
		if ( *aajcode > '0' && *aajcode <= '9' ) {
			acomm->kohoPos = *aajcode - '1' ;
			 *aajcode = CR ;
		}
		if ( *aajcode == CR ) {
			acomm->word_hinshi = acomm->kohoCurPage * pgMax + acomm->kohoPos + 1 ;
			acomm->word_mode <<= 1 ;
		}
		else if ( *aajcode == CTR_G ||
			*aajcode == EESC ) {
			acomm->word_mode >>= 1 ;
		}
		return 0 ;
	}
	/*
	 * 登録確認のダイアログ表示中のOP
	 */
	else if ( acomm->word_mode == W_YORN ) {
		if ( *aajcode == 'y' || *aajcode == 'Y'  ) {
			acomm->word_str[acomm->word_strLen] = 0x00 ;
			acomm->word_str[acomm->word_strLen+1] = 0x00 ;

			acomm->wordStatus = atokWordRegister(
				acomm,
				acomm->word_str,
				acomm->word_strLen,
				acomm->word_yomi,
				acomm->word_yomiLen,
				acomm->word_hinshi ) ;
			acomm->word_mode <<= 1 ;
			/* *op  = OP_CANCEL ; */
		}
		else if ( *aajcode == 'n' || *aajcode == 'N' || *aajcode == BS ) {
			atokWordStyleList( acomm ) ;
			acomm->convStatus |= M_CAND ;
			acomm->word_mode >>= 1 ;
			*aajcode = XFER ;
		}
		else if ( *aajcode == EESC ) {
			acomm->word_mode = 0 ;
			*op = OP_CANCEL ;
		}
		return 0 ;
	}
	/*
	 * 登録結果表示中のOP
	 */
	else if ( acomm->word_mode == W_RESULT ) {
		if ( *aajcode == CR || *aajcode == EESC ) {
			acomm->word_mode = 0 ;
			*op = OP_CANCEL ;
		}
		return 0 ;
	}
	return 0 ;
		
#ifdef	ATOK_DEBUG
	printf( "WORD STR [%s]\n", pp ) ;
#endif	/* ATOK_DEBUG */
}

/* Page */
/*
 * [関数名]
 *		wordEndFunc( )
 * [表題]
 *              単語登録メニューの後処理
 * [呼出形式]
 *	int	wordEndFunc( _XatokRecPtr  acomm )
 *
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		_XatokRecPtr : acomm   : i/o : kinput2用変換バッファ
 *
 * [返り値]
 *		1 : 常に１
 *
 * [使用関数]
 *		なし
 * [機能]
 *		単語登録メニューを削除する為に、データの後処理を行なう。
 *		
 */
int wordEndFunc(acomm)
_XatokRecPtr    acomm;
{

	/* printf( "wordEndFunc\n" ) ; */
	return 1 ;
}
/*
 * Str
 * 単語登録 : 単語 [XXXXX] 読み [XXXX] 品詞 :
 */
/* Page */
/*
 * [関数名]
 *		wordStringFunc( )
 * [表題]
 *              単語登録メニューの文字列作成
 * [呼出形式]
 *	int	wordStringFunc( _XatokRecPtr  acomm, menuAux *mAux,
 *			wchar *echoStr, int *echoLen, int *revLen,
 *			int *revPos )
 *
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		_XatokRecPtr : acomm   : i/o : kinput2用変換バッファ
 *		menuAux      : *mAux   : i  : 現在の MENU CLASS
 *		wchar        : *echoStr:   o : 表示文字列格納領域
 *		int          : *echoLen :  o : 表示文字列の長さ
 *		int          : *revLen :   o : 表示文字列の反転の長さ
 *		int          : *revPos :   o : 表示文字テルの反転の開始位置
 *
 * [返り値]
 *		表示文字列の長さ
 *
 * [使用関数]
 *		euc2wcs( )
 *		s2e( )
 * [機能]
 *		単語登録メニューを表示時の表示用の文字列を作成する。
 *		表記入力時は、表記確定文字列と未確定文字列、
 *		よみ入力時は、表記確定文字列と、よみ確定文字列、よみ未確定文字列
 *		品詞選択時は、読み入力時と同じ、
 *		登録確認時は、表記確定文字列,よみ文字列、品詞
 *		登録完了時は、ステータス文字列
 *		を作成する。
 */
int wordStringFunc(acomm, mAux, echoStr, echoLen, revLen, revPos)
_XatokRecPtr    acomm;
menuAux		*mAux;
wchar		*echoStr;
int		*echoLen;
int		*revLen;
int		*revPos;
{
	char	tbuf[BUFSIZ*2] ;
	int	len ;
	int	n ;
	int	i, j, k, es ;
	char	*ep ;
	int		status ;
	int	c ;

	*revPos = 0 ;
	*revLen = 0 ;

	ep = tbuf ;
	if ( mAux->str ) {
		strcpy( ep, mAux->str ) ;
		ep += strlen( ep ) ;
	}
#ifdef	ATOK_DEBUG
printf(  "WORD MODE %d\n", acomm->word_mode );
	printf( "  \303\261\270\354 [%s"/*=  単語 [%s=*/, acomm->word_str ) ;
printf( "\303\261\270\354\263\316\304\352\272\321\312\270\273\372\316\363(%d) [%s]\n"/*=単語確定済文字列(%d) [%s]\n=*/, acomm->word_strLen, acomm->word_str ) ;
printf( "\303\261\270\354  \263\316\304\352\244\267\244\277\312\270\273\372\316\363(%d) [%s]\n"/*=単語  確定した文字列(%d) [%s]\n=*/, acomm->wlen, acomm->wbuf ) ;
printf( "\303\261\270\354 \314\244\263\316\304\352\312\270\273\372\316\363[%s]\n"/*=単語 未確定文字列[%s]\n=*/, acomm->echoStr ) ;
for( i = 0 ; i < acomm->echoLen ; i++ ) {
	printf( "%x ", acomm->echoStr[i] ) ;
}
printf( "\n" );
#endif	/* ATOK_DEBUG */


	/*
	 * 単語登録のガイドを設定
	 */
	sprintf( ep, "  \303\261\270\354 ["/*=  単語 [=*/ ) ;
	es = strlen( tbuf ) ;
	len = euc2wcs( tbuf, es, echoStr ) ;
	for( i = 0 ; i < acomm->word_strLen ; i++ ) {
		echoStr[len++] = acomm->word_str[i] ;
	}
	/*
	 * 単語入力中の表示
	 */
	if (  acomm->word_mode == W_WORD ) {
		/* 
		 * 今回の入力で確定した文字列がある場合には、
		 * 確定単語文字列として設定する。
		 */
		for( i = 0 ; i < acomm->wlen ; i++ ) {
			if ( acomm->word_strLen+i >= WORD_HYOKI_MAX ) {
				/* printf( "単語がながすぎる。(%d)\n", i ) ; */
				break ;
			}
			acomm->word_str[acomm->word_strLen+i] = acomm->wbuf[i] ;
			echoStr[len++] = acomm->word_str[acomm->word_strLen+i] ;
/* printf( "0x%x ", acomm->wbuf[i] ) ; */
		}
/* printf( "\n" ) ; */
		acomm->word_strLen += i ;
		acomm->word_str[acomm->word_strLen] = 0x00 ;
		/*
		 * 未確定文字列を設定
		 */
		if ( acomm->echoLen > 0 ) {
			*revPos = len + acomm->revPos ;
			*revLen = acomm->revLen ;
			for( i = 0 ; i < acomm->echoLen ; i++ ) {
				echoStr[len++] = acomm->echoStr[i] ;
			}
		}
		else { 
			*revPos = len ;
			*revLen = 1 ;
		}
	}
	n = euc2wcs( "]", 1, &echoStr[len] ) ;
	len += n ;
	*echoLen = len ;
	if ( acomm->word_mode < W_YOMI ) {
		return ;
	}
	sprintf( tbuf, "  \306\311\244\337 ["/*=  読み [=*/ ) ;
	es = strlen( tbuf ) ;
	n = euc2wcs( tbuf, es, &echoStr[len] ) ;
	len += n ;

	/*
	 * よみのながさは 漢字とANKでは 同じである。
	 */
	for( i = 0 ; i < acomm->word_yomiLen ; i++ ) {
		echoStr[len++] = acomm->word_yomi[i] ;
	}
	if ( acomm->word_mode == W_YOMI ) {
		/* 
		 * 今回の入力で確定した文字列がある場合には、
		 * 確定単語文字列として設定する。
		 */
		for( i = 0 ; i < acomm->wlen ; i++ ) {
			if ( acomm->word_yomiLen+i >= WORD_YOMI_MAX ) {
				/* printf( "よみがながすぎる。(%d)\n", i ) ; */
				break ;
			}
			acomm->word_yomi[acomm->word_yomiLen+i] = acomm->wbuf[i] ;
			echoStr[len++] = acomm->word_yomi[acomm->word_yomiLen+i] ;
		}
		acomm->word_yomiLen += i ;
		/*
		 * 未確定文字列を設定
		 */
		if ( acomm->echoLen > 0 ) {
			*revPos = len + acomm->revPos ;
			*revLen = acomm->revLen ;
			for( i = 0 ; i < acomm->echoLen ; i++ ) {
				echoStr[len++] = acomm->echoStr[i] ;
			}
		}
		else { 
			*revPos = len ;
			*revLen = 1 ;
		}
	}

	n = euc2wcs( "]", 1, &echoStr[len] ) ;
	len += n ;

	*echoLen = len ;
	if ( acomm->word_mode < W_HINSHI ) {
		return len ;
	}
	if ( acomm->word_mode > W_HINSHI ) {
		sprintf( tbuf, "  \311\312\273\354  ["/*=  品詞  [=*/ ) ;
		es = strlen( tbuf ) ;
		n = euc2wcs( tbuf, es, &echoStr[len] ) ;
		len += n ;

		s2e( acomm->hinsiPtr[acomm->word_hinshi-1].szHinsiStr, 
			strlen( acomm->hinsiPtr[acomm->word_hinshi-1].szHinsiStr ),
			tbuf, sizeof( tbuf )) ;

		es = strlen( tbuf ) ;
		n = euc2wcs( tbuf, es, &echoStr[len] ) ;
		len += n ;
			
		sprintf( tbuf, "] \244\350\244\355\244\267\244\244\244\307\244\271\244\253 [y/n]"/*=] よろしいですか [y/n]=*/, tbuf ) ;
		es = strlen( tbuf ) ;
		n = euc2wcs( tbuf, es, &echoStr[len] ) ;
		len += n ;

		*revPos = len - 1 ;
		*revLen = 1 ;
	}
	if ( acomm->word_mode == W_RESULT ) {
		switch( acomm->wordStatus ) {
			case 0 :
				sprintf( tbuf, "\305\320\317\277\275\252\316\273\244\267\244\336\244\267\244\277\241\243"/*=登録終了しました。=*/ ) ;
				break ;
			case 1 :
				sprintf( tbuf, "\244\263\244\316\306\311\244\337\244\307\244\317\244\263\244\354\260\312\276\345\305\320\317\277\244\307\244\255\244\336\244\273\244\363\241\243"/*=この読みではこれ以上登録できません。=*/ ) ;
				break ;
			case 2 :
				sprintf( tbuf, "\244\263\244\354\260\312\276\345\305\320\317\277\244\307\244\255\244\336\244\273\244\363\241\243"/*=これ以上登録できません。=*/ ) ;
				break ;
			case 3 :
				sprintf( tbuf, "\274\255\275\361\300\337\304\352\244\254\311\324\305\254\300\332\244\307\244\271\241\243"/*=辞書設定が不適切です。=*/ ) ;
				break ;
			case 4 :
				sprintf( tbuf, "\303\261\270\354\244\313\311\324\305\254\300\332\244\312\312\270\273\372\244\254\264\336\244\336\244\354\244\306\244\244\244\336\244\271\241\243"/*=単語に不適切な文字が含まれています。=*/ ) ;
				break ;
			case 5 :
				sprintf( tbuf, "\305\320\317\277\244\307\244\255\244\336\244\273\244\363\241\243"/*=登録できません。=*/ ) ;
				break ;
			case 6 :
				sprintf( tbuf, "\306\311\244\337\244\313\311\324\305\254\300\332\244\312\312\270\273\372\244\254\264\336\244\336\244\354\244\306\244\244\244\336\244\271\241\243"/*=読みに不適切な文字が含まれています。=*/ ) ;
				break ;
			case 7 :
				sprintf( tbuf, "\264\373\244\313\305\320\317\277\244\265\244\354\244\306\244\244\244\336\244\271\241\243"/*=既に登録されています。=*/ ) ;
				break ;
			default:
				sprintf( tbuf, "\303\261\270\354\305\320\317\277\244\313\274\272\307\324\244\267\244\336\244\267\244\277\241\243"/*=単語登録に失敗しました。=*/ ) ;
				break;
		}
		strcat( tbuf, " " ) ;
		es = strlen( tbuf ) ;
		n = euc2wcs( tbuf, es, echoStr ) ;
		len = n ;
		*revPos = len - 1 ;
		*revLen = 1 ;
	}
	*echoLen = len ;

#ifdef	ATOK_DEBUG
printf( "\303\261\270\354 EUC STR [%s]\n"/*=単語 EUC STR [%s]\n=*/, tbuf ) ;
#endif	/* ATOK_DEBUG */
	return len ;
}


#define	OFF	0
#define	ON	1
#define	itemSize( item )	sizeof( item ) / sizeof( item[0] )

/*
 * 単語登録
 */
menuItem	auxWordRegItem[] = {
	  { CTR_G,   OFF, OP_CANCEL, NULL, NULL, NULL } 
} ;
menuAux		auxWordRegMenu[] = {
	M_WORD, -1, 
		wordStartFunc,
		wordFunc,
		NULL, 
		wordStringFunc,
		"\303\261\270\354\305\320\317\277\241\247"/*=単語登録：=*/, NULL,
		auxWordRegItem, itemSize( auxWordRegItem ) 
} ;

#define	REVERSE		printf( "\033[7m" )
#define	NORMAL		printf( "\033[m" )

menuAux	*getMenuInstance( )
{
	return auxWordRegMenu ;
}

/* Page */
/*
 * [関数名]
 *		menuPrevOn( )
 * [表題]
 *              メニュー項目の移動
 * [呼出形式]
 *	int	menuPrevOn( int cur, int mode, menuAux *mAux )
 *
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		int          : cur    : i    : 現在の項目
 *		int          : mode   : i    : 移動モード
 *		menuAux      : *mAux  : i  : 現在のメニューデータ
 *
 * [返り値]
 *		移動後の項目番号
 *
 * [使用関数]
 *		なし
 * [機能]
 *		mode が０の時は、現在選択されている項目より１つ前の
 *		選択可能な項目を探す。
 *		mode が１の時は、現在のメニューで１番後ろの選択可能な
 *		項目を探す。
 *		選択可能な項目とはitem の mode がONの時である。
 */
int menuPrevOn(cur, mode, mAux)
int	cur;
int	mode;
menuAux	*mAux;
{
	register	menuItem	*item ;
	int		last = -1 ;
	int		target = -1 ;
	int		i ;

	item = &( mAux->items[mAux->nitems-1] ) ;
	for( i = mAux->nitems - 1 ; i >= 0 ; i-- ) {
#ifdef	ATOK_DEBUG
		printf( "item->mode = (%s) %s\n", 
			item->itemStr ?  item->itemStr : "NULL", 
			item->mode == ON ? "ON" : "OFF" ) ;
#endif	/* ATOK_DEBUG */
		if ( item->mode == ON ) {
			if ( last == -1 ) {
				last = i ;
				if ( mode ) {
					/* printf( "LAST FOUND %d\n", last ); */
					return last ;
				}
			}
			if ( target == -1 && i < cur ) {
				target = i ;
			}
		}
		item-- ;
	}
	if ( target == -1 ) {
		target = last ;
	}

	return target ;
}

/* Page */
/*
 * [関数名]
 *		menuNextOn( )
 * [表題]
 *              メニュー項目の移動
 * [呼出形式]
 *	int	menuNextOn( int cur, int mode, menuAux *mAux )
 *
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		int          : cur    : i    : 現在の項目
 *		int          : mode   : i    : 移動モード
 *		menuAux      : *mAux : i  : 現在のメニューデータ
 *
 * [返り値]
 *		移動後の項目番号
 *
 * [使用関数]
 *		なし
 * [機能]
 *		mode が０の時は、現在選択されている項目より１つ後ろの
 *		選択可能な項目を探す。
 *		mode が１の時は、現在のメニューで先頭の選択可能な
 *		項目を探す。
 *		選択可能な項目とはitem の mode がONの時である。
 */
int menuNextOn(cur, mode, mAux)
int	cur;
int	mode;
menuAux	*mAux;
{
	register menuItem	*item ;
	int	first = -1 ;
	int	target = -1 ;
	int	i ;

	item = mAux->items ;
	for( i = 0 ; i < mAux->nitems ; i++ ) {
		if ( item->mode == ON ) {
			if ( first == -1 ) {
				first = i ;
				if ( mode ) {
					return first ;
				}
			}
			if ( target == -1 && i > cur ) {
				target = i ;
			}
		}
		item++ ;
	}
	if ( target == -1 ) {
		target = first ;
	}

	return target ;
}

/* Page */
/*
 * [関数名]
 *		menuKey( )
 * [表題]
 *              メニュー内有効キーのチェック
 * [呼出形式]
 *		int	menuKey( menuAux *mAux, WORD key, int *cur )
 *
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		menuAux      : *mAux : i  : 現在のメニューデータ
 *		WORD         : key      : i  : 入力されたキー
 *		int          : *cur    :   o    : 選択された項目
 *
 * [返り値]
 *		0 : 項目は選択されていない。 
 *		その他: 選択された項目のアドレス
 *
 * [使用関数]
 *		なし
 * [機能]
 *		指定されたキーが、 現在表示しているメニューより、
 *		項目を選択するための有効キーかチェックし、
 *		有効キー立った場合には、項目が選択された物とし、
 *		その項目番号を設定し、項目のアドレスを返す。
 */
menuItem *menuKey(mAux, key, cur)
menuAux	*mAux;
WORD	key;
int	*cur;
{
	register menuItem	*item ;
	register int		i ;

	*cur = 0 ;
	item = mAux->items ;
	for ( i = 0 ; i < mAux->nitems ; i++ ) {
		/*
		printf( "Check Keys for '%s'\n",
			item->itemStr ?  item->itemStr : "NULL" ) ;
		*/
		if ( item->key == key ) {
		/* printf( "found\n" ) ; */
			*cur = i ;
			return item ;
		}
		item++ ;
	}
	/* printf( "not found\n" ) ; */
	return ( menuItem * )NULL ;
}

/* Page */
/*
 * [関数名]
 *		MenuCtrl( )
 * [表題]
 *              メニュー表示時のイベントコントロール
 * [呼出形式]
 *		int	MenuCtrl( _XatokRecPtr acomm, menuAux **curMenu,
 *			WORD *aajcode, int op, int *length )
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		_XatokRecPtr : acomm   : i/o : kinput2用変換バッファ
 *		menuAux      : **curMenu : i/o : 現在のメニューデータ
 *		WORD         : *aajcode : i/o : 入力されたAAJCODE
 *		int          : op       : i   : 開始ファンクションの呼び出し
 *		int          : *length  : o   : 入力文字列の長さ
 *
 * [返り値]
 *		0 : メニューの変更無し
 *		1 : メニュー処理キャンセル
 *		2 : メニュー選択項目の処理決定
 *
 * [使用関数]
 *		menuAux->startProc( )
 *		menuAux->defaultProc( )
 *		menuAux->endProc( )
 *		menuNextOn( )
 *		menuPrevOn( )
 * [機能]
 *		AUXウィンドウにメニューが表示されている時の
 *		イベント管理を行なう。
 *		イベントにより、表示しているメニューデータの
 *		切替えや、AUXウィンドウの終了を行なう。
 */
int MenuCtrl(acomm, curMenu, aajcode, op, length)
_XatokRecPtr    acomm;
menuAux		**curMenu;
WORD		*aajcode;
int		op;
int		*length;
{

	menuItem	*target ;
	menuItem	sel ;
	int		newNbr ;
	int		changes =-1 ;
	int		cur ;

	*length = 0 ;
	if ( op ) {	/* Start Proc */
		if (( *curMenu  )->startProc ) {
			( *curMenu )->startProc( acomm ) ;
		}
		return ;
	}

	target = menuKey( *curMenu, *aajcode, &newNbr ) ;
	/*
	 * 上位でDEFAULT PROCを実行する。
	 */
	if ( !target ) {
#ifdef	ATOK_DEBUG
		printf( "AUX DEFAULT FUNC\n" ) ;
#endif	/* ATOK_DEBUG */

		if (( *curMenu )->defaultProc ) {
			sel.op = 0 ;
			*length = ( *curMenu )->defaultProc( acomm, *curMenu, aajcode, &sel.op ) ;
			if ( sel.op ) {
				target = &sel ;
				goto menu_op ;
			}
#ifdef	ATOK_DEBUG
		printf( "\274\302\271\324\n"/*=実行\n=*/ ) ;
#endif	/* ATOK_DEBUG */
		}
		else {
#ifdef	ATOK_DEBUG
			printf( "\244\312\244\267\n"/*=なし\n=*/ ) ;
#endif	/* ATOK_DEBUG */
		}
		return -1 ;
	}
	/*
	 *  表示しているメニューより、選択しているものが変わった。
	 */
menu_op :
	switch( target->op ) {
		case OP_NEXT :
			( *curMenu )->cur = menuNextOn(( *curMenu )->cur, 0, *curMenu ) ;
			changes = 0 ;
			break ;
		case OP_FIRST :
			( *curMenu )->cur = menuNextOn(( *curMenu )->cur, 1, *curMenu ) ;
			changes = 0 ;
			break ;
		case OP_PREV :
			( *curMenu )->cur = menuPrevOn(( *curMenu )->cur, 0, *curMenu ) ;
			changes = 0 ;
			break ;
		case OP_LAST :
			( *curMenu )->cur = menuPrevOn(( *curMenu )->cur, 1, *curMenu ) ;
			changes = 0 ;
			break ;
		case OP_SPEC :
			/*
			 * 指定した番号
			 */
			( *curMenu )->cur = newNbr ;
			changes = 0 ;
			break ;
		case OP_CANCEL :
			/*
			 * キャンセルが押されたので上位メニューへ
			 */
			if (( *curMenu )->endProc ) {
				( *curMenu )->endProc( acomm ) ;
			}
			*curMenu = ( *curMenu )->parent ;
			if ( *curMenu ) {
				if (( *curMenu )->startProc ) {
					( *curMenu )->startProc( acomm ) ;
				}
			}
			changes = 1 ;
			break ;
		case OP_SELECT :
			/*
			 * 決定キーが押されたので処理実行
			 * 下位メニューがあれば、メニュー変更
			 */
			cur = ( *curMenu )->cur ;
			if (( *curMenu )->items[cur].child ) {
				( *curMenu )->items[cur].child->parent = ( *curMenu ) ;
				*curMenu = ( *curMenu )->items[cur].child ;
				if (( *curMenu )->startProc ) {
					( *curMenu )->startProc( acomm, *aajcode ) ;
				}
			}
			changes = 2 ;
			break ;
		default :
			changes = 0 ;
			break ;
	}

	return changes ;
}


/* Page */
/*
 * [関数名]
 *		auxMenuCopy( )
 * [表題]
 *              メニューデータのコピー
 * [呼出形式]
 *		menuAux *auxMenuCopy( menuAux *orgAux )
 *
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		menuAux      : *orgAux : i   : コピー元メニューデータ
 *
 * [返り値]
 *		コピーしたメニューデータ
 *
 * [使用関数]
 *		auxMenuCopy( )
 * [機能]
 *		指定されたメニューデータを新規バッファにコピーして
 *		そのアドレスを返す。
 */
menuAux	*auxMenuCopy(orgAux)
menuAux	*orgAux;
{
	menuItem	*item ;
	int		nitem ;
	int		i ;
	menuAux		*newAux ;

	newAux = ( menuAux * )malloc( sizeof( menuAux )) ;
	if ( !newAux )	return 0 ;

	memcpy(( char * )newAux, ( char * )orgAux, sizeof( menuAux )) ;

	item = ( menuItem * )malloc( sizeof( menuItem ) * orgAux->nitems ) ;
	newAux->items = item ;
	for( i = 0 ; i < orgAux->nitems ; i++ ) {
		memcpy(( char * )item, ( char * )&orgAux->items[i], 
			sizeof( menuItem )) ;
		if ( item->child ) {
			item->child = auxMenuCopy( item->child ) ;
		}
		item++ ;
	}
	return newAux ;
}

/* Page */
/*
 * [関数名]
 *		auxMenuString( )
 * [表題]
 *              メニュー表示用文字列の作成
 * [呼出形式]
 *	int	auxMenuString( _XatokRecPtr  acomm, menuAux *mAux,
 *			wchar *echoStr, int *echoLen, int *revLen,
 *			int *revPos )
 *
 * [引数]
 *              型           : 名  称  : I O : 説      明
 *		_XatokRecPtr : acomm   : i/o : kinput2用変換バッファ
 *		menuAux      : *mAux : i  : 現在の MENU CLASS
 *		wchar        : *echoStr:   o : 表示文字列格納領域
 *		int          : *echoLen :  o : 表示文字列の長さ
 *
 * [返り値]
 *		表示文字列の長さ
 *
 * [使用関数]
 *		menuAux->stringProc( )
 *		euc2wcs( )
 * [機能]
 *		メニューに表示するための文字列を作成する。
 *		現在表示しているメニューに文字列を作成するための
 *		関数(menuAux->stirngProc)が指定されている場合には、
 *		その関数を呼び出して、文字列を作成する。
 */
int auxMenuString(acomm, mAux, echoStr, echoLen, revLen, revPos)
_XatokRecPtr    acomm;
menuAux		*mAux;
wchar		*echoStr;
int		*echoLen;
int		*revLen;
int		*revPos;
{
	register menuItem	*item ;
	char	tbuf[BUFSIZ*2] ;
	int	len ;
	int	i, es ;
	char	*ep ;
	int		status ;

	item = mAux->items ;

	*revPos = 0 ;
	*revLen = 0 ;
	len = 0 ;
	bzero( tbuf, sizeof( tbuf )) ;

#ifdef	ATOK_DEBUG
printf( "AUX ADDR (STR) 0x%x\n", mAux ) ;
#endif	/* ATOK_DEBUG */
	if ( mAux->stringProc ) {
#ifdef	ATOK_DEBUG
	printf( "STRING PROC\n" ) ;
#endif	/* ATOK_DEBUG */
		len = ( mAux->stringProc )( acomm, mAux,
			echoStr, echoLen, revLen, revPos ) ;
		return len ;
	}
	ep = tbuf ;
	if ( mAux->str ) {
		strcpy( ep, mAux->str ) ;
		ep += strlen( ep ) ;
	}

#ifdef	ATOK_DEBUG
	printf( "auxMenuString START(%d)\n", mAux->nitems ) ;
#endif	/* ATOK_DEBUG */

	for( i = 0 ; i < mAux->nitems ; i++ ) {

#ifdef	ATOK_DEBUG
	printf( "ITEMS %s\n", item->itemStr ? item->itemStr : "NULL" ) ;
#endif	/* ATOK_DEBUG */
		if ( item->mode == ON ) {
			if ( mAux->cur == i ) {
				es = strlen( tbuf ) ;
				*revPos = euc2wcs( tbuf, es, echoStr )  ;
#ifdef	ATOK_DEBUG
			printf( "SET ECHO POS %d\n", *revPos ) ;
#endif	/* ATOK_DEBUG */

				len += *revPos ;
				es = strlen( item->numStr ) ;

				*revLen = euc2wcs( item->numStr, es, 
						&echoStr[*revPos] ) ;
				len += *revLen ;
#ifdef	ATOK_DEBUG
			printf( "SET ECHO LEN %d\n", *revLen ) ;
#endif	/* ATOK_DEBUG */
				ep = tbuf ;
				sprintf( ep, " %s", item->itemStr ) ;
				ep += strlen( ep ) ;

				REVERSE ;
				printf( "%s", item->numStr ) ;
				NORMAL ;
				printf( " %s", item->itemStr ) ;
			}
			else {
				sprintf( ep, "%s %s", item->numStr, item->itemStr ) ;
				ep += strlen( ep ) ;

#ifdef	ATOK_DEBUG
				printf( "%s", item->numStr ) ;
				printf( " %s", item->itemStr ) ;
#endif	/* ATOK_DEBUG */
			}
			sprintf( ep, "    " ) ;
			ep += strlen( ep ) ;
		}
		item++ ;
	}

	if (( es = strlen( tbuf ))) {
		es = euc2wcs( tbuf, es, &echoStr[*revPos + *revLen] ) ;
#ifdef	ATOK_DEBUG
		printf( "STR [%s] set +len = %d\n", tbuf, es ) ;
#endif	/* ATOK_DEBUG */
		len += es ;
	}
	*echoLen = len ;
#ifdef	ATOK_DEBUG
	printf( "len = %d, echoLen = %d\n", len, *echoLen ) ;
#endif	/* ATOK_DEBUG */
	return len ;
}

/*
 * ここから下はDEBUG用
 */

menuPrint(mAux)
menuAux	*mAux;
{
	int	i ;
	register menuItem	*item ;

	item = mAux->items ;
	if ( mAux->str ) {
		printf( "%s", mAux->str ) ;
	}
	for( i = 0 ; i < mAux->nitems ; i++ ) {
		if ( item->mode == ON ) {
			if ( mAux->cur == i ) {
				REVERSE ;
				printf( "%s", item->numStr ) ;
				NORMAL ;
				printf( " %s", item->itemStr ) ;
			}
			else {
				printf( "%s", item->numStr ) ;
				printf( " %s", item->itemStr ) ;
			}
			printf( "    " ) ;
		}
		item++ ;
	}
	printf( "\n" ) ;
}

#ifdef	STAND
void
main (argc, argv)
int argc;
char **argv;
{
	
	char	buf[BUFSIZ] ;
	menuAux *topMenu ;
	menuAux *cur ;

	int	status ;
	int	key ;

	topMenu = auxTopMenu ;
	cur = topMenu ;

	while( 1 ) {
		menuPrint( cur ) ;
		printf( "enter command => " ) ;
		gets( buf ) ;
		key = buf[0] ;
		status = MenuCtrl( &cur, key ) ;
		if ( status == 1 ) {
			if ( !cur ) {
				printf( "Program terminated\n" ) ;
				exit( 0 ) ;
			}
		}
		else if ( status == 2 ) {
			printf( "go to Chiled\n" ) ;
		}
		else if ( status != 0 ) {
			switch( cur->mode ) {
				case M_AUX :
					printf( "TOP MENU FUNC\n" ) ;
					break ;
				case 	M_CODE	:
					printf( "CODE MENU FUNC\n" ) ;
					break ;
				case 	M_CODESJ	:
					printf( "SJ FUNC\n" ) ;
					break ;
				case 	M_CODEEUC	:
					printf( "EUC FUNC\n" ) ;
					break ;
				case 	M_CODEJIS	:
					printf( "JIS FUNC\n" ) ;
					break ;
				case 	M_CODEKUTEN	:
					printf( "KUTEN FUNC\n" ) ;
					break ;
				case 	M_KIGOU		:
					printf( "KIGOU FUNC\n" ) ;
					break ;
				default: 
					printf( "UNKNOWN\n" ) ;
					break ;
			}
		}
	}
}

#endif	STAND
