#include "Src/ModdingAPI.h"

int main() {
	cppsrc_t GameState = OpenCppFile(L".\\Src\\GameState.cpp");
	wcout << GameState.GetText();
}