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
	
	Texture_t testTexture;
	Font_t testFont;
	Shader_t simpleShader;
	RenderState_t renderState;
};

#endif // _APP_DATA_H