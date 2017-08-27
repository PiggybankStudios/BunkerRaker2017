/*
File:   appLoadingFunctions.c
Author: Taylor Robbins
Date:   08\26\2017
Description: 
	** Houses some useful functions for loading assets from disk 
*/

Texture_t CreateTexture(const u8* bitmapData, i32 width, i32 height, bool pixelated = false, bool repeat = true)
{
	Texture_t result = {};
	
	result.width = width;
	result.height = height;
	
	glGenTextures(1, &result.id);
	glBindTexture(GL_TEXTURE_2D, result.id);
	
	glTexImage2D(
		GL_TEXTURE_2D, 		//bound texture type
		0,					//image level
		GL_RGBA,			//internal format
		width,		        //image width
		width,		        //image height
		0,					//border
		GL_RGBA,			//format
		GL_UNSIGNED_BYTE,	//type
		bitmapData);		//data
	
	
	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, pixelated ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, pixelated ? GL_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	return result;
}

void DestroyTexture(Texture_t* texturePntr)
{
	glDeleteTextures(1, &texturePntr->id);
	texturePntr->id = 0;
	texturePntr->width = 0;
	texturePntr->height = 0;
}

VertexBuffer_t CreateVertexBuffer(const Vertex_t* vertices, u32 numVertices)
{
	VertexBuffer_t result = {};
	result.numVertices = numVertices;
	
	glGenBuffers(1, &result.id);
	glBindBuffer(GL_ARRAY_BUFFER, result.id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex_t) * numVertices, vertices, GL_STATIC_DRAW);
	
	return result;
}

FrameBuffer_t CreateFrameBuffer(const Texture_t* texture)
{
	FrameBuffer_t result = {};
	
	glGenFramebuffers(1, &result.id);
	glBindFramebuffer(GL_FRAMEBUFFER, result.id);
	
	glGenRenderbuffers(1, &result.depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, result.depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, texture->width, texture->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, result.depthBuffer);
	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture->id, 0);
	
	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, DrawBuffers);
	
	result.renderTexture = texture;
	
	return result;
}

Shader_t LoadShader(const char* vertShaderFileName, const char* fragShaderFileName)
{
	const PlatformInfo_t* PlatformInfo = Gl_PlatformInfo;
	Shader_t result = {};
	GLint compiled;
	int logLength;
	char* logBuffer;
	
	FileInfo_t vertexShaderFile = PlatformInfo->ReadEntireFilePntr(vertShaderFileName);
	FileInfo_t fragmentShaderFile = PlatformInfo->ReadEntireFilePntr(fragShaderFileName);
	
	result.vertId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(result.vertId, 1, (const char* const*)&vertexShaderFile.content, NULL);
	glCompileShader(result.vertId);
	
	glGetShaderiv(result.vertId, GL_COMPILE_STATUS, &compiled);
	glGetShaderiv(result.vertId, GL_INFO_LOG_LENGTH, &logLength);
	DEBUG_PrintLine("%s: Compiled %s : %d byte log",
		vertShaderFileName,
		compiled ? "Successfully" : "Unsuccessfully", logLength);
	if (logLength > 0)
	{
		logBuffer = (char*)malloc(logLength);
		glGetShaderInfoLog(result.vertId, logLength, NULL, logBuffer);
		DEBUG_PrintLine("Log: \"%s\"", logBuffer);
		free(logBuffer);
		Assert(false);
	}
	
	result.fragId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(result.fragId, 1, (const char* const*)&fragmentShaderFile.content, NULL);
	glCompileShader(result.fragId);
	
	glGetShaderiv(result.fragId, GL_COMPILE_STATUS, &compiled);
	glGetShaderiv(result.fragId, GL_INFO_LOG_LENGTH, &logLength);
	DEBUG_PrintLine("%s: Compiled %s : %d byte log",
		fragShaderFileName,
		compiled ? "Successfully" : "Unsuccessfully", logLength);
	if (logLength > 0)
	{
		logBuffer = (char*)malloc(logLength);
		glGetShaderInfoLog(result.fragId, logLength, NULL, logBuffer);
		DEBUG_PrintLine("Log: \"%s\"", logBuffer);
		free(logBuffer);
		Assert(false);
	}
	
	PlatformInfo->FreeFileMemoryPntr(&vertexShaderFile);
	PlatformInfo->FreeFileMemoryPntr(&fragmentShaderFile);
	
	result.programId = glCreateProgram();
	glAttachShader(result.programId, result.fragId);
	glAttachShader(result.programId, result.vertId);
	glLinkProgram(result.programId);
	
	glGetProgramiv(result.programId, GL_LINK_STATUS, &compiled);
	glGetProgramiv(result.programId, GL_INFO_LOG_LENGTH, &logLength);
	DEBUG_PrintLine("Shader: Linked %s : %d byte log",
		compiled ? "Successfully" : "Unsuccessfully", logLength);
	if (logLength > 0)
	{
		logBuffer = (char*)malloc(logLength);
		glGetProgramInfoLog(result.programId, logLength, NULL, logBuffer);
		DEBUG_PrintLine("Log: \"%s\"", logBuffer);
		free(logBuffer);
		Assert(false);
	}
	
	result.positionAttribLocation      = glGetAttribLocation(result.programId, "inPosition");
	result.colorAttribLocation         = glGetAttribLocation(result.programId, "inColor");
	result.texCoordAttribLocation      = glGetAttribLocation(result.programId, "inTexCoord");
	
	result.worldMatrixLocation         = glGetUniformLocation(result.programId, "WorldMatrix");
	result.viewMatrixLocation          = glGetUniformLocation(result.programId, "ViewMatrix");
	result.projectionMatrixLocation    = glGetUniformLocation(result.programId, "ProjectionMatrix");
	result.diffuseTextureLocation      = glGetUniformLocation(result.programId, "DiffuseTexture");
	result.alphaTextureLocation        = glGetUniformLocation(result.programId, "AlphaTexture");
	result.diffuseColorLocation        = glGetUniformLocation(result.programId, "DiffuseColor");
	result.secondaryColorLocation      = glGetUniformLocation(result.programId, "SecondaryColor");
	result.doGrayscaleGradientLocation = glGetUniformLocation(result.programId, "DoGrayscaleGradient");
	result.sourceRectangleLocation     = glGetUniformLocation(result.programId, "SourceRectangle");
	result.useAlphaTextureLocation     = glGetUniformLocation(result.programId, "UseAlphaTexture");
	result.textureSizeLocation         = glGetUniformLocation(result.programId, "TextureSize");
	
	glGenVertexArrays(1, &result.vertexArray);
	glBindVertexArray(result.vertexArray);
	glEnableVertexAttribArray(result.positionAttribLocation);
	glEnableVertexAttribArray(result.colorAttribLocation);
	glEnableVertexAttribArray(result.texCoordAttribLocation);
	
	return result;
}

