/******************************************************************************
* $B%U%!%$%kL>(B   : KEY_DEF.H
* $B%U%!%$%k35MW(B : $BFbIt%-!<%3!<%I(B,$B5!G=%-!<HV9f(B,$B%-!<%G!<%?9=B$$NDj5A(B
* $B:n@.<T(B   :
* $B:n@.F|(B   :
* $BE,MQ%7%9%F%`(B :
* $BE,MQ5!<o(B :
* $BFC5-;v9`(B :
******************************************************************************/

/*===========================================================================*/
/* $B5!G=$NE}GQ9g$K$D$$$F$N%3%a%s%H!#(B  $B%+%9%?%^%$%6$N%9%?%$%k%3%s%P!<%H$K4X78!#(B*/
/*===========================================================================*/
/* ATOK11$B$N5!G=$r(BATOK12$B$G$OE}9g!"J,3d!":o=|$9$k5!G=$O0J2<$NDL$j(B */
/*
	ATOK11						-> ATOK12

	FUNC_CAND_NEXT(0x3b)  $B<!8uJdI=<(Cf$N<!8uJd0\F0(B	-> FUNC_CNV_NEXT(0x08) $BJQ49(B($B<!8uJd(B)$B$XE}9g(B
	FUNC_CAND_PREV(0x3d)  $B<!8uJdI=<(Cf$NA08uJd0\F0(B	-> FUNC_CNV_PREV(0x09) $BJQ49(B($BA08uJd(B)$B$XE}9g(B
	FUNC_CNV_CODE(0x69)   $B%3!<%IJQ49(B                -> FUNC_CNV_NEXT(0x08) $BJQ49(B($B<!8uJd(B)$B$XE}9g(B
	FUNC_DECIDEONE(0x7d)  $BJQ49J8@a#1J8;z3NDj(B	-> FUNC_KAKU_ONE(0x0a) $B#12;3NDj$XE}9g(B
	FUNC_DESTRUCTONE(0x7e)$BJQ49J8@a#1J8;z>C5n(B	-> FUNC_CHR_DEL(0x01)  $B#1J8;z:o=|(B($B%+!<%=%k0LCV(B)$B$XE}9g(B
	FUNC_REVERTFLUSH(0x82)$BJQ49<h>C$7$FA4J8;z:o=|(B	-> FUNC_CNV_CANCL(0x32) $BJQ49<h>C(B($BCmL\J8@a0J9_(B) $B$H(B FUNC_DEL_ALL(0x1e) $BA4J8;z:o=|$XJ,3d(B
	FUNC_ICONEXEC(0xb1)   $B:G>.2=(B			-> $B:o=|(B
	FUNC_OPPLEXEC(0xb5)   $BA`:n%Q%l%C%H5/F0(B          -> $B:o=|(B
*/


/*===========================================================================*/
/*      ATOK$B5!G=%-!<HV9fDj5A(B                                                 */
/*===========================================================================*/
/*
	00$B!A(Bff$B$r5!G=%-!<HV9f$H$7$FDj5A$9$k(B
	(@$B%^!<%/$O%Q%l%C%HMQ$K8x3+$7$F$$$k5!G=HV9f(B. @@$B$O(BWien$B$GDI2CM=Dj(B)
	($B!_%^!<%/$O%-!<!"%Q%l%C%H$H$b$K8x3+$7$J$$5!G=HV9f$G$"$k(B)
	(<<$B%^!<%/$O(BATOK12$B$+$i$NJQ992U=j$G$"$k(B)
*/

/* $B"#JQ99MzNr(B */
/*    98/06/18	$B:o=|(B FUNC_CNV_FUKUGO	0x9d $BJ#9g8lJQ49(B */
/*    98/06/18	$B:o=|(B FUNC_MENU_ENV	0x5b $B4D6-@_Dj0l3g@ZBX%a%K%e!<(B */

