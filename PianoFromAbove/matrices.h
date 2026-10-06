//Layout definitions
#include <WidthsWrapper.h>
#include <Fonts.h>
#define PropertyWindowW 300
#define PropertyWindowH 150
#define TrackWindowW 400
#define TrackWindowH 250
#define AboutWindowW 250
#define AboutWindowH 100
#define hLoadingWindowW 125
#define mhLoadingWindowW 0-hLoadingWindowW
#define LoadingWindowW hLoadingWindowW+hLoadingWindowW
#define mLoadingWindowW mhLoadingWindowW+mhLoadingWindowW
#define LoadingWindowH 50
#define ResolutionWindowW 100
#define ResolutionWindowH 60
#define DialogStyle DS_SETFONT | WS_CHILD | WS_CAPTION
#define AltDialogStyle DS_SETFONT | WS_POPUP | WS_CAPTION
#define BoxSpacing 14
#define dBoxSpacing BoxSpacing+BoxSpacing //Double
#define tBoxSpacing dBoxSpacing+BoxSpacing //Triple
#define qBoxSpacing tBoxSpacing+BoxSpacing //Quadruple
#define iBoxSpacing qBoxSpacing+BoxSpacing //quIntuple
#define xBoxSpacing iBoxSpacing+BoxSpacing //seXtuple
#define pBoxSpacing xBoxSpacing+BoxSpacing //sePtuple
#define oBoxSpacing pBoxSpacing+BoxSpacing //Octuple
#define nBoxSpacing oBoxSpacing+BoxSpacing //Nonuple
#define cBoxSpacing nBoxSpacing+BoxSpacing //deCuple
#define mBoxSpacing 0-BoxSpacing //Minus
#define mdBoxSpacing mBoxSpacing+mBoxSpacing //Double
#define mtBoxSpacing mdBoxSpacing+mBoxSpacing //Triple
#define mqBoxSpacing mtBoxSpacing+mBoxSpacing //Quadruple
#define miBoxSpacing mqBoxSpacing+mBoxSpacing //quIntuple
#define mxBoxSpacing miBoxSpacing+mBoxSpacing //seXtuple
#define mpBoxSpacing mxBoxSpacing+mBoxSpacing //sePtuple
#define moBoxSpacing mpBoxSpacing+mBoxSpacing //Octuple
#define mnBoxSpacing moBoxSpacing+mBoxSpacing //Nonuple
#define mcBoxSpacing mnBoxSpacing+mBoxSpacing //deCuple
#define hMargin 2 //Half
#define Margin hMargin+hMargin
#define dMargin Margin+Margin //Double
#define tMargin dMargin+Margin //Triple
#define qMargin tMargin+Margin //Quadruple
#define iMargin qMargin+Margin //quIntuple
#define xMargin iMargin+Margin //seXtuple
#define pMargin xMargin+Margin //sePtuple
#define oMargin pMargin+Margin //Octuple
#define nMargin oMargin+Margin //Nonuple
#define cMargin nMargin+Margin //deCuple
#define mhMargin 0-hMargin //MinusHalf
#define mMargin 0-hMargin-hMargin //Minus
#define mdMargin mMargin+mMargin //Double
#define mtMargin mdMargin+mMargin //Triple
#define mqMargin mtMargin+mMargin //Quadruple
#define miMargin mqMargin+mMargin //quIntuple
#define mxMargin miMargin+mMargin //seXtuple
#define mpMargin mxMargin+mMargin //sePtuple
#define moMargin mpMargin+mMargin //Octuple
#define mnMargin moMargin+mMargin //Nonuple
#define mcMargin mnMargin+mMargin //deCuple
#define thMargin hMargin+hMargin+hMargin //TripleHalf
#define dthMargin thMargin+thMargin //Double
#define tthMargin dthMargin+thMargin //Triple
#define qthMargin tthMargin+thMargin //Quadruple
#define ithMargin qthMargin+thMargin //quIntuple
#define xthMargin ithMargin+thMargin //seXtuple
#define pthMargin xthMargin+thMargin //sePtuple
#define othMargin pthMargin+thMargin //Octuple
#define nthMargin othMargin+thMargin //Nonuple
#define cthMargin nthMargin+thMargin //deCuple
#define mthMargin 0-hMargin-hMargin-hMargin //Minus
#define mdthMargin mthMargin+mthMargin //Double
#define mtthMargin mdthMargin+mthMargin //Triple
#define mqthMargin mtthMargin+mthMargin //Quadruple
#define mithMargin mqthMargin+mthMargin //quIntuple
#define mxthMargin mithMargin+mthMargin //seXtuple
#define mpthMargin mxthMargin+mthMargin //sePtuple
#define mothMargin mpthMargin+mthMargin //Octuple
#define mnthMargin mothMargin+mthMargin //Nonuple
#define mcthMargin mnthMargin+mthMargin //deCuple
#define dhMargin hMargin+hMargin //Double
#define qhMargin thMargin+hMargin //Quadruple
#define ihMargin qhMargin+hMargin //quIntuple
#define xhMargin ihMargin+hMargin //seXtuple
#define phMargin xhMargin+hMargin //sePtuple
#define ohMargin phMargin+hMargin //Octuple
#define nhMargin ohMargin+hMargin //Nonuple
#define chMargin nhMargin+hMargin //deCuple
#define mdhMargin 0-hMargin-hMargin //Double
#define mqhMargin mthMargin+mhMargin //Quadruple
#define mihMargin mqhMargin+mhMargin //quIntuple
#define mxhMargin mihMargin+mhMargin //seXtuple
#define mphMargin mxhMargin+mhMargin //sePtuple
#define mohMargin mphMargin+mhMargin //Octuple
#define mnhMargin mohMargin+mhMargin //Nonuple
#define mchMargin mnhMargin+mhMargin //deCuple
#define ContentHeight 10
#define dContentHeight ContentHeight+ContentHeight //Double
#define tContentHeight dContentHeight+ContentHeight //Triple
#define qContentHeight tContentHeight+ContentHeight //Quadruple
#define iContentHeight qContentHeight+ContentHeight //quIntuple
#define xContentHeight iContentHeight+ContentHeight //seXtuple
#define pContentHeight xContentHeight+ContentHeight //sePtuple
#define oContentHeight pContentHeight+ContentHeight //Octuple
#define nContentHeight oContentHeight+ContentHeight //Nonuple
#define cContentHeight nContentHeight+ContentHeight //deCuple
#define mContentHeight 0-ContentHeight //Minus
#define mdContentHeight mContentHeight+mContentHeight //Double
#define mtContentHeight mdContentHeight+mContentHeight //Triple
#define mqContentHeight mtContentHeight+mContentHeight //Quadruple
#define miContentHeight mqContentHeight+mContentHeight //quIntuple
#define mxContentHeight miContentHeight+mContentHeight //seXtuple
#define mpContentHeight mxContentHeight+mContentHeight //sePtuple
#define moContentHeight mpContentHeight+mContentHeight //Octuple
#define mnContentHeight moContentHeight+mContentHeight //Nonuple
#define mcContentHeight mnContentHeight+mContentHeight //deCuple
#define ColorBox 16
#define dColorBox ColorBox+ColorBox //Double
#define tColorBox dColorBox+ColorBox //Triple
#define qColorBox tColorBox+ColorBox //Quadruple
#define iColorBox qColorBox+ColorBox //quIntuple
#define xColorBox iColorBox+ColorBox //seXtuple
#define pColorBox xColorBox+ColorBox //sePtuple
#define oColorBox pColorBox+ColorBox //Octuple
#define nColorBox oColorBox+ColorBox //Nonuple
#define cColorBox nColorBox+ColorBox //deCuple
#define KeySelectW 30+Margin
#define SpinnerW 40+Margin
#define MarkerEncodingW 80+Margin
#if TrackText1W >= TrackText2W && TrackText1W >= TrackText3W && TrackText1W >= TrackText4W
#define TrackAlign TrackText1W
#elif TrackText2W >= TrackText3W && TrackText2W >= TrackText4W && TrackText2W >= TrackText1W
#define TrackAlign TrackText2W
#elif TrackText3W >= TrackText4W && TrackText3W >= TrackText1W && TrackText3W >= TrackText2W
#define TrackAlign TrackText3W
#elif TrackText4W >= TrackText1W && TrackText4W >= TrackText2W && TrackText4W >= TrackText3W
#define TrackAlign TrackText4W
#endif
//Alternative Macro Naming Conventions
#define BoxSpacing_1x BoxSpacing
#define BoxSpacing_2x dBoxSpacing
#define BoxSpacing_3x tBoxSpacing
#define BoxSpacing_4x qBoxSpacing
#define BoxSpacing_5x iBoxSpacing
#define BoxSpacing_6x xBoxSpacing
#define BoxSpacing_7x pBoxSpacing
#define BoxSpacing_8x oBoxSpacing
#define BoxSpacing_9x nBoxSpacing
#define BoxSpacing_0x cBoxSpacing
#define mBoxSpacing_1x mBoxSpacing
#define mBoxSpacing_2x mdBoxSpacing
#define mBoxSpacing_3x mtBoxSpacing
#define mBoxSpacing_4x mqBoxSpacing
#define mBoxSpacing_5x miBoxSpacing
#define mBoxSpacing_6x mxBoxSpacing
#define mBoxSpacing_7x mpBoxSpacing
#define mBoxSpacing_8x moBoxSpacing
#define mBoxSpacing_9x mnBoxSpacing
#define mBoxSpacing_0x mcBoxSpacing
#define Margin_1x Margin
#define Margin_2x dMargin
#define Margin_3x tMargin
#define Margin_4x qMargin
#define Margin_5x iMargin
#define Margin_6x xMargin
#define Margin_7x pMargin
#define Margin_8x oMargin
#define Margin_9x nMargin
#define Margin_0x cMargin
#define mMargin_1x mMargin
#define mMargin_2x mdMargin
#define mMargin_3x mtMargin
#define mMargin_4x mqMargin
#define mMargin_5x miMargin
#define mMargin_6x mxMargin
#define mMargin_7x mpMargin
#define mMargin_8x moMargin
#define mMargin_9x mnMargin
#define mMargin_0x mcMargin
#define thMargin_1x thMargin
#define thMargin_2x dthMargin
#define thMargin_3x tthMargin
#define thMargin_4x qthMargin
#define thMargin_5x ithMargin
#define thMargin_6x xthMargin
#define thMargin_7x pthMargin
#define thMargin_8x othMargin
#define thMargin_9x nthMargin
#define thMargin_0x cthMargin
#define mthMargin_1x mthMargin
#define mthMargin_2x mdthMargin
#define mthMargin_3x mtthMargin
#define mthMargin_4x mqthMargin
#define mthMargin_5x mithMargin
#define mthMargin_6x mxthMargin
#define mthMargin_7x mpthMargin
#define mthMargin_8x mothMargin
#define mthMargin_9x mnthMargin
#define mthMargin_0x mcthMargin
#define hMargin_1x hMargin
#define hMargin_2x dhMargin
#define hMargin_3x thMargin
#define hMargin_4x qhMargin
#define hMargin_5x ihMargin
#define hMargin_6x xhMargin
#define hMargin_7x phMargin
#define hMargin_8x ohMargin
#define hMargin_9x nhMargin
#define hMargin_0x chMargin
#define mhMargin_1x mhMargin
#define mhMargin_2x mdhMargin
#define mhMargin_3x mthMargin
#define mhMargin_4x mqhMargin
#define mhMargin_5x mihMargin
#define mhMargin_6x mxhMargin
#define mhMargin_7x mphMargin
#define mhMargin_8x mohMargin
#define mhMargin_9x mnhMargin
#define mhMargin_0x mchMargin
#define ContentHeight_1x ContentHeight
#define ContentHeight_2x dContentHeight
#define ContentHeight_3x tContentHeight
#define ContentHeight_4x qContentHeight
#define ContentHeight_5x iContentHeight
#define ContentHeight_6x xContentHeight
#define ContentHeight_7x pContentHeight
#define ContentHeight_8x oContentHeight
#define ContentHeight_9x nContentHeight
#define ContentHeight_0x cContentHeight
#define mContentHeight_1x mContentHeight
#define mContentHeight_2x mdContentHeight
#define mContentHeight_3x mtContentHeight
#define mContentHeight_4x mqContentHeight
#define mContentHeight_5x miContentHeight
#define mContentHeight_6x mxContentHeight
#define mContentHeight_7x mpContentHeight
#define mContentHeight_8x moContentHeight
#define mContentHeight_9x mnContentHeight
#define mContentHeight_0x mcContentHeight
#define ColorBox_1x ColorBox
#define ColorBox_2x dColorBox
#define ColorBox_3x tColorBox
#define ColorBox_4x qColorBox
#define ColorBox_5x iColorBox
#define ColorBox_6x xColorBox
#define ColorBox_7x pColorBox
#define ColorBox_8x oColorBox
#define ColorBox_9x nColorBox
#define ColorBox_0x cColorBox
#define BoxSpacing_m1x mBoxSpacing
#define BoxSpacing_m2x mdBoxSpacing
#define BoxSpacing_m3x mtBoxSpacing
#define BoxSpacing_m4x mqBoxSpacing
#define BoxSpacing_m5x miBoxSpacing
#define BoxSpacing_m6x mxBoxSpacing
#define BoxSpacing_m7x mpBoxSpacing
#define BoxSpacing_m8x moBoxSpacing
#define BoxSpacing_m9x mnBoxSpacing
#define BoxSpacing_m0x mcBoxSpacing
#define Margin_m1x mMargin
#define Margin_m2x mdMargin
#define Margin_m3x mtMargin
#define Margin_m4x mqMargin
#define Margin_m5x miMargin
#define Margin_m6x mxMargin
#define Margin_m7x mpMargin
#define Margin_m8x moMargin
#define Margin_m9x mnMargin
#define Margin_m0x mcMargin
#define Margin_th1x thMargin
#define Margin_th2x dthMargin
#define Margin_th3x tthMargin
#define Margin_th4x qthMargin
#define Margin_th5x ithMargin
#define Margin_th6x xthMargin
#define Margin_th7x pthMargin
#define Margin_th8x othMargin
#define Margin_th9x nthMargin
#define Margin_th0x cthMargin
#define Margin_mth1x mthMargin
#define Margin_mth2x mdthMargin
#define Margin_mth3x mtthMargin
#define Margin_mth4x mqthMargin
#define Margin_mth5x mithMargin
#define Margin_mth6x mxthMargin
#define Margin_mth7x mpthMargin
#define Margin_mth8x mothMargin
#define Margin_mth9x mnthMargin
#define Margin_mth0x mcthMargin
#define Margin_h1x hMargin
#define Margin_h2x dhMargin
#define Margin_h3x thMargin
#define Margin_h4x qhMargin
#define Margin_h5x ihMargin
#define Margin_h6x xhMargin
#define Margin_h7x phMargin
#define Margin_h8x ohMargin
#define Margin_h9x nhMargin
#define Margin_h0x chMargin
#define Margin_mh1x mhMargin
#define Margin_mh2x mdhMargin
#define Margin_mh3x mthMargin
#define Margin_mh4x mqhMargin
#define Margin_mh5x mihMargin
#define Margin_mh6x mxhMargin
#define Margin_mh7x mphMargin
#define Margin_mh8x mohMargin
#define Margin_mh9x mnhMargin
#define Margin_mh0x mchMargin
#define ContentHeight_m1x mContentHeight
#define ContentHeight_m2x mdContentHeight
#define ContentHeight_m3x mtContentHeight
#define ContentHeight_m4x mqContentHeight
#define ContentHeight_m5x miContentHeight
#define ContentHeight_m6x mxContentHeight
#define ContentHeight_m7x mpContentHeight
#define ContentHeight_m8x moContentHeight
#define ContentHeight_m9x mnContentHeight
#define ContentHeight_m0x mcContentHeight
#define BoxSpacing01 BoxSpacing
#define BoxSpacing02 dBoxSpacing
#define BoxSpacing03 tBoxSpacing
#define BoxSpacing04 qBoxSpacing
#define BoxSpacing05 iBoxSpacing
#define BoxSpacing06 xBoxSpacing
#define BoxSpacing07 pBoxSpacing
#define BoxSpacing08 oBoxSpacing
#define BoxSpacing09 nBoxSpacing
#define BoxSpacing10 cBoxSpacing
#define mBoxSpacing01 mBoxSpacing
#define mBoxSpacing02 mdBoxSpacing
#define mBoxSpacing03 mtBoxSpacing
#define mBoxSpacing04 mqBoxSpacing
#define mBoxSpacing05 miBoxSpacing
#define mBoxSpacing06 mxBoxSpacing
#define mBoxSpacing07 mpBoxSpacing
#define mBoxSpacing08 moBoxSpacing
#define mBoxSpacing09 mnBoxSpacing
#define mBoxSpacing10 mcBoxSpacing
#define Margin01 Margin
#define Margin02 dMargin
#define Margin03 tMargin
#define Margin04 qMargin
#define Margin05 iMargin
#define Margin06 xMargin
#define Margin07 pMargin
#define Margin08 oMargin
#define Margin09 nMargin
#define Margin10 cMargin
#define mMargin01 mMargin
#define mMargin02 mdMargin
#define mMargin03 mtMargin
#define mMargin04 mqMargin
#define mMargin05 miMargin
#define mMargin06 mxMargin
#define mMargin07 mpMargin
#define mMargin08 moMargin
#define mMargin09 mnMargin
#define mMargin10 mcMargin
#define thMargin01 thMargin
#define thMargin02 dthMargin
#define thMargin03 tthMargin
#define thMargin04 qthMargin
#define thMargin05 ithMargin
#define thMargin06 xthMargin
#define thMargin07 pthMargin
#define thMargin08 othMargin
#define thMargin09 nthMargin
#define thMargin10 cthMargin
#define mthMargin01 mthMargin
#define mthMargin02 mdthMargin
#define mthMargin03 mtthMargin
#define mthMargin04 mqthMargin
#define mthMargin05 mithMargin
#define mthMargin06 mxthMargin
#define mthMargin07 mpthMargin
#define mthMargin08 mothMargin
#define mthMargin09 mnthMargin
#define mthMargin10 mcthMargin
#define hMargin01 hMargin
#define hMargin02 dhMargin
#define hMargin03 thMargin
#define hMargin04 qhMargin
#define hMargin05 ihMargin
#define hMargin06 xhMargin
#define hMargin07 phMargin
#define hMargin08 ohMargin
#define hMargin09 nhMargin
#define hMargin10 chMargin
#define mhMargin01 mhMargin
#define mhMargin02 mdhMargin
#define mhMargin03 mthMargin
#define mhMargin04 mqhMargin
#define mhMargin05 mihMargin
#define mhMargin06 mxhMargin
#define mhMargin07 mphMargin
#define mhMargin08 mohMargin
#define mhMargin09 mnhMargin
#define mhMargin10 mchMargin
#define ContentHeight01 ContentHeight
#define ContentHeight02 dContentHeight
#define ContentHeight03 tContentHeight
#define ContentHeight04 qContentHeight
#define ContentHeight05 iContentHeight
#define ContentHeight06 xContentHeight
#define ContentHeight07 pContentHeight
#define ContentHeight08 oContentHeight
#define ContentHeight09 nContentHeight
#define ContentHeight10 cContentHeight
#define mContentHeight01 mContentHeight
#define mContentHeight02 mdContentHeight
#define mContentHeight03 mtContentHeight
#define mContentHeight04 mqContentHeight
#define mContentHeight05 miContentHeight
#define mContentHeight06 mxContentHeight
#define mContentHeight07 mpContentHeight
#define mContentHeight08 moContentHeight
#define mContentHeight09 mnContentHeight
#define mContentHeight10 mcContentHeight
#define ColorBox01 ColorBox
#define ColorBox02 dColorBox
#define ColorBox03 tColorBox
#define ColorBox04 qColorBox
#define ColorBox05 iColorBox
#define ColorBox06 xColorBox
#define ColorBox07 pColorBox
#define ColorBox08 oColorBox
#define ColorBox09 nColorBox
#define ColorBox10 cColorBox
#define BoxSpacing_1 BoxSpacing
#define BoxSpacing_2 dBoxSpacing
#define BoxSpacing_3 tBoxSpacing
#define BoxSpacing_4 qBoxSpacing
#define BoxSpacing_5 iBoxSpacing
#define BoxSpacing_6 xBoxSpacing
#define BoxSpacing_7 pBoxSpacing
#define BoxSpacing_8 oBoxSpacing
#define BoxSpacing_9 nBoxSpacing
#define BoxSpacing_0 cBoxSpacing
#define mBoxSpacing_1 mBoxSpacing
#define mBoxSpacing_2 mdBoxSpacing
#define mBoxSpacing_3 mtBoxSpacing
#define mBoxSpacing_4 mqBoxSpacing
#define mBoxSpacing_5 miBoxSpacing
#define mBoxSpacing_6 mxBoxSpacing
#define mBoxSpacing_7 mpBoxSpacing
#define mBoxSpacing_8 moBoxSpacing
#define mBoxSpacing_9 mnBoxSpacing
#define mBoxSpacing_0 mcBoxSpacing
#define Margin_1 Margin
#define Margin_2 dMargin
#define Margin_3 tMargin
#define Margin_4 qMargin
#define Margin_5 iMargin
#define Margin_6 xMargin
#define Margin_7 pMargin
#define Margin_8 oMargin
#define Margin_9 nMargin
#define Margin_0 cMargin
#define mMargin_1 mMargin
#define mMargin_2 mdMargin
#define mMargin_3 mtMargin
#define mMargin_4 mqMargin
#define mMargin_5 miMargin
#define mMargin_6 mxMargin
#define mMargin_7 mpMargin
#define mMargin_8 moMargin
#define mMargin_9 mnMargin
#define mMargin_0 mcMargin
#define thMargin_1 thMargin
#define thMargin_2 dthMargin
#define thMargin_3 tthMargin
#define thMargin_4 qthMargin
#define thMargin_5 ithMargin
#define thMargin_6 xthMargin
#define thMargin_7 pthMargin
#define thMargin_8 othMargin
#define thMargin_9 nthMargin
#define thMargin_0 cthMargin
#define mthMargin_1 mthMargin
#define mthMargin_2 mdthMargin
#define mthMargin_3 mtthMargin
#define mthMargin_4 mqthMargin
#define mthMargin_5 mithMargin
#define mthMargin_6 mxthMargin
#define mthMargin_7 mpthMargin
#define mthMargin_8 mothMargin
#define mthMargin_9 mnthMargin
#define mthMargin_0 mcthMargin
#define hMargin_1 hMargin
#define hMargin_2 dhMargin
#define hMargin_3 thMargin
#define hMargin_4 qhMargin
#define hMargin_5 ihMargin
#define hMargin_6 xhMargin
#define hMargin_7 phMargin
#define hMargin_8 ohMargin
#define hMargin_9 nhMargin
#define hMargin_0 chMargin
#define mhMargin_1 mhMargin
#define mhMargin_2 mdhMargin
#define mhMargin_3 mthMargin
#define mhMargin_4 mqhMargin
#define mhMargin_5 mihMargin
#define mhMargin_6 mxhMargin
#define mhMargin_7 mphMargin
#define mhMargin_8 mohMargin
#define mhMargin_9 mnhMargin
#define mhMargin_0 mchMargin
#define ContentHeight_1 ContentHeight
#define ContentHeight_2 dContentHeight
#define ContentHeight_3 tContentHeight
#define ContentHeight_4 qContentHeight
#define ContentHeight_5 iContentHeight
#define ContentHeight_6 xContentHeight
#define ContentHeight_7 pContentHeight
#define ContentHeight_8 oContentHeight
#define ContentHeight_9 nContentHeight
#define ContentHeight_0 cContentHeight
#define mContentHeight_1 mContentHeight
#define mContentHeight_2 mdContentHeight
#define mContentHeight_3 mtContentHeight
#define mContentHeight_4 mqContentHeight
#define mContentHeight_5 miContentHeight
#define mContentHeight_6 mxContentHeight
#define mContentHeight_7 mpContentHeight
#define mContentHeight_8 moContentHeight
#define mContentHeight_9 mnContentHeight
#define mContentHeight_0 mcContentHeight
#define ColorBox_1 ColorBox
#define ColorBox_2 dColorBox
#define ColorBox_3 tColorBox
#define ColorBox_4 qColorBox
#define ColorBox_5 iColorBox
#define ColorBox_6 xColorBox
#define ColorBox_7 pColorBox
#define ColorBox_8 oColorBox
#define ColorBox_9 nColorBox
#define ColorBox_0 cColorBox
#define BoxSpacing_m1 mBoxSpacing
#define BoxSpacing_m2 mdBoxSpacing
#define BoxSpacing_m3 mtBoxSpacing
#define BoxSpacing_m4 mqBoxSpacing
#define BoxSpacing_m5 miBoxSpacing
#define BoxSpacing_m6 mxBoxSpacing
#define BoxSpacing_m7 mpBoxSpacing
#define BoxSpacing_m8 moBoxSpacing
#define BoxSpacing_m9 mnBoxSpacing
#define BoxSpacing_m0 mcBoxSpacing
#define Margin_m1 mMargin
#define Margin_m2 mdMargin
#define Margin_m3 mtMargin
#define Margin_m4 mqMargin
#define Margin_m5 miMargin
#define Margin_m6 mxMargin
#define Margin_m7 mpMargin
#define Margin_m8 moMargin
#define Margin_m9 mnMargin
#define Margin_m0 mcMargin
#define Margin_th1 thMargin
#define Margin_th2 dthMargin
#define Margin_th3 tthMargin
#define Margin_th4 qthMargin
#define Margin_th5 ithMargin
#define Margin_th6 xthMargin
#define Margin_th7 pthMargin
#define Margin_th8 othMargin
#define Margin_th9 nthMargin
#define Margin_th0 cthMargin
#define Margin_mth1 mthMargin
#define Margin_mth2 mdthMargin
#define Margin_mth3 mtthMargin
#define Margin_mth4 mqthMargin
#define Margin_mth5 mithMargin
#define Margin_mth6 mxthMargin
#define Margin_mth7 mpthMargin
#define Margin_mth8 mothMargin
#define Margin_mth9 mnthMargin
#define Margin_mth0 mcthMargin
#define Margin_h1 hMargin
#define Margin_h2 dhMargin
#define Margin_h3 thMargin
#define Margin_h4 qhMargin
#define Margin_h5 ihMargin
#define Margin_h6 xhMargin
#define Margin_h7 phMargin
#define Margin_h8 ohMargin
#define Margin_h9 nhMargin
#define Margin_h0 chMargin
#define Margin_mh1 mhMargin
#define Margin_mh2 mdhMargin
#define Margin_mh3 mthMargin
#define Margin_mh4 mqhMargin
#define Margin_mh5 mihMargin
#define Margin_mh6 mxhMargin
#define Margin_mh7 mphMargin
#define Margin_mh8 mohMargin
#define Margin_mh9 mnhMargin
#define Margin_mh0 mchMargin
#define ContentHeight_m1 mContentHeight
#define ContentHeight_m2 mdContentHeight
#define ContentHeight_m3 mtContentHeight
#define ContentHeight_m4 mqContentHeight
#define ContentHeight_m5 miContentHeight
#define ContentHeight_m6 mxContentHeight
#define ContentHeight_m7 mpContentHeight
#define ContentHeight_m8 moContentHeight
#define ContentHeight_m9 mnContentHeight
#define ContentHeight_m0 mcContentHeight
#define bx01 BoxSpacing
#define bx02 dBoxSpacing
#define bx03 tBoxSpacing
#define bx04 qBoxSpacing
#define bx05 iBoxSpacing
#define bx06 xBoxSpacing
#define bx07 pBoxSpacing
#define bx08 oBoxSpacing
#define bx09 nBoxSpacing
#define bx10 cBoxSpacing
#define mbx01 mBoxSpacing
#define mbx02 mdBoxSpacing
#define mbx03 mtBoxSpacing
#define mbx04 mqBoxSpacing
#define mbx05 miBoxSpacing
#define mbx06 mxBoxSpacing
#define mbx07 mpBoxSpacing
#define mbx08 moBoxSpacing
#define mbx09 mnBoxSpacing
#define mbx10 mcBoxSpacing
#define rg01 Margin
#define rg02 dMargin
#define rg03 tMargin
#define rg04 qMargin
#define rg05 iMargin
#define rg06 xMargin
#define rg07 pMargin
#define rg08 oMargin
#define rg09 nMargin
#define rg10 cMargin
#define mrg01 mMargin
#define mrg02 mdMargin
#define mrg03 mtMargin
#define mrg04 mqMargin
#define mrg05 miMargin
#define mrg06 mxMargin
#define mrg07 mpMargin
#define mrg08 moMargin
#define mrg09 mnMargin
#define mrg10 mcMargin
#define thrg01 thMargin
#define thrg02 dthMargin
#define thrg03 tthMargin
#define thrg04 qthMargin
#define thrg05 ithMargin
#define thrg06 xthMargin
#define thrg07 pthMargin
#define thrg08 othMargin
#define thrg09 nthMargin
#define thrg10 cthMargin
#define mthrg01 mthMargin
#define mthrg02 mdthMargin
#define mthrg03 mtthMargin
#define mthrg04 mqthMargin
#define mthrg05 mithMargin
#define mthrg06 mxthMargin
#define mthrg07 mpthMargin
#define mthrg08 mothMargin
#define mthrg09 mnthMargin
#define mthrg10 mcthMargin
#define hrg01 hMargin
#define hrg02 dhMargin
#define hrg03 thMargin
#define hrg04 qhMargin
#define hrg05 ihMargin
#define hrg06 xhMargin
#define hrg07 phMargin
#define hrg08 ohMargin
#define hrg09 nhMargin
#define hrg10 chMargin
#define mhrg01 mhMargin
#define mhrg02 mdhMargin
#define mhrg03 mthMargin
#define mhrg04 mqhMargin
#define mhrg05 mihMargin
#define mhrg06 mxhMargin
#define mhrg07 mphMargin
#define mhrg08 mohMargin
#define mhrg09 mnhMargin
#define mhrg10 mchMargin
#define ct01 ContentHeight
#define ct02 dContentHeight
#define ct03 tContentHeight
#define ct04 qContentHeight
#define ct05 iContentHeight
#define ct06 xContentHeight
#define ct07 pContentHeight
#define ct08 oContentHeight
#define ct09 nContentHeight
#define ct10 cContentHeight
#define mct01 mContentHeight
#define mct02 mdContentHeight
#define mct03 mtContentHeight
#define mct04 mqContentHeight
#define mct05 miContentHeight
#define mct06 mxContentHeight
#define mct07 mpContentHeight
#define mct08 moContentHeight
#define mct09 mnContentHeight
#define mct10 mcContentHeight
#define lr01 ColorBox
#define lr02 dColorBox
#define lr03 tColorBox
#define lr04 qColorBox
#define lr05 iColorBox
#define lr06 xColorBox
#define lr07 pColorBox
#define lr08 oColorBox
#define lr09 nColorBox
#define lr10 cColorBox
#define bx BoxSpacing
#define dbx dBoxSpacing
#define tbx tBoxSpacing
#define qbx qBoxSpacing
#define ibx iBoxSpacing
#define xbx xBoxSpacing
#define pbx pBoxSpacing
#define obx oBoxSpacing
#define nbx nBoxSpacing
#define cbx cBoxSpacing
#define mbx mBoxSpacing
#define mdbx mdBoxSpacing
#define mtbx mtBoxSpacing
#define mqbx mqBoxSpacing
#define mibx miBoxSpacing
#define mxbx mxBoxSpacing
#define mpbx mpBoxSpacing
#define mobx moBoxSpacing
#define mnbx mnBoxSpacing
#define mcbx mcBoxSpacing
#define rg Margin
#define drg dMargin
#define trg tMargin
#define qrg qMargin
#define irg iMargin
#define xrg xMargin
#define prg pMargin
#define org oMargin
#define nrg nMargin
#define crg cMargin
#define mrg mMargin
#define mdrg mdMargin
#define mtrg mtMargin
#define mqrg mqMargin
#define mirg miMargin
#define mxrg mxMargin
#define mprg mpMargin
#define morg moMargin
#define mnrg mnMargin
#define mcrg mcMargin
#define thrg thMargin
#define dthrg dthMargin
#define tthrg tthMargin
#define qthrg qthMargin
#define ithrg ithMargin
#define xthrg xthMargin
#define pthrg pthMargin
#define othrg othMargin
#define nthrg nthMargin
#define cthrg cthMargin
#define mthrg mthMargin
#define mdthrg mdthMargin
#define mtthrg mtthMargin
#define mqthrg mqthMargin
#define mithrg mithMargin
#define mxthrg mxthMargin
#define mpthrg mpthMargin
#define mothrg mothMargin
#define mnthrg mnthMargin
#define mcthrg mcthMargin
#define hrg hMargin
#define dhrg dhMargin
#define qhrg qhMargin
#define ihrg ihMargin
#define xhrg xhMargin
#define phrg phMargin
#define ohrg ohMargin
#define nhrg nhMargin
#define chrg chMargin
#define mhrg mhMargin
#define mdhrg mdhMargin
#define mqhrg mqhMargin
#define mihrg mihMargin
#define mxhrg mxhMargin
#define mphrg mphMargin
#define mohrg mohMargin
#define mnhrg mnhMargin
#define mchrg mchMargin
#define ct ContentHeight
#define dct dContentHeight
#define tct tContentHeight
#define qct qContentHeight
#define ict iContentHeight
#define xct xContentHeight
#define pct pContentHeight
#define oct oContentHeight
#define nct nContentHeight
#define cct cContentHeight
#define mct mContentHeight
#define mdct mdContentHeight
#define mtct mtContentHeight
#define mqct mqContentHeight
#define mict miContentHeight
#define mxct mxContentHeight
#define mpct mpContentHeight
#define moct moContentHeight
#define mnct mnContentHeight
#define mcct mcContentHeight
#define lr ColorBox
#define dlr dColorBox
#define tlr tColorBox
#define qlr qColorBox
#define ilr iColorBox
#define xlr xColorBox
#define plr pColorBox
#define olr oColorBox
#define nlr nColorBox
#define clr cColorBox
