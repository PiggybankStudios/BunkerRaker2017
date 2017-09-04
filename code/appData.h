/*
File:   appData.h
Author: Taylor Robbins
Date:   08\26\2017
*/

#ifndef _APP_DATA_H
#define _APP_DATA_H

struct AppData_t
{
	MemoryArena_t mainHeap;
	MemoryArena_t tempArena;
	
	Texture_t testTexture;
	Font_t testFont;
	Shader_t simpleShader;
	RenderState_t renderState;
	
	v2i colorArraySize;
	u32* colorArray;
	Texture_t colorArrayTexture;
	u32* bevelOverlayColors;
	Texture_t bevelOverlayTexture;
	
	Texture_t sandTexture;
	
	v2 testStartPos;
	v2i testStartPixel;
	v2i metaBallPos;
	r32 metaBallSize;
	bool drawDebug;
	
	u32 numVertices;
	v2* vertices;
};

#endif // _APP_DATA_H
