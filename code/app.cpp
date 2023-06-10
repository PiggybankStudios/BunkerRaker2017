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

#define COLOAR_ARRAY_WIDTH    300
#define COLOAR_ARRAY_HEIGHT   300

#define FIXED_META_BALL_POS_X 150
#define FIXED_META_BALL_POS_Y 150
#define FIXED_META_BALL_SIZE  15
#define MOUSE_META_BALL_SIZE  30

#define TANGENT_CHECK_RADIUS  10
#define STEP_SIZE             10
#define MAX_DROP              5
#define MAX_DIVISION_BACKSTEP 5
#define MAX_VERTICES          200

#define RAKE_WIDTH            30

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
#include "tempMemory.h"
#include "tempMemory.cpp"
#include "tempList.h"

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

void SetColorInArray(u32* colorArray, v2i arraySize, i32 xPos, i32 yPos, Color_t color)
{
	u32* colorPntr = &colorArray[yPos*arraySize.x + xPos];
	u8* redPntr    = &((u8*)colorPntr)[0];
	u8* greenPntr  = &((u8*)colorPntr)[1];
	u8* bluePntr   = &((u8*)colorPntr)[2];
	u8* alphaPntr  = &((u8*)colorPntr)[3];
	
	*redPntr   = color.r;
	*greenPntr = color.g;
	*bluePntr  = color.b;
	*alphaPntr = color.a;
}

Color_t GetColorInArrayAt(u32* colorArray, v2i arraySize, i32 xPos, i32 yPos)
{
	if (xPos < 0) return {Color_White};
	if (yPos < 0) return {Color_White};
	if (xPos >= arraySize.x) return {Color_White};
	if (yPos >= arraySize.y) return {Color_White};
	
	u8* colorPntr = (u8*)(&colorArray[yPos*arraySize.x + xPos]);
	
	return NewColor(
		colorPntr[0],
		colorPntr[1],
		colorPntr[2],
		colorPntr[3]
	);
}

