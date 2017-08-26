/*
File:   win32_appLoading.cpp
Author: Taylor Robbins
Date:   06\06\2017
Description: 
	** Contains functions and types related to loading the application DLL

#included from win32_main.cpp 
*/

//+================================================================+
//|                           Structs                              |
//+================================================================+
struct LoadedApp_t
{
	bool isValid;
	Version_t version;
	HMODULE module;
	FILETIME lastWriteTime;
	
	AppGetVersion_f*      AppGetVersionPntr;
	AppReloaded_f*        AppReloadedPntr;
	AppInitialize_f*      AppInitializePntr;
	AppUpdate_f*          AppUpdatePntr;
	AppGetSoundSamples_f* AppGetSoundSamplesPntr;
	AppClosing_f*         AppClosingPntr;
};

//+================================================================+
//|                       Stub Functions                           |
//+================================================================+
AppGetVersion_DEFINITION(AppGetVersion_Stub)
{
	Version_t version = { 0, 0, 0 };
	return version;
}
AppInitialize_DEFINITION(AppInitialize_Stub)
{
	
}
AppReloaded_DEFINITION(AppReloaded_Stub)
{
	
}
AppUpdate_DEFINITION(AppUpdate_Stub)
{
	
}
AppGetSoundSamples_DEFINITION(AppGetSoundSamples_Stub)
{
	
}
AppClosing_DEFINITION(AppClosing_Stub)
{
	
}


//+================================================================+
//|                       Main Functions                           |
//+================================================================+
bool LoadDllCode(const char* appDllName, const char* tempDllName, LoadedApp_t* loadedApp)
{
	ClearPointer(loadedApp);
	
	loadedApp->lastWriteTime = GetFileWriteTime(appDllName);
	
	#if DEBUG
		u32 copyTries = 0;
		while (!CopyFileA(appDllName, tempDllName, false))
		{
			copyTries++;
			//TODO: Seems this is dependant on how long the file is locked during compilation
			//		We should revise it so it's more robust rather than hard-coded
			if (copyTries >= 50000) 
			{
				Win32_WriteLine("Could not copy DLL.");
				return false;
			}
		}
		// Win32_PrintLine("Tried to copy %u times", copyTries);
		loadedApp->module = LoadLibraryA(tempDllName);
	#else
		loadedApp->module = LoadLibraryA(appDllName);
	#endif
	
	
	if (loadedApp->module != 0)
	{
		loadedApp->AppGetVersionPntr =      (AppGetVersion_f*)      GetProcAddress(loadedApp->module, "App_GetVersion");
		loadedApp->AppInitializePntr =      (AppInitialize_f*)      GetProcAddress(loadedApp->module, "App_Initialize");
		loadedApp->AppReloadedPntr =        (AppReloaded_f*)        GetProcAddress(loadedApp->module, "App_Reloaded");
		loadedApp->AppUpdatePntr =          (AppUpdate_f*)          GetProcAddress(loadedApp->module, "App_Update");
		loadedApp->AppGetSoundSamplesPntr = (AppGetSoundSamples_f*) GetProcAddress(loadedApp->module, "App_GetSoundSamples");
		loadedApp->AppClosingPntr =         (AppClosing_f*)         GetProcAddress(loadedApp->module, "App_Closing");
		
		loadedApp->isValid = (
			loadedApp->AppGetVersionPntr != nullptr &&
			loadedApp->AppInitializePntr != nullptr &&
			loadedApp->AppReloadedPntr != nullptr &&
			loadedApp->AppUpdatePntr != nullptr &&
			loadedApp->AppGetSoundSamplesPntr != nullptr &&
			loadedApp->AppClosingPntr != nullptr
		);
	}
	
	if (!loadedApp->isValid)
	{
		loadedApp->AppGetVersionPntr =      AppGetVersion_Stub;
		loadedApp->AppInitializePntr =      AppInitialize_Stub;
		loadedApp->AppReloadedPntr =        AppReloaded_Stub;
		loadedApp->AppUpdatePntr =          AppUpdate_Stub;
		loadedApp->AppGetSoundSamplesPntr = AppGetSoundSamples_Stub;
		loadedApp->AppClosingPntr =         AppClosing_Stub;
	}
	
	loadedApp->version = loadedApp->AppGetVersionPntr(nullptr);
	
	return loadedApp->isValid;
}

void FreeDllCode(LoadedApp_t* loadedApp)
{
	if (loadedApp != nullptr &&
		loadedApp->module != 0)
	{
		bool32 freeResult = FreeLibrary(loadedApp->module);
		Assert(freeResult);
	}
	
	ClearPointer(loadedApp);
}

