//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* game
#include "Game/Unit/SandboxUnit.h"

//* engine [framework]
#include <Engine/Runtime/Framework/Core/Context.h>

//* engine [unit]
#include <Engine/Unit/EngineUnit.h>

//* windows
#include <windows.h>

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Sxx::Framework::Context::Push<Sxx::EngineUnit>();
	Sxx::Framework::Context::Push<SandboxUnit>();

	Sxx::Framework::Context::Run();
	
	return 0;

}