#define FUNC_CHR_BS		0x00 	/*@ $B#1J8;z:o=|(B($B%+!<%=%k$N:8B&(B)  */
#define FUNC_CHR_DEL		0x01	/*@ $B#1J8;z:o=|(B($B%+!<%=%k0LCV(B)	$B5l$NJQ49J8@a#1J8;z>C5n5!G=$r%W%i%9(B */
#define FUNC_CHR_RIGHT		0x02	/*@ $B%+!<%=%k0\F0(B($B#1J8;z1&(B)      */
#define FUNC_CHR_LEFT		0x03	/*@ $B%+!<%=%k0\F0(B($B#1J8;z:8(B)      */
#define FUNC_CHR_TAIL		0x04	/*@ $B%+!<%=%k0\F0(B($BJ8Kv(B)          */
#define FUNC_CHR_HEAD		0x05	/*@ $B%+!<%=%k0\F0(B($BJ8F,(B)          */
#define FUNC_CNV_BUSYU		0x06	/*@ $BIt<sJQ49(B                    */
#define FUNC_CNV_KEEP		0x07	/*  $B7QB3JQ49(B($B<+F0JQ49;~(B)        */
#define FUNC_CNV_NEXT		0x08	/*@ $BJQ49(B($B<!8uJd(B)				$B5lL>(B FUNC_CNV */
#define FUNC_CNV_PREV		0x09	/*  $BJQ49(B($BA08uJd(B)                <<$BDI2C(B*/
#define FUNC_KAKU_ONE		0x0a	/*@ $B#12;3NDj(B                    $B5l$NJQ49J8@a#1J8;z3NDj5!G=$r%W%i%9(B */
#define FUNC_CNV_HMUHEN		0x0b	/*@ $BH>3QL5JQ498eJQ49(B			$B5lL>(B FUNC_CNV_HANMU */
#define FUNC_CNV_HIRA		0x0c	/*@ $B$R$i$,$J(B($B8e(B)$BJQ49(B            */
#define FUNC_CNV_ZKATA		0x0d	/*@ $B%+%?%+%J(B($B8e(B)$BJQ49(B            $B5lL>(B FUNC_CNV_KANA */
#define FUNC_CNV_HAN		0x0e	/*@ $BH>3Q(B($B8e(B)$BJQ49(B                */
#define FUNC_CNV_ZMUHEN		0x0f	/*@ $BL5JQ49(B($B8e(B)$BJQ49(B              $B5lL>(B FUNC_CNV_MUHEN */
#define FUNC_MENU_DIC		0x10	/*@ $BJQ49<-=q@ZBXA*Br%a%K%e!<(B    */
#define FUNC_CNV_DIC01		0x11	/*@ $B<-=q#1JQ49(B                  */
#define FUNC_CNV_DIC02		0x12	/*@ $B<-=q#2JQ49(B                  */
#define FUNC_CNV_DIC03		0x13	/*@ $B<-=q#3JQ49(B                  */
#define FUNC_CNV_DIC04		0x14	/*@ $B<-=q#4JQ49(B                  */
#define FUNC_CNV_DIC05		0x15	/*@ $B<-=q#5JQ49(B                  */
#define FUNC_CNV_DIC06		0x16	/*@ $B<-=q#6JQ49(B                  */
#define FUNC_CNV_DIC07		0x17	/*@ $B<-=q#7JQ49(B                  */
#define FUNC_CNV_DIC08		0x18	/*@ $B<-=q#8JQ49(B                  */
#define FUNC_CNV_DIC09		0x19	/*@ $B<-=q#9JQ49(B                  */
#define FUNC_CNV_DIC10		0x1a	/*@ $B<-=q#0JQ49(B                  */
#define FUNC_KAKU_BUBUN		0x1b	/*@ $BItJ,3NDj(B($BCmL\J8@a$^$G(B)      */
#define FUNC_CANDZ   		0x1c	/*@ $BA48uJdJQ49(B	                $B5lL>(B FUNC_CAND_ALL */
#define FUNC_KAKU_ALL		0x1d	/*@ $BA4J83NDj(B                    */
#define FUNC_DEL_ALL		0x1e	/*@ $BA4J8;z:o=|(B                  */
#define FUNC_REPEAT		0x1f    /*@ $B3NDj%j%T!<%H(B                */
#define FUNC_UNDO		0x20	/*@ $B3NDj%"%s%I%%(B                */
#define FUNC_KAKU_BUBUNONE	0x21	/*$B!_ItJ,3NDj(B($B@hF,J8@a$N$_(B)      <<$BDI2C(B $BHs8x3+(BHN */
#define FUNC_MENU_ALL		0x22	/*@ ATOK$B%a%K%e!<(B                */
#define FUNC_MENU_MOJI		0x23	/*@ $BF~NOJ8;z<oA*Br%a%K%e!<(B      */
#define FUNC_MENU_KUTO		0x24	/*@ $B6gFIE@%b!<%IA*Br%a%K%e!<(B    */
#define FUNC_SWTCH_AFIX		0x25	/*  $B8GDjF~NO=g<!@ZBX(B            $B5lL>(B FUNC_SWTCH_AKOTE */
#define FUNC_SWTCH_EFIX	    	0x26	/*  $B8GDjF~NO1Q;z=g<!@ZBX(B        $B5lL>(B FUNC_SWTCH_EKOTE */
#define FUNC_SWTCH_KFIX	    	0x27	/*  $B8GDjF~NO%+%?%+%J=g<!@ZBX(B    $B5lL>(B FUNC_SWTCH_KKOTE */
#define FUNC_SWTCH_KUTO		0x28	/*  $B6gFIE@%b!<%I@ZBX(B            */
#define FUNC_ONOFF_FIXHIRA	0x29	/*@ $BA43Q$R$i$,$J8GDj(B($B$"(B)ON/OFF  $B5lL>(B FUNC_ONOFF_HIRA */
#define FUNC_ONOFF_FIXZKATA	0x2a	/*@ $BA43Q%+%?%+%J8GDj(B($B%"(B)ON/OFF  $B5lL>(B FUNC_ONOFF_ZKANA */
#define FUNC_ONOFF_FIXHKATA	0x2b	/*@ $BH>3Q%+%?%+%J8GDj(B($B%"(B)ON/OFF   $B5lL>(B FUNC_ONOFF_HKANA */
#define FUNC_ONOFF_FIXZMUHEN	0x2c	/*@ $BL5JQ49A43Q8GDj(B($B#A(B)ON/OFF    $B5lL>(B FUNC_ONOFF_MUHEN */
#define FUNC_ONOFF_FIXHMUHEN	0x2d	/*@ $BL5JQ49H>3Q8GDj(B(A)ON/OFF     $B5lL>(B FUNC_ONOFF_HAN */
#define FUNC_ONOFF_EIJI		0x2e	/*@ $B1Q;zF~NO(BON/OFF              */
#define FUNC_CHR_ERRYOMI	0x2f	/*@ $BF~NOFI$_8m$j0LCV%8%c%s%W(B    */
#define FUNC_KAKU_HEAD		0x30	/*@ $B#1J8;z3NDj(B($BJ8F,(B)            $B5lL>(B FUNC_OUT_HEAD */
#define FUNC_KAKU_TAIL		0x31	/*@ $B#1J8;z3NDj(B($BJ8Kv(B)            $B5lL>(B FUNC_OUT_TAIL */
#define FUNC_CNV_CANCL		0x32	/*@ $BJQ49<h>C(B($BCmL\J8@a0J9_(B)      */
#define FUNC_CNV_CANCLALL	0x33	/*@ $BA4JQ49<h>C(B                  */
#define FUNC_KUGI_RIGHT		0x34	/*@ $BJ8@a6h@Z$j(B($B#1J8;z1&(B)        $B5lL>(B FUNC_FOCUS_RIGHT */
#define FUNC_KUGI_LEFT		0x35	/*@ $BJ8@a6h@Z$j(B($B#1J8;z:8(B)        $B5lL>(B FUNC_FOCUS_LEFT */
#define FUNC_BST_NEXT		0x36	/*@ $BJ8@a0\F0(B($B1&(B)                $B5lL>(B FUNC_FOCUS_NEXT */
#define FUNC_BST_PREV		0x37	/*@ $BJ8@a0\F0(B($B:8(B)                $B5lL>(B FUNC_FOCUS_PREV */
#define FUNC_BST_TAIL		0x38	/*@ $BJ8@a0\F0(B($B:G=*(B)              $B5lL>(B FUNC_FOCUS_TAIL */
#define FUNC_BST_HEAD		0x39	/*@ $BJ8@a0\F0(B($B@hF,(B)              $B5lL>(B FUNC_FOCUS_HEAD */
#define FUNC_TAN_DEL		0x3a	/*@ $BC18l:o=|(B                    */
/*				0x3b	    $B3HD%MQ(B                      <<$B:o=|(B FUNC_CAND_NEXT $B<!8uJd0\F0(B */
/*				0x3c	    $B3HD%MQ(B                      */
/*				0x3d	    $B3HD%MQ(B                      <<$B:o=|(B FUNC_CAND_PREV $BA08uJd0\F0(B */
#define FUNC_CAND_PGNEXT	0x3e	/*  $B<!8uJd72I=<((B                $B5lL>(B FUNC_CNV_PGNEXT */
#define FUNC_CAND_PGPREV	0x3f	/*  $BA08uJd72I=<((B                $B5lL>(B FUNC_CNV_PGPREV */
#define FUNC_SWTCH_ZORH		0x40	/*  $BA43Q!?H>3QJ8;z@ZBX(B          */
#define FUNC_TAN_TOROKU		0x41	/*@ $BC18lEPO?(B                    */
/*				0x42	    $B3HD%MQ(B                      */
#define FUNC_SWTCH_OKRI		0x43	/*  $BAw$j$,$J%b!<%I=g<!@ZBX(B      */
#define FUNC_SWTCH_ROMA		0x44	/*  $B4A;zF~NO%b!<%I@ZBX(B          */
#define FUNC_SWTCH_HEN		0x45	/*  $BJQ49%b!<%I=g<!@ZBX(B          */
#define FUNC_SWTCH_AKCI		0x46	/*  $BF~NOJ8;z<o=g<!@ZBX(B          */
#define FUNC_SWTCH_EKCI		0x47	/*  $BF~NOJ8;z<o1Q;z=g<!@ZBX(B      */
#define FUNC_SWTCH_KKCI		0x48	/*  $BF~NOJ8;z<o%+%?%+%J=g<!@ZBX(B  */
#define FUNC_TO_KCIHIRA		0x49	/*@ $BF~NOJ8;z<oA43Q$R$i$,$J(B($B$"(B)	$B5lL>(B FUNC_TO_KC_HIRA */
#define FUNC_TO_KCIZKATA	0x4a	/*@ $BF~NOJ8;z<oA43Q%+%?%+%J(B($B%"(B)  $B5lL>(B FUNC_TO_KC_ZKANA */
#define FUNC_TO_KCIHKATA	0x4b	/*@ $BF~NOJ8;z<oH>3Q%+%?%+%J(B($B%"(B)   $B5lL>(B FUNC_TO_KC_HKANA */
#define FUNC_TO_KCIZMUHEN	0x4c	/*@ $BF~NOJ8;z<oL5JQ49A43Q(B($B#A(B)    $B5lL>(B FUNC_TO_KC_MUHEN */
#define FUNC_TO_KCIHMUHEN	0x4d	/*@ $BF~NOJ8;z<oL5JQ49H>3Q(B(A)     $B5lL>(B FUNC_TO_KC_HAN */
#define FUNC_TO_DIC		0x4e	/*@ $B<-=q!&3X=,JQ99(B              */
#define FUNC_SWTCH_KOUGO	0x4f	/*  $B8}8lBN%b!<%I@ZBX(B            */
#define FUNC_MENU_KOUGO		0x50	/*@ $B8}8lBN%b!<%IA*Br%a%K%e!<(B    */
#define FUNC_MENU_ROMA		0x51	/*@ $B4A;zF~NO%b!<%IA*Br%a%K%e!<(B  */
#define FUNC_MENU_HEN		0x52	/*@ $BJQ49%b!<%IA*Br%a%K%e!<(B      */
/*				0x53	    $B3HD%MQ(B                      */
#define FUNC_MENU_OKRI		0x54	/*@ $BAw$j$,$JA*Br%a%K%e!<(B        */
#define FUNC_MENU_COMP		0x55	/*$B!_(BATOK$BL$3NDjJ8;z%a%K%e!<(B      */
#define FUNC_MENU_CAND		0x56	/*$B!_(BATOK$B8uJd%a%K%e!<(B            */
#define FUNC_CHG_KANHAN		0x57	/*  $B4A;z!?H>3Q%b!<%I@ZBX(B        */
#define FUNC_MENU_IM		0x58	/*@ $BF~NO%b!<%IA*Br%a%K%e!<(B      */
#define FUNC_ONOFF_ATOK		0x59	/*  ATOK ON/OFF                 <<$B8x3+I|3h(B */
#define FUNC_SWTCH_IM		0x5a	/*  $BF~NO%b!<%I=g<!@ZBX(B          */
/* #define FUNC_MENU_ENV	0x5b	/* @@$B4D6-@_Dj0l3g@ZBX%a%K%e!<(B	<<$BDI2C(B */
#define FUNC_MENU_OPE		0x5c	/*@@$BA`:n%a%K%e!<(B		<<$BDI2C(B */
#define FUNC_MENU_REYOMI	0x5d	/*  $B:FJQ49JLFI$_%a%K%e!<(B	<<$BDI2C(B */
/*				0x5e	    $B3HD%MQ(B                      */
/*				0x5f	    $B3HD%MQ(B                      */
/*				0x60	    $B3HD%MQ(B                      */
/*				0x61	    $B3HD%MQ(B                      */
/*				0x62	    $B3HD%MQ(B                      */
/*				0x63	    $B3HD%MQ(B                      */
/*				0x64	    $B3HD%MQ(B                      */
/*				0x65	    $B3HD%MQ(B                      */
#define FUNC_MENU_HAN		0x66	/*@ $BH>3Q%b!<%IA*Br%a%K%e!<(B      */
#define FUNC_MENU_KCODE		0x67	/*  $B%3!<%IBN7OA*Br%a%K%e!<(B      */
#define FUNC_MENU_GUIDE		0x68	/*$B!_(BATOK$B%,%$%I%a%K%e!<(B		*/
/*				0x69	    $B3HD%MQ(B                      <<$B:o=|(B FUNC_CNV_CODE $B%3!<%IJQ49(B */
#define FUNC_SWTCH_KCODE	0x6a	/*  $B%3!<%IBN7O=g<!@ZBX(B          */
/*				0x6b	    $B3HD%MQ(B                      */
#define FUNC_SWTCH_HAN		0x6c	/*  $BH>3Q%b!<%I=g<!@ZBX(B          */
#define FUNC_KIGO_KAKU		0x6d	/*  $B5-9f3NDj(B                    */
#define FUNC_KIGO_NEXT		0x6e	/*  $B5-9f<!8uJd0\F0(B              */
#define FUNC_KIGO_PREV		0x6f	/*  $B5-9fA08uJd0\F0(B              */
#define FUNC_KIGO_PGNEXT	0x70	/*  $B5-9f<!8uJd72I=<((B            */
#define FUNC_KIGO_PGPREV	0x71	/*  $B5-9fA08uJd72I=<((B            */
#define FUNC_KUTEN_NEXT		0x72	/*  $B5-9f<!6hE@I=<((B              */
#define FUNC_KUTEN_PREV		0x73	/*  $B5-9fA06hE@I=<((B              */
#define FUNC_KANJ_TOP		0x74	/*  $B5-9f@hF,I=<((B                */
#define FUNC_GAIJ_TOP		0x75	/*  $B5-9f30;z@hF,I=<((B            */
#define FUNC_CAND_NEXTATR	0x76	/*  $B<!8uJdB0@-%8%c%s%W(B          $B5lL>(B FUNC_KOHOATR_NEXT */
#define FUNC_CAND_PREVATR	0x77	/*  $BA08uJdB0@-%8%c%s%W(B          $B5lL>(B FUNC_KOHOATR_PREV */
#define FUNC_CNV_ACNV		0x78	/*  $B=g<!8eJQ49(B                  $B5lL>(B FUNC_CNV_AATHN */
#define FUNC_CNV_ECNV		0x79	/*  $B=g<!L5JQ498eJQ49(B            $B5lL>(B FUNC_CNV_EATHN */
#define FUNC_CNV_KCNV		0x7a	/*  $B=g<!%+%?%+%J8eJQ49(B          $B5lL>(B FUNC_CNV_KATHN */
#define FUNC_KUGI_ROMARIGHT	0x7b	/*@ $B%m!<%^;z6h@Z$jD>$7(B($B1&(B)      $B5lL>(B FUNC_CHG_RMADJUST_R */
#define FUNC_KUGI_ROMALEFT	0x7c	/*@ $B%m!<%^;z6h@Z$jD>$7(B($B:8(B)      $B5lL>(B FUNC_CHG_RMADJUST_L */
/*				0x7d	    $B3HD%MQ(B			<<$B:o=|(B FUNC_DECIDEONE   $BJQ49J8@a#1J8;z3NDj(B */
/*				0x7e	    $B3HD%MQ(B			<<$B:o=|(B FUNC_DESTRUCTONE $BJQ49J8@a#1J8;z>C5n(B */
#define FUNC_CNVREAD_AORK	0x7f	/*  $B1Q;zFI$_%+%JFI$_=g<!JQ49(B    */
#define FUNC_CNVREAD_KTOA	0x80	/*  $B1Q;zFI$_JQ49(B                */
#define FUNC_CNVREAD_ATOK	0x81	/*  $B%+%JFI$_JQ49(B                */
#define FUNC_SWTCH_HIRAKATA	0x82	/*  $BJ8;z<o=g<!@ZBX(B($B$"(B/$B%"(B)       <<$BDI2C(BHN <<$B:o=|(B FUNC_REVERTFLUSH $BJQ49<h>C$7$FA4J8;z:o=|(B */
#define FUNC_SWTCH_HIRAKATAEI	0x83	/*  $BJ8;z<o=g<!@ZBX(B($B$"(B/$B%"(B/$B#A(B)    <<$BDI2C(BHN */
#define FUNC_SWTCH_HIKAEI	0x84	/*  $BJ8;z<o=g<!@ZBX(B($B$"(B,$B%"(B/$B#A(B)    <<$BDI2C(BHN */
#define FUNC_SWTCH_HIRAEI	0x85	/*  $BJ8;z<o=g<!@ZBX(B($B$"(B/$B#A(B)       <<$BDI2C(BHN */
#define FUNC_SWTCH_KATAEI	0x86	/*  $BJ8;z<o=g<!@ZBX(B($B%"(B/$B#A(B)	    <<$BDI2C(BHN */
#define FUNC_ONOFF_FIXMODE	0x87	/*  $B8GDjF~NO(BON/OFF              <<$BDI2C(BHN */
/*				0x88	    $B3HD%MQ(B                      */
/*				0x89	    $B3HD%MQ(B                      */
/*				0x8a	    $B3HD%MQ(B                      */
/*				0x8b	    $B3HD%MQ(B                      */
/*				0x8c	    $B3HD%MQ(B			*/
#define FUNC_CAND_SORTFULL	0x8d	/*  $B8uJdJB$SBX$((B($B%3!<%I=g(B)	<<$BDI2C(BHN*/
#define FUNC_CAND_SORTHEAD	0x8e	/*  $B8uJdJB$SBX$((B($B@hF,J8;z(B)	<<$BDI2C(BHN*/
#define FUNC_CAND_SORTTAIL	0x8f	/*  $B8uJdJB$SBX$((B($BKvHxJ8;z(B)	<<$BDI2C(BHN*/
#define FUNC_MENU_AMET		0x90	/*@ $B#A#M#E#T@ZBXA*Br%a%K%e!<(B    $B5lL>(B FUNC_MENU_VARI */
#define FUNC_AMET01		0x91	/*@ $B#A#M#E#T#1(B                  $B5lL>(B FUNC_OUT_VARI01 */
#define FUNC_AMET02		0x92	/*@ $B#A#M#E#T#2(B                  $B5lL>(B FUNC_OUT_VARI02 */
#define FUNC_AMET03		0x93	/*@ $B#A#M#E#T#3(B                  $B5lL>(B FUNC_OUT_VARI03 */
#define FUNC_AMET04		0x94	/*@ $B#A#M#E#T#4(B                  $B5lL>(B FUNC_OUT_VARI04 */
#define FUNC_AMET05		0x95	/*@ $B#A#M#E#T#5(B                  $B5lL>(B FUNC_OUT_VARI05 */
#define FUNC_AMET06		0x96	/*@ $B#A#M#E#T#6(B                  $B5lL>(B FUNC_OUT_VARI06 */
#define FUNC_AMET07		0x97	/*@ $B#A#M#E#T#7(B                  $B5lL>(B FUNC_OUT_VARI07 */
#define FUNC_AMET08		0x98	/*@ $B#A#M#E#T#8(B                  $B5lL>(B FUNC_OUT_VARI08 */
#define FUNC_AMET09		0x99	/*@ $B#A#M#E#T#9(B                  $B5lL>(B FUNC_OUT_VARI09 */
#define FUNC_AMET10		0x9a	/*@ $B#A#M#E#T#0(B                  $B5lL>(B FUNC_OUT_VARI10 */
/*				0x9b	    $B3HD%MQ(B                      */
/*				0x9c	    $B3HD%MQ(B                      */
/* #define FUNC_CNV_FUKUGO	0x9d	/*  $BJ#9g8lJQ49(B                  <<$BDI2CHyL/(B*/
#define FUNC_CNV_ABBREV		0x9e	/*@@$B>JF~NOJQ49(B                  <<$BDI2C(BHN */
#define FUNC_CNV_UNDO		0x9f	/*@@$B:FJQ49(B                      <<$BDI2C(B */
#define FUNC_TO_KANJ		0xa0	/*  $B4A;z%b!<%I(B                  */
#define FUNC_TO_HAN		0xa1	/*  $BH>3Q%b!<%I(B                  */
#define FUNC_TO_CODE		0xa2	/*  $B%3!<%IF~NO%b!<%I(B            */
#define FUNC_TO_KIGO		0xa3	/*  $B5-9f%b!<%I(B                  */
/*				0xa4	    $B3HD%MQ(B                      */
#define FUNC_ON_KANALK		0xa5	/*  $B%+%J%m%C%/(BON                */
#define FUNC_OFF_KANALK 	0xa6	/*  $B%+%J%m%C%/(BOFF               */
#define FUNC_ONOFF_KANALK	0xa7	/*  $B%+%J%m%C%/(BONOFF             */
#define FUNC_CANDZ_NEXT		0xa8	/*  $BA48uJd<!9`L\0\F0(B		*/
#define FUNC_CAND_HEAD		0xa9	/*  $B8uJd0\F0(B($B@hF,(B)              <<$BDI2C(B */
#define FUNC_CANDZ_PREV		0xaa	/*  $BA48uJdA09`L\0\F0(B		*/
#define FUNC_CANDZ_LNNEXT	0xab	/*  $BA48uJd<!9T9`L\0\F0(B          $B5lL>(B FUNC_CAND_LNNEXT */
#define FUNC_CANDZ_LNPREV	0xac	/*  $BA48uJdA09T9`L\0\F0(B          $B5lL>(B FUNC_CAND_LNPREV */
#define FUNC_CANDZ_KAKU		0xad	/*  $BA48uJd9`L\A*Br3NDj(B          $B5lL>(B FUNC_CAND_KAKU */
#define FUNC_CANDZ_ESC		0xae	/*  $BA48uJd9`L\A*Br<h>C(B          $B5lL>(B FUNC_CAND_ESC */
#define FUNC_CAND_TAIL		0xaf	/*  $B8uJd0\F0(B($B:G=*(B)              <<$BDI2C(B */
#define FUNC_APLEXEC_STARTNO	0xb0    /* $B%"%W%j5/F04X783+;O(BNO.  */
/* #define FUNC_HELPEXEC	0xb0	/*@ $B%X%k%W5/F0(B                  */
#define FUNC_MENU_HELP		0xb0	/*@ $B%X%k%W%a%K%e!<5/F0(B		*/
/*    $BK\Mh!"5/F07O$G$J$$$,%9%?%$%k%3%s%P!<%H$J$I$N8_49@-$N0Y$K$3$3$KF~$l$k(B */
/*				0xb1	    $B3HD%MQ(B                      <<$B:o=|(B FUNC_ICONEXEC $B:G>.2=(B */
#define FUNC_ATUTEXEC		0xb2	/*@ ATOK$B%W%m%Q%F%#5/F0(B          */
#define FUNC_UTEXEC		0xb3	/*@ $B<-=q%f!<%F%#%j%F%#5/F0(B      */
#define FUNC_MJPLEXEC		0xb4	/*@ $BJ8;z%Q%l%C%H5/F0(B            */
/*				0xb5	/*  $B3HD%MQ(B						<<$B:o=|(B FUNC_OPPLEXEC $BA`:n%Q%l%C%H(B */
#define FUNC_SOFTKEYBOARD	0xb6	/*@ $B%/%j%C%/%Q%l%C%H(B		*/
#define FUNC_RESULTRECORD	0xb7	/*@ $BJ8;zNs%\%C%/%95/F0(B		*/
#define FUNC_FUNCKEYGUIDE	0xb8	/*@@$B%U%!%s%/%7%g%s%-!<%,%$%I(B	*/
#define FUNC_HELPTOPICEXEC	0xb9	/*$B!_%X%k%W%H%T%C%/5/F0(B		*/
#define FUNC_HELPWEBEXEC	0xba	/*$B!_(BWeb$B%X%k%W5/F0(B		*/
#define FUNC_VERINFOEXEC	0xbb	/*$B!_%P!<%8%g%s>pJs5/F0(B		*/

