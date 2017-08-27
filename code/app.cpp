/*
File:   app.cpp
Author: Taylor Robbins
Date:   08\26\2017
Description: 
	** Holds the declarations for all exported functions in the application DLL
	** Also includes all the other files that comprise the application 
*/

#define RELOAD_APPLICATION    false
#define TEST_FONT_SIZE        16
#define TANGENT_CHECK_RADIUS  10
#define STEP_SIZE             10
#define MAX_DROP              5
#define MAX_DIVISION_BACKSTEP 5

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

const Color_t MyColors[] = { {Color_White}, {Color_Red}, {Color_Turquoise}, {Color_Purple}, {Color_Blue} };

void FillColorArray(AppData_t* appData, v2i mousePos)
{
	#define NUM_BLOBS 3
	v2 blobCenter[NUM_BLOBS];
	blobCenter[0] = NewVec2(20, 20);
	blobCenter[1] = NewVec2(50, 20);
	blobCenter[2] = NewVec2((r32)mousePos.x, (r32)mousePos.y);
	r32 blobRadius[NUM_BLOBS];
	blobRadius[0] = 5;
	blobRadius[1] = 5;
	blobRadius[2] = 15;
	
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
			
			Color_t pixelColor = { Color_Black };
			
			// *redPntr   = 0; //(u8)((r32)xPos / (r32)appData->colorArraySize.x * 255);
			// *greenPntr = 0; //(u8)((r32)yPos / (r32)appData->colorArraySize.y * 255);
			// *bluePntr  = 0;
			// *alphaPntr = 255;
			
			r32 value = 0;
			for (i32 bIndex = 0; bIndex < NUM_BLOBS; bIndex++)
			{
				r32 distance = Vec2Length(NewVec2((r32)xPos, (r32)yPos) - blobCenter[bIndex]);
				r32 addValue = blobRadius[bIndex] / distance;
				value += addValue;
			}
			
			if (value >= 1.0f)
			{
				pixelColor = { Color_SandyBrown };
			}
			
			*redPntr = pixelColor.r;
			*greenPntr = pixelColor.g;
			*bluePntr = pixelColor.b;
			*alphaPntr = pixelColor.a;
			
			// if (xPos%2 == 0) *bluePntr = 0;
			// if (yPos%2 == 0) *redPntr = 0;
			
			// r32 sineValue = 100 + Sin32(xPos/30.0f)*50;
			// if (yPos < sineValue)
			// {
			// 	*colorPntr = 0x00000000;
			// 	*alphaPntr = 255;
			// }
			
			// if ((rand() % 100) < 70)
			// {
			// 	*colorPntr = 0x00000000;
			// 	*alphaPntr = 255;
			// }
			
			// if (Vec2Length(NewVec2((r32)xPos, (r32)yPos) - center) > 45)
			// {
			// 	*colorPntr = 0x00000000;
			// 	*alphaPntr = 255;
			// }
		}
	}
	appData->colorArrayTexture = CreateTexture((u8*)appData->colorArray, appData->colorArraySize.x, appData->colorArraySize.y, true, false);
}

