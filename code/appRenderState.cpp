/*
File:   appRenderState.cpp
Author: Taylor Robbins
Date:   08\26\2017
Description: 
	** Houses all of the functions related to the RenderState_t utility structure 
*/

void InitializeRenderState(const PlatformInfo_t* PlatformInfo, RenderState_t* renderState)
{
	ClearPointer(renderState);
	
	Vertex_t squareVertices[] =
	{
		{  {0.0f, 0.0f, 0.0f}, ColorToVec4({Color_White}), {0.0f, 0.0f} },
		{  {1.0f, 0.0f, 0.0f}, ColorToVec4({Color_White}), {1.0f, 0.0f} },
		{  {0.0f, 1.0f, 0.0f}, ColorToVec4({Color_White}), {0.0f, 1.0f} },
		
		{  {0.0f, 1.0f, 0.0f}, ColorToVec4({Color_White}), {0.0f, 1.0f} },
		{  {1.0f, 0.0f, 0.0f}, ColorToVec4({Color_White}), {1.0f, 0.0f} },
		{  {1.0f, 1.0f, 0.0f}, ColorToVec4({Color_White}), {1.0f, 1.0f} },
	};
	renderState->squareBuffer = CreateVertexBuffer(squareVertices, ArrayCount(squareVertices));
	
	renderState->gradientTexture = LoadTexture("Resources/Textures/gradient.png", false, false);
	renderState->circleTexture = LoadTexture("Resources/Sprites/circle.png", false, false);
	
	Color_t textureData = {Color_White};
	renderState->dotTexture = CreateTexture((u8*)&textureData, 1, 1);
	
	renderState->viewport = NewRectangle(0, 0, (r32)PlatformInfo->screenSize.x, (r32)PlatformInfo->screenSize.y);
	renderState->worldMatrix = Matrix4_Identity;
	renderState->viewMatrix = Matrix4_Identity;
	renderState->projectionMatrix = Matrix4_Identity;
}


//+================================================================+
//|                     State Change Functions                     |
//+================================================================+
void RenderState_t::BindShader(const Shader_t* shaderPntr)
{
	this->boundShader = shaderPntr;
	
	glUseProgram(shaderPntr->programId);
}

void RenderState_t::UpdateShader()
{
	Assert(this->boundShader != nullptr);
	
	if (this->boundBuffer != nullptr)
	{
		glBindVertexArray(this->boundShader->vertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, this->boundBuffer->id);
		glVertexAttribPointer(this->boundShader->positionAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_t), (void*)0);
		glVertexAttribPointer(this->boundShader->colorAttribLocation,    4, GL_FLOAT, GL_FALSE, sizeof(Vertex_t), (void*)sizeof(v3));
		glVertexAttribPointer(this->boundShader->texCoordAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_t), (void*)(sizeof(v3)+sizeof(v4)));
	}
	if (this->boundTexture != nullptr)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->boundTexture->id);
		glUniform1i(this->boundShader->diffuseTextureLocation, 0);
		glUniform2f(this->boundShader->textureSizeLocation, (r32)this->boundTexture->width, (r32)this->boundTexture->height);
	}
	if (this->boundAlphaTexture != nullptr)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, this->boundAlphaTexture->id);
		glUniform1i(this->boundShader->alphaTextureLocation, 1);
		glUniform1i(this->boundShader->useAlphaTextureLocation, 1);
	}
	else
	{
		glUniform1i(this->boundShader->useAlphaTextureLocation, 0);
	}
	if (this->boundFrameBuffer != nullptr)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, this->boundFrameBuffer->id);
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	glViewport(
		(i32)this->viewport.x, 
		(i32)this->viewport.y, 
		(i32)this->viewport.width, 
		(i32)this->viewport.height
	);
	
	glUniformMatrix4fv(this->boundShader->worldMatrixLocation,      1, GL_FALSE, &this->worldMatrix.values[0][0]);
	glUniformMatrix4fv(this->boundShader->viewMatrixLocation,       1, GL_FALSE, &this->viewMatrix.values[0][0]);
	glUniformMatrix4fv(this->boundShader->projectionMatrixLocation, 1, GL_FALSE, &this->projectionMatrix.values[0][0]);
}

void RenderState_t::BindFont(const Font_t* fontPntr)
{
	this->boundFont = fontPntr;
}