#define FUNC_APLEXEC_ENDNO	0xbb   /* $B%"%W%j5/F04X78=*N;(BNO     */
/*				0xbc	    $B3HD%MQ(B                      */
/*				0xbd	    $B3HD%MQ(B                      */
/*				0xbe	    $B3HD%MQ(B                      */
/*				0xbf	    $B3HD%MQ(B                      */
/*				0xc0	    $B3HD%MQ(B                      */
/*				0xc1	    $B3HD%MQ(B                      */
/*				0xc2	    $B3HD%MQ(B                      */
/*				0xc3	    $B3HD%MQ(B                      */
/*				0xc4	    $B3HD%MQ(B                      */
/*				0xc5	    $B3HD%MQ(B                      */
/*				0xc6	    $B3HD%MQ(B                      */
/*				0xc7	    $B3HD%MQ(B                      */
/*				0xc8	    $B3HD%MQ(B                      */
/*				0xc9	    $B3HD%MQ(B                      */
/*				0xca	    $B3HD%MQ(B                      */
/*				0xcb	    $B3HD%MQ(B                      */
/*				0xcc	    $B3HD%MQ(B                      */
/*				0xcd	    $B3HD%MQ(B                      */
/*				0xce	    $B3HD%MQ(B                      */
/*				0xcf	    $B3HD%MQ(B                      */
/*				0xd0	    $B3HD%MQ(B                      */
/*				0xd1	    $B3HD%MQ(B                      */
/*				0xd2	    $B3HD%MQ(B                      */
/*				0xd3	    $B3HD%MQ(B                      */
/*				0xd4	    $B3HD%MQ(B                      */
/*				0xd5	    $B3HD%MQ(B                      */
/*				0xd6	    $B3HD%MQ(B                      */
/*				0xd7	    $B3HD%MQ(B                      */
/*				0xd8	    $B3HD%MQ(B                      */
/*				0xd9	    $B3HD%MQ(B                      */
/*				0xda	    $B3HD%MQ(B                      */
/*				0xdb	    $B3HD%MQ(B                      */
/*				0xdc	    $B3HD%MQ(B                      */
/*				0xdd	    $B3HD%MQ(B                      */
/*				0xde	    $B3HD%MQ(B                      */
/*				0xdf	    $B3HD%MQ(B			*/
/*--------------------------------------------------------------------*/
/* $B0J2<$O!"(BATOK$BFbIt2>A[5!G=%-!<HV9fDj5A(B($B%+%9%?%^%$%6$K$OL54X78$J$b$N(B  */
/*--------------------------------------------------------------------*/
#define VFUNC_MENU_ESC		0xe0	/*$B!_%a%K%e!<(B($B%b!<%I(B)$B2r=|(B        */
#define VFUNC_MENU_KAKU		0xe1	/*$B!_%a%K%e!<(B($B%b!<%I(B)$B3NDj(B        */
#define VFUNC_CANDDIRECT	0xe2	/*$B!_<!8uJd%@%$%l%/%H(B            */
#define VFUNC_FLUSH     	0xe3	/*$B!_%U%i%C%7%e(B                  */
#define VFUNC_FLUSH_KAKU	0xe4	/*$B!_%U%i%C%7%e(B($B3NDj$7$F(B)        */
#define VFUNC_GUIDE_KAKU	0xe5	/*$B!_%,%$%I%i%$%sF~NO3NDj(B        */
#define VFUNC_OTHERCANDLIST	0xe6	/*$B!_<!$NJL8uJd%j%9%H$X$N@ZBX(B    <<$BDI2C(B*/
#define VFUNC_SOLOSHIFTKEY	0xe7	/*$B!_C1FH(BShift$B%-!<$G$N2r=|=hM}(B   <<$BDI2C(B*/
/*				0xe8	    $B3HD%MQ(B                      */
/*				0xe9	    $B3HD%MQ(B                      */
/*				0xea	    $B3HD%MQ(B                      */
/*				0xeb	    $B3HD%MQ(B                      */
/*				0xec	    $B3HD%MQ(B                      */
/*				0xed	    $B3HD%MQ(B                      */
/*				0xee	    $B3HD%MQ(B                      */
/*				0xef	    $B3HD%MQ(B                      */
/*				0xf0	    $B3HD%MQ(B                      */
/*				0xf1	    $B3HD%MQ(B                      */
/*				0xf2	    $B3HD%MQ(B                      */
/*				0xf3	    $B3HD%MQ(B                      */
/*				0xf4	    $B3HD%MQ(B                      */
/*				0xf5	    $B3HD%MQ(B                      */
/*				0xf6	    $B3HD%MQ(B                      */
/*				0xf7	    $B3HD%MQ(B                      */
/*				0xf8	    $B3HD%MQ(B			*/
/*				0xf9	    $B3HD%MQ(B                      */
/*				0xfa	    $B3HD%MQ(B                      */
/*				0xfb	    $B3HD%MQ(B                      */
/*				0xfc	    $B3HD%MQ(B                      */
/*				0xfd	    $B3HD%MQ(B                      */
/*				0xfe	    $B3HD%MQ(B                      */
/*				0xff	    $B3HD%MQ(B                      */

