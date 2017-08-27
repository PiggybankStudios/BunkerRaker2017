@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64

set TimeString=%date:~-4,4%%date:~-10,2%%date:~-7,2%%time:~0,2%%time:~3,2%%time:~6,2%

mkdir build > NUL 2> NUL
rem echo Running from %cd%
rem echo Time is %TimeString%

set CompilePlatform=0
set CompileApplication=1
set DebugBuild=1
set ProjectName=BunkerRaker2017
set BuildFolder=build\

if "%DebugBuild%"=="1" (
	set DebugDependantFlags=/MTd -DDEBUG=1
	set DebugDependantPaths=/LIBPATH:"W:\lib\glew-2.0.0\lib\Debug\x64" /LIBPATH:"W:\lib\glfw-3.2.1\build\src\Debug"
	set DebugDependantLibraries=glew32d.lib
) else (
	set DebugDependantFlags=/MT -DDEBUG=0
	set DebugDependantPaths=/LIBPATH:"W:\lib\glew-2.0.0\lib\Release\x64" /LIBPATH:"W:\lib\glfw-3.2.1\build\src\Release"
	set DebugDependantLibraries=glew32.lib
)

set CompilerFlags=%DebugDependantFlags% /FC /Zi /EHsc /nologo /GS- /Gm- -GR- /EHa- /Fm /Od /Oi /WX /W4 /wd4201 /wd4100 /wd4189 /wd4996 /wd4127 /wd4505 /wd4101 /wd4702 /wd4458 /wd4324
set LinkerFlags=-incremental:no
set IncludeDirectories=/I"W:\lib\mylib" /I"W:\lib\glew-2.0.0\include" /I"W:\lib\glfw-3.2.1\include" /I"W:\lib\stb"
set LibraryDirectories=%DebugDependantPaths%
set Libraries=gdi32.lib User32.lib Shell32.lib opengl32.lib glfw3.lib Shlwapi.lib %DebugDependantLibraries%
set AppExports=/EXPORT:App_GetVersion /EXPORT:App_Initialize /EXPORT:App_Update /EXPORT:App_GetSoundSamples /EXPORT:App_Closing /EXPORT:App_Reloaded

rem echo [Building...]

REM Compile the resources file to generate resources.res which defines our program icon
rem rc /nologo build\resources.rc

del build\*.pdb > NUL 2> NUL

if "%CompilePlatform%"=="1" (
	echo[
	
	python IncrementVersionNumber.py code\win32_version.h
	
	cl /Fe%BuildFolder%%ProjectName%.exe %CompilerFlags% %IncludeDirectories% code\win32_main.cpp /link %LibraryDirectories% %LinkerFlags% %Libraries% kernel32.lib

	if "%ERRORLEVEL%"=="0" (
		echo [Platform Build Succeeded!]
	) else (
		echo [Platform Build Failed! %ERRORLEVEL%]
	)
)

if "%CompileApplication%"=="1" (
	echo[
	
	python IncrementVersionNumber.py code\app_version.h
	
	cl /Fe%BuildFolder%%ProjectName%.dll %CompilerFlags% %IncludeDirectories% code\app.cpp /link %LibraryDirectories% %LinkerFlags% %Libraries% %AppExports% /DLL /PDB:"%BuildFolder%%ProjectName%_%TimeString%.pdb"

	if "%ERRORLEVEL%"=="0" (
		echo [Application Build Succeeded!]
	) else (
		echo [Application Build Failed! %ERRORLEVEL%]
	)
)

echo[

REM Delete the object files that were created during compilation
del *.obj > NUL 2> NUL

rem echo [Done!]