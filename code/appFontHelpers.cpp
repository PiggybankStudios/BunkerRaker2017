/*
File:   appFontHelpers.cpp
Author: Taylor Robbins
Date:   08\26\2017
Description: 
	** Houses some functions that are helpful when working with a baked Font_t 
*/

inline u32 GetFontCharIndex(const Font_t* font, char character)
{
	u32 cIndex = 0;
	if (character >= font->firstChar && character - font->firstChar < font->numChars)
	{
		cIndex = character - font->firstChar;
	}
	else
	{
		cIndex = 127 - font->firstChar;
	}
	
	return cIndex;
}

inline v2 MeasureString(const Font_t* font, const char* string, u32 numChars)
{
	v2 currentPos = Vec2_Zero;
	for (u32 cIndex = 0; cIndex < numChars; cIndex++)
	{
		if (string[cIndex] == '\t')
		{
			u32 spaceIndex = GetFontCharIndex(font, ' ');
			currentPos.x += font->chars[spaceIndex].advanceX * 4;
		}
		else if (string[cIndex] == '\r' || 
			string[cIndex] == 0x01 || string[cIndex] == 0x02 || string[cIndex] == 0x03 || string[cIndex] == 0x04 || string[cIndex] == 0x05)
		{
			//Don't do anything
		}
		else
		{
			u32 charIndex = GetFontCharIndex(font, string[cIndex]);
			const FontCharInfo_t* charInfo = &font->chars[charIndex];
			currentPos.x += charInfo->advanceX;
		}
	}
	
	return NewVec2(currentPos.x, font->lineHeight);
}

inline v2 MeasureString(const Font_t* font, const char* nullTermString)
{
	return MeasureString(font, nullTermString, (u32)strlen(nullTermString));
}

//TODO: Handle this more correctly with line wrapping
inline i32 GetStringIndexForLocation(const Font_t* font, const char* nullTermString, v2 relativePos)
{
	i32 result = 0;
	
	if (relativePos.y > MeasureString(font, nullTermString).y + 5) //TODO: 5 is for line spacing
	{
		result = (i32)strlen(nullTermString);
		return result;
	}
	
	v2 lastStringSize = Vec2_Zero;
	for (i32 cIndex = 0; nullTermString[cIndex] != '\0'; cIndex++)
	{
		v2 stringSize = MeasureString(font, nullTermString, cIndex+1);
		if (stringSize.x > relativePos.x || nullTermString[cIndex+1] == '\0')
		{
			if (Abs32(relativePos.x - lastStringSize.x) < Abs32(relativePos.x - stringSize.x))
			{
				result = cIndex;
			}
			else
			{
				result = cIndex+1;
			}
			
			break;
		}
		
		lastStringSize = stringSize;
	}
	
	return result;
}