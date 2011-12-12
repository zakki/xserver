/******************************************************************************
* ファイル名   : KEY_DEF.H
* ファイル概要 : 内部キーコード,機能キー番号,キーデータ構造の定義
* 作成者   :
* 作成日   :
* 適用システム :
* 適用機種 :
* 特記事項 :
******************************************************************************/

/*===========================================================================*/
/* 機能の統廃合についてのコメント。  カスタマイザのスタイルコンバートに関係。*/
/*===========================================================================*/
/* ATOK11の機能をATOK12では統合、分割、削除する機能は以下の通り */
/*
	ATOK11						-> ATOK12

	FUNC_CAND_NEXT(0x3b)  次候補表示中の次候補移動	-> FUNC_CNV_NEXT(0x08) 変換(次候補)へ統合
	FUNC_CAND_PREV(0x3d)  次候補表示中の前候補移動	-> FUNC_CNV_PREV(0x09) 変換(前候補)へ統合
	FUNC_CNV_CODE(0x69)   コード変換                -> FUNC_CNV_NEXT(0x08) 変換(次候補)へ統合
	FUNC_DECIDEONE(0x7d)  変換文節１文字確定	-> FUNC_KAKU_ONE(0x0a) １音確定へ統合
	FUNC_DESTRUCTONE(0x7e)変換文節１文字消去	-> FUNC_CHR_DEL(0x01)  １文字削除(カーソル位置)へ統合
	FUNC_REVERTFLUSH(0x82)変換取消して全文字削除	-> FUNC_CNV_CANCL(0x32) 変換取消(注目文節以降) と FUNC_DEL_ALL(0x1e) 全文字削除へ分割
	FUNC_ICONEXEC(0xb1)   最小化			-> 削除
	FUNC_OPPLEXEC(0xb5)   操作パレット起動          -> 削除
*/


/*===========================================================================*/
/*      ATOK機能キー番号定義                                                 */
/*===========================================================================*/
/*
	00〜ffを機能キー番号として定義する
	(@マークはパレット用に公開している機能番号. @@はWienで追加予定)
	(×マークはキー、パレットともに公開しない機能番号である)
	(<<マークはATOK12からの変更箇所である)
*/

/* ■変更履歴 */
/*    98/06/18	削除 FUNC_CNV_FUKUGO	0x9d 複合語変換 */
/*    98/06/18	削除 FUNC_MENU_ENV	0x5b 環境設定一括切替メニュー */

