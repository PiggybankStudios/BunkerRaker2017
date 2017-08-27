/*
File:   app.cpp
Author: Taylor Robbins
Date:   08\26\2017
Description: 
	** Holds the declarations for all exported functions in the application DLL
	** Also includes all the other files that comprise the application 
*/

#define RELOAD_APPLICATION  false
#define TEST_FONT_SIZE      16

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
		*resetApplication = RELOAD_APPLICATION;
	}

	return version;
}

void FillColorArray(AppData_t* appData)
{
	v2 center = NewVec2(appData->colorArraySize.x/2.0f, appData->colorArraySize.y/2.0f);
	for (i32 yPos = 0; yPos < appData->colorArraySize.x; yPos++)
	{
		for (i32 xPos = 0; xPos < appData->colorArraySize.x; xPos++)
		{
			u32* colorPntr = &appData->colorArray[yPos*appData->colorArraySize.x + xPos];
			u8* redPntr    = &((u8*)colorPntr)[0];
			u8* greenPntr  = &((u8*)colorPntr)[1];
			u8* bluePntr   = &((u8*)colorPntr)[2];
			u8* alphaPntr  = &((u8*)colorPntr)[3];
			
			*redPntr   = (u8)((r32)xPos / (r32)appData->colorArraySize.x * 255);
			*greenPntr = (u8)((r32)yPos / (r32)appData->colorArraySize.y * 255);
			*bluePntr  = 128;
			*alphaPntr = 255;
			
			if (xPos%2 == 0) *bluePntr = 0;
			if (yPos%2 == 0) *redPntr = 0;
			
			r32 sineValue = 50 + Sin32(xPos/10.0f)*20;
			if (yPos < sineValue)
			{
				*colorPntr = 0x00000000;
				*alphaPntr = 255;
			}
			
			// if (Vec2Length(NewVec2((r32)xPos, (r32)yPos) - center) > 45)
			// {
			// 	*colorPntr = 0x00000000;
			// 	*alphaPntr = 255;
			// }
		}
	}
	appData->colorArrayTexture = CreateTexture((u8*)appData->colorArray, appData->colorArraySize.x, appData->colorArraySize.y, true, false);
}

Color_t GetColorInArrayAt(AppData_t* appData, i32 xPos, i32 yPos)
{
	if (xPos < 0) return {Color_White};
	if (yPos < 0) return {Color_White};
	if (xPos >= appData->colorArraySize.x) return {Color_White};
	if (yPos >= appData->colorArraySize.y) return {Color_White};
	
	u8* colorPntr = (u8*)(&appData->colorArray[yPos*appData->colorArraySize.x + xPos]);
	
	return NewColor(
		colorPntr[0],
		colorPntr[1],
		colorPntr[2],
		colorPntr[3]
	);
}