void RenderState_t::BindTexture(const Texture_t* texturePntr)
{
	this->boundTexture = texturePntr;
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->boundTexture->id);
	glUniform1i(this->boundShader->diffuseTextureLocation, 0);
	glUniform2f(this->boundShader->textureSizeLocation, (r32)this->boundTexture->width, (r32)this->boundTexture->height);
}

void RenderState_t::BindAlphaTexture(const Texture_t* texturePntr)
{
	this->boundAlphaTexture = texturePntr;
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->boundAlphaTexture->id);
	glUniform1i(this->boundShader->alphaTextureLocation, 1);
	glUniform1i(this->boundShader->useAlphaTextureLocation, 1);
}
void RenderState_t::DisableAlphaTexture()
{
	this->boundAlphaTexture = nullptr;
	
	glUniform1i(this->boundShader->useAlphaTextureLocation, 0);
}

void RenderState_t::BindBuffer(const VertexBuffer_t* vertBufferPntr)
{
	this->boundBuffer = vertBufferPntr;
	
	glBindVertexArray(this->boundShader->vertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, vertBufferPntr->id);
	glVertexAttribPointer(this->boundShader->positionAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_t), (void*)0);
	glVertexAttribPointer(this->boundShader->colorAttribLocation,    4, GL_FLOAT, GL_FALSE, sizeof(Vertex_t), (void*)sizeof(v3));
	glVertexAttribPointer(this->boundShader->texCoordAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_t), (void*)(sizeof(v3)+sizeof(v4)));
}

void RenderState_t::BindFrameBuffer(const FrameBuffer_t* frameBuffer)
{
	this->boundFrameBuffer = frameBuffer;
	
	if (frameBuffer == nullptr)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->id);
	}
}

void RenderState_t::SetWorldMatrix(const Matrix4_t& worldMatrix)
{
	this->worldMatrix = worldMatrix;
	glUniformMatrix4fv(this->boundShader->worldMatrixLocation, 1, GL_FALSE, &worldMatrix.values[0][0]);
}
void RenderState_t::SetViewMatrix(const Matrix4_t& viewMatrix)
{
	this->viewMatrix = viewMatrix;
	glUniformMatrix4fv(this->boundShader->viewMatrixLocation, 1, GL_FALSE, &viewMatrix.values[0][0]);
}
void RenderState_t::SetProjectionMatrix(const Matrix4_t& projectionMatrix)
{
	this->projectionMatrix = projectionMatrix;
	glUniformMatrix4fv(this->boundShader->projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix.values[0][0]);
}

void RenderState_t::SetViewport(rec viewport)
{
	this->viewport = viewport;
	
	glViewport(
		(i32)this->viewport.x, 
		(i32)this->viewport.y, 
		(i32)this->viewport.width, 
		(i32)this->viewport.height
	);
}

void RenderState_t::SetColor(Color_t color)
{
	v4 colorVec = ColorToVec4(color);
	glUniform4f(this->boundShader->diffuseColorLocation, colorVec.r, colorVec.g, colorVec.b, colorVec.a);
}
void RenderState_t::SetSecondaryColor(Color_t color)
{
	v4 colorVec = ColorToVec4(color);
	glUniform4f(this->boundShader->secondaryColorLocation, colorVec.r, colorVec.g, colorVec.b, colorVec.a);
}

void RenderState_t::SetSourceRectangle(rec sourceRectangle)
{
	glUniform4f(this->boundShader->sourceRectangleLocation, sourceRectangle.x, sourceRectangle.y, sourceRectangle.width, sourceRectangle.height);
}

void RenderState_t::SetGradientEnabled(bool doGradient)
{
	glUniform1i(this->boundShader->doGrayscaleGradientLocation, doGradient ? 1 : 0);
}