#define FUNC_CHR_BS		0x00 	/*@ １文字削除(カーソルの左側)  */
#define FUNC_CHR_DEL		0x01	/*@ １文字削除(カーソル位置)	旧の変換文節１文字消去機能をプラス */
#define FUNC_CHR_RIGHT		0x02	/*@ カーソル移動(１文字右)      */
#define FUNC_CHR_LEFT		0x03	/*@ カーソル移動(１文字左)      */
#define FUNC_CHR_TAIL		0x04	/*@ カーソル移動(文末)          */
#define FUNC_CHR_HEAD		0x05	/*@ カーソル移動(文頭)          */
#define FUNC_CNV_BUSYU		0x06	/*@ 部首変換                    */
#define FUNC_CNV_KEEP		0x07	/*  継続変換(自動変換時)        */
#define FUNC_CNV_NEXT		0x08	/*@ 変換(次候補)				旧名 FUNC_CNV */
#define FUNC_CNV_PREV		0x09	/*  変換(前候補)                <<追加*/
#define FUNC_KAKU_ONE		0x0a	/*@ １音確定                    旧の変換文節１文字確定機能をプラス */
#define FUNC_CNV_HMUHEN		0x0b	/*@ 半角無変換後変換			旧名 FUNC_CNV_HANMU */
#define FUNC_CNV_HIRA		0x0c	/*@ ひらがな(後)変換            */
#define FUNC_CNV_ZKATA		0x0d	/*@ カタカナ(後)変換            旧名 FUNC_CNV_KANA */
#define FUNC_CNV_HAN		0x0e	/*@ 半角(後)変換                */
#define FUNC_CNV_ZMUHEN		0x0f	/*@ 無変換(後)変換              旧名 FUNC_CNV_MUHEN */
#define FUNC_MENU_DIC		0x10	/*@ 変換辞書切替選択メニュー    */
#define FUNC_CNV_DIC01		0x11	/*@ 辞書１変換                  */
#define FUNC_CNV_DIC02		0x12	/*@ 辞書２変換                  */
#define FUNC_CNV_DIC03		0x13	/*@ 辞書３変換                  */
#define FUNC_CNV_DIC04		0x14	/*@ 辞書４変換                  */
#define FUNC_CNV_DIC05		0x15	/*@ 辞書５変換                  */
#define FUNC_CNV_DIC06		0x16	/*@ 辞書６変換                  */
#define FUNC_CNV_DIC07		0x17	/*@ 辞書７変換                  */
#define FUNC_CNV_DIC08		0x18	/*@ 辞書８変換                  */
#define FUNC_CNV_DIC09		0x19	/*@ 辞書９変換                  */
#define FUNC_CNV_DIC10		0x1a	/*@ 辞書０変換                  */
#define FUNC_KAKU_BUBUN		0x1b	/*@ 部分確定(注目文節まで)      */
#define FUNC_CANDZ   		0x1c	/*@ 全候補変換	                旧名 FUNC_CAND_ALL */
#define FUNC_KAKU_ALL		0x1d	/*@ 全文確定                    */
#define FUNC_DEL_ALL		0x1e	/*@ 全文字削除                  */
#define FUNC_REPEAT		0x1f    /*@ 確定リピート                */
#define FUNC_UNDO		0x20	/*@ 確定アンドゥ                */
#define FUNC_KAKU_BUBUNONE	0x21	/*×部分確定(先頭文節のみ)      <<追加 非公開HN */
#define FUNC_MENU_ALL		0x22	/*@ ATOKメニュー                */
#define FUNC_MENU_MOJI		0x23	/*@ 入力文字種選択メニュー      */
#define FUNC_MENU_KUTO		0x24	/*@ 句読点モード選択メニュー    */
#define FUNC_SWTCH_AFIX		0x25	/*  固定入力順次切替            旧名 FUNC_SWTCH_AKOTE */
#define FUNC_SWTCH_EFIX	    	0x26	/*  固定入力英字順次切替        旧名 FUNC_SWTCH_EKOTE */
#define FUNC_SWTCH_KFIX	    	0x27	/*  固定入力カタカナ順次切替    旧名 FUNC_SWTCH_KKOTE */
#define FUNC_SWTCH_KUTO		0x28	/*  句読点モード切替            */
#define FUNC_ONOFF_FIXHIRA	0x29	/*@ 全角ひらがな固定(あ)ON/OFF  旧名 FUNC_ONOFF_HIRA */
#define FUNC_ONOFF_FIXZKATA	0x2a	/*@ 全角カタカナ固定(ア)ON/OFF  旧名 FUNC_ONOFF_ZKANA */
#define FUNC_ONOFF_FIXHKATA	0x2b	/*@ 半角カタカナ固定(ア)ON/OFF   旧名 FUNC_ONOFF_HKANA */
#define FUNC_ONOFF_FIXZMUHEN	0x2c	/*@ 無変換全角固定(Ａ)ON/OFF    旧名 FUNC_ONOFF_MUHEN */
#define FUNC_ONOFF_FIXHMUHEN	0x2d	/*@ 無変換半角固定(A)ON/OFF     旧名 FUNC_ONOFF_HAN */
#define FUNC_ONOFF_EIJI		0x2e	/*@ 英字入力ON/OFF              */
#define FUNC_CHR_ERRYOMI	0x2f	/*@ 入力読み誤り位置ジャンプ    */
#define FUNC_KAKU_HEAD		0x30	/*@ １文字確定(文頭)            旧名 FUNC_OUT_HEAD */
#define FUNC_KAKU_TAIL		0x31	/*@ １文字確定(文末)            旧名 FUNC_OUT_TAIL */
#define FUNC_CNV_CANCL		0x32	/*@ 変換取消(注目文節以降)      */
#define FUNC_CNV_CANCLALL	0x33	/*@ 全変換取消                  */
#define FUNC_KUGI_RIGHT		0x34	/*@ 文節区切り(１文字右)        旧名 FUNC_FOCUS_RIGHT */
#define FUNC_KUGI_LEFT		0x35	/*@ 文節区切り(１文字左)        旧名 FUNC_FOCUS_LEFT */
#define FUNC_BST_NEXT		0x36	/*@ 文節移動(右)                旧名 FUNC_FOCUS_NEXT */
#define FUNC_BST_PREV		0x37	/*@ 文節移動(左)                旧名 FUNC_FOCUS_PREV */
#define FUNC_BST_TAIL		0x38	/*@ 文節移動(最終)              旧名 FUNC_FOCUS_TAIL */
#define FUNC_BST_HEAD		0x39	/*@ 文節移動(先頭)              旧名 FUNC_FOCUS_HEAD */
#define FUNC_TAN_DEL		0x3a	/*@ 単語削除                    */
/*				0x3b	    拡張用                      <<削除 FUNC_CAND_NEXT 次候補移動 */
/*				0x3c	    拡張用                      */
/*				0x3d	    拡張用                      <<削除 FUNC_CAND_PREV 前候補移動 */
#define FUNC_CAND_PGNEXT	0x3e	/*  次候補群表示                旧名 FUNC_CNV_PGNEXT */
#define FUNC_CAND_PGPREV	0x3f	/*  前候補群表示                旧名 FUNC_CNV_PGPREV */
#define FUNC_SWTCH_ZORH		0x40	/*  全角／半角文字切替          */
#define FUNC_TAN_TOROKU		0x41	/*@ 単語登録                    */
/*				0x42	    拡張用                      */
#define FUNC_SWTCH_OKRI		0x43	/*  送りがなモード順次切替      */
#define FUNC_SWTCH_ROMA		0x44	/*  漢字入力モード切替          */
#define FUNC_SWTCH_HEN		0x45	/*  変換モード順次切替          */
#define FUNC_SWTCH_AKCI		0x46	/*  入力文字種順次切替          */
#define FUNC_SWTCH_EKCI		0x47	/*  入力文字種英字順次切替      */
#define FUNC_SWTCH_KKCI		0x48	/*  入力文字種カタカナ順次切替  */
#define FUNC_TO_KCIHIRA		0x49	/*@ 入力文字種全角ひらがな(あ)	旧名 FUNC_TO_KC_HIRA */
#define FUNC_TO_KCIZKATA	0x4a	/*@ 入力文字種全角カタカナ(ア)  旧名 FUNC_TO_KC_ZKANA */
#define FUNC_TO_KCIHKATA	0x4b	/*@ 入力文字種半角カタカナ(ア)   旧名 FUNC_TO_KC_HKANA */
#define FUNC_TO_KCIZMUHEN	0x4c	/*@ 入力文字種無変換全角(Ａ)    旧名 FUNC_TO_KC_MUHEN */
#define FUNC_TO_KCIHMUHEN	0x4d	/*@ 入力文字種無変換半角(A)     旧名 FUNC_TO_KC_HAN */
#define FUNC_TO_DIC		0x4e	/*@ 辞書・学習変更              */
#define FUNC_SWTCH_KOUGO	0x4f	/*  口語体モード切替            */
#define FUNC_MENU_KOUGO		0x50	/*@ 口語体モード選択メニュー    */
#define FUNC_MENU_ROMA		0x51	/*@ 漢字入力モード選択メニュー  */
#define FUNC_MENU_HEN		0x52	/*@ 変換モード選択メニュー      */
/*				0x53	    拡張用                      */
#define FUNC_MENU_OKRI		0x54	/*@ 送りがな選択メニュー        */
#define FUNC_MENU_COMP		0x55	/*×ATOK未確定文字メニュー      */
#define FUNC_MENU_CAND		0x56	/*×ATOK候補メニュー            */
#define FUNC_CHG_KANHAN		0x57	/*  漢字／半角モード切替        */
#define FUNC_MENU_IM		0x58	/*@ 入力モード選択メニュー      */
#define FUNC_ONOFF_ATOK		0x59	/*  ATOK ON/OFF                 <<公開復活 */
#define FUNC_SWTCH_IM		0x5a	/*  入力モード順次切替          */
/* #define FUNC_MENU_ENV	0x5b	/* @@環境設定一括切替メニュー	<<追加 */
#define FUNC_MENU_OPE		0x5c	/*@@操作メニュー		<<追加 */
#define FUNC_MENU_REYOMI	0x5d	/*  再変換別読みメニュー	<<追加 */
/*				0x5e	    拡張用                      */
/*				0x5f	    拡張用                      */
/*				0x60	    拡張用                      */
/*				0x61	    拡張用                      */
/*				0x62	    拡張用                      */
/*				0x63	    拡張用                      */
/*				0x64	    拡張用                      */
/*				0x65	    拡張用                      */
#define FUNC_MENU_HAN		0x66	/*@ 半角モード選択メニュー      */
#define FUNC_MENU_KCODE		0x67	/*  コード体系選択メニュー      */
#define FUNC_MENU_GUIDE		0x68	/*×ATOKガイドメニュー		*/
/*				0x69	    拡張用                      <<削除 FUNC_CNV_CODE コード変換 */
#define FUNC_SWTCH_KCODE	0x6a	/*  コード体系順次切替          */
/*				0x6b	    拡張用                      */
#define FUNC_SWTCH_HAN		0x6c	/*  半角モード順次切替          */
#define FUNC_KIGO_KAKU		0x6d	/*  記号確定                    */
#define FUNC_KIGO_NEXT		0x6e	/*  記号次候補移動              */
#define FUNC_KIGO_PREV		0x6f	/*  記号前候補移動              */
#define FUNC_KIGO_PGNEXT	0x70	/*  記号次候補群表示            */
#define FUNC_KIGO_PGPREV	0x71	/*  記号前候補群表示            */
#define FUNC_KUTEN_NEXT		0x72	/*  記号次区点表示              */
#define FUNC_KUTEN_PREV		0x73	/*  記号前区点表示              */
#define FUNC_KANJ_TOP		0x74	/*  記号先頭表示                */
#define FUNC_GAIJ_TOP		0x75	/*  記号外字先頭表示            */
#define FUNC_CAND_NEXTATR	0x76	/*  次候補属性ジャンプ          旧名 FUNC_KOHOATR_NEXT */
#define FUNC_CAND_PREVATR	0x77	/*  前候補属性ジャンプ          旧名 FUNC_KOHOATR_PREV */
#define FUNC_CNV_ACNV		0x78	/*  順次後変換                  旧名 FUNC_CNV_AATHN */
#define FUNC_CNV_ECNV		0x79	/*  順次無変換後変換            旧名 FUNC_CNV_EATHN */
#define FUNC_CNV_KCNV		0x7a	/*  順次カタカナ後変換          旧名 FUNC_CNV_KATHN */
#define FUNC_KUGI_ROMARIGHT	0x7b	/*@ ローマ字区切り直し(右)      旧名 FUNC_CHG_RMADJUST_R */
#define FUNC_KUGI_ROMALEFT	0x7c	/*@ ローマ字区切り直し(左)      旧名 FUNC_CHG_RMADJUST_L */
/*				0x7d	    拡張用			<<削除 FUNC_DECIDEONE   変換文節１文字確定 */
/*				0x7e	    拡張用			<<削除 FUNC_DESTRUCTONE 変換文節１文字消去 */
#define FUNC_CNVREAD_AORK	0x7f	/*  英字読みカナ読み順次変換    */
#define FUNC_CNVREAD_KTOA	0x80	/*  英字読み変換                */
#define FUNC_CNVREAD_ATOK	0x81	/*  カナ読み変換                */
#define FUNC_SWTCH_HIRAKATA	0x82	/*  文字種順次切替(あ/ア)       <<追加HN <<削除 FUNC_REVERTFLUSH 変換取消して全文字削除 */
#define FUNC_SWTCH_HIRAKATAEI	0x83	/*  文字種順次切替(あ/ア/Ａ)    <<追加HN */
#define FUNC_SWTCH_HIKAEI	0x84	/*  文字種順次切替(あ,ア/Ａ)    <<追加HN */
#define FUNC_SWTCH_HIRAEI	0x85	/*  文字種順次切替(あ/Ａ)       <<追加HN */
#define FUNC_SWTCH_KATAEI	0x86	/*  文字種順次切替(ア/Ａ)	    <<追加HN */
#define FUNC_ONOFF_FIXMODE	0x87	/*  固定入力ON/OFF              <<追加HN */
/*				0x88	    拡張用                      */
/*				0x89	    拡張用                      */
/*				0x8a	    拡張用                      */
/*				0x8b	    拡張用                      */
/*				0x8c	    拡張用			*/
#define FUNC_CAND_SORTFULL	0x8d	/*  候補並び替え(コード順)	<<追加HN*/
#define FUNC_CAND_SORTHEAD	0x8e	/*  候補並び替え(先頭文字)	<<追加HN*/
#define FUNC_CAND_SORTTAIL	0x8f	/*  候補並び替え(末尾文字)	<<追加HN*/
#define FUNC_MENU_AMET		0x90	/*@ ＡＭＥＴ切替選択メニュー    旧名 FUNC_MENU_VARI */
#define FUNC_AMET01		0x91	/*@ ＡＭＥＴ１                  旧名 FUNC_OUT_VARI01 */
#define FUNC_AMET02		0x92	/*@ ＡＭＥＴ２                  旧名 FUNC_OUT_VARI02 */
#define FUNC_AMET03		0x93	/*@ ＡＭＥＴ３                  旧名 FUNC_OUT_VARI03 */
#define FUNC_AMET04		0x94	/*@ ＡＭＥＴ４                  旧名 FUNC_OUT_VARI04 */
#define FUNC_AMET05		0x95	/*@ ＡＭＥＴ５                  旧名 FUNC_OUT_VARI05 */
#define FUNC_AMET06		0x96	/*@ ＡＭＥＴ６                  旧名 FUNC_OUT_VARI06 */
#define FUNC_AMET07		0x97	/*@ ＡＭＥＴ７                  旧名 FUNC_OUT_VARI07 */
#define FUNC_AMET08		0x98	/*@ ＡＭＥＴ８                  旧名 FUNC_OUT_VARI08 */
#define FUNC_AMET09		0x99	/*@ ＡＭＥＴ９                  旧名 FUNC_OUT_VARI09 */
#define FUNC_AMET10		0x9a	/*@ ＡＭＥＴ０                  旧名 FUNC_OUT_VARI10 */
/*				0x9b	    拡張用                      */
/*				0x9c	    拡張用                      */
/* #define FUNC_CNV_FUKUGO	0x9d	/*  複合語変換                  <<追加微妙*/
#define FUNC_CNV_ABBREV		0x9e	/*@@省入力変換                  <<追加HN */
#define FUNC_CNV_UNDO		0x9f	/*@@再変換                      <<追加 */
#define FUNC_TO_KANJ		0xa0	/*  漢字モード                  */
#define FUNC_TO_HAN		0xa1	/*  半角モード                  */
#define FUNC_TO_CODE		0xa2	/*  コード入力モード            */
#define FUNC_TO_KIGO		0xa3	/*  記号モード                  */
/*				0xa4	    拡張用                      */
#define FUNC_ON_KANALK		0xa5	/*  カナロックON                */
#define FUNC_OFF_KANALK 	0xa6	/*  カナロックOFF               */
#define FUNC_ONOFF_KANALK	0xa7	/*  カナロックONOFF             */
#define FUNC_CANDZ_NEXT		0xa8	/*  全候補次項目移動		*/
#define FUNC_CAND_HEAD		0xa9	/*  候補移動(先頭)              <<追加 */
#define FUNC_CANDZ_PREV		0xaa	/*  全候補前項目移動		*/
#define FUNC_CANDZ_LNNEXT	0xab	/*  全候補次行項目移動          旧名 FUNC_CAND_LNNEXT */
#define FUNC_CANDZ_LNPREV	0xac	/*  全候補前行項目移動          旧名 FUNC_CAND_LNPREV */
#define FUNC_CANDZ_KAKU		0xad	/*  全候補項目選択確定          旧名 FUNC_CAND_KAKU */
#define FUNC_CANDZ_ESC		0xae	/*  全候補項目選択取消          旧名 FUNC_CAND_ESC */
#define FUNC_CAND_TAIL		0xaf	/*  候補移動(最終)              <<追加 */
#define FUNC_APLEXEC_STARTNO	0xb0    /* アプリ起動関係開始NO.  */
/* #define FUNC_HELPEXEC	0xb0	/*@ ヘルプ起動                  */
#define FUNC_MENU_HELP		0xb0	/*@ ヘルプメニュー起動		*/
/*    本来、起動系でないがスタイルコンバートなどの互換性の為にここに入れる */
/*				0xb1	    拡張用                      <<削除 FUNC_ICONEXEC 最小化 */
#define FUNC_ATUTEXEC		0xb2	/*@ ATOKプロパティ起動          */
#define FUNC_UTEXEC		0xb3	/*@ 辞書ユーティリティ起動      */
#define FUNC_MJPLEXEC		0xb4	/*@ 文字パレット起動            */
/*				0xb5	/*  拡張用						<<削除 FUNC_OPPLEXEC 操作パレット */
#define FUNC_SOFTKEYBOARD	0xb6	/*@ クリックパレット		*/
#define FUNC_RESULTRECORD	0xb7	/*@ 文字列ボックス起動		*/
#define FUNC_FUNCKEYGUIDE	0xb8	/*@@ファンクションキーガイド	*/
#define FUNC_HELPTOPICEXEC	0xb9	/*×ヘルプトピック起動		*/
#define FUNC_HELPWEBEXEC	0xba	/*×Webヘルプ起動		*/
#define FUNC_VERINFOEXEC	0xbb	/*×バージョン情報起動		*/