bool IsPixelFilled(Color_t color)
{
	if (color.r == 0 && color.g == 0 && color.b == 0)
	{
		return false;
	}
	else if (color.a == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

v2 PixelCenter(r32 pixelSize, i32 xPos, i32 yPos)
{
	return NewVec2(xPos*pixelSize + pixelSize/2, yPos*pixelSize + pixelSize/2);
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
	
	appData->colorArraySize = NewVec2i(100, 100);
	appData->colorArray = PushArray(&appData->mainHeap, u32, appData->colorArraySize.x*appData->colorArraySize.y);
	FillColorArray(appData);
	
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
	
	DEBUG_WriteLine("Re-creating color array");
	DestroyTexture(&appData->colorArrayTexture);
	FillColorArray(appData);
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
	v2i screenSize = PlatformInfo->screenSize;
	r32 pixelSize = screenSize.x / (r32)appData->colorArrayTexture.width;
	v2 mousePos = AppInput->mousePos;
	
	// +================================+
	// |        Recreate Texture        |
	// +================================+
	if (ButtonDown(Button_Control) && ButtonPressed(Button_R))
	{
		DEBUG_WriteLine("Re-creating color array");
		DestroyTexture(&appData->colorArrayTexture);
		FillColorArray(appData);
	}
	
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
	rs->BindTexture(&appData->colorArrayTexture);
	rs->DrawTexturedRec(NewRectangle(0, 0, (r32)screenSize.x, (r32)screenSize.y), {Color_White});
	
	#define TANGENT_CHECK_RADIUS 10
	
	v2i mPixPos = NewVec2i((i32)(mousePos.x / pixelSize), (i32)(mousePos.y / pixelSize));
	v2 mPixCenter = NewVec2(mPixPos.x*pixelSize + pixelSize/2, mPixPos.y*pixelSize + pixelSize/2);
	rs->PrintString(NewVec2(0, appData->testFont.maxExtendUp), {Color_White}, 1.0f, "Grid Pos: (%d, %d)", mPixPos.x, mPixPos.y);
	rs->DrawButton(NewRectangle(mPixPos.x * pixelSize, mPixPos.y * pixelSize, pixelSize, pixelSize), {Color_TransparentBlack}, {Color_Red});
	rs->DrawButton(NewRectangle(
		mPixPos.x * pixelSize - TANGENT_CHECK_RADIUS*pixelSize,
		mPixPos.y * pixelSize - TANGENT_CHECK_RADIUS*pixelSize,
		TANGENT_CHECK_RADIUS*pixelSize*2 + pixelSize,
		TANGENT_CHECK_RADIUS*pixelSize*2 + pixelSize),
		{Color_TransparentBlack}, {Color_Red}
	);
	
	v2i minPos = NewVec2i(max(0, mPixPos.x - TANGENT_CHECK_RADIUS), max(0, mPixPos.y - TANGENT_CHECK_RADIUS));
	v2i maxPos = NewVec2i(min(appData->colorArraySize.x, mPixPos.x + TANGENT_CHECK_RADIUS), min(appData->colorArraySize.y, mPixPos.y + TANGENT_CHECK_RADIUS));
	
	v2 resultVector = Vec2_Zero;
	u32 numVectors = 0;
	for (i32 yPos = minPos.y; yPos <= maxPos.y; yPos++)
	{
		for (i32 xPos = minPos.x; xPos <= maxPos.x; xPos++)
		{
			v2 pixelCenter = PixelCenter(pixelSize, xPos, yPos);
			if (Vec2Length(pixelCenter - mPixCenter) <= TANGENT_CHECK_RADIUS*pixelSize)
			{
				Color_t pixels[3][3];
				pixels[0][0] = GetColorInArrayAt(appData, xPos-1, yPos-1);
				pixels[1][0] = GetColorInArrayAt(appData, xPos,   yPos-1);
				pixels[2][0] = GetColorInArrayAt(appData, xPos+1, yPos-1);
				pixels[0][1] = GetColorInArrayAt(appData, xPos-1, yPos);
				pixels[1][1] = GetColorInArrayAt(appData, xPos,   yPos);
				pixels[2][1] = GetColorInArrayAt(appData, xPos+1, yPos);
				pixels[0][2] = GetColorInArrayAt(appData, xPos-1, yPos+1);
				pixels[1][2] = GetColorInArrayAt(appData, xPos,   yPos+1);
				pixels[2][2] = GetColorInArrayAt(appData, xPos+1, yPos+1);
				Color_t* curPixel = &pixels[1][1];
				
				v2 centers[3][3];
				centers[0][0] = PixelCenter(pixelSize, xPos-1, yPos-1);
				centers[1][0] = PixelCenter(pixelSize, xPos,   yPos-1);
				centers[2][0] = PixelCenter(pixelSize, xPos+1, yPos-1);
				centers[0][1] = PixelCenter(pixelSize, xPos-1, yPos);
				centers[1][1] = PixelCenter(pixelSize, xPos,   yPos);
				centers[2][1] = PixelCenter(pixelSize, xPos+1, yPos);
				centers[0][2] = PixelCenter(pixelSize, xPos-1, yPos+1);
				centers[1][2] = PixelCenter(pixelSize, xPos,   yPos+1);
				centers[2][2] = PixelCenter(pixelSize, xPos+1, yPos+1);
				
				if (IsPixelFilled(*curPixel) &&
					(!IsPixelFilled(pixels[1][0]) || !IsPixelFilled(pixels[1][2]) || 
					 !IsPixelFilled(pixels[0][1]) || !IsPixelFilled(pixels[2][1])))
				{
					rs->DrawButton(NewRectangle(xPos*pixelSize, yPos*pixelSize, pixelSize, pixelSize), {Color_TransparentBlack}, {Color_White});
					
					Color_t lineColor = ColorTransparent({Color_Yellow}, 0.5f);
					r32 lineThickness = 2;
					//Top Left to Top Middle
					if (!IsPixelFilled(pixels[0][0]) && !IsPixelFilled(pixels[1][0]) &&
						IsPixelFilled(pixels[0][1]))
					{
						v2 vector = Vec2Normalize(centers[1][0] - centers[0][0]);
						resultVector += vector;
						numVectors++;
						rs->DrawLine(centers[1][0], centers[0][0], lineColor, lineThickness);
					}
					//Top Middle to Top Right
					if (!IsPixelFilled(pixels[1][0]) && !IsPixelFilled(pixels[2][0]) &&
						IsPixelFilled(pixels[2][1]))
					{
						v2 vector = Vec2Normalize(centers[2][0] - centers[1][0]);
						resultVector += vector;
						numVectors++;
						rs->DrawLine(centers[2][0], centers[1][0], lineColor, lineThickness);
					}
					//Top Right to Right
					if (!IsPixelFilled(pixels[2][0]) && !IsPixelFilled(pixels[2][1]) &&
						IsPixelFilled(pixels[1][0]))
					{
						v2 vector = Vec2Normalize(centers[2][1] - centers[2][0]);
						resultVector += vector;
						numVectors++;
						rs->DrawLine(centers[2][1], centers[2][0], lineColor, lineThickness);
					}
					//Right to Bottom Right
					if (!IsPixelFilled(pixels[2][1]) && !IsPixelFilled(pixels[2][2]) &&
						IsPixelFilled(pixels[1][2]))
					{
						v2 vector = Vec2Normalize(centers[2][2] - centers[2][1]);
						resultVector += vector;
						numVectors++;
						rs->DrawLine(centers[2][2], centers[2][1], lineColor, lineThickness);
					}
					//Bottom Right to Bottom Middle
					if (!IsPixelFilled(pixels[2][2]) && !IsPixelFilled(pixels[1][2]) &&
						IsPixelFilled(pixels[2][1]))
					{
						v2 vector = Vec2Normalize(centers[1][2] - centers[2][2]);
						resultVector += vector;
						numVectors++;
						rs->DrawLine(centers[1][2], centers[2][2], lineColor, lineThickness);
					}
					//Bottom Middle to Bottom Left
					if (!IsPixelFilled(pixels[1][2]) && !IsPixelFilled(pixels[0][2]) &&
						IsPixelFilled(pixels[0][1]))
					{
						v2 vector = Vec2Normalize(centers[0][2] - centers[1][2]);
						resultVector += vector;
						numVectors++;
						rs->DrawLine(centers[0][2], centers[1][2], lineColor, lineThickness);
					}
					//Bottom Left to Left
					if (!IsPixelFilled(pixels[0][2]) && !IsPixelFilled(pixels[0][1]) &&
						IsPixelFilled(pixels[1][2]))
					{
						v2 vector = Vec2Normalize(centers[0][1] - centers[0][2]);
						resultVector += vector;
						numVectors++;
						rs->DrawLine(centers[0][1], centers[0][2], lineColor, lineThickness);
					}
					//Left to Top Left
					if (!IsPixelFilled(pixels[0][1]) && !IsPixelFilled(pixels[0][0]) &&
						IsPixelFilled(pixels[1][0]))
					{
						v2 vector = Vec2Normalize(centers[0][0] - centers[0][1]);
						resultVector += vector;
						numVectors++;
						rs->DrawLine(centers[0][0], centers[0][1], lineColor, lineThickness);
					}
					
					//Left to Top
					if (!IsPixelFilled(pixels[0][1]) && !IsPixelFilled(pixels[1][0]))
					{
						v2 vector = Vec2Normalize(centers[1][0] - centers[0][1]);
						resultVector += vector;
						resultVector += vector;
						numVectors += 2;
						rs->DrawLine(centers[1][0], centers[0][1], lineColor, lineThickness);
					}
					//Top to Right
					if (!IsPixelFilled(pixels[1][0]) && !IsPixelFilled(pixels[2][1]))
					{
						v2 vector = Vec2Normalize(centers[2][1] - centers[1][0]);
						resultVector += vector;
						resultVector += vector;
						numVectors += 2;
						rs->DrawLine(centers[2][1], centers[1][0], lineColor, lineThickness);
					}
					//Right to Bottom
					if (!IsPixelFilled(pixels[2][1]) && !IsPixelFilled(pixels[1][2]))
					{
						v2 vector = Vec2Normalize(centers[1][2] - centers[2][1]);
						resultVector += vector;
						resultVector += vector;
						numVectors += 2;
						rs->DrawLine(centers[1][2], centers[2][1], lineColor, lineThickness);
					}
					//Bottom to Left
					if (!IsPixelFilled(pixels[1][2]) && !IsPixelFilled(pixels[0][1]))
					{
						v2 vector = Vec2Normalize(centers[0][1] - centers[1][2]);
						resultVector += vector;
						resultVector += vector;
						numVectors += 2;
						rs->DrawLine(centers[0][1], centers[1][2], lineColor, lineThickness);
					}
				}
			}
		}
	}
	
	resultVector.x = (resultVector.x / (r32)numVectors);
	resultVector.y = (resultVector.y / (r32)numVectors);
	
	rs->DrawLine(mousePos - resultVector*50, mousePos + resultVector*50, {Color_Cyan});
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