//+================================================================+
//|                       Drawing Functions                        |
//+================================================================+
void RenderState_t::DrawTexturedRec(rec rectangle, Color_t color)
{
	this->BindTexture(this->boundTexture);
	this->SetSourceRectangle(NewRectangle(0, 0, 1, 1));
	this->SetColor(color);
	m4 worldMatrix = Mat4Mult(
		Matrix4Translate(NewVec3(rectangle.x, rectangle.y, 0.0f)),       //Position
		Matrix4Scale(NewVec3(rectangle.width, rectangle.height, 1.0f))); //Scale
	this->SetWorldMatrix(worldMatrix);
	this->BindBuffer(&this->squareBuffer);
	glDrawArrays(GL_TRIANGLES, 0, this->squareBuffer.numVertices);
}
void RenderState_t::DrawTexturedRec(rec rectangle, Color_t color, rec sourceRectangle)
{
	rec realSourceRec = NewRectangle(
		sourceRectangle.x / (r32)this->boundTexture->width,
		sourceRectangle.y / (r32)this->boundTexture->height,
		sourceRectangle.width / (r32)this->boundTexture->width,
		sourceRectangle.height / (r32)this->boundTexture->height);
	this->SetSourceRectangle(realSourceRec);
	this->BindTexture(this->boundTexture);
	this->SetColor(color);
	m4 worldMatrix = Mat4Mult(
		Matrix4Translate(NewVec3(rectangle.x, rectangle.y, 0.0f)),       //Position
		Matrix4Scale(NewVec3(rectangle.width, rectangle.height, 1.0f))); //Scale
	this->SetWorldMatrix(worldMatrix);
	this->BindBuffer(&this->squareBuffer);
	glDrawArrays(GL_TRIANGLES, 0, this->squareBuffer.numVertices);
}

void RenderState_t::DrawRectangle(rec rectangle, Color_t color)
{
	this->BindTexture(&this->dotTexture);
	this->SetSourceRectangle(NewRectangle(0, 0, 1, 1));
	this->SetColor(color);
	m4 worldMatrix = Mat4Mult(
		Matrix4Translate(NewVec3(rectangle.x, rectangle.y, 0.0f)),       //Position
		Matrix4Scale(NewVec3(rectangle.width, rectangle.height, 1.0f))); //Scale
	this->SetWorldMatrix(worldMatrix);
	this->BindBuffer(&this->squareBuffer);
	glDrawArrays(GL_TRIANGLES, 0, this->squareBuffer.numVertices);
}

void RenderState_t::DrawButton(rec rectangle, Color_t backgroundColor, Color_t borderColor, r32 borderWidth)
{
	this->DrawRectangle(rectangle, backgroundColor);
	
	this->DrawRectangle(NewRectangle(rectangle.x, rectangle.y, rectangle.width, borderWidth), borderColor);
	this->DrawRectangle(NewRectangle(rectangle.x, rectangle.y, borderWidth, rectangle.height), borderColor);
	this->DrawRectangle(NewRectangle(rectangle.x, rectangle.y + rectangle.height - borderWidth, rectangle.width, borderWidth), borderColor);
	this->DrawRectangle(NewRectangle(rectangle.x + rectangle.width - borderWidth, rectangle.y, borderWidth, rectangle.height), borderColor);
}

void RenderState_t::DrawGradient(rec rectangle, Color_t color1, Color_t color2, Direction2D_t direction)
{
	this->BindTexture(&this->gradientTexture);
	this->SetSourceRectangle(NewRectangle(0, 0, 1, 1));
	this->SetColor(color1);
	this->SetSecondaryColor(color2);
	this->SetGradientEnabled(true);
	
	m4 worldMatrix = Matrix4_Identity;
	switch (direction)
	{
		case Direction2D_Right:
		default:
		{
			worldMatrix = Mat4Mult(
				Matrix4Translate(NewVec3(rectangle.x, rectangle.y, 0.0f)),
				Matrix4Scale(NewVec3(rectangle.width, rectangle.height, 1.0f)));
		} break;
		
		case Direction2D_Left:
		{
			worldMatrix = Mat4Mult(
				Matrix4Translate(NewVec3(rectangle.x + rectangle.width, rectangle.y, 0.0f)),
				Matrix4Scale(NewVec3(-rectangle.width, rectangle.height, 1.0f)));
		} break;
		
		case Direction2D_Down:
		{
			worldMatrix = Matrix4Multiply(
				Matrix4Translate(NewVec3(rectangle.x + rectangle.width, rectangle.y, 0.0f)),
				Matrix4RotateZ(ToRadians(90)),
				Matrix4Scale(NewVec3(rectangle.height, rectangle.width, 1.0f)));
		} break;
		
		case Direction2D_Up:
		{
			worldMatrix = Matrix4Multiply(
				Matrix4Translate(NewVec3(rectangle.x + rectangle.width, rectangle.y + rectangle.height, 0.0f)),
				Matrix4RotateZ(ToRadians(90)),
				Matrix4Scale(NewVec3(-rectangle.height, rectangle.width, 1.0f)));
		} break;
	};
	this->SetWorldMatrix(worldMatrix);
	
	this->BindBuffer(&this->squareBuffer);
	glDrawArrays(GL_TRIANGLES, 0, this->squareBuffer.numVertices);
	
	this->SetGradientEnabled(false);
}