#define FUNC_APLEXEC_ENDNO	0xbb   /* アプリ起動関係終了NO     */
/*				0xbc	    拡張用                      */
/*				0xbd	    拡張用                      */
/*				0xbe	    拡張用                      */
/*				0xbf	    拡張用                      */
/*				0xc0	    拡張用                      */
/*				0xc1	    拡張用                      */
/*				0xc2	    拡張用                      */
/*				0xc3	    拡張用                      */
/*				0xc4	    拡張用                      */
/*				0xc5	    拡張用                      */
/*				0xc6	    拡張用                      */
/*				0xc7	    拡張用                      */
/*				0xc8	    拡張用                      */
/*				0xc9	    拡張用                      */
/*				0xca	    拡張用                      */
/*				0xcb	    拡張用                      */
/*				0xcc	    拡張用                      */
/*				0xcd	    拡張用                      */
/*				0xce	    拡張用                      */
/*				0xcf	    拡張用                      */
/*				0xd0	    拡張用                      */
/*				0xd1	    拡張用                      */
/*				0xd2	    拡張用                      */
/*				0xd3	    拡張用                      */
/*				0xd4	    拡張用                      */
/*				0xd5	    拡張用                      */
/*				0xd6	    拡張用                      */
/*				0xd7	    拡張用                      */
/*				0xd8	    拡張用                      */
/*				0xd9	    拡張用                      */
/*				0xda	    拡張用                      */
/*				0xdb	    拡張用                      */
/*				0xdc	    拡張用                      */
/*				0xdd	    拡張用                      */
/*				0xde	    拡張用                      */
/*				0xdf	    拡張用			*/
/*--------------------------------------------------------------------*/
/* 以下は、ATOK内部仮想機能キー番号定義(カスタマイザには無関係なもの  */
/*--------------------------------------------------------------------*/
#define VFUNC_MENU_ESC		0xe0	/*×メニュー(モード)解除        */
#define VFUNC_MENU_KAKU		0xe1	/*×メニュー(モード)確定        */
#define VFUNC_CANDDIRECT	0xe2	/*×次候補ダイレクト            */
#define VFUNC_FLUSH     	0xe3	/*×フラッシュ                  */
#define VFUNC_FLUSH_KAKU	0xe4	/*×フラッシュ(確定して)        */
#define VFUNC_GUIDE_KAKU	0xe5	/*×ガイドライン入力確定        */
#define VFUNC_OTHERCANDLIST	0xe6	/*×次の別候補リストへの切替    <<追加*/
#define VFUNC_SOLOSHIFTKEY	0xe7	/*×単独Shiftキーでの解除処理   <<追加*/
/*				0xe8	    拡張用                      */
/*				0xe9	    拡張用                      */
/*				0xea	    拡張用                      */
/*				0xeb	    拡張用                      */
/*				0xec	    拡張用                      */
/*				0xed	    拡張用                      */
/*				0xee	    拡張用                      */
/*				0xef	    拡張用                      */
/*				0xf0	    拡張用                      */
/*				0xf1	    拡張用                      */
/*				0xf2	    拡張用                      */
/*				0xf3	    拡張用                      */
/*				0xf4	    拡張用                      */
/*				0xf5	    拡張用                      */
/*				0xf6	    拡張用                      */
/*				0xf7	    拡張用                      */
/*				0xf8	    拡張用			*/
/*				0xf9	    拡張用                      */
/*				0xfa	    拡張用                      */
/*				0xfb	    拡張用                      */
/*				0xfc	    拡張用                      */
/*				0xfd	    拡張用                      */
/*				0xfe	    拡張用                      */
/*				0xff	    拡張用                      */