/*===========================================================================*/
/*      $B%+%9%?%^%$%65!G=%-!<HV9fM-8z>uBVDj5A(B                                 */
/*===========================================================================*/
/*
   $B%+%9%?%^%$%65!G=%-!<HV9f$NM-8z>uBV$O(B
	$B4A(B1 : $BJ8;zL$F~NO(B          $BH>(B : $BH>3QF~NO(B
	$B4A(B2 : $BJ8;zF~NOCf(B          $B5-(B : $B5-9fF~NO(B
	$B4A(B3 : $BJQ49Cf(B
	$B4A(B4 : $B<!8uJdI=<(Cf(B      ($B%3!<%IF~NO>uBV$O$J$$(B   ($BJ8@a=$@5Cf>uBV$O$J$$(B
	$B4A(B5 : $BA48uJdI=<(Cf(B       $B%3(B1 : $B4A(B1,$B5-$N%5%V%;%C%H(B $B4A(B1$B$H$7$F07$&(B)
	$B4A(B6 : $BJ8@a6h@Z$jD>$7Cf(B   $B%3(B2 : $B4A(B2$B$N%5%V%;%C%H(B)
   $B$NAH$_9g$o$;$GDj5A$5$l$k(B
*/
#define FNG_NONE	    0x0000	/* $B>uBV$J$7(B : 0$B8GDjCM(B */
#define FNG_KANNONE         0x0001	/* $B4A(B1 : $BJ8;zL$F~NO(B */
#define FNG_KANINPUT        0x0002	/* $B4A(B2 : $BJ8;zF~NOCf(B */
#define FNG_KANHENKAN       0x0004	/* $B4A(B3 : $BJQ49Cf(B */
#define FNG_KANKOHO         0x0008	/* $B4A(B4 : $B<!8uJdI=<(Cf(B */
#define FNG_KANZKOHO        0x0010	/* $B4A(B5 : $BA48uJdI=<(Cf(B */
#define FNG_KANKUGIRI       0x0020	/* $B4A(B6 : $BJ8@a6h@Z$jD>$7Cf(B */
#define FNG_HAN             0x4000	/* $BH>(B  : $BH>3QF~NO(B */
#define FNG_KIGOU           0x8000	/* $B5-(B  : $B5-9fF~NO(B */
/*
  $B"(Cm0U(B $B%+%9%?%^%$%6>uBVDj5A$NHO0O$O(B0x0001$B!A(B0x8000$B$H$9$k!#(B
  ATOK$BFbIt>uBVDj5A$NHO0O$O(B0x00010000$B!A(B0x80000000$B$H$9$k!#(B
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
/*      ATOKCE$BFbIt>uBVDj5A(B $B%+%9%?%^%$%6$K$O4X78$J$$ItJ,(B $B$3$3$+$i(B             */
/*===========================================================================*/