void RenderState_t::DrawCircle(v2 center, r32 radius, Color_t color)
{
	this->BindAlphaTexture(&this->circleTexture);
	this->DrawRectangle(NewRectangle(center.x - radius, center.y - radius, radius*2, radius*2), color);
	this->DisableAlphaTexture();
}

void RenderState_t::DrawCharacter(u32 charIndex, v2 bottomLeft, Color_t color, r32 scale)
{
	const FontCharInfo_t* charInfo = &this->boundFont->chars[charIndex];
	
	rec sourceRectangle = NewRectangle((r32)charInfo->x, (r32)charInfo->y, (r32)charInfo->width, (r32)charInfo->height);
	rec drawRectangle = NewRectangle(
		bottomLeft.x + scale*charInfo->offset.x, 
		bottomLeft.y + scale*charInfo->offset.y, 
		scale*charInfo->width, 
		scale*charInfo->height);
	
	if (this->boundTexture != &this->boundFont->bitmap)
	{
		this->BindTexture(&this->boundFont->bitmap);
	}
	
	this->DrawTexturedRec(drawRectangle, color, sourceRectangle);
}

void RenderState_t::DrawString(const char* string, u32 numCharacters, v2 position, Color_t color, r32 scale)
{
	this->BindTexture(&this->boundFont->bitmap);
	
	v2 currentPos = position;
	for (u32 cIndex = 0; cIndex < numCharacters; cIndex++)
	{
		if (string[cIndex] == '\t')
		{
			u32 spaceIndex = GetFontCharIndex(this->boundFont, ' ');
			currentPos.x += this->boundFont->chars[spaceIndex].advanceX * 4 * scale;
		}
		else if (string[cIndex] == '\r' ||
			string[cIndex] == 0x01 || string[cIndex] == 0x02 || string[cIndex] == 0x03 || string[cIndex] == 0x04 || string[cIndex] == 0x05)
		{
			//Don't do anything
		}
		else
		{
			u32 charIndex = GetFontCharIndex(this->boundFont, string[cIndex]);
			this->DrawCharacter(charIndex, currentPos, color, scale);
			currentPos.x += this->boundFont->chars[charIndex].advanceX * scale;
		}
	}
}

void RenderState_t::DrawString(const char* nullTermString, v2 position, Color_t color, r32 scale)
{
	this->DrawString(nullTermString, (u32)strlen(nullTermString), position, color, scale);
}

void RenderState_t::PrintString(v2 position, Color_t color, r32 scale, const char* formatString, ...)
{
	char printBuffer[256] = {};
	va_list args;
	
	va_start(args, formatString);
	u32 length = (u32)vsnprintf(printBuffer, 256-1, formatString, args);
	va_end(args);
	
	this->DrawString(printBuffer, length, position, color, scale);
}

void RenderState_t::DrawLine(v2 startPos, v2 endPos, Color_t color, r32 width)
{
	this->BindTexture(&this->dotTexture);
	this->SetSourceRectangle(NewRectangle(0, 0, 1, 1));
	this->SetColor(color);
	
	r32 lineLength = Vec2Length(endPos-startPos);
	r32 lineRotation = (r32)atan2(endPos.y - startPos.y, endPos.x - startPos.x);
	
	m4 worldMatrix = Matrix4Translate(NewVec3(0, -0.5f, 0.0f));
	worldMatrix = Matrix4Scale(NewVec3(lineLength, width, 1.0f)) * worldMatrix;
	worldMatrix = Matrix4RotateZ(lineRotation) * worldMatrix;
	worldMatrix = Matrix4Translate(NewVec3(startPos.x, startPos.y, 0.0f)) * worldMatrix;
	this->SetWorldMatrix(worldMatrix);
	this->BindBuffer(&this->squareBuffer);
	glDrawArrays(GL_TRIANGLES, 0, this->squareBuffer.numVertices);
}