/*===========================================================================*/
/*      カスタマイザ機能キー番号有効状態定義                                 */
/*===========================================================================*/
/*
   カスタマイザ機能キー番号の有効状態は
	漢1 : 文字未入力          半 : 半角入力
	漢2 : 文字入力中          記 : 記号入力
	漢3 : 変換中
	漢4 : 次候補表示中      (コード入力状態はない   (文節修正中状態はない
	漢5 : 全候補表示中       コ1 : 漢1,記のサブセット 漢1として扱う)
	漢6 : 文節区切り直し中   コ2 : 漢2のサブセット)
   の組み合わせで定義される
*/
#define FNG_NONE	    0x0000	/* 状態なし : 0固定値 */
#define FNG_KANNONE         0x0001	/* 漢1 : 文字未入力 */
#define FNG_KANINPUT        0x0002	/* 漢2 : 文字入力中 */
#define FNG_KANHENKAN       0x0004	/* 漢3 : 変換中 */
#define FNG_KANKOHO         0x0008	/* 漢4 : 次候補表示中 */
#define FNG_KANZKOHO        0x0010	/* 漢5 : 全候補表示中 */
#define FNG_KANKUGIRI       0x0020	/* 漢6 : 文節区切り直し中 */
#define FNG_HAN             0x4000	/* 半  : 半角入力 */
#define FNG_KIGOU           0x8000	/* 記  : 記号入力 */
/*
  ※注意 カスタマイザ状態定義の範囲は0x0001〜0x8000とする。
  ATOK内部状態定義の範囲は0x00010000〜0x80000000とする。
*/
#define FNG_NEUTRAL	  (FNG_KANNONE | FNG_HAN | FNG_KIGOU)
#define FNG_KANALLMIGHTY  (FNG_KANNONE | FNG_KANINPUT | FNG_KANHENKAN | FNG_KANKOHO | FNG_KANZKOHO | FNG_KANKUGIRI)
#define FNG_ALLMIGHTY	  (FNG_KANALLMIGHTY | FNG_HAN | FNG_KIGOU)
#define FNG_KANALLINPUT   (FNG_KANNONE | FNG_KANINPUT )
#define FNG_KANSTOCK	  (FNG_KANINPUT | FNG_KANHENKAN | FNG_KANKOHO | FNG_KANZKOHO | FNG_KANKUGIRI)
#define FNG_KANSTOCK234   (FNG_KANINPUT | FNG_KANHENKAN | FNG_KANKOHO )
#define FNG_KANSTOCK2346  (FNG_KANINPUT | FNG_KANHENKAN | FNG_KANKOHO | FNG_KANKUGIRI)
#define FNG_KANSTOCK236   (FNG_KANINPUT  | FNG_KANHENKAN | FNG_KANKUGIRI)
#define FNG_KANSTOCK34    (FNG_KANHENKAN | FNG_KANKOHO)
#define FNG_KANSTOCK345   (FNG_KANHENKAN | FNG_KANKOHO | FNG_KANZKOHO)
#define FNG_KANSTOCK346   (FNG_KANHENKAN | FNG_KANKOHO | FNG_KANKUGIRI)
#define FNG_KANSTOCK3456  (FNG_KANHENKAN | FNG_KANKOHO | FNG_KANZKOHO | FNG_KANKUGIRI)
#define FNG_KANKOHOCMN    (FNG_KANKOHO | FNG_KANZKOHO)
/*
#define FNG_KANUNDO     (FNG_KANNONE | FNG_KANINPUT | FNG_KANHENKAN)
#define FNG_KANUNDOTOGGLE (FNG_KANNONE | FNG_KANINPUT | FNG_KANHENKAN | FNG_KANKOHO | FNG_KANKUGIRI)
*/

/*===========================================================================*/
/*      ATOKCE内部状態定義 カスタマイザには関係ない部分 ここから             */
/*===========================================================================*/

/*
   ATOKエンジン内部機能キー番号の有効状態はカスタマイザ状態に加えて以下の状態がある。
   この状態はカスタマイザには無関係。
*/
/* ※下記を変更する場合は、atok12ce.rcのFNGR_xxxも変更する必要あり。*/
#define FNG_ATOKOFF	0x00100000	/* OFF : 漢字OFF			（キーは、特別処理）*/
#define FNG_CODENONE	0x00010000	/* コ1  : コード未入力		（キーは、漢1(文字未入力)状態で参照）*/
#define FNG_CODEINPUT	0x00020000	/* コ2  : コード入力中		（キーは、漢2(文字入力中)状態で参照）*/
#define FNG_BUSYU	0x00040000	/* 部  : 部首、コード変換中	（キーは、記号	         状態で参照）*/
#define FNG_TANDEL	0x00200000	/* 削  : 単語削除中			（キーは、漢3(変換中)    状態で参照）*/
#define FNGSP_CHGINPUT	0x10000000	/* 入力モード(rot)変更に関わる機能(特別)*/
#define FNGSP_AMET	0x20000000	/* AMET起動機能(特別)*/
/*
   ※カスタマイザ状態定義の範囲は0x0001〜0x8000とする。
     ATOK内部状態定義の範囲は0x00010000〜0x80000000とする。
*/