/*
   ATOK$B%(%s%8%sFbIt5!G=%-!<HV9f$NM-8z>uBV$O%+%9%?%^%$%6>uBV$K2C$($F0J2<$N>uBV$,$"$k!#(B
   $B$3$N>uBV$O%+%9%?%^%$%6$K$OL54X78!#(B
*/
/* $B"(2<5-$rJQ99$9$k>l9g$O!"(Batok12ce.rc$B$N(BFNGR_xxx$B$bJQ99$9$kI,MW$"$j!#(B*/
#define FNG_ATOKOFF	0x00100000	/* OFF : $B4A;z(BOFF			$B!J%-!<$O!"FCJL=hM}!K(B*/
#define FNG_CODENONE	0x00010000	/* $B%3(B1  : $B%3!<%IL$F~NO(B		$B!J%-!<$O!"4A(B1($BJ8;zL$F~NO(B)$B>uBV$G;2>H!K(B*/
#define FNG_CODEINPUT	0x00020000	/* $B%3(B2  : $B%3!<%IF~NOCf(B		$B!J%-!<$O!"4A(B2($BJ8;zF~NOCf(B)$B>uBV$G;2>H!K(B*/
#define FNG_BUSYU	0x00040000	/* $BIt(B  : $BIt<s!"%3!<%IJQ49Cf(B	$B!J%-!<$O!"5-9f(B	         $B>uBV$G;2>H!K(B*/
#define FNG_TANDEL	0x00200000	/* $B:o(B  : $BC18l:o=|Cf(B			$B!J%-!<$O!"4A(B3($BJQ49Cf(B)    $B>uBV$G;2>H!K(B*/
#define FNGSP_CHGINPUT	0x10000000	/* $BF~NO%b!<%I(B(rot)$BJQ99$K4X$o$k5!G=(B($BFCJL(B)*/
#define FNGSP_AMET	0x20000000	/* AMET$B5/F05!G=(B($BFCJL(B)*/
/*
   $B"(%+%9%?%^%$%6>uBVDj5A$NHO0O$O(B0x0001$B!A(B0x8000$B$H$9$k!#(B
     ATOK$BFbIt>uBVDj5A$NHO0O$O(B0x00010000$B!A(B0x80000000$B$H$9$k!#(B
*/