Texture_t LoadTexture(const char* fileName, bool pixelated = false, bool repeat = true)
{
	const PlatformInfo_t* PlatformInfo = Gl_PlatformInfo;
	Texture_t result = {};
	
	FileInfo_t textureFile = PlatformInfo->ReadEntireFilePntr(fileName);
	
	i32 numChannels;
	i32 width, height;
	u8* imageData = stbi_load_from_memory(
		(u8*)textureFile.content, textureFile.size,
		&width, &height, &numChannels, 4);
	
	result = CreateTexture(imageData, width, height, pixelated, repeat);
	
	stbi_image_free(imageData);
	PlatformInfo->FreeFileMemoryPntr(&textureFile);
	
	return result;
}

Font_t LoadFont(const char* fileName, 
	r32 fontSize, i32 bitmapWidth, i32 bitmapHeight,
	u8 firstCharacter, u8 numCharacters)
{
	const PlatformInfo_t* PlatformInfo = Gl_PlatformInfo;
	Font_t result = {};
	
	FileInfo_t fontFile = PlatformInfo->ReadEntireFilePntr(fileName);
	
	result.numChars = numCharacters;
	result.firstChar = firstCharacter;
	result.fontSize = fontSize;
	
	u8* grayscaleData = (u8*)malloc(sizeof(u8) * bitmapWidth * bitmapHeight);
	stbtt_bakedchar* charInfos = (stbtt_bakedchar*)malloc(sizeof(stbtt_bakedchar) * numCharacters);
	
	int bakeResult = stbtt_BakeFontBitmap((u8*)fontFile.content, 
		0, fontSize,
		grayscaleData, bitmapWidth, bitmapHeight, 
		firstCharacter, numCharacters, charInfos);
	DEBUG_PrintLine("STB Bake Result: %d", bakeResult);
	
	for (u8 cIndex = 0; cIndex < numCharacters; cIndex++)
	{
		result.chars[cIndex].position = NewVec2i(
			charInfos[cIndex].x0, 
			charInfos[cIndex].y0);
		result.chars[cIndex].size = NewVec2i(
			charInfos[cIndex].x1 - charInfos[cIndex].x0, 
			charInfos[cIndex].y1 - charInfos[cIndex].y0);
		result.chars[cIndex].offset = NewVec2(
			charInfos[cIndex].xoff, 
			charInfos[cIndex].yoff);
		result.chars[cIndex].advanceX = charInfos[cIndex].xadvance;
	}
	
	u8* bitmapData = (u8*)malloc(sizeof(u8)*4 * bitmapWidth * bitmapHeight);
	
	for (i32 y = 0; y < bitmapHeight; y++)
	{
		for (i32 x = 0; x < bitmapWidth; x++)
		{
			u8 grayscaleValue = grayscaleData[y*bitmapWidth + x];
			
			bitmapData[(y*bitmapWidth+x)*4 + 0] = 255;
			bitmapData[(y*bitmapWidth+x)*4 + 1] = 255;
			bitmapData[(y*bitmapWidth+x)*4 + 2] = 255;
			bitmapData[(y*bitmapWidth+x)*4 + 3] = grayscaleValue; 
		}
	}
	
	result.bitmap = CreateTexture(bitmapData, bitmapWidth, bitmapHeight);
	
	free(grayscaleData);
	free(charInfos);
	free(bitmapData);
	PlatformInfo->FreeFileMemoryPntr(&fontFile);
	
	//Create information about character sizes
	{
		v2 maxSize = Vec2_Zero;
		v2 extendVertical = Vec2_Zero;
		for (u32 cIndex = 0; cIndex < result.numChars; cIndex++)
		{
			FontCharInfo_t* charInfo = &result.chars[cIndex];
			
			if (charInfo->height > maxSize.y)
				maxSize.y = (r32)charInfo->height;
			
			if (-charInfo->offset.y > extendVertical.x)
				extendVertical.x = -charInfo->offset.y;
			
			if (charInfo->offset.y + charInfo->height > extendVertical.y)
				extendVertical.y = charInfo->offset.y + charInfo->height;
			
			if (charInfo->advanceX > maxSize.x)
				maxSize.x = charInfo->advanceX;
		}
		
		result.maxCharWidth = maxSize.x;
		result.maxCharHeight = maxSize.y;
		result.maxExtendUp = extendVertical.x;
		result.maxExtendDown = extendVertical.y;
		result.lineHeight = result.maxExtendDown + result.maxExtendUp;
	}
	
	return result;
}
