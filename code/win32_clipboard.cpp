/*
File:   win32_clipboard.cpp
Author: Taylor Robbins
Date:   06\18\2017
Description: 
	** Holds functions for interacting with the clipboard 
*/


CopyToClipboard_DEFINITION(Win32_CopyToClipboard)
{
	HWND windowHandle = GetActiveWindow();
	
	HGLOBAL globalCopy = GlobalAlloc(GMEM_MOVEABLE, dataSize); 
	if (globalCopy == nullptr)
	{
		Win32_WriteLine("Couldn't allocate space for clipboard data!");
		return;
	}
	
	u8* lockPntr = (u8*)GlobalLock(globalCopy);
	memcpy(lockPntr, dataPntr, dataSize);
	// ((u8*)globalCopy)[dataSize] = '\0';
	GlobalUnlock(globalCopy);
	
	if (OpenClipboard(windowHandle) == false)
	{
		Win32_WriteLine("Couldn't open clipboard!");
		return;
	}
	if (EmptyClipboard() == false)
	{
		Win32_WriteLine("Could not empty current clipboard contents!");
		CloseClipboard(); return;
	}
	
	SetClipboardData(CF_TEXT, globalCopy);
	
	CloseClipboard();
}

CopyFromClipboard_DEFINITION(Win32_CopyFromClipboard)
{
	u32 result = 0;
	
	
	
	return result;
}