/*===========================================================================*/
/*      ATOKエンジン入力キーコード定義(AAJキーコード表)                      */
/*===========================================================================*/
/*****************************************************************************/
/* AAJキーコード表(拡張イベント定義)                                         */
/*****************************************************************************/
#define BUTTONEVENT	0x7ffa	/* Mouse Event*/
#define KANJIINPUTEVENT	0x7ffb	/* 漢字コード入力 Event*/
/*
   AAJコードの定義が2バイトの構成であるため、状況によりAAJ拡張エリアを使用する
   1.拡張1 .AtCEIncode(入力キーコード)の値がBUTTONEVENTの場合
     マウス操作イベントと解釈され、以下の項目を参照する。(MSE_DEF.H参照)
      .AtCEInMouseMsg  (入力マウスメッセージタイプ)
      .AtCEInWndNo     (入力マウスメッセージ発生ウインドウ)
      .AtCEInEventPosX (入力マウスメッセージ発生文字位置     1〜)
      .AtCEInEventPosY (入力マウスメッセージ発生文字位置補足 0/1 前半/後半)
   2.拡張2 .AtCEIncode(入力キーコード)の値がKANJIINPUTEVENTの場合
     UCS2文字入力イベントと解釈され、以下の項目を参照する。
      .AtCEInkanjicode (入力キーコード(UCS2文字コード))
   
   以外はAAJキーコード値が設定される。(KEY_DEF.H参照)
    [AAJコード]
      NKEY                     無効キー定義
      (VKEY|機能キー番号)      仮想キー定義(ATOK機能キー)
      (VMOJIKEY|仮想文字番号)  仮想キー定義(独仏発音文字キー)
      物理キーコード
     |                               |               |
     | 15  14  13  12| 11  10  09  08| 07          00|
     |---|---|---|---|---|---|---|---|-----.....-----|
           |   |   |       |   |   |    キーコード値
           |   |   |       |   |   | 
           |   |   |       |   |   |__ FNCKEY(機能キー bit)
           |   |   |       |   |______ TENKEY(テンキー bit)
           |   |   |       |__________ SHIFT (Shift状態bit)
           |   |   | 
           |   |   |__________________ CTRL  (Ctrl状態 bit)
           |   |______________________ ALT   (Alt状態  bit)
           |__________________________ CAPS  (CAPS状態 bit)
*/

/*****************************************************************************/
/* AAJキーコード表(制御bit定義)                                              */
/*****************************************************************************/
#define NKEY		0xffff		/* 無効キーコード定義*/
#define VKEY		0x0300		/* 0x03xx  仮想機能キーコードbit*/
#define VKEY_H		0x03		/* 0x03xx  仮想機能キーコードbit*/
#define FNCKEY		0x0100		/* 機能キーbit*/
#define TENKEY		0x0200		/* テンキーbit*/
#define SHIFT		0x0400		/* Shift状態bit*/
#define VMOJIKEY	0x0a00		/* 0x0axx  仮想文字キーコードbit*/
#define VMOJIKEY_H	0x0a		/* 0x0axx  仮想文字キーコードbit*/
#define CTRL		0x1000		/* Ctrl状態bit*/
#define ALT		0x2000		/* Alt状態 bit*/
#define GRPH		ALT		/*  "*/
#define CAPS		0x4000		/* CAPS状態 bit*/

#define NOTMOJIKEY_MASK	(FNCKEY|CTRL|ALT) /*文字キー判定用Mask*/
#define VKEY_MASK	0x0300		/* 仮想機能キー判定用Mask*/
#define VMOJIKEY_MASK	0x0a00		/* 仮想文字キーコード判定用Mask*/

/************************************************************************/
/* AAJキーコード表(物理キーコード値定義)                                */
/************************************************************************/
#define SPACE			0x0020			/* Space */
#define BS			(FNCKEY|0x0008) 	/* BackSpace */
#define TAB			(FNCKEY|0x0009) 	/* Tab */
#define CR			(FNCKEY|0x000d)		/* Enter */
#define EESC			(FNCKEY|0x001b)		/* Esc */
#define F1			(FNCKEY|0x0021)		/* F1 〜 F20 */
#define F2			(FNCKEY|0x0022)		/**/
#define F3			(FNCKEY|0x0023)		/**/
#define F4			(FNCKEY|0x0024)		/**/
#define F5			(FNCKEY|0x0025)		/**/
#define F6			(FNCKEY|0x0026)		/**/
#define F7			(FNCKEY|0x0027)		/**/
#define F8			(FNCKEY|0x0028)		/**/
#define F9			(FNCKEY|0x0029)		/**/
#define F10			(FNCKEY|0x002A)		/**/
#define F11			(FNCKEY|0x002B)		/**/
#define F12			(FNCKEY|0x002C)		/**/
#define F13			(FNCKEY|0x002D)		/**/
#define F14			(FNCKEY|0x002E)		/**/
#define F15			(FNCKEY|0x002F)		/**/
#define F16			(FNCKEY|0x0030)		/**/
#define F17			(FNCKEY|0x0031)		/**/
#define F18			(FNCKEY|0x0032)		/**/
#define F19			(FNCKEY|0x0033)		/**/
#define F20			(FNCKEY|0x0034)		/**/
/*#define JIKKO			(FNCKEY|0x0046)		/* 実行 */
/*#define CLEAR			(FNCKEY|0x0047)		/* クリア */
/*#define SHURYOU		(FNCKEY|0x0048)		/* 終了 */
/*#define TORIKESI		(FNCKEY|0x0049)		/* 取消 */
#define EEND			(FNCKEY|0x004f)		/* End */
#define HIRAGANA		(FNCKEY|0x0050)		/* ひらがな */
#define KATAKANA		(FNCKEY|0x0051)		/* カタカナ */
#define EIJI			(FNCKEY|0x0052)		/* 英数 */
#define ZORH			(FNCKEY|0x0053)		/* 全角・半角 */
#define ROMA			(FNCKEY|0x0054)		/* ローマ字<< */
/*#define LWIN			(FNCKEY|0x005a)		/* Windows(Left) */
/*#define RWIN			(FNCKEY|0x005b)		/* Windows(Right) */
/*#define APPS			(FNCKEY|0x005c)		/* Application */
#define XFER			(FNCKEY|0x0060)		/* 変換 */
#define PGDN			(FNCKEY|0x0061)		/* PageDown */
#define RLUP			PGDN			/* " */
#define PGUP			(FNCKEY|0x0062)		/* PageUp */
#define RLDN			PGUP			/* " */
#define INS			(FNCKEY|0x0063)		/* Insert */
#define DEL			(FNCKEY|0x0064)		/* Delete */
#define UP			(FNCKEY|0x0065)		/* ↑ */
#define LEFT			(FNCKEY|0x0066)		/* ← */
#define RIGHT			(FNCKEY|0x0067)		/* → */
#define DOWN			(FNCKEY|0x0068)		/* ↓ */
#define HOME			(FNCKEY|0x0069)		/* Home */
#define HELP			(FNCKEY|0x006a)		/* Help */
#define NFER			(FNCKEY|0x006b)		/* 無変換 */
#define KANJ			(FNCKEY|0x006c)		/* 漢字 */