/*===========================================================================*/
/*      ATOK$B%(%s%8%sF~NO%-!<%3!<%IDj5A(B(AAJ$B%-!<%3!<%II=(B)                      */
/*===========================================================================*/
/*****************************************************************************/
/* AAJ$B%-!<%3!<%II=(B($B3HD%%$%Y%s%HDj5A(B)                                         */
/*****************************************************************************/
#define BUTTONEVENT	0x7ffa	/* Mouse Event*/
#define KANJIINPUTEVENT	0x7ffb	/* $B4A;z%3!<%IF~NO(B Event*/
/*
   AAJ$B%3!<%I$NDj5A$,(B2$B%P%$%H$N9=@.$G$"$k$?$a!">u67$K$h$j(BAAJ$B3HD%%(%j%"$r;HMQ$9$k(B
   1.$B3HD%(B1 .AtCEIncode($BF~NO%-!<%3!<%I(B)$B$NCM$,(BBUTTONEVENT$B$N>l9g(B
     $B%^%&%9A`:n%$%Y%s%H$H2r<a$5$l!"0J2<$N9`L\$r;2>H$9$k!#(B(MSE_DEF.H$B;2>H(B)
      .AtCEInMouseMsg  ($BF~NO%^%&%9%a%C%;!<%8%?%$%W(B)
      .AtCEInWndNo     ($BF~NO%^%&%9%a%C%;!<%8H/@8%&%$%s%I%&(B)
      .AtCEInEventPosX ($BF~NO%^%&%9%a%C%;!<%8H/@8J8;z0LCV(B     1$B!A(B)
      .AtCEInEventPosY ($BF~NO%^%&%9%a%C%;!<%8H/@8J8;z0LCVJdB-(B 0/1 $BA0H>(B/$B8eH>(B)
   2.$B3HD%(B2 .AtCEIncode($BF~NO%-!<%3!<%I(B)$B$NCM$,(BKANJIINPUTEVENT$B$N>l9g(B
     UCS2$BJ8;zF~NO%$%Y%s%H$H2r<a$5$l!"0J2<$N9`L\$r;2>H$9$k!#(B
      .AtCEInkanjicode ($BF~NO%-!<%3!<%I(B(UCS2$BJ8;z%3!<%I(B))
   
   $B0J30$O(BAAJ$B%-!<%3!<%ICM$,@_Dj$5$l$k!#(B(KEY_DEF.H$B;2>H(B)
    [AAJ$B%3!<%I(B]
      NKEY                     $BL58z%-!<Dj5A(B
      (VKEY|$B5!G=%-!<HV9f(B)      $B2>A[%-!<Dj5A(B(ATOK$B5!G=%-!<(B)
      (VMOJIKEY|$B2>A[J8;zHV9f(B)  $B2>A[%-!<Dj5A(B($BFHJ)H/2;J8;z%-!<(B)
      $BJ*M}%-!<%3!<%I(B
     |                               |               |
     | 15  14  13  12| 11  10  09  08| 07          00|
     |---|---|---|---|---|---|---|---|-----.....-----|
           |   |   |       |   |   |    $B%-!<%3!<%ICM(B
           |   |   |       |   |   | 
           |   |   |       |   |   |__ FNCKEY($B5!G=%-!<(B bit)
           |   |   |       |   |______ TENKEY($B%F%s%-!<(B bit)
           |   |   |       |__________ SHIFT (Shift$B>uBV(Bbit)
           |   |   | 
           |   |   |__________________ CTRL  (Ctrl$B>uBV(B bit)
           |   |______________________ ALT   (Alt$B>uBV(B  bit)
           |__________________________ CAPS  (CAPS$B>uBV(B bit)
*/

