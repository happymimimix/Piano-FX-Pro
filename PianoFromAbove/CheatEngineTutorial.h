#include <TutorialFramework.h>

TutorialDocument LoadCheatEngineTutorial() {
	TutorialDocument Document;
	Document.DocumentLength = 500;
	Document.TextContent = R"MultilineText(Testing document...

 ┌─────────────────────────────┐
 │                             │
 │                             │
 │                             │
 │                             │
 │                             │
 │                             │
 │                             │
 │                             │  Testing diagram #1
 │                             │
 └─────────────────────────────┘



                          ┌───────────────────────────────────────────────────────────────────────┐
       Testing diagram #2 │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          │                                                                       │
                          └───────────────────────────────────────────────────────────────────────┘

     [1;31mColor [32mtext [33mtest
)MultilineText";
	Diagram BMP = {};
	BMP.BMPid = IDB_BITMAP1;
	BMP.X = 2;
	BMP.Y = 3;
	BMP.W = 29;
	BMP.H = 9;
	Document.DiagramsList.push_back(BMP);
	BMP.BMPid = IDB_BITMAP2;
	BMP.X = 27;
	BMP.Y = 17;
    BMP.W = 71;
    BMP.H = 29;
	Document.DiagramsList.push_back(BMP);
	return Document;
}