#define SFT_SP			(SHIFT+SPACE)
#define SFT_BS			(SHIFT+BS)
#define SFT_TAB			(SHIFT+TAB)
#define SFT_CR			(SHIFT+CR)
#define SFT_ESC			(SHIFT+EESC)
#define SFT_F1			(SHIFT+F1)
#define SFT_F2			(SHIFT+F2)
#define SFT_F3			(SHIFT+F3)
#define SFT_F4			(SHIFT+F4)
#define SFT_F5			(SHIFT+F5)
#define SFT_F6			(SHIFT+F6)
#define SFT_F7			(SHIFT+F7)
#define SFT_F8			(SHIFT+F8)
#define SFT_F9			(SHIFT+F9)
#define SFT_F10			(SHIFT+F10)
#define SFT_F11			(SHIFT+F11)
#define SFT_F12			(SHIFT+F12)
#define SFT_F13			(SHIFT+F13)
#define SFT_F14			(SHIFT+F14)
#define SFT_F15			(SHIFT+F15)
#define SFT_F16			(SHIFT+F16)
#define SFT_F17			(SHIFT+F17)
#define SFT_F18			(SHIFT+F18)
#define SFT_F19			(SHIFT+F19)
#define SFT_F20			(SHIFT+F20)
/*#define SFT_JIKKO		(SHIFT+JIKKO)	*/
/*#define SFT_CLEAR		(SHIFT+CLEAR)	*/
/*#define SFT_SHURYOU		(SHIFT+SHURYOU)	*/
/*#define SFT_TORIKESI		(SHIFT+TORIKESI)*/
#define SFT_EEND		(SHIFT+EEND)
/*#define SFT_HIRAGANA		(SHIFT+HIRAGANA)*/
/*#define SFT_KATAKANA		(SHIFT+KATAKANA)*/
/*#define SFT_EIJI		(SHIFT+EIJI)	*/
/*#define SFT_ZORH		(SHIFT+ZORH)	*/
/*#define SFT_ROMA		(SHIFT+ROMA)	*/
/*#define SFT_LWIN		(SHIFT+LWIN)	*/
/*#define SFT_RWIN		(SHIFT+RWIN)	*/
/*#define SFT_APPS		(SHIFT+APPS)	*/
#define SFT_XFER		(SHIFT+XFER)
#define SFT_PGDN		(SHIFT+PGDN)
#define SFT_RLUP		SFT_PGDN
#define SFT_PGUP		(SHIFT+PGUP)
#define SFT_RLDN		SFT_PGUP
#define SFT_INS			(SHIFT+INS)
#define SFT_DEL			(SHIFT+DEL)
#define SFT_UP			(SHIFT+UP)
#define SFT_LEFT		(SHIFT+LEFT)
#define SFT_RIGHT		(SHIFT+RIGHT)
#define SFT_DOWN		(SHIFT+DOWN)
#define SFT_HOME		(SHIFT+HOME)
#define SFT_HELP		(SHIFT+HELP)
#define SFT_NFER		(SHIFT+NFER)
#define SFT_KANJ		(SHIFT+KANJ)

#define CTR_SP			(CTRL+SPACE)
#define CTR_BS			(CTRL+BS)
#define CTR_TAB			(CTRL+TAB)
#define CTR_CR			(CTRL+CR)
#define CTR_ESC			(CTRL+EESC)
#define CTR_F1			(CTRL+F1)
#define CTR_F2			(CTRL+F2)
#define CTR_F3			(CTRL+F3)
#define CTR_F4			(CTRL+F4)
#define CTR_F5			(CTRL+F5)
#define CTR_F6			(CTRL+F6)
#define CTR_F7			(CTRL+F7)
#define CTR_F8			(CTRL+F8)
#define CTR_F9			(CTRL+F9)
#define CTR_F10			(CTRL+F10)
#define CTR_F11			(CTRL+F11)
#define CTR_F12			(CTRL+F12)
#define CTR_F13			(CTRL+F13)
#define CTR_F14			(CTRL+F14)
#define CTR_F15			(CTRL+F15)
#define CTR_F16			(CTRL+F16)
#define CTR_F17			(CTRL+F17)
#define CTR_F18			(CTRL+F18)
#define CTR_F19			(CTRL+F19)
#define CTR_F20			(CTRL+F20)
/*#define CTR_JIKKO		(CTRL+JIKKO)	*/
/*#define CTR_CLEAR		(CTRL+CLEAR)	*/
/*#define CTR_SHURYOU		(CTRL+SHURYOU)	*/
/*#define CTR_TORIKESI		(CTRL+TORIKESI)	*/
#define CTR_EEND		(CTRL+EEND)
/*#define CTR_HIRAGANA		(CTRL+HIRAGANA)	*/
/*#define CTR_KATAKANA		(CTRL+KATAKANA)	*/
/*#define CTR_EIJI		(CTRL+EIJI)	*/
/*#define CTR_ZORH		(CTRL+ZORH)	*/
/*#define CTR_ROMA		(CTRL+ROMA)	*/
/*#define CTR_LWIN		(CTRL+LWIN)	*/
/*#define CTR_RWIN		(CTRL+RWIN)	*/
/*#define CTR_APPS		(CTRL+APPS)	*/
#define CTR_XFER		(CTRL+XFER)
#define CTR_PGDN		(CTRL+PGDN)
#define CTR_RLUP		CTR_PGDN
#define CTR_PGUP		(CTRL+PGUP)
#define CTR_RLDN		CTR_PGUP
#define CTR_INS			(CTRL+INS)
#define CTR_DEL			(CTRL+DEL)
#define CTR_UP			(CTRL+UP)
#define CTR_LEFT		(CTRL+LEFT)
#define CTR_RIGHT		(CTRL+RIGHT)
#define CTR_DOWN		(CTRL+DOWN)
#define CTR_HOME		(CTRL+HOME)
#define CTR_HELP		(CTRL+HELP)
#define CTR_NFER		(CTRL+NFER)
#define CTR_KANJ		(CTRL+KANJ)