void FillColorArray(AppData_t* appData, v2i mousePos)
{
	#define NUM_BLOBS 3
	v2 blobCenter[NUM_BLOBS];
	blobCenter[0] = NewVec2((r32)appData->metaBallPos.x, (r32)appData->metaBallPos.y);
	blobCenter[1] = NewVec2(FIXED_META_BALL_POS_X, FIXED_META_BALL_POS_Y);
	blobCenter[2] = NewVec2((r32)mousePos.x, (r32)mousePos.y);
	r32 blobRadius[NUM_BLOBS];
	blobRadius[0] = appData->metaBallSize;
	blobRadius[1] = FIXED_META_BALL_SIZE;
	blobRadius[2] = MOUSE_META_BALL_SIZE;
	
	v2 center = NewVec2(appData->colorArraySize.x/2.0f, appData->colorArraySize.y/2.0f);
	for (i32 yPos = 0; yPos < appData->colorArraySize.y; yPos++)
	{
		for (i32 xPos = 0; xPos < appData->colorArraySize.x; xPos++)
		{
			Color_t pixelColor = { Color_Black };
			
			r32 value = 0;
			for (i32 bIndex = 0; bIndex < NUM_BLOBS; bIndex++)
			{
				r32 distance = Vec2Length(NewVec2((r32)xPos, (r32)yPos) - blobCenter[bIndex]);
				r32 addValue = blobRadius[bIndex] / distance;
				value += addValue;
			}
			
			if (value >= 1.0f)
			{
				pixelColor = NewColor(216, 168, 93, 255);
			}
			
			SetColorInArray(appData->colorArray, appData->colorArraySize, xPos, yPos, pixelColor);
			
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
	// pixels[0][0] = GetColorInArrayAt(appData->colorArray, appData->colorArraySize, xPos-1, yPos-1);
	pixels[1][0] = GetColorInArrayAt(appData->colorArray, appData->colorArraySize, xPos,   yPos-1);
	// pixels[2][0] = GetColorInArrayAt(appData->colorArray, appData->colorArraySize, xPos+1, yPos-1);
	pixels[0][1] = GetColorInArrayAt(appData->colorArray, appData->colorArraySize, xPos-1, yPos);
	pixels[1][1] = GetColorInArrayAt(appData->colorArray, appData->colorArraySize, xPos,   yPos);
	pixels[2][1] = GetColorInArrayAt(appData->colorArray, appData->colorArraySize, xPos+1, yPos);
	// pixels[0][2] = GetColorInArrayAt(appData->colorArray, appData->colorArraySize, xPos-1, yPos+1);
	pixels[1][2] = GetColorInArrayAt(appData->colorArray, appData->colorArraySize, xPos,   yPos+1);
	// pixels[2][2] = GetColorInArrayAt(appData->colorArray, appData->colorArraySize, xPos+1, yPos+1);
	
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
				pixels[0][0] = GetColorInArrayAt(appData->colorArray, appData->colorArraySize, xPos-1, yPos-1);
				pixels[1][0] = GetColorInArrayAt(appData->colorArray, appData->colorArraySize, xPos,   yPos-1);
				pixels[2][0] = GetColorInArrayAt(appData->colorArray, appData->colorArraySize, xPos+1, yPos-1);
				pixels[0][1] = GetColorInArrayAt(appData->colorArray, appData->colorArraySize, xPos-1, yPos);
				pixels[1][1] = GetColorInArrayAt(appData->colorArray, appData->colorArraySize, xPos,   yPos);
				pixels[2][1] = GetColorInArrayAt(appData->colorArray, appData->colorArraySize, xPos+1, yPos);
				pixels[0][2] = GetColorInArrayAt(appData->colorArray, appData->colorArraySize, xPos-1, yPos+1);
				pixels[1][2] = GetColorInArrayAt(appData->colorArray, appData->colorArraySize, xPos,   yPos+1);
				pixels[2][2] = GetColorInArrayAt(appData->colorArray, appData->colorArraySize, xPos+1, yPos+1);
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

u32 FindOutlineVertices(AppData_t* appData, v2i startPos, i32 maxVertices, r32 pixelSize, v2* verticesOut, RenderState_t* rs = nullptr)
{
	u32 result = 0;
	
	v2 startPosCenter = PixelCenter(pixelSize, startPos.x, startPos.y);
	v2 startPosTangent = Vec2_Zero;
	if (rs != nullptr) { HighlightPixel(rs, pixelSize, startPos, {Color_Turquoise}); }
	
	v2i curPos = startPos;
	for (i32 cIndex = 0; cIndex < maxVertices; cIndex++)
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
			if (appData->drawDebug && rs != nullptr) { rs->DrawLine(curCenter, armEnd, MyColors[divisionCount], 2); }
			
			armEndPixPos = NewVec2i((i32)(armEnd.x / pixelSize), (i32)(armEnd.y / pixelSize));
			perpDir = NewVec2(tangent.y, -tangent.x);
			if (!IsPixelFilled(GetColorInArrayAt(appData->colorArray, appData->colorArraySize, armEndPixPos.x, armEndPixPos.y)))
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
		
		if (appData->drawDebug && rs != nullptr)
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
		
		v2 newVertex = PixelCenter(pixelSize, newPos.x, newPos.y);
		if (verticesOut != nullptr)
		{
			verticesOut[result] = newVertex;
		}
		result++;
		
		if (rs != nullptr)
		{
			// rs->DrawLine(curCenter, armEnd, {Color_Red}, 2);
			rs->DrawLine(curCenter, newVertex, {Color_Turquoise}, 2);
			if (appData->drawDebug) { HighlightPixel(rs, pixelSize, newPos, {Color_Purple}); }
		}
		curPos = newPos;
		
		if (foundStart)
		{
			break;
		}
	}
	
	return result;
}

v2 ClosestPointOnLine(v2 point, v2 l1, v2 l2)
{
	r32 lineLength = Vec2Length(l2 - l1);
	v2 lineVector = (l2 - l1) / lineLength;
	
	r32 dot = Vec2Dot(point - l1, lineVector);
	if (dot <= 0)
	{
		return l1;
	}
	else if (dot >= lineLength)
	{
		return l2;
	}
	else
	{
		return l1 + (dot * lineVector);
	}
}

v2 FindClosestSide(AppData_t* appData, v2 position, r32* distanceOut, i32* sideIndexOut)
{
	v2 result = Vec2_Zero;
	
	r32 minDistance = 0;
	for (i32 vIndex = 0; vIndex < (i32)appData->numVertices; vIndex++)
	{
		v2 currentVertex = appData->vertices[vIndex];
		v2 nextVertex    = appData->vertices[(vIndex+1) % appData->numVertices];
		v2 closest = ClosestPointOnLine(position, currentVertex, nextVertex);
		r32 closestDist = Vec2Length(closest - position);
		
		if (vIndex == 0 || closestDist < minDistance)
		{
			minDistance = closestDist;
			result = closest;
			*sideIndexOut = vIndex;
			*distanceOut = minDistance;
		}
	}
	
	return result;
}

void GenerateBevelOverlay(AppData_t* appData, r32 pixelSize, r32 bevelSize, r32 bevelAmount, bool additive = false)
{
	v2 lightDir = Vec2Normalize(NewVec2(-1, 3));
	
	for (i32 yPos = 0; yPos < appData->colorArraySize.y; yPos++)
	{
		for (i32 xPos = 0; xPos < appData->colorArraySize.x; xPos++)
		{
			if (additive == false)
			{
				SetColorInArray(appData->bevelOverlayColors, appData->colorArraySize, xPos, yPos, {Color_TransparentBlack});
			}
			
			if (appData->vertices != nullptr &&
				IsPixelFilled(GetColorInArrayAt(appData->colorArray, appData->colorArraySize, xPos, yPos)))
			{
				v2 pixelPos = PixelCenter(pixelSize, xPos, yPos);
				r32 sideDistance;
				i32 sideIndex;
				v2 sidePos = FindClosestSide(appData, pixelPos, &sideDistance, &sideIndex);
				v2 sideV1 = appData->vertices[sideIndex];
				v2 sideV2 = appData->vertices[(sideIndex+1) % appData->numVertices];
				
				if (sideDistance < bevelSize)
				{
					r32 dot = Vec2Dot(Vec2Normalize(sidePos - pixelPos), lightDir);
					
					r32 colorShift = Clamp32(dot, -1.0f, 1.0f) * bevelAmount;
					
					Color_t pixelColor = {Color_TransparentBlack};
					pixelColor.r = (colorShift >= 0 ? 255 : 0);
					pixelColor.g = (colorShift >= 0 ? 255 : 0);
					pixelColor.b = (colorShift >= 0 ? 255 : 0);
					pixelColor.a = (u8)Abs32(colorShift * 255);
					if (additive)
					{
						Color_t oldPixelColor = GetColorInArrayAt(appData->bevelOverlayColors, appData->colorArraySize, xPos, yPos);
						pixelColor.r = min(255, oldPixelColor.r + pixelColor.r);
						pixelColor.g = min(255, oldPixelColor.g + pixelColor.g);
						pixelColor.b = min(255, oldPixelColor.b + pixelColor.b);
						pixelColor.a = min(255, oldPixelColor.a + pixelColor.a);
					}
					SetColorInArray(appData->bevelOverlayColors, appData->colorArraySize, xPos, yPos, pixelColor);
				}
			}
		}
	}
	appData->bevelOverlayTexture = CreateTexture((u8*)appData->bevelOverlayColors, appData->colorArraySize.x, appData->colorArraySize.y, true, false);
}

bool SliceContains(const Slice_t* slice, r32 value, Span_t** spanPntrOut = nullptr)
{
	Assert(slice != nullptr);
	
	for (u32 sIndex = 0; sIndex < slice->numSpans; sIndex++)
	{
		Span_t* spanPntr = &slice->spans[sIndex];
		if (spanPntr->start <= value && spanPntr->end >= value)
		{
			return true;
		}
	}
	
	return false;
}

Slice_t CombineSlices(const Slice_t* slice1, const Slice_t* slice2)
{
	Assert(slice1 != nullptr);
	Assert(slice2 != nullptr);
	
	Slice_t result = {};
	
	#if DEBUG
	//Ensure that the spans are in order
	{
		Span_t* lastSpan = nullptr;
		for (u32 sIndex = 0; sIndex < slice1->numSpans; sIndex++)
		{
			Span_t* span = &slice1->spans[sIndex];
			if (span->start > span->end)
			{
				Assert(false); //Span starts at a higher value than it ends
			}
			if (lastSpan != nullptr && lastSpan->end > span->start)
			{
				Assert(false); //Span is out of order, ends before the last span ends
			}
			lastSpan = span;
		}
		lastSpan = nullptr;
		for (u32 sIndex = 0; sIndex < slice2->numSpans; sIndex++)
		{
			Span_t* span = &slice2->spans[sIndex];
			if (span->start > span->end)
			{
				Assert(false); //Span starts at a higher value than it ends
			}
			if (lastSpan != nullptr && lastSpan->end > span->start)
			{
				Assert(false); //Span is out of order, ends before the last span ends
			}
			lastSpan = span;
		}
	}
	#endif
	
	TempPushMark();
	
	TempList_t newSpansList;
	TempListInit(&newSpansList, sizeof(Span_t));
	
	bool inSlice1 = false, inSlice2 = false;
	u32 index1 = 0, index2 = 0;
	bool spanStarted = false;
	r32 spanStart = 0;
	while (index1 < slice1->numSpans*2 || index2 < slice2->numSpans*2)
	{
		Assert(index1 <= slice1->numSpans*2);
		Assert(index2 <= slice2->numSpans*2);
		
		r32* next1 = (index1%2 == 0) ? &slice1->spans[index1/2].start : &slice1->spans[index1/2].end;
		r32* next2 = (index2%2 == 0) ? &slice2->spans[index2/2].start : &slice2->spans[index2/2].end;
		
		if (index1 < slice1->numSpans*2 && (index2 >= slice2->numSpans*2 || *next1 <= *next2))
		{
			inSlice1 = (index1%2 == 0);
			
			if (inSlice1 && inSlice2)
			{
				spanStarted = true;
				spanStart = *next1;
			}
			else if (spanStarted)
			{
				spanStarted = false;
				Span_t newSpan = {};
				newSpan.start = spanStart;
				newSpan.end = *next1;
				TempListAdd(&newSpansList, Span_t, &newSpan);
			}
			
			index1++;
		}
		else
		{
			inSlice2 = (index2%2 == 0);
			
			if (inSlice1 && inSlice2)
			{
				spanStarted = true;
				spanStart = *next2;
			}
			else if (spanStarted)
			{
				spanStarted = false;
				Span_t newSpan = {};
				newSpan.start = spanStart;
				newSpan.end = *next2;
				TempListAdd(&newSpansList, Span_t, &newSpan);
			}
			
			index2++;
		}
	}
	
	result.numSpans = newSpansList.numItems;
	
	if (result.numSpans > 0)
	{
		Span_t* spansTemp = TempListToArray(&newSpansList, Span_t, &GL_AppData->mainHeap);
		
		TempPopMark();
		
		result.spans = TempArray(Span_t, result.numSpans);
		memcpy(result.spans, spansTemp, sizeof(Span_t) * result.numSpans);
		ArenaPop(&GL_AppData->mainHeap, spansTemp);
		
		return result;
	}
	else
	{
		TempPopMark();
		return result;
	}
	
}

u32 GetSpans(AppData_t* appData, v2 sliceStart, v2 sliceEnd, Span_t** spansOut, RenderState_t* rs = nullptr)
{
	u32 numIntersections = 0;
	r32* intersectTimes = nullptr;
	u32* intersectSides = nullptr;
	
	TempPushMark();
	
	//Find intersection points
	{
		TempList_t intersectTimesList, intersectSidesList;
		TempListInit(&intersectTimesList, sizeof(r32));
		TempListInit(&intersectSidesList, sizeof(u32));
		
		for (u32 sIndex = 0; sIndex < appData->numVertices; sIndex++)
		{
			v2 vert1 = appData->vertices[(sIndex+0) % appData->numVertices];
			v2 vert2 = appData->vertices[(sIndex+1) % appData->numVertices];
			
			v2 newIntersect = Vec2_Zero;
			r32 newIntersectTime = 0;
			if (LineVsLine(vert1, vert2, sliceStart, sliceEnd, &newIntersect, &newIntersectTime) == 1)
			{
				u32 itemIndex = 0;
				TempListItem_t* currentItem = intersectTimesList.firstItem;
				while (currentItem != nullptr && itemIndex < intersectTimesList.numItems)
				{
					r32* valuePntr = (r32*)&currentItem->itemStart;
					if (*valuePntr > newIntersectTime) { break; }
					itemIndex++;
					currentItem = currentItem->nextItem;
				}
				TempListAddAt(&intersectTimesList, r32, itemIndex, &newIntersectTime);
				TempListAddAt(&intersectSidesList, u32, itemIndex, &sIndex);
				
				// rs->DrawLine(vert1, vert2, {Color_Red});
				
			}
		}
		
		//Copy info into mainHeap temporarily
		numIntersections = intersectTimesList.numItems;
		r32* intersectTimesTemp = TempListToArray(&intersectTimesList, r32, &appData->mainHeap);
		u32* intersectSidesTemp = TempListToArray(&intersectSidesList, u32, &appData->mainHeap);
		
		TempPopMark(); //Reset back to original mark pos
		TempPushMark();
		
		if (numIntersections > 0)
		{
			//Move it back onto the TempArena and pop from mainHeap
			intersectTimes = TempArray(r32, numIntersections);
			memcpy(intersectTimes, intersectTimesTemp, sizeof(r32) * numIntersections);
			ArenaPop(&appData->mainHeap, intersectTimesTemp);
			
			intersectSides = TempArray(u32, numIntersections);
			memcpy(intersectSides, intersectSidesTemp, sizeof(u32) * numIntersections);
			ArenaPop(&appData->mainHeap, intersectSidesTemp);
		}
		else
		{
			Assert(intersectTimesTemp == nullptr);
			Assert(intersectSidesTemp == nullptr);
			*spansOut = nullptr;
			TempPopMark();
			return 0;
		}
	}
	
	//Turn the intersections into spans
	{	
		TempList_t spansList;
		TempListInit(&spansList, sizeof(Span_t));
		
		v2 textPos = NewVec2(10, 10 + appData->testFont.maxExtendUp);
		v2 lineDir = Vec2Normalize(sliceEnd - sliceStart);
		bool insideBunker = false;
		v2 enterPos = Vec2_Zero;
		r32 enterTime = 0;
		for (u32 iIndex = 0; iIndex < numIntersections; iIndex++)
		{
			r32 intTime = intersectTimes[iIndex];
			u32 intSide = intersectSides[iIndex];
			v2 intersectPos = sliceStart + (sliceEnd-sliceStart) * intTime;
			v2 vert1 = appData->vertices[intSide];
			v2 vert2 = appData->vertices[(intSide+1) % appData->numVertices];
			v2 sideDir = Vec2Normalize(vert2 - vert1);
			v2 sideTangent = NewVec2(-sideDir.y, sideDir.x);
			r32 tangentDot = Vec2Dot(lineDir, sideTangent);
			
			if (tangentDot > 0 && insideBunker == false)
			{
				enterPos = intersectPos;
				enterTime = intTime;
				insideBunker = true;
				
				if (rs != nullptr)
				{
					rs->DrawCircle(intersectPos, 2, {Color_Green});
					// rs->PrintString(textPos, {Color_White}, 1.0f, "Intersect %u: Entering Side %u", iIndex, intSide); textPos.y += appData->testFont.lineHeight;
				}
			}
			else if (tangentDot < 0 && insideBunker == true)
			{
				insideBunker = false;
				
				Span_t newSpan = {};
				newSpan.start = enterTime;
				newSpan.end = intTime;
				if (newSpan.start > newSpan.end) //Make sure that start is less than end
				{
					newSpan.start = intTime;
					newSpan.end = enterTime;
				}
				TempListAdd(&spansList, Span_t, &newSpan);
				
				if (rs != nullptr)
				{
					rs->DrawLine(enterPos, intersectPos, {Color_White});
					rs->DrawCircle(intersectPos, 2, {Color_Red});
					// rs->PrintString(textPos, {Color_White}, 1.0f, "Intersect %u: Exiting Side %u", iIndex, intSide); textPos.y += appData->testFont.lineHeight;
				}
			}
			else
			{
				if (rs != nullptr)
				{
					rs->DrawCircle(intersectPos, 5, {Color_Purple});
					// rs->PrintString(textPos, {Color_White}, 1.0f, "Intersect %u: Crossing Side %u", iIndex, intSide); textPos.y += appData->testFont.lineHeight;
				}
			}
			
			// if (rs != nullptr) { rs->DrawLine(vert1, vert2, {Color_Red}); }
		}
		
		//Move into mainHeap temporarily
		u32 numSpans = spansList.numItems;
		Span_t* spansTemp = TempListToArray(&spansList, Span_t, &appData->mainHeap);
		
		TempPopMark(); //Pop the intersections arrays and spansList
		
		if (numSpans > 0)
		{
			//Copy back into temp memory so it can be returned to caller on the TempArena
			*spansOut = TempArray(Span_t, numSpans);
			memcpy(*spansOut, spansTemp, sizeof(Span_t) * numSpans);
			ArenaPop(&appData->mainHeap, spansTemp);
			
			return numSpans;
		}
		else
		{
			Assert(spansTemp == nullptr);
			*spansOut = nullptr;
			return 0;
		}
		
	}
}

u32 GetBunkerSlices(AppData_t* appData, v2 sliceVec, r32 rakeWidth, Slice_t** slicesOut, v2* firstSliceStart, v2* firstSliceEnd, RenderState_t* rs = nullptr)
{
	Assert(slicesOut != nullptr);
	
	v2 sliceTangent = NewVec2(-sliceVec.y, sliceVec.x);
	
	Span_t extent = {};
	Span_t extentTangent = {};
	for (u32 vIndex = 0; vIndex < appData->numVertices; vIndex++)
	{
		v2 vert = appData->vertices[vIndex];
		r32 dot = Vec2Dot(vert, sliceVec);
		r32 dotTangent = Vec2Dot(vert, sliceTangent);
		
		if (vIndex == 0 || dot < extent.start) { extent.start = dot; }
		if (vIndex == 0 || dot > extent.end)   { extent.end = dot; }
		if (vIndex == 0 || dotTangent < extentTangent.start) { extentTangent.start = dotTangent; }
		if (vIndex == 0 || dotTangent > extentTangent.end) { extentTangent.end = dotTangent; }
	}
	
	extent.start -= 10;
	extent.end += 10;
	
	v2 vert1 = (extent.start * sliceVec) + (extentTangent.start * sliceTangent);
	v2 vert2 = (extent.end * sliceVec)   + (extentTangent.start * sliceTangent);
	v2 vert3 = (extent.end * sliceVec)   + (extentTangent.end * sliceTangent);
	v2 vert4 = (extent.start * sliceVec) + (extentTangent.end * sliceTangent);
	
	if (rs != nullptr && appData->drawDebug)
	{
		// rs->DrawLine(vert1, vert2, {Color_Red});
		rs->DrawLine(vert2, vert3, {Color_Red});
		// rs->DrawLine(vert3, vert4, {Color_Red});
		rs->DrawLine(vert4, vert1, {Color_Red});
	}
	
	TempPushMark();
	
	*firstSliceStart = vert1 + sliceTangent * rakeWidth/2;
	*firstSliceEnd   = vert2 + sliceTangent * rakeWidth/2;
	
	TempList_t slicesList;
	TempListInit(&slicesList, sizeof(Slice_t));
	for (r32 addY = rakeWidth/2; addY < extentTangent.end - extentTangent.start; addY += rakeWidth)
	{
		v2 sliceStart = vert1 + sliceTangent * addY;
		v2 sliceEnd = vert2 + sliceTangent * addY;
		
		Slice_t newSlice = {};
		newSlice.numSpans = GetSpans(appData, sliceStart, sliceEnd, &newSlice.spans, appData->drawDebug ? rs : nullptr);
		TempListAdd(&slicesList, Slice_t, &newSlice);
		
		if (rs != nullptr && appData->drawDebug)
		{
			rs->DrawLine(sliceStart, sliceEnd, {Color_Red});
		}
	}
	
	u32 numSlices = slicesList.numItems;
	if (numSlices > 0)
	{
		Slice_t* slicesTemp = TempListToArray(&slicesList, Slice_t, &appData->mainHeap);
		//Move each span array into mainHeap
		for (u32 sIndex = 0; sIndex < numSlices; sIndex++)
		{
			Span_t* spansPntr = slicesTemp[sIndex].spans;
			Span_t* spansTemp = PushArray(&appData->mainHeap, Span_t, slicesTemp[sIndex].numSpans);
			memcpy(spansTemp, spansPntr, sizeof(Span_t) * slicesTemp[sIndex].numSpans);
			slicesTemp[sIndex].spans  = spansTemp;
		}
		
		TempPopMark();
		
		*slicesOut = TempArray(Slice_t, numSlices);
		//Move each span array back onto TempArena and pop off mainHeap
		for (u32 sIndex = 0; sIndex < numSlices; sIndex++)
		{
			Span_t* spansPntr = slicesTemp[sIndex].spans;
			Span_t* spansTemp = TempArray(Span_t, slicesTemp[sIndex].numSpans);
			memcpy(spansTemp, spansPntr, sizeof(Span_t) * slicesTemp[sIndex].numSpans);
			slicesTemp[sIndex].spans  = spansTemp;
			ArenaPop(&appData->mainHeap, spansPntr);
		}
		memcpy(*slicesOut, slicesTemp, sizeof(Slice_t) * numSlices);
		
		ArenaPop(&appData->mainHeap, slicesTemp);
		
		return numSlices;
	}
	else
	{
		TempPopMark();
		return 0;
	}
	
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
	
	InitializeMemoryArenaTemp(&appData->tempArena, AppMemory->transientPntr, AppMemory->transientSize, 16);
	TempArena = &appData->tempArena;
	TempPushMark();
	
	v2i screenSize = PlatformInfo->screenSize;
	r32 pixelSize = screenSize.x / (r32)appData->colorArrayTexture.width;
	
	InitializeRenderState(PlatformInfo, &appData->renderState);
	
	appData->simpleShader = LoadShader(
		"Resources/Shaders/simple-vertex.glsl",
		"Resources/Shaders/simple-fragment.glsl");
	
	appData->testTexture = LoadTexture("Resources/Sprites/close.png");
	appData->testFont = LoadFont("Resources/Fonts/consola.ttf", TEST_FONT_SIZE, 1024, 1024, ' ', 96);
	appData->sandTexture = LoadTexture("Resources/Textures/sand2.jpeg");
	
	appData->colorArraySize = NewVec2i(COLOAR_ARRAY_WIDTH, COLOAR_ARRAY_HEIGHT);
	appData->colorArray = PushArray(&appData->mainHeap, u32, appData->colorArraySize.x*appData->colorArraySize.y);
	appData->metaBallPos = NewVec2i(20, 20);
	appData->metaBallSize = 10;
	FillColorArray(appData, NewVec2i(FIXED_META_BALL_POS_X, FIXED_META_BALL_POS_Y));
	
	appData->bevelOverlayColors = PushArray(&appData->mainHeap, u32, appData->colorArraySize.x*appData->colorArraySize.y);
	GenerateBevelOverlay(appData, pixelSize, 10, 0.1f);
	
	TempPopMark();
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
	v2i screenSize = PlatformInfo->screenSize;
	r32 pixelSize = screenSize.x / (r32)appData->colorArrayTexture.width;
	
	// DEBUG_WriteLine("Re-creating color array");
	// DestroyTexture(&appData->colorArrayTexture);
	// FillColorArray(appData, NewVec2i(FIXED_META_BALL_POS_X, FIXED_META_BALL_POS_Y));
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
	TempArena = &appData->tempArena;
	TempPushMark();
	
	RenderState_t* rs = &appData->renderState;
	v2i screenSize = PlatformInfo->screenSize;
	r32 pixelSize = screenSize.x / (r32)appData->colorArrayTexture.width;
	v2 mousePos = AppInput->mousePos;
	v2i mPixPos = NewVec2i((i32)(mousePos.x / pixelSize), (i32)(mousePos.y / pixelSize));
	v2 mPixCenter = NewVec2(mPixPos.x*pixelSize + pixelSize/2, mPixPos.y*pixelSize + pixelSize/2);
	
	// +================================+
	// |        Recreate Texture        |
	// +================================+
	if (ButtonDown(Button_R))
	{
		DEBUG_WriteLine("Re-creating color array");
		DestroyTexture(&appData->colorArrayTexture);
		FillColorArray(appData, mPixPos);
	}
	
	if (ButtonPressed(MouseButton_Right))
	{
		appData->testStartPos = mousePos;
	}
	
	if (ButtonPressed(Button_D))
	{
		appData->drawDebug = !appData->drawDebug;
	}
	
	if (ButtonDown(Button_Left) ||
		ButtonDown(Button_Right) ||
		ButtonDown(Button_Up) ||
		ButtonDown(Button_Down))
	{
		if (ButtonDown(Button_Left))  { appData->metaBallPos.x--; }
		if (ButtonDown(Button_Right)) { appData->metaBallPos.x++; }
		if (ButtonDown(Button_Up))    { appData->metaBallPos.y--; }
		if (ButtonDown(Button_Down))  { appData->metaBallPos.y++; }
		
		// DEBUG_WriteLine("Re-creating color array");
		// DestroyTexture(&appData->colorArrayTexture);
		// FillColorArray(appData, mPixPos);
	}
	
	if (AppInput->scrollDelta.y != 0)
	{
		appData->metaBallSize *= 1.0f + (0.1f * AppInput->scrollDelta.y);
		
		// DEBUG_WriteLine("Re-creating color array");
		// DestroyTexture(&appData->colorArrayTexture);
		// FillColorArray(appData, mPixPos);
	}
	
	if (ButtonPressed(Button_B))
	{
		DEBUG_WriteLine("Beveling the bunker");
		GenerateBevelOverlay(appData, pixelSize, 10 * pixelSize, 0.05f, false);
		GenerateBevelOverlay(appData, pixelSize, 5 * pixelSize, 0.15f, true);
	}
	
	// +================================+
	// |          Find Outline          |
	// +================================+
	if (ButtonDown(Button_Space) && IsPixelFilled(GetColorInArrayAt(appData->colorArray, appData->colorArraySize, appData->testStartPixel.x, appData->testStartPixel.y)))
	{
		DEBUG_WriteLine("Finding bunker outline...");
		
		if (appData->vertices != nullptr)
		{
			ArenaPop(&appData->mainHeap, appData->vertices);
			appData->vertices = nullptr;
			appData->numVertices = 0;
		}
		
		appData->numVertices = FindOutlineVertices(appData, appData->testStartPixel, MAX_VERTICES, pixelSize, nullptr);
		DEBUG_PrintLine("Found %u vertices", appData->numVertices);
		
		appData->vertices = PushArray(&appData->mainHeap, v2, appData->numVertices);
		FindOutlineVertices(appData, appData->testStartPixel, MAX_VERTICES, pixelSize, appData->vertices);
		
		DEBUG_WriteLine("Done!");
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
	rs->BindTexture(&appData->bevelOverlayTexture);
	rs->DrawTexturedRec(NewRectangle(0, 0, (r32)screenSize.x, (r32)screenSize.y), {Color_White});
	
	// +================================+
	// |     Test Slice Calculation     |
	// +================================+
	if (appData->testStartPos.x != mousePos.x || appData->testStartPos.y != mousePos.y)
	{
		rs->DrawLine(appData->testStartPos, mousePos, {Color_Yellow});
		
		if (true && appData->vertices != nullptr)
		{
			TempPushMark();
			
			Span_t* spans = nullptr;
			u32 numSpans = GetSpans(appData, appData->testStartPos, mousePos, &spans, rs);
			
			if (numSpans > 0)
			{
				// DEBUG_PrintLine("%u Spans", numSpans);
			}
			
			v2 rakeDirection = Vec2Normalize(mousePos - appData->testStartPos);
			v2 firstSliceStart, firstSliceEnd;
			Slice_t* slices;
			u32 numSlices = GetBunkerSlices(appData, rakeDirection, RAKE_WIDTH, &slices, &firstSliceStart, &firstSliceEnd, rs);
			
			// v2 textPos d= NewVec2(10, 10 + appData->testFont.maxExtendUp);
			v2 rakeDirTangent = NewVec2(-rakeDirection.y, rakeDirection.x);
			for (u32 sIndex = 1; sIndex < numSlices; sIndex++)
			{
				Slice_t* lastSlice = &slices[sIndex-1];
				Slice_t* slice = &slices[sIndex];
				
				v2 leftLineStart   = firstSliceStart + rakeDirTangent * (RAKE_WIDTH * (r32)(sIndex-1));
				v2 leftLineEnd     = firstSliceEnd   + rakeDirTangent * (RAKE_WIDTH * (r32)(sIndex-1));
				v2 rightLineStart  = firstSliceStart + rakeDirTangent * (RAKE_WIDTH * (r32)sIndex);
				v2 rightLineEnd    = firstSliceEnd   + rakeDirTangent * (RAKE_WIDTH * (r32)sIndex);
				v2 centerLineStart = firstSliceStart + rakeDirTangent * (RAKE_WIDTH * (r32)(sIndex-1) + RAKE_WIDTH/2.0f);
				v2 centerLineEnd   = firstSliceEnd   + rakeDirTangent * (RAKE_WIDTH * (r32)(sIndex-1) + RAKE_WIDTH/2.0f);
				
				// rs->DrawLine(leftLineStart, leftLineEnd, {Color_Red});
				// rs->DrawLine(rightLineStart, rightLineEnd, {Color_Blue});
				
				Slice_t combined = CombineSlices(lastSlice, slice);
				// rs->PrintString(textPos, {Color_White}, 1.0f, "Slice[%u] %u Spans:", sIndex, combined.numSpans); textPos.y += appData->testFont.lineHeight;
				for (u32 spanIndex = 0; spanIndex < combined.numSpans; spanIndex++)
				{
					Span_t* spanPntr = &combined.spans[spanIndex];
					
					// rs->PrintString(textPos, {Color_White}, 1.0f, "\tSpan[%u] %.2f - %.2f", spanIndex, spanPntr->start, spanPntr->end); textPos.y += appData->testFont.lineHeight;
					
					v2 vert1 = leftLineStart  + (leftLineEnd  - leftLineStart)  * spanPntr->start;
					v2 vert2 = leftLineStart  + (leftLineEnd  - leftLineStart)  * spanPntr->end;
					v2 vert3 = rightLineStart + (rightLineEnd - rightLineStart) * spanPntr->end;
					v2 vert4 = rightLineStart + (rightLineEnd - rightLineStart) * spanPntr->start;
					
					// rs->DrawLine(vert1, vert2, {Color_Red});
					// rs->DrawLine(vert2, vert3, {Color_Red});
					// rs->DrawLine(vert3, vert4, {Color_Red});
					// rs->DrawLine(vert4, vert1, {Color_Red});
					
					v2 rakeVert1 = centerLineStart + (centerLineEnd - centerLineStart) * spanPntr->start;
					v2 rakeVert2 = centerLineStart + (centerLineEnd - centerLineStart) * spanPntr->end;
					Color_t rakeColor = (sIndex%2 == 0) ? NewColor(194, 145, 70, 255) : NewColor(238, 193, 122, 255);
					rs->DrawLine(rakeVert1, rakeVert2, rakeColor, RAKE_WIDTH);
				}
			}
			
			TempPopMark();
		}
	}
	
	// +==================================+
	// |      Draw Meta Balls Debug       |
	// +==================================+
	if (appData->drawDebug)
	{
		v2 blobCenter[NUM_BLOBS];
		blobCenter[0] = NewVec2((r32)appData->metaBallPos.x, (r32)appData->metaBallPos.y);
		blobCenter[1] = NewVec2(FIXED_META_BALL_POS_Y, FIXED_META_BALL_POS_X);
		blobCenter[2] = NewVec2((r32)mousePos.x / pixelSize, (r32)mousePos.y / pixelSize);
		r32 blobRadius[NUM_BLOBS];
		blobRadius[0] = appData->metaBallSize;
		blobRadius[1] = FIXED_META_BALL_SIZE;
		blobRadius[2] = MOUSE_META_BALL_SIZE;
		
		for (u32 bIndex = 0; bIndex < NUM_BLOBS; bIndex++)
		{
			rs->DrawCircle(blobCenter[bIndex] * pixelSize, blobRadius[bIndex] * pixelSize, {Color_HalfTransparent});
		}
	}
	
	// +================================+
	// |   Tangent Vector Calculation   |
	// +================================+
	if (ButtonDown(Button_T))
	{
		rs->PrintString(NewVec2(0, 16 + appData->testFont.maxExtendUp), {Color_White}, 1.0f, "Grid Pos: (%d, %d)", mPixPos.x, mPixPos.y);
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
	rs->DrawCircle(appData->testStartPos, 2, {Color_Red});
	if (appData->testStartPos.x != mousePos.x || appData->testStartPos.y != mousePos.y)
	{
		v2i raycastEnd;
		if (Raycast(appData, appData->testStartPos, mousePos - appData->testStartPos, pixelSize, &raycastEnd, appData->drawDebug ? rs : nullptr))
		{
			rs->DrawLine(appData->testStartPos, PixelCenter(pixelSize, raycastEnd.x, raycastEnd.y), {Color_Turquoise}, 1);
			if (ButtonDown(MouseButton_Left))
			{
				appData->testStartPixel = raycastEnd;
			}
		}
		
		if (appData->drawDebug)
		{
			rs->DrawLine(appData->testStartPos, appData->testStartPos + Vec2Normalize(mousePos - appData->testStartPos) * 1000, {Color_Purple});
		}
	}
	
	HighlightPixel(rs, pixelSize, appData->testStartPixel, {Color_Turquoise});
	
	// +==================================+
	// |          Draw Vertices           |
	// +==================================+
	if (appData->drawDebug && appData->vertices != nullptr)
	{
		for (u32 vIndex = 0; vIndex < appData->numVertices; vIndex++)
		{
			v2 currentVertex = appData->vertices[vIndex];
			v2 nextVertex    = appData->vertices[(vIndex+1) % appData->numVertices];
			
			rs->DrawLine(currentVertex, nextVertex, {Color_Turquoise}, 1);
		}
	}
	
	// +================================+
	// | Draw Debug FindOutlineVertices |
	// +================================+
	if (ButtonDown(Button_Space))
	{
		FindOutlineVertices(appData, appData->testStartPixel, MAX_VERTICES, pixelSize, nullptr, rs);
	}
	
	// +================================+
	// |       Test Closest Point       |
	// +================================+
	if (ButtonDown(Button_C))
	{
		i32 closestSideIndex;
		r32 closestDistance;
		v2 closestPoint = FindClosestSide(appData, mousePos, &closestDistance, &closestSideIndex);
		v2 vert1 = appData->vertices[closestSideIndex];
		v2 vert2 = appData->vertices[(closestSideIndex+1) % appData->numVertices];
		
		rs->DrawLine(mousePos, closestPoint, {Color_Red}, 1);
		rs->DrawLine(vert1, vert2, {Color_Red}, 1);
		rs->DrawCircle(closestPoint, 2, {Color_Blue});
	}
	
	// rs->PrintString(NewVec2(0, appData->testFont.maxExtendUp), {Color_White}, 1.0f, "Num Vertices: %u", appData->numVertices);
	// rs->PrintString(NewVec2(0, appData->testFont.maxExtendUp), {Color_White}, 1.0f, "Ball Pos: (%d, %d)", appData->metaBallPos.x, appData->metaBallPos.y);
	
	// +================================+
	// |       Draw Controls Help       |
	// +================================+
	if (ButtonDown(Button_Tab))
	{
		rs->DrawButton(NewRectangle(10, 10, (r32)screenSize.x - 20, (r32)screenSize.y - 20), NewColor(0, 0, 0, 200), {Color_White}, 1);
		
		v2 textPos = NewVec2(20, 20 + appData->testFont.maxExtendUp);
		
		rs->DrawString("Left Click - Set first vertex position", textPos, {Color_White}); textPos.y += appData->testFont.lineHeight;
		rs->DrawString("Right Click - Set ray start pos", textPos, {Color_White}); textPos.y += appData->testFont.lineHeight;
		rs->DrawString("Space - Generate outline vertices from first position", textPos, {Color_White}); textPos.y += appData->testFont.lineHeight;
		rs->DrawString("R - Regenerate Bunker Texture", textPos, {Color_White}); textPos.y += appData->testFont.lineHeight;
		rs->DrawString("T - Test tangent calculation function", textPos, {Color_White}); textPos.y += appData->testFont.lineHeight;
		rs->DrawString("D - Toggle debug drawing", textPos, {Color_White}); textPos.y += appData->testFont.lineHeight;
		rs->DrawString("B - Generate bunker bevel texture", textPos, {Color_White}); textPos.y += appData->testFont.lineHeight;
		rs->DrawString("Arrow Keys - Move meta ball", textPos, {Color_White}); textPos.y += appData->testFont.lineHeight;
		rs->DrawString("Scroll Wheel - Scale meta ball", textPos, {Color_White}); textPos.y += appData->testFont.lineHeight;
		textPos.y += appData->testFont.lineHeight;
		u32 highWaterMark = ArenaGetHighWaterMark(TempArena);
		rs->PrintString(textPos, {Color_White}, 1.0f, "TempArena High: %u/%u (%.2f%%)", highWaterMark, TempArena->size, (r32)highWaterMark / (r32)TempArena->size); textPos.y += appData->testFont.lineHeight;
	}
	else
	{
		const char* infoStr = "Hold [Tab] to see controls...";
		v2 infoPos = NewVec2(10, screenSize.y - appData->testFont.maxExtendDown);
		rs->DrawString(infoStr, infoPos + Vec2_One, NewColor(0, 0, 0, 200));
		rs->DrawString(infoStr, infoPos, {Color_White});
	}
	
	TempPopMark();
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
