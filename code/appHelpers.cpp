/*
File:   appHelpers.cpp
Author: Taylor Robbins
Date:   08\26\2017
Description: 
	** Holds some macros that are used across the whole application 
*/

#define DEBUG_Write(formatStr) do {                 \
	if (Gl_PlatformInfo != nullptr &&               \
		Gl_PlatformInfo->DebugWritePntr != nullptr) \
	{                                               \
		Gl_PlatformInfo->DebugWritePntr(formatStr); \
	}                                               \
} while (0)

#define DEBUG_WriteLine(formatStr) do {                 \
	if (Gl_PlatformInfo != nullptr &&                   \
		Gl_PlatformInfo->DebugWriteLinePntr != nullptr) \
	{                                                   \
		Gl_PlatformInfo->DebugWriteLinePntr(formatStr); \
	}                                                   \
} while (0)

#define DEBUG_Print(formatStr, ...) do {                         \
	if (Gl_PlatformInfo != nullptr &&                            \
		Gl_PlatformInfo->DebugPrintPntr != nullptr)              \
	{                                                            \
		Gl_PlatformInfo->DebugPrintPntr(formatStr, __VA_ARGS__); \
	}                                                            \
} while (0)

#define DEBUG_PrintLine(formatStr, ...) do {                         \
	if (Gl_PlatformInfo != nullptr &&                                \
		Gl_PlatformInfo->DebugPrintLinePntr != nullptr)              \
	{                                                                \
		Gl_PlatformInfo->DebugPrintLinePntr(formatStr, __VA_ARGS__); \
	}                                                                \
} while (0)

//NOTE: Must have access to AppInput
#define ButtonPressed(button) ((AppInput->buttons[button].isDown && AppInput->buttons[button].transCount > 0) || AppInput->buttons[button].transCount >= 2)
#define ButtonReleased(button) ((!AppInput->buttons[button].isDown && AppInput->buttons[button].transCount > 0) || AppInput->buttons[button].transCount >= 2)
#define ButtonDown(button) (AppInput->buttons[button].isDown)
