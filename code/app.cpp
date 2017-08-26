/*
File:   app.cpp
Author: Taylor Robbins
Date:   08\26\2017
Description: 
	** Holds the declarations for all exported functions in the application DLL
	** Also includes all the other files that comprise the application 
*/

#define RESET_APPLICATION  false
#define TEST_FONT_SIZE     16

#include <stdarg.h>
#include "platformInterface.h"
#include "app_version.h"
#include "Colors.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "memoryArena.h"
#include "linkedList.h"
#include "easing.h"

const PlatformInfo_t* Gl_PlatformInfo = nullptr;
const AppMemory_t*    Gl_AppMemory    = nullptr;

#include "appHelpers.cpp"

// +----------------------------------------------------------------+
// |                      Application Includes                      |
// +----------------------------------------------------------------+
#include "appStructs.h"
#include "appRenderState.h"
#include "appData.h"

AppData_t*      GL_AppData = nullptr;

// +----------------------------------------------------------------+
// |                    Application Source Files                    |
// +----------------------------------------------------------------+
#include "appFontHelpers.cpp"
#include "appLoadingFunctions.cpp"
#include "appRenderState.cpp"

// +------------------------------------------------------------------+
// |                          AppGetVersion                           |
// +------------------------------------------------------------------+
AppGetVersion_DEFINITION(App_GetVersion)
{
	Version_t version = {
		APP_VERSION_MAJOR,
		APP_VERSION_MINOR,
		APP_VERSION_BUILD,
	};

	if (resetApplication != nullptr)
	{
		*resetApplication = RESET_APPLICATION;
	}

	return version;
}

// +------------------------------------------------------------------+
// |                          AppInitialize                           |
// +------------------------------------------------------------------+
AppInitialize_DEFINITION(App_Initialize)
{
	Gl_PlatformInfo = PlatformInfo;
	Gl_AppMemory = AppMemory;
	DEBUG_WriteLine("Initializing Application...");
	
	AppData_t* appData = (AppData_t*)AppMemory->permanantPntr;
	ClearPointer(appData);
	GL_AppData = appData;
	
	void* arenaBase = (void*)(appData+1);
	u32 arenaSize = AppMemory->permanantSize - sizeof(AppData_t);
	InitializeMemoryArenaHeap(&appData->mainHeap, arenaBase, arenaSize);
	
	InitializeRenderState(PlatformInfo, &appData->renderState);
	
	v2i screenSize = PlatformInfo->screenSize;
	
	appData->simpleShader = LoadShader(
		"Resources/Shaders/simple-vertex.glsl",
		"Resources/Shaders/simple-fragment.glsl");
	
	appData->testTexture = LoadTexture("Resources/Sprites/close.png");
	appData->testFont = LoadFont("Resources/Fonts/consola.ttf", TEST_FONT_SIZE, 1024, 1024, ' ', 96);
	
	DEBUG_WriteLine("Initialization Done!");
}

// +------------------------------------------------------------------+
// |                           AppReloaded                            |
// +------------------------------------------------------------------+
AppReloaded_DEFINITION(App_Reloaded)
{
	Gl_PlatformInfo = PlatformInfo;
	Gl_AppMemory = AppMemory;
	AppData_t* appData = (AppData_t*)AppMemory->permanantPntr;
	GL_AppData = appData;
	
	// Do nothing for now
}

// +------------------------------------------------------------------+
// |                            AppUpdate                             |
// +------------------------------------------------------------------+
AppUpdate_DEFINITION(App_Update)
{
	Gl_PlatformInfo = PlatformInfo;
	Gl_AppMemory = AppMemory;
	AppData_t* appData = (AppData_t*)AppMemory->permanantPntr;
	GL_AppData = appData;
	RenderState_t* rs = &appData->renderState;
	
	//TODO: Put stuff here
	
	// +==================================+
	// |         Rendering Setup          |
	// +==================================+
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	rs->SetViewport(NewRectangle(0, 0, (r32)PlatformInfo->screenSize.x, (r32)PlatformInfo->screenSize.y));
	
	v3 backgroundColorVec = ColorToVec3({Color_CadetBlue});
	glClearColor(backgroundColorVec.x, backgroundColorVec.y, backgroundColorVec.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	rs->BindFrameBuffer(nullptr);
	rs->BindShader(&appData->simpleShader);
	rs->BindFont(&appData->testFont);
	rs->SetGradientEnabled(false);
	
	Matrix4_t worldMatrix, viewMatrix, projMatrix;
	viewMatrix = Matrix4_Identity;
	projMatrix = Matrix4Scale(NewVec3(2.0f/PlatformInfo->screenSize.x, -2.0f/PlatformInfo->screenSize.y, 1.0f));
	projMatrix = Mat4Mult(projMatrix, Matrix4Translate(NewVec3(-PlatformInfo->screenSize.x/2.0f, -PlatformInfo->screenSize.y/2.0f, 0.0f)));
	rs->SetViewMatrix(viewMatrix);
	rs->SetProjectionMatrix(projMatrix);
	
	// +==================================+
	// |            Rendering             |
	// +==================================+
	rs->DrawGradient(NewRectangle(0, 0, 300, 300), {Color_Red}, {Color_Blue}, Direction2D_Right);
}

// +----------------------------------------------------------------+
// |                       AppGetSoundSamples                       |
// +----------------------------------------------------------------+
AppGetSoundSamples_DEFINITION(App_GetSoundSamples)
{
	Gl_PlatformInfo = PlatformInfo;
	Gl_AppMemory = AppMemory;
	AppData_t* appData = (AppData_t*)AppMemory->permanantPntr;
	GL_AppData = appData;
	
	// Do nothing for now
}

// +----------------------------------------------------------------+
// |                           AppClosing                           |
// +----------------------------------------------------------------+
AppClosing_DEFINITION(App_Closing)
{
	Gl_PlatformInfo = PlatformInfo;
	Gl_AppMemory = AppMemory;
	AppData_t* appData = (AppData_t*)AppMemory->permanantPntr;
	GL_AppData = appData;
	
	DEBUG_WriteLine("Application closing!");
}
