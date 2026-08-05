//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* game
#include "Game/Unit/SandboxUnit.h"

//* engine [framework]
#include <Runtime/Framework/Core/Context.h>
#include <Runtime/Framework/Unit/EngineUnit.h>

//* windows
#include <windows.h>

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Sxx::Framework::Context::Push<Sxx::Framework::EngineUnit>();
	Sxx::Framework::Context::Push<SandboxUnit>();

	Sxx::Framework::Context::Run();
	
	return 0;

}
