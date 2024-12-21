#include <Tutorials.h>
#include <CheatEngineTutorial.h>
#include <VisualizePitchBendTutorial.h>
#include <GDITutorial.h>
#include <TutorialFramework.h>

void DisplayTutorial(Tutorials DocumentName) {
	switch (DocumentName) {
	case CheatEngineTutorial:
		OpenTutorialDocument(LoadCheatEngineTutorial());
		break;
	case VisualizePitchBendTutorial:

		break;
	case GDITutorial:

		break;
	default:
		cout << "[1;1H[40m[91mERROR: Unknown tutorial document: ";
		cout << (short)DocumentName;
		cout << GetLastError();
		cout << "\nThe program will now stop. \n";
		while (true) {
			// Make the program hang instead of closing! 
			// This way the user can clearly see the error message. 
		}
	}
}