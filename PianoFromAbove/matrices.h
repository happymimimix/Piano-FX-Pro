//Matix definitions
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

//Alternative Macro Naming Convention
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