void HighlightPixel(RenderState_t* rs, r32 pixelSize, v2i pixelPos, Color_t color)
{
	rs->DrawButton(NewRectangle(pixelPos.x * pixelSize, pixelPos.y * pixelSize, pixelSize, pixelSize), {Color_TransparentBlack}, color);
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

bool IsPixelEdge(AppData_t* appData, i32 xPos, i32 yPos)
{
	bool result = false;
	
	Color_t pixels[3][3];
	// pixels[0][0] = GetColorInArrayAt(appData, xPos-1, yPos-1);
	pixels[1][0] = GetColorInArrayAt(appData, xPos,   yPos-1);
	// pixels[2][0] = GetColorInArrayAt(appData, xPos+1, yPos-1);
	pixels[0][1] = GetColorInArrayAt(appData, xPos-1, yPos);
	pixels[1][1] = GetColorInArrayAt(appData, xPos,   yPos);
	pixels[2][1] = GetColorInArrayAt(appData, xPos+1, yPos);
	// pixels[0][2] = GetColorInArrayAt(appData, xPos-1, yPos+1);
	pixels[1][2] = GetColorInArrayAt(appData, xPos,   yPos+1);
	// pixels[2][2] = GetColorInArrayAt(appData, xPos+1, yPos+1);
	
	if (IsPixelFilled(pixels[1][1]))
	{
		if (!IsPixelFilled(pixels[1][0]) ||
		    !IsPixelFilled(pixels[2][1]) ||
		    !IsPixelFilled(pixels[1][2]) ||
		    !IsPixelFilled(pixels[0][1]))
		{
			result = true;
		}
	}
	
	return result;
}

v2 PixelCenter(r32 pixelSize, i32 xPos, i32 yPos)
{
	return NewVec2(xPos*pixelSize + pixelSize/2, yPos*pixelSize + pixelSize/2);
}

bool Raycast(AppData_t* appData, v2 startPos, v2 direction, r32 pixelSize, v2i* endPosOut, RenderState_t* rs = nullptr)
{
	v2i curPixel = NewVec2i((i32)(startPos.x / pixelSize), (i32)(startPos.y / pixelSize));
	v2 offset = startPos - NewVec2(curPixel.x * pixelSize, curPixel.y * pixelSize);
	v2 dir = Vec2Normalize(direction);
	
	if (dir.x == 0 && dir.y == 0)
	{
		return false;
	}
	
	while (!IsPixelEdge(appData, curPixel.x, curPixel.y))
	{
		if (rs != nullptr)
		{
			HighlightPixel(rs, pixelSize, curPixel, {Color_Yellow});
		}
		
		r32 xDist = 0;
		if (dir.x > 0)
		{
			xDist = pixelSize - offset.x;
		}
		else if (dir.x < 0)
		{
			xDist = offset.x;
		}
		else
		{
			//no x-axis movement
		}
		
		r32 yDist = 0;
		if (dir.y > 0)
		{
			yDist = pixelSize - offset.y;
		}
		else if (dir.y < 0)
		{
			yDist = offset.y;
		}
		else
		{
			//no y-axis movement
		}
		
		r32 xTime = 0;
		if (dir.x != 0)
		{
			xTime = xDist / Abs32(dir.x);
		}
		
		r32 yTime = 0;
		if (dir.y != 0)
		{
			yTime = yDist / Abs32(dir.y);
		}
		
		if (dir.x != 0 && (dir.y == 0 || xTime <= yTime))
		{
			//Move along x-axis
			if (dir.x > 0)
			{
				curPixel.x += 1;
				offset.x = 0;
				offset.y += dir.y * xTime;
			}
			else
			{
				curPixel.x -= 1;
				offset.x = pixelSize;
				offset.y += dir.y * xTime;
			}
		}
		else
		{
			//Move along y-axis
			if (dir.y > 0)
			{
				curPixel.y += 1;
				offset.y = 0;
				offset.x += dir.x * yTime;
			}
			else
			{
				curPixel.y -= 1;
				offset.y = pixelSize;
				offset.x += dir.x * yTime;
			}
		}
		
		if (curPixel.x < 0)
		{
			return false;
		}
		if (curPixel.y < 0)
		{
			return false;
		}
		if (curPixel.x >= appData->colorArraySize.x)
		{
			return false;
		}
		if (curPixel.y >= appData->colorArraySize.y)
		{
			return false;
		}
	}
	
	if (rs != nullptr)
	{
		HighlightPixel(rs, pixelSize, curPixel, {Color_Red});
	}
	
	*endPosOut = curPixel;
	return true;
}

v2 FindTangentVector(AppData_t* appData, v2i position, i32 averageRadius, r32 pixelSize, RenderState_t* rs = nullptr)
{
	v2i minPos = NewVec2i(max(0, position.x - averageRadius), max(0, position.y - averageRadius));
	v2i maxPos = NewVec2i(min(appData->colorArraySize.x, position.x + averageRadius), min(appData->colorArraySize.y, position.y + averageRadius));
	v2 startPixCenter = PixelCenter(pixelSize, position.x, position.y);
	
	v2 result = Vec2_Zero;
	u32 numVectors = 0;
	for (i32 yPos = minPos.y; yPos <= maxPos.y; yPos++)
	{
		for (i32 xPos = minPos.x; xPos <= maxPos.x; xPos++)
		{
			v2 pixelCenter = PixelCenter(pixelSize, xPos, yPos);
			if (Vec2Length(pixelCenter - startPixCenter) <= averageRadius*pixelSize)
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
					if (rs != nullptr)
					{
						HighlightPixel(rs, pixelSize, NewVec2i(xPos, yPos), {Color_White});
					}
					
					Color_t lineColor = ColorTransparent({Color_Yellow}, 0.5f);
					r32 lineThickness = 2;
					//Top Left to Top Middle
					if (!IsPixelFilled(pixels[0][0]) && !IsPixelFilled(pixels[1][0]) &&
						IsPixelFilled(pixels[0][1]))
					{
						v2 vector = Vec2Normalize(centers[1][0] - centers[0][0]);
						result += vector;
						numVectors++;
						if (rs != nullptr)
						{
							rs->DrawLine(centers[1][0], centers[0][0], lineColor, lineThickness);
						}
					}
					//Top Middle to Top Right
					if (!IsPixelFilled(pixels[1][0]) && !IsPixelFilled(pixels[2][0]) &&
						IsPixelFilled(pixels[2][1]))
					{
						v2 vector = Vec2Normalize(centers[2][0] - centers[1][0]);
						result += vector;
						numVectors++;
						if (rs != nullptr)
						{
							rs->DrawLine(centers[2][0], centers[1][0], lineColor, lineThickness);
						}
					}
					//Top Right to Right
					if (!IsPixelFilled(pixels[2][0]) && !IsPixelFilled(pixels[2][1]) &&
						IsPixelFilled(pixels[1][0]))
					{
						v2 vector = Vec2Normalize(centers[2][1] - centers[2][0]);
						result += vector;
						numVectors++;
						if (rs != nullptr)
						{
							rs->DrawLine(centers[2][1], centers[2][0], lineColor, lineThickness);
						}
					}
					//Right to Bottom Right
					if (!IsPixelFilled(pixels[2][1]) && !IsPixelFilled(pixels[2][2]) &&
						IsPixelFilled(pixels[1][2]))
					{
						v2 vector = Vec2Normalize(centers[2][2] - centers[2][1]);
						result += vector;
						numVectors++;
						if (rs != nullptr)
						{
							rs->DrawLine(centers[2][2], centers[2][1], lineColor, lineThickness);
						}
					}
					//Bottom Right to Bottom Middle
					if (!IsPixelFilled(pixels[2][2]) && !IsPixelFilled(pixels[1][2]) &&
						IsPixelFilled(pixels[2][1]))
					{
						v2 vector = Vec2Normalize(centers[1][2] - centers[2][2]);
						result += vector;
						numVectors++;
						if (rs != nullptr)
						{
							rs->DrawLine(centers[1][2], centers[2][2], lineColor, lineThickness);
						}
					}
					//Bottom Middle to Bottom Left
					if (!IsPixelFilled(pixels[1][2]) && !IsPixelFilled(pixels[0][2]) &&
						IsPixelFilled(pixels[0][1]))
					{
						v2 vector = Vec2Normalize(centers[0][2] - centers[1][2]);
						result += vector;
						numVectors++;
						if (rs != nullptr)
						{
							rs->DrawLine(centers[0][2], centers[1][2], lineColor, lineThickness);
						}
					}
					//Bottom Left to Left
					if (!IsPixelFilled(pixels[0][2]) && !IsPixelFilled(pixels[0][1]) &&
						IsPixelFilled(pixels[1][2]))
					{
						v2 vector = Vec2Normalize(centers[0][1] - centers[0][2]);
						result += vector;
						numVectors++;
						if (rs != nullptr)
						{
							rs->DrawLine(centers[0][1], centers[0][2], lineColor, lineThickness);
						}
					}
					//Left to Top Left
					if (!IsPixelFilled(pixels[0][1]) && !IsPixelFilled(pixels[0][0]) &&
						IsPixelFilled(pixels[1][0]))
					{
						v2 vector = Vec2Normalize(centers[0][0] - centers[0][1]);
						result += vector;
						numVectors++;
						if (rs != nullptr)
						{
							rs->DrawLine(centers[0][0], centers[0][1], lineColor, lineThickness);
						}
					}
					
					//Left to Top
					if (!IsPixelFilled(pixels[0][1]) && !IsPixelFilled(pixels[1][0]))
					{
						v2 vector = Vec2Normalize(centers[1][0] - centers[0][1]);
						result += vector;
						result += vector;
						numVectors += 2;
						if (rs != nullptr)
						{
							rs->DrawLine(centers[1][0], centers[0][1], lineColor, lineThickness);
						}
					}
					//Top to Right
					if (!IsPixelFilled(pixels[1][0]) && !IsPixelFilled(pixels[2][1]))
					{
						v2 vector = Vec2Normalize(centers[2][1] - centers[1][0]);
						result += vector;
						result += vector;
						numVectors += 2;
						if (rs != nullptr)
						{
							rs->DrawLine(centers[2][1], centers[1][0], lineColor, lineThickness);
						}
					}
					//Right to Bottom
					if (!IsPixelFilled(pixels[2][1]) && !IsPixelFilled(pixels[1][2]))
					{
						v2 vector = Vec2Normalize(centers[1][2] - centers[2][1]);
						result += vector;
						result += vector;
						numVectors += 2;
						if (rs != nullptr)
						{
							rs->DrawLine(centers[1][2], centers[2][1], lineColor, lineThickness);
						}
					}
					//Bottom to Left
					if (!IsPixelFilled(pixels[1][2]) && !IsPixelFilled(pixels[0][1]))
					{
						v2 vector = Vec2Normalize(centers[0][1] - centers[1][2]);
						result += vector;
						result += vector;
						numVectors += 2;
						if (rs != nullptr)
						{
							rs->DrawLine(centers[0][1], centers[1][2], lineColor, lineThickness);
						}
					}
				}
			}
		}
	}
	
	result.x = (result.x / (r32)numVectors);
	result.y = (result.y / (r32)numVectors);
	
	return result;
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
	FillColorArray(appData, NewVec2i(50, 50));
	
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
	FillColorArray(appData, NewVec2i(50, 50));
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
	v2i mPixPos = NewVec2i((i32)(mousePos.x / pixelSize), (i32)(mousePos.y / pixelSize));
	v2 mPixCenter = NewVec2(mPixPos.x*pixelSize + pixelSize/2, mPixPos.y*pixelSize + pixelSize/2);
	
	// +================================+
	// |        Recreate Texture        |
	// +================================+
	if (ButtonDown(Button_Control) && ButtonDown(Button_R))
	{
		DEBUG_WriteLine("Re-creating color array");
		DestroyTexture(&appData->colorArrayTexture);
		FillColorArray(appData, mPixPos);
	}
	
	if (ButtonPressed(MouseButton_Left))
	{
		appData->testStartPos = mousePos;
	}
	
	if (ButtonPressed(Button_D))
	{
		appData->drawDebug = !appData->drawDebug;
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
	
	// +================================+
	// |   Tangent Vector Calculation   |
	// +================================+
	if (ButtonDown(Button_T))
	{
		rs->PrintString(NewVec2(0, appData->testFont.maxExtendUp), {Color_White}, 1.0f, "Grid Pos: (%d, %d)", mPixPos.x, mPixPos.y);
		rs->DrawButton(NewRectangle(mPixPos.x * pixelSize, mPixPos.y * pixelSize, pixelSize, pixelSize), {Color_TransparentBlack}, {Color_Red});
		rs->DrawButton(NewRectangle(
			mPixPos.x * pixelSize - TANGENT_CHECK_RADIUS*pixelSize,
			mPixPos.y * pixelSize - TANGENT_CHECK_RADIUS*pixelSize,
			TANGENT_CHECK_RADIUS*pixelSize*2 + pixelSize,
			TANGENT_CHECK_RADIUS*pixelSize*2 + pixelSize),
			{Color_TransparentBlack}, {Color_Red}
		);
		
		v2 testTangent = FindTangentVector(appData, mPixPos, TANGENT_CHECK_RADIUS, pixelSize, rs);
		
		rs->DrawLine(mousePos - testTangent*50, mousePos + testTangent*50, {Color_Cyan});
	}
	
	// +================================+
	// |          Test Raycast          |
	// +================================+
	if (appData->testStartPos.x != mousePos.x || appData->testStartPos.y != mousePos.y)
	{
		v2i raycastEnd;
		if (Raycast(appData, appData->testStartPos, mousePos - appData->testStartPos, pixelSize, &raycastEnd, appData->drawDebug ? rs : nullptr))
		{
			rs->DrawLine(appData->testStartPos, PixelCenter(pixelSize, raycastEnd.x, raycastEnd.y), {Color_Turquoise}, 1);
			if (ButtonDown(MouseButton_Right))
			{
				appData->testStartPixel = raycastEnd;
			}
		}
		else
		{
			rs->DrawLine(appData->testStartPos, mousePos, {Color_Purple});
		}
	}
	
	// +================================+
	// |        Step Around Test        |
	// +================================+
	if (IsPixelFilled(GetColorInArrayAt(appData, appData->testStartPixel.x, appData->testStartPixel.y)))
	{
		v2i startPos = appData->testStartPixel;
		v2 startPosCenter = PixelCenter(pixelSize, startPos.x, startPos.y);
		v2 startPosTangent = Vec2_Zero;
		HighlightPixel(rs, pixelSize, startPos, {Color_Turquoise});
		
		v2i curPos = startPos;
		for (i32 cIndex = 0; cIndex < 50; cIndex++)
		{
			v2 curCenter = PixelCenter(pixelSize, curPos.x, curPos.y);
			v2 tangent = FindTangentVector(appData, curPos, TANGENT_CHECK_RADIUS, pixelSize);
			if (cIndex == 0) startPosTangent = tangent;
			
			r32 stepSize = STEP_SIZE;
			i32 divisionCount = 0;
			v2 armEnd; v2i armEndPixPos; r32 dropDist; v2 perpDir; v2i newPos = Vec2i_Zero;
			do
			{
				armEnd = curCenter + tangent * stepSize * pixelSize;
				if (appData->drawDebug) { rs->DrawLine(curCenter, armEnd, MyColors[divisionCount], 2); }
				
				armEndPixPos = NewVec2i((i32)(armEnd.x / pixelSize), (i32)(armEnd.y / pixelSize));
				perpDir = NewVec2(tangent.y, -tangent.x);
				if (!IsPixelFilled(GetColorInArrayAt(appData, armEndPixPos.x, armEndPixPos.y)))
				{
					perpDir = NewVec2(-tangent.y, tangent.x);
				}
				
				if (Raycast(appData, armEnd, perpDir, pixelSize, &newPos))
				{
					dropDist = Vec2Length(armEnd - PixelCenter(pixelSize, newPos.x, newPos.y));
					if (dropDist <= MAX_DROP*pixelSize || divisionCount >= MAX_DIVISION_BACKSTEP-1)
					{
						break;
					}
				}
				
				stepSize = stepSize / 2;
				
				divisionCount++;
				if (divisionCount >= MAX_DIVISION_BACKSTEP)
				{
					break;
				}
				
			} while (true);
			
			if (appData->drawDebug)
			{
				// v2i dummyPixelPos;
				// Raycast(appData, armEnd, perpDir, pixelSize, &dummyPixelPos, rs);
				rs->DrawLine(armEnd, PixelCenter(pixelSize, newPos.x, newPos.y), {Color_Green});
			}
			
			bool foundStart = false;
			r32 distToStart = Vec2Length(PixelCenter(pixelSize, newPos.x, newPos.y) - startPosCenter);
			if (cIndex != 0 &&
				distToStart < STEP_SIZE*pixelSize &&
				Vec2Dot(tangent, startPosTangent) > 0)
			{
				foundStart = true;
				newPos = startPos;
			}
			
			// rs->DrawLine(curCenter, armEnd, {Color_Red}, 2);
			rs->DrawLine(curCenter, PixelCenter(pixelSize, newPos.x, newPos.y), {Color_Turquoise}, 2);
			if (appData->drawDebug) { HighlightPixel(rs, pixelSize, newPos, {Color_Purple}); }
			curPos = newPos;
			
			if (foundStart)
			{
				break;
			}
		}
	}
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
