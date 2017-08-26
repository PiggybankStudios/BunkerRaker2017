/*
File:   platformInterface.h
Author: Taylor Robbins
Date:   08\26\2017
Description: 
	** This is the only common file between the application and the platform layer
*/

#ifndef _PLATFORM_INTERFACE_H
#define _PLATFORM_INTERFACE_H

//TODO: How do we get rid of this??
#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdint.h>

#include "win32_defines.h"
#include "win32_intrinsics.h"
#include "myMath.h"
#include "timeStructs.h"

struct FileInfo_t
{
	uint32 size;
	void* content;
};

//+================================================================+
//|                Platform Function Definitions                   |
//+================================================================+
#define FreeFileMemory_DEFINITION(functionName)  void functionName(FileInfo_t* fileInfo)
typedef FreeFileMemory_DEFINITION(FreeFileMemory_f);

#define ReadEntireFile_DEFINITION(functionName)  FileInfo_t functionName(const char* filename)
typedef ReadEntireFile_DEFINITION(ReadEntireFile_f);

#define WriteEntireFile_DEFINITION(functionName) bool32 functionName(const char* filename, void* memory, uint32 memorySize)
typedef WriteEntireFile_DEFINITION(WriteEntireFile_f);


#define DebugWrite_DEFINITION(functionName)     void functionName(const char* string)
typedef DebugWrite_DEFINITION(DebugWrite_f);

#define DebugWriteLine_DEFINITION(functionName) void functionName(const char* message)
typedef DebugWriteLine_DEFINITION(DebugWriteLine_f);

#define DebugPrint_DEFINITION(functionName)     void functionName(const char* formatString, ...)
typedef DebugPrint_DEFINITION(DebugPrint_f);

#define DebugPrintLine_DEFINITION(functionName) void functionName(const char* formatString, ...)
typedef DebugPrintLine_DEFINITION(DebugPrintLine_f);


#define CopyToClipboard_DEFINITION(functionName) void functionName(const void* dataPntr, u32 dataSize)
typedef CopyToClipboard_DEFINITION(CopyToClipboard_f);

#define CopyFromClipboard_DEFINITION(functionName) u32 functionName(void* outputBuffer, u32 maxSize)
typedef CopyFromClipboard_DEFINITION(CopyFromClipboard_f);

//+================================================================+
//|                          Structures                            |
//+================================================================+

struct Version_t
{
	u32 major;
	u32 minor;
	u32 build;
};

typedef enum
{
	Platform_Windows,
	Platform_Linux,
	Platform_OSX,
} PlatformType_t;

struct PlatformInfo_t
{
	PlatformType_t platformType;
	
	v2i screenSize;
	bool windowResized;
	bool windowHasFocus;
	bool windowIsMinimized;
	
	FreeFileMemory_f*    FreeFileMemoryPntr;
	ReadEntireFile_f*    ReadEntireFilePntr;
	WriteEntireFile_f*   WriteEntireFilePntr;
	DebugWrite_f*        DebugWritePntr;
	DebugWriteLine_f*    DebugWriteLinePntr;
	DebugPrint_f*        DebugPrintPntr;
	DebugPrintLine_f*    DebugPrintLinePntr;
	CopyToClipboard_f*   CopyToClipboardPntr;
	CopyFromClipboard_f* CopyFromClipboardPntr;
	
	r64 timeDelta;
	r64 programTime;
	RealTime_t systemTime;
	RealTime_t localTime;
	
	GLFWwindow* window;
};

struct AppMemory_t
{
	u32 permanantSize;
	u32 transientSize;
	
	void* permanantPntr;
	void* transientPntr;
};

typedef enum
{
	//NOTE: We put the MouseButtons at the bottom so we can use them
	//		when referencing AppInput mouseStartPos and mouseMaxDist
	MouseButton_Left = 0,
	MouseButton_Right,
	MouseButton_Middle,
	
	Button_A,
	Button_B,
	Button_C,
	Button_D,
	Button_E,
	Button_F,
	Button_G,
	Button_H,
	Button_I,
	Button_J,
	Button_K,
	Button_L,
	Button_M,
	Button_N,
	Button_O,
	Button_P,
	Button_Q,
	Button_R,
	Button_S,
	Button_T,
	Button_U,
	Button_V,
	Button_W,
	Button_X,
	Button_Y,
	Button_Z,
	
	Button_0,
	Button_1,
	Button_2,
	Button_3,
	Button_4,
	Button_5,
	Button_6,
	Button_7,
	Button_8,
	Button_9,
	
	Button_F1,
	Button_F2,
	Button_F3,
	Button_F4,
	Button_F5,
	Button_F6,
	Button_F7,
	Button_F8,
	Button_F9,
	Button_F10,
	Button_F11,
	Button_F12,
	
	Button_Enter,
	Button_Backspace,
	Button_Escape,
	Button_Insert,
	Button_Delete,
	Button_Home,
	Button_End,
	Button_PageUp,
	Button_PageDown,
	Button_Tab,
	Button_CapsLock,
	
	Button_Control,
	Button_Alt,
	Button_Shift,
	
	Button_Right,
	Button_Left,
	Button_Up,
	Button_Down,
	
	Button_Plus,
	Button_Minus,
	Button_Pipe,
	Button_OpenBracket,
	Button_CloseBracket,
	Button_Colon,
	Button_Quote,
	Button_Comma,
	Button_Period,
	Button_QuestionMark,
	Button_Tilde,
	Button_Space,
	
	Buttons_NumButtons,
} Buttons_t;