#define CTRSFT_SP		(CTRL+SHIFT+SPACE)
#define CTRSFT_BS		(CTRL+SHIFT+BS)
#define CTRSFT_TAB		(CTRL+SHIFT+TAB)
#define CTRSFT_CR		(CTRL+SHIFT+CR)
#define CTRSFT_ESC		(CTRL+SHIFT+EESC)
#define CTRSFT_F1		(CTRL+SHIFT+F1)
#define CTRSFT_F2		(CTRL+SHIFT+F2)
#define CTRSFT_F3		(CTRL+SHIFT+F3)
#define CTRSFT_F4		(CTRL+SHIFT+F4)
#define CTRSFT_F5		(CTRL+SHIFT+F5)
#define CTRSFT_F6		(CTRL+SHIFT+F6)
#define CTRSFT_F7		(CTRL+SHIFT+F7)
#define CTRSFT_F8		(CTRL+SHIFT+F8)
#define CTRSFT_F9		(CTRL+SHIFT+F9)
#define CTRSFT_F10		(CTRL+SHIFT+F10)
#define CTRSFT_F11		(CTRL+SHIFT+F11)
#define CTRSFT_F12		(CTRL+SHIFT+F12)
#define CTRSFT_F13		(CTRL+SHIFT+F13)
#define CTRSFT_F14		(CTRL+SHIFT+F14)
#define CTRSFT_F15		(CTRL+SHIFT+F15)
#define CTRSFT_F16		(CTRL+SHIFT+F16)
#define CTRSFT_F17		(CTRL+SHIFT+F17)
#define CTRSFT_F18		(CTRL+SHIFT+F18)
#define CTRSFT_F19		(CTRL+SHIFT+F19)
#define CTRSFT_F20		(CTRL+SHIFT+F20)
/*#define CTRSFT_JIKKO		(CTRL+SHIFT+JIKKO)	*/
/*#define CTRSFT_CLEAR		(CTRL+SHIFT+CLEAR)	*/
/*#define CTRSFT_SHURYOU	(CTRL+SHIFT+SHURYOU)	*/
/*#define CTRSFT_TORIKESI	(CTRL+SHIFT+TORIKESI)	*/
#define CTRSFT_EEND		(CTRL+SHIFT+EEND)
/*#define CTRSFT_HIRAGANA	(CTRL+SHIFT+HIRAGANA)	*/
/*#define CTRSFT_KATAKANA	(CTRL+SHIFT+KATAKANA)	*/
/*#define CTRSFT_EIJI		(CTRL+SHIFT+EIJI)	*/
/*#define CTRSFT_ZORH		(CTRL+SHIFT+ZORH)	*/
/*#define CTRSFT_ROMA		(CTRL+SHIFT+ROMA)	*/
/*#define CTRSFT_LWIN		(CTRL+SHIFT+LWIN)	*/
/*#define CTRSFT_RWIN		(CTRL+SHIFT+RWIN)	*/
/*#define CTRSFT_APPS		(CTRL+SHIFT+APPS)	*/
#define CTRSFT_XFER		(CTRL+SHIFT+XFER)
#define CTRSFT_PGDN		(CTRL+SHIFT+PGDN)
#define CTRSFT_RLUP		CTRSFT_PGDN
#define CTRSFT_PGUP		(CTRL+SHIFT+PGUP)
#define CTRSFT_RLDN		CTRSFT_PGUP
#define CTRSFT_INS		(CTRL+SHIFT+INS)
#define CTRSFT_DEL		(CTRL+SHIFT+DEL)
#define CTRSFT_UP		(CTRL+SHIFT+UP)
#define CTRSFT_LEFT		(CTRL+SHIFT+LEFT)
#define CTRSFT_RIGHT		(CTRL+SHIFT+RIGHT)
#define CTRSFT_DOWN		(CTRL+SHIFT+DOWN)
#define CTRSFT_HOME		(CTRL+SHIFT+HOME)
#define CTRSFT_HELP		(CTRL+SHIFT+HELP)
#define CTRSFT_NFER		(CTRL+SHIFT+NFER)
#define CTRSFT_KANJ		(CTRL+SHIFT+KANJ)
#define SFTCTR_SP		CTRSFT_SP
#define SFTCTR_BS		CTRSFT_BS
#define SFTCTR_TAB		CTRSFT_TAB
#define SFTCTR_CR		CTRSFT_CR
#define SFTCTR_ESC		CTRSFT_ESC
#define SFTCTR_F1		CTRSFT_F1
#define SFTCTR_F2		CTRSFT_F2
#define SFTCTR_F3		CTRSFT_F3
#define SFTCTR_F4		CTRSFT_F4
#define SFTCTR_F5		CTRSFT_F5
#define SFTCTR_F6		CTRSFT_F6
#define SFTCTR_F7		CTRSFT_F7
#define SFTCTR_F8		CTRSFT_F8
#define SFTCTR_F9		CTRSFT_F9
#define SFTCTR_F10		CTRSFT_F10
#define SFTCTR_F11		CTRSFT_F11
#define SFTCTR_F12		CTRSFT_F12
#define SFTCTR_F13		CTRSFT_F13
#define SFTCTR_F14		CTRSFT_F14
#define SFTCTR_F15		CTRSFT_F15
#define SFTCTR_F16		CTRSFT_F16
#define SFTCTR_F17		CTRSFT_F17
#define SFTCTR_F18		CTRSFT_F18
#define SFTCTR_F19		CTRSFT_F19
#define SFTCTR_F20		CTRSFT_F20
/*#define SFTCTR_JIKKO		CTRSFT_JIKKO		*/
/*#define SFTCTR_CLEAR		CTRSFT_CLEAR		*/
/*#define SFTCTR_SHURYOU	CTRSFT_SHURYOU		*/
/*#define SFTCTR_TORIKESI	CTRSFT_TORIKESI		*/
#define SFTCTR_EEND		CTRSFT_EEND
/*#define SFTCTR_HIRAGANA	CTRSFT_HIRAGANA		*/
/*#define SFTCTR_KATAKANA	CTRSFT_KATAKANA		*/
/*#define SFTCTR_EIJI		CTRSFT_EIJI		*/
/*#define SFTCTR_ZORH		CTRSFT_ZORH		*/
/*#define SFTCTR_ROMA		CTRSFT_ROMA		*/
/*#define SFTCTR_LWIN		CTRSFT_LWIN		*/
/*#define SFTCTR_RWIN		CTRSFT_RWIN		*/
/*#define SFTCTR_APPS		CTRSFT_APPS		*/
#define SFTCTR_XFER		CTRSFT_XFER
#define SFTCTR_PGDN		CTRSFT_PGDN
#define SFTCTR_RLUP		CTRSFT_RLUP
#define SFTCTR_PGUP		CTRSFT_PGUP
#define SFTCTR_RLDN		CTRSFT_RLDN
#define SFTCTR_INS		CTRSFT_INS
#define SFTCTR_DEL		CTRSFT_DEL
#define SFTCTR_UP		CTRSFT_UP
#define SFTCTR_LEFT		CTRSFT_LEFT
#define SFTCTR_RIGHT		CTRSFT_RIGHT
#define SFTCTR_DOWN		CTRSFT_DOWN
#define SFTCTR_HOME		CTRSFT_HOME
#define SFTCTR_HELP		CTRSFT_HELP
#define SFTCTR_NFER		CTRSFT_NFER
#define SFTCTR_KANJ		CTRSFT_KANJ

#define CTR_AT			(CTRL+0x0000)      	/* CTRL+@ */
#define CTR_A			(CTRL+0x0001)
#define CTR_B			(CTRL+0x0002)
#define CTR_C			(CTRL+0x0003)
#define CTR_D			(CTRL+0x0004)
#define CTR_E			(CTRL+0x0005)
#define CTR_F			(CTRL+0x0006)
#define CTR_G			(CTRL+0x0007)
#define CTR_H			(CTRL+0x0008)
#define CTR_I			(CTRL+0x0009)
#define CTR_J			(CTRL+0x000a)
#define CTR_K			(CTRL+0x000b)
#define CTR_L			(CTRL+0x000c)
#define CTR_M			(CTRL+0x000d)
#define CTR_N			(CTRL+0x000e)
#define CTR_O			(CTRL+0x000f)
#define CTR_P			(CTRL+0x0010)
#define CTR_Q			(CTRL+0x0011)
#define CTR_R			(CTRL+0x0012)
#define CTR_S			(CTRL+0x0013)
#define CTR_T			(CTRL+0x0014)
#define CTR_U			(CTRL+0x0015)
#define CTR_V			(CTRL+0x0016)
#define CTR_W			(CTRL+0x0017)
#define CTR_X			(CTRL+0x0018)
#define CTR_Y			(CTRL+0x0019)
#define CTR_Z			(CTRL+0x001a)
#define CTR_KL			(CTRL+0x001b)		/* CTRL+[ */
#define CTR_YEN			(CTRL+0x001c)		/* CTRL+\ */
#define CTR_RAG			(CTRL+0x001d)		/* CTRL+] */
#define CTR_HAT			(CTRL+0x001e)		/* CTRL+^ */
#define CTR__			(CTRL+0x001f)		/* CTRL+_ */