/*****************************************************************************/
/* AAJ$B%-!<%3!<%II=(B($B@)8f(Bbit$BDj5A(B)                                              */
/*****************************************************************************/
#define NKEY		0xffff		/* $BL58z%-!<%3!<%IDj5A(B*/
#define VKEY		0x0300		/* 0x03xx  $B2>A[5!G=%-!<%3!<%I(Bbit*/
#define VKEY_H		0x03		/* 0x03xx  $B2>A[5!G=%-!<%3!<%I(Bbit*/
#define FNCKEY		0x0100		/* $B5!G=%-!<(Bbit*/
#define TENKEY		0x0200		/* $B%F%s%-!<(Bbit*/
#define SHIFT		0x0400		/* Shift$B>uBV(Bbit*/
#define VMOJIKEY	0x0a00		/* 0x0axx  $B2>A[J8;z%-!<%3!<%I(Bbit*/
#define VMOJIKEY_H	0x0a		/* 0x0axx  $B2>A[J8;z%-!<%3!<%I(Bbit*/
#define CTRL		0x1000		/* Ctrl$B>uBV(Bbit*/
#define ALT		0x2000		/* Alt$B>uBV(B bit*/
#define GRPH		ALT		/*  "*/
#define CAPS		0x4000		/* CAPS$B>uBV(B bit*/