inline const char* GetButtonName(Buttons_t button)
{
	switch (button)
	{
		case Button_A:            return "A";
		case Button_B:            return "B";
		case Button_C:            return "C";
		case Button_D:            return "D";
		case Button_E:            return "E";
		case Button_F:            return "F";
		case Button_G:            return "G";
		case Button_H:            return "H";
		case Button_I:            return "I";
		case Button_J:            return "J";
		case Button_K:            return "K";
		case Button_L:            return "L";
		case Button_M:            return "M";
		case Button_N:            return "N";
		case Button_O:            return "O";
		case Button_P:            return "P";
		case Button_Q:            return "Q";
		case Button_R:            return "R";
		case Button_S:            return "S";
		case Button_T:            return "T";
		case Button_U:            return "U";
		case Button_V:            return "V";
		case Button_W:            return "W";
		case Button_X:            return "X";
		case Button_Y:            return "Y";
		case Button_Z:            return "Z";
		case Button_0:            return "0";
		case Button_1:            return "1";
		case Button_2:            return "2";
		case Button_3:            return "3";
		case Button_4:            return "4";
		case Button_5:            return "5";
		case Button_6:            return "6";
		case Button_7:            return "7";
		case Button_8:            return "8";
		case Button_9:            return "9";
		case Button_F1:           return "F1";
		case Button_F2:           return "F2";
		case Button_F3:           return "F3";
		case Button_F4:           return "F4";
		case Button_F5:           return "F5";
		case Button_F6:           return "F6";
		case Button_F7:           return "F7";
		case Button_F8:           return "F8";
		case Button_F9:           return "F9";
		case Button_F10:          return "F10";
		case Button_F11:          return "F11";
		case Button_F12:          return "F12";
		case Button_Enter:        return "Enter";
		case Button_Backspace:    return "Backspace";
		case Button_Escape:       return "Escape";
		case Button_Insert:       return "Insert";
		case Button_Delete:       return "Delete";
		case Button_Home:         return "Home";
		case Button_End:          return "End";
		case Button_PageUp:       return "PageUp";
		case Button_PageDown:     return "PageDown";
		case Button_Tab:          return "Tab";
		case Button_CapsLock:     return "Caps Lock";
		case Button_Control:      return "Control";
		case Button_Alt:          return "Alt";
		case Button_Shift:        return "Shift";
		case Button_Right:        return "Right";
		case Button_Left:         return "Left";
		case Button_Up:           return "Up";
		case Button_Down:         return "Down";
		case Button_Plus:         return "Plus";
		case Button_Minus:        return "Minus";
		case Button_Pipe:         return "Pipe";
		case Button_OpenBracket:  return "Open Bracket";
		case Button_CloseBracket: return "Close Bracket";
		case Button_Colon:        return "Colon";
		case Button_Quote:        return "Quote";
		case Button_Comma:        return "Comma";
		case Button_Period:       return "Period";
		case Button_QuestionMark: return "Question Mark";
		case Button_Tilde:        return "Tilde";
		case Button_Space:        return "Space";
		case MouseButton_Left:    return "Mouse Left";
		case MouseButton_Right:   return "Mouse Right";
		case MouseButton_Middle:  return "Mouse Middle";
		default:                  return "Unknown";
	};
}

typedef enum : u8
{
	Modifier_Ctrl     = 0x01,
	Modifier_Alt      = 0x02,
	Modifier_Shift    = 0x04,
	Modifier_CapsLock = 0x08,
} ButtonModifier_t;

struct ButtonState_t
{
	//Transition count since last frame
	u32  transCount;
	//Whether or not the button ended down on this frame
	bool isDown;
};

struct AppInput_t
{
	v2 mousePos;
	v2 mouseStartPos[3];
	r32 mouseMaxDist[3];
	v2 scrollDelta;
	v2 scrollValue;
	
	uint32_t numButtonsDown;
	ButtonModifier_t modifiers;
	ButtonState_t buttons[Buttons_NumButtons];
	
	uint8_t textInputLength;
	char textInput[64];
};

typedef enum
{
	Cursor_Default,
	Cursor_Text,
	Cursor_Pointer,
	Cursor_ResizeHorizontal,
	Cursor_ResizeVertical,
	
	NumCursorTypes,
} CursorType_t;

struct AppOutput_t
{
	bool recenterMouse;
	bool showMouse;
	bool closeWindow;
	
	CursorType_t cursorType;
	
	char windowTitle[64];
};


//+================================================================+
//|                  Game Function Definitions                     |
//+================================================================+

#define AppGetVersion_DEFINITION(functionName)      Version_t functionName(bool* resetApplication)
typedef AppGetVersion_DEFINITION(AppGetVersion_f);

#define AppInitialize_DEFINITION(functionName)      void functionName(const PlatformInfo_t* PlatformInfo, const AppMemory_t* AppMemory)
typedef AppInitialize_DEFINITION(AppInitialize_f);

#define AppReloaded_DEFINITION(functionName)        void functionName(const PlatformInfo_t* PlatformInfo, const AppMemory_t* AppMemory)
typedef AppReloaded_DEFINITION(AppReloaded_f);

#define AppUpdate_DEFINITION(functionName)          void functionName(const PlatformInfo_t* PlatformInfo, const AppMemory_t* AppMemory, const AppInput_t* AppInput, AppOutput_t* AppOutput)
typedef AppUpdate_DEFINITION(AppUpdate_f);

#define AppGetSoundSamples_DEFINITION(functionName) void functionName(const PlatformInfo_t* PlatformInfo, const AppMemory_t* AppMemory, const AppInput_t* AppInput)
typedef AppGetSoundSamples_DEFINITION(AppGetSoundSamples_f);

#define AppClosing_DEFINITION(functionName)         void functionName(const PlatformInfo_t* PlatformInfo, const AppMemory_t* AppMemory)
typedef AppClosing_DEFINITION(AppClosing_f);

#endif