#define CTRSFT_AT		(CTRL+SHIFT+0x0000)     /* CTRL+@ */
#define CTRSFT_A		(CTRL+SHIFT+0x0001)
#define CTRSFT_B		(CTRL+SHIFT+0x0002)
#define CTRSFT_C		(CTRL+SHIFT+0x0003)
#define CTRSFT_D		(CTRL+SHIFT+0x0004)
#define CTRSFT_E		(CTRL+SHIFT+0x0005)
#define CTRSFT_F		(CTRL+SHIFT+0x0006)
#define CTRSFT_G		(CTRL+SHIFT+0x0007)
#define CTRSFT_H		(CTRL+SHIFT+0x0008)
#define CTRSFT_I		(CTRL+SHIFT+0x0009)
#define CTRSFT_J		(CTRL+SHIFT+0x000a)
#define CTRSFT_K		(CTRL+SHIFT+0x000b)
#define CTRSFT_L		(CTRL+SHIFT+0x000c)
#define CTRSFT_M		(CTRL+SHIFT+0x000d)
#define CTRSFT_N		(CTRL+SHIFT+0x000e)
#define CTRSFT_O		(CTRL+SHIFT+0x000f)
#define CTRSFT_P		(CTRL+SHIFT+0x0010)
#define CTRSFT_Q		(CTRL+SHIFT+0x0011)
#define CTRSFT_R		(CTRL+SHIFT+0x0012)
#define CTRSFT_S		(CTRL+SHIFT+0x0013)
#define CTRSFT_T		(CTRL+SHIFT+0x0014)
#define CTRSFT_U		(CTRL+SHIFT+0x0015)
#define CTRSFT_V		(CTRL+SHIFT+0x0016)
#define CTRSFT_W		(CTRL+SHIFT+0x0017)
#define CTRSFT_X		(CTRL+SHIFT+0x0018)
#define CTRSFT_Y		(CTRL+SHIFT+0x0019)
#define CTRSFT_Z		(CTRL+SHIFT+0x001a)
#define CTRSFT_KL		(CTRL+SHIFT+0x001b)	/* CTRL+[ */
#define CTRSFT_YEN		(CTRL+SHIFT+0x001c)	/* CTRL+\ */
#define CTRSFT_RAG		(CTRL+SHIFT+0x001d)	/* CTRL+] */
#define CTRSFT_HAT		(CTRL+SHIFT+0x001e)	/* CTRL+^ */
#define CTRSFT__		(CTRL+SHIFT+0x001f)	/* CTRL+_ */
#define SFTCTR_AT		CTRSFT_AT		/* CTRL+@ */
#define SFTCTR_A		CTRSFT_A
#define SFTCTR_B		CTRSFT_B
#define SFTCTR_C		CTRSFT_C
#define SFTCTR_D		CTRSFT_D
#define SFTCTR_E		CTRSFT_E
#define SFTCTR_F		CTRSFT_F
#define SFTCTR_G		CTRSFT_G
#define SFTCTR_H		CTRSFT_H
#define SFTCTR_I		CTRSFT_I
#define SFTCTR_J		CTRSFT_J
#define SFTCTR_K		CTRSFT_K
#define SFTCTR_L		CTRSFT_L
#define SFTCTR_M		CTRSFT_M
#define SFTCTR_N		CTRSFT_N
#define SFTCTR_O		CTRSFT_O
#define SFTCTR_P		CTRSFT_P
#define SFTCTR_Q		CTRSFT_Q
#define SFTCTR_R		CTRSFT_R
#define SFTCTR_S		CTRSFT_S
#define SFTCTR_T		CTRSFT_T
#define SFTCTR_U		CTRSFT_U
#define SFTCTR_V		CTRSFT_V
#define SFTCTR_W		CTRSFT_W
#define SFTCTR_X		CTRSFT_X
#define SFTCTR_Y		CTRSFT_Y
#define SFTCTR_Z		CTRSFT_Z
#define SFTCTR_KL		CTRSFT_KL		/* CTRL+[ */
#define SFTCTR_YEN		CTRSFT_YEN		/* CTRL+\ */
#define SFTCTR_RAG		CTRSFT_RAG		/* CTRL+] */
#define SFTCTR_HAT		CTRSFT_HAT		/* CTRL+^ */
#define SFTCTR__		CTRSFT__		/* CTRL+_ */

/************************************************************************/
/* AAJキーコード表(独仏発音キーボード用仮想文字キーコード値定義)         */
/************************************************************************/
#define TDRJSTATE_DEAD		0x00	/* 綴り字何もしない */
#define TDRJSTATE_ACST		0x01	/* 綴り字【´】にする */
#define TDRJSTATE_ACSC		0x02	/* 綴り字【｀】にする */
#define TDRJSTATE_ACSY		0x03	/* 綴り字【＾】にする */
#define TDRJSTATE_UMRT		0x04	/* 綴り字【¨】にする */
#define VMOJI_TDRJSTATE_DEAD	(VMOJIKEY+TDRJSTATE_DEAD)
#define VMOJI_TDRJSTATE_ACST	(VMOJIKEY+TDRJSTATE_ACST)
#define VMOJI_TDRJSTATE_ACSC	(VMOJIKEY+TDRJSTATE_ACSC)
#define VMOJI_TDRJSTATE_ACSY	(VMOJIKEY+TDRJSTATE_ACSY)
#define VMOJI_TDRJSTATE_UMRT	(VMOJIKEY+TDRJSTATE_UMRT)

#define HATU_AE			0xe6	/* * ae	1キー */
#define HATU_A1			0xbb	/* a	2キー */
#define HATU_O			0xa4	/* o	3キー */
#define HATU_A2			0xb4	/* ^	4キー */
#define HATU_A3			0xa7	/*  	5キー */
#define HATU_A4			0xab	/*  	6キー */
#define HATU_E			0xb0	/* ε	7キー */
#define HATU_TH			0xb7	/* θ	8キー */
#define HATU_Z			0xf0	/* * 	9キー */
#define HATU_S			0xb8	/* S	0キー */
#define HATU_A5			0xbe	/* з	^キー */
#define HATU_NG			0xbf	/* ng	\キー */
#define HATU_COLON		0xae	/* : 	@キー */
#define VMOJI_HATU_AE		HATU_AE
#define VMOJI_HATU_A1		(VMOJIKEY+HATU_A1)
#define VMOJI_HATU_O		(VMOJIKEY+HATU_O)
#define VMOJI_HATU_A2		(VMOJIKEY+HATU_A2)
#define VMOJI_HATU_A3		(VMOJIKEY+HATU_A3)
#define VMOJI_HATU_A4		(VMOJIKEY+HATU_A4)
#define VMOJI_HATU_E		(VMOJIKEY+HATU_E)
#define VMOJI_HATU_TH		(VMOJIKEY+HATU_TH)
#define VMOJI_HATU_Z		HATU_Z
#define VMOJI_HATU_S		(VMOJIKEY+HATU_S)
#define VMOJI_HATU_A5		(VMOJIKEY+HATU_A5)
#define VMOJI_HATU_NG		(VMOJIKEY+HATU_NG)
#define VMOJI_HATU_COLON	(VMOJIKEY+HATU_COLON)

#define FUTU_OE			0x9c	/* oe	\キー */
#define FUTU_OE2		0x8c	/* OE	|キー */
#define VMOJI_FUTU_OE		(VMOJIKEY+FUTU_OE)
#define VMOJI_FUTU_OE2		(VMOJIKEY+FUTU_OE2)