#define NOTMOJIKEY_MASK	(FNCKEY|CTRL|ALT) /*$BJ8;z%-!<H=DjMQ(BMask*/
#define VKEY_MASK	0x0300		/* $B2>A[5!G=%-!<H=DjMQ(BMask*/
#define VMOJIKEY_MASK	0x0a00		/* $B2>A[J8;z%-!<%3!<%IH=DjMQ(BMask*/

/************************************************************************/
/* AAJ$B%-!<%3!<%II=(B($BJ*M}%-!<%3!<%ICMDj5A(B)                                */
/************************************************************************/
#define SPACE			0x0020			/* Space */
#define BS			(FNCKEY|0x0008) 	/* BackSpace */
#define TAB			(FNCKEY|0x0009) 	/* Tab */
#define CR			(FNCKEY|0x000d)		/* Enter */
#define EESC			(FNCKEY|0x001b)		/* Esc */
#define F1			(FNCKEY|0x0021)		/* F1 $B!A(B F20 */
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
/*#define JIKKO			(FNCKEY|0x0046)		/* $B<B9T(B */
/*#define CLEAR			(FNCKEY|0x0047)		/* $B%/%j%"(B */
/*#define SHURYOU		(FNCKEY|0x0048)		/* $B=*N;(B */
/*#define TORIKESI		(FNCKEY|0x0049)		/* $B<h>C(B */
#define EEND			(FNCKEY|0x004f)		/* End */
#define HIRAGANA		(FNCKEY|0x0050)		/* $B$R$i$,$J(B */
#define KATAKANA		(FNCKEY|0x0051)		/* $B%+%?%+%J(B */
#define EIJI			(FNCKEY|0x0052)		/* $B1Q?t(B */
#define ZORH			(FNCKEY|0x0053)		/* $BA43Q!&H>3Q(B */
#define ROMA			(FNCKEY|0x0054)		/* $B%m!<%^;z(B<< */
/*#define LWIN			(FNCKEY|0x005a)		/* Windows(Left) */
/*#define RWIN			(FNCKEY|0x005b)		/* Windows(Right) */
/*#define APPS			(FNCKEY|0x005c)		/* Application */
#define XFER			(FNCKEY|0x0060)		/* $BJQ49(B */
#define PGDN			(FNCKEY|0x0061)		/* PageDown */
#define RLUP			PGDN			/* " */
#define PGUP			(FNCKEY|0x0062)		/* PageUp */
#define RLDN			PGUP			/* " */
#define INS			(FNCKEY|0x0063)		/* Insert */
#define DEL			(FNCKEY|0x0064)		/* Delete */
#define UP			(FNCKEY|0x0065)		/* $B",(B */
#define LEFT			(FNCKEY|0x0066)		/* $B"+(B */
#define RIGHT			(FNCKEY|0x0067)		/* $B"*(B */
#define DOWN			(FNCKEY|0x0068)		/* $B"-(B */
#define HOME			(FNCKEY|0x0069)		/* Home */
#define HELP			(FNCKEY|0x006a)		/* Help */
#define NFER			(FNCKEY|0x006b)		/* $BL5JQ49(B */
#define KANJ			(FNCKEY|0x006c)		/* $B4A;z(B */

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
/* AAJ$B%-!<%3!<%II=(B($BFHJ)H/2;%-!<%\!<%IMQ2>A[J8;z%-!<%3!<%ICMDj5A(B)         */
/************************************************************************/
#define TDRJSTATE_DEAD		0x00	/* $BDV$j;z2?$b$7$J$$(B */
#define TDRJSTATE_ACST		0x01	/* $BDV$j;z!Z!-![$K$9$k(B */
#define TDRJSTATE_ACSC		0x02	/* $BDV$j;z!Z!.![$K$9$k(B */
#define TDRJSTATE_ACSY		0x03	/* $BDV$j;z!Z!0![$K$9$k(B */
#define TDRJSTATE_UMRT		0x04	/* $BDV$j;z!Z!/![$K$9$k(B */
#define VMOJI_TDRJSTATE_DEAD	(VMOJIKEY+TDRJSTATE_DEAD)
#define VMOJI_TDRJSTATE_ACST	(VMOJIKEY+TDRJSTATE_ACST)
#define VMOJI_TDRJSTATE_ACSC	(VMOJIKEY+TDRJSTATE_ACSC)
#define VMOJI_TDRJSTATE_ACSY	(VMOJIKEY+TDRJSTATE_ACSY)
#define VMOJI_TDRJSTATE_UMRT	(VMOJIKEY+TDRJSTATE_UMRT)

#define HATU_AE			0xe6	/* * ae	1$B%-!<(B */
#define HATU_A1			0xbb	/* a	2$B%-!<(B */
#define HATU_O			0xa4	/* o	3$B%-!<(B */
#define HATU_A2			0xb4	/* ^	4$B%-!<(B */
#define HATU_A3			0xa7	/*  	5$B%-!<(B */
#define HATU_A4			0xab	/*  	6$B%-!<(B */
#define HATU_E			0xb0	/* $B&E(B	7$B%-!<(B */
#define HATU_TH			0xb7	/* $B&H(B	8$B%-!<(B */
#define HATU_Z			0xf0	/* * 	9$B%-!<(B */
#define HATU_S			0xb8	/* S	0$B%-!<(B */
#define HATU_A5			0xbe	/* $B'Y(B	^$B%-!<(B */
#define HATU_NG			0xbf	/* ng	\$B%-!<(B */
#define HATU_COLON		0xae	/* : 	@$B%-!<(B */
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

#define FUTU_OE			0x9c	/* oe	\$B%-!<(B */
#define FUTU_OE2		0x8c	/* OE	|$B%-!<(B */
#define VMOJI_FUTU_OE		(VMOJIKEY+FUTU_OE)
#define VMOJI_FUTU_OE2		(VMOJIKEY+FUTU_OE2)
