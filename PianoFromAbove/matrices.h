//Matix definitions
#include <Widths.h>
#include "imgui/Fonts.h"
#define PropertyWindowW 300
#define PropertyWindowH 150
#define TrackWindowW 300
#define TrackWindowH 200
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
#define mhBoxSpacing 0-hBoxSpacing //Half
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
#define mhMargin 0-hMargin //Half
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
