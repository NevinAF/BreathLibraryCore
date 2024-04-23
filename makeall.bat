@REM cd out\build\x64-Debug
@REM "C:\PROGRAM FILES\MICROSOFT VISUAL STUDIO\2022\COMMUNITY\COMMON7\IDE\COMMONEXTENSIONS\MICROSOFT\CMAKE\CMake\bin\cmake.exe" -G "Ninja"  -DCMAKE_BUILD_TYPE:STRING="Debug" -DCMAKE_INSTALL_PREFIX:PATH="C:\Users\nevin\Documents\BreathLibraryCore\out\install\x64-Debug" -DCMAKE_C_COMPILER:FILEPATH="C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.32.31326/bin/Hostx64/x64/cl.exe" -DCMAKE_CXX_COMPILER:FILEPATH="C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.32.31326/bin/Hostx64/x64/cl.exe"  -DCMAKE_MAKE_PROGRAM="C:\PROGRAM FILES\MICROSOFT VISUAL STUDIO\2022\COMMUNITY\COMMON7\IDE\COMMONEXTENSIONS\MICROSOFT\CMAKE\Ninja\ninja.exe" "C:\Users\nevin\Documents\BreathLibraryCore" 2>&1

@echo off

set cmake="C:\PROGRAM FILES\MICROSOFT VISUAL STUDIO\2022\COMMUNITY\COMMON7\IDE\COMMONEXTENSIONS\MICROSOFT\CMAKE\CMake\bin\cmake.exe"
set ninja = "C:\PROGRAM FILES\MICROSOFT VISUAL STUDIO\2022\COMMUNITY\COMMON7\IDE\COMMONEXTENSIONS\MICROSOFT\CMAKE\Ninja\ninja.exe"

SET ANDROID_NDK=C:/Microsoft/AndroidNDK/android-ndk-r21e
set android_toolchain="C:\Microsoft\AndroidNDK\android-ndk-r21e\build\cmake\android.toolchain.cmake"
set android_min_sdk="21"

call :Cleanup

set build_type=Debug

call :BuildAndroid armeabi-v7a
@REM call :BuildAndroid arm64-v8a
@REM call :BuildAndroid x86
@REM call :BuildAndroid x86_64

@REM call :BuildWindows x86_64

call :PostBuild

EXIT /B 0

:Cleanup
if exist build (
	rmdir /S /Q build
)
if exist out (
	rmdir /S /Q out
)
EXIT /B 0

@REM BuildAndroid <arch>
:BuildAndroid
@REM set make_build_args=-DCMAKE_TOOLCHAIN_FILE=%android_toolchain% -DANDROID_ABI=%1 -DANDROID_PLATFORM=android-%android_min_sdk%
set make_build_args=-DCMAKE_SYSTEM_NAME=Android -DCMAKE_SYSTEM_VERSION=21 -DCMAKE_ANDROID_ARCH_ABI=%1 -DCMAKE_ANDROID_NDK=%ANDROID_NDK%
call :BuildMain android %1
EXIT /B 0

@REM BuildWindows <arch>
:BuildWindows
set make_build_args=-D_CRT_SECURE_NO_WARNINGS=1
call :BuildMain windows %1
EXIT /B 0

@REM BuildMain <platform> <arch>
:BuildMain

echo "%1 --> %make_build_args%"

@REM Create folders and configuration if needed, then build.
if not exist build (mkdir build)
cd build
	if not exist %1 (mkdir %1)
	cd %1
		if not exist %2 (
			mkdir %2
			cd %2
				%cmake% -DCMAKE_BUILD_TYPE:STRING="%build_type%" %make_build_args% ..\..\.. 2>&1
			cd ..
		)

		cd %2
			mkdir ARM/Debug/
			%cmake% --build . --config %build_type%
		cd ..
	cd ..
cd ..

@REM Copy the output to the output folder. All *.dll, *.lib, and *.so files are copied.
if not exist out mkdir out
if not exist out\%1 mkdir out\%1
if not exist out\%1\%2 mkdir out\%1\%2

cd build\%1\%2\%build_type%
	for %%a in (*.dll *.lib *.so) do (
		for /r %%b in (%%a) do (
			copy "%%b" ..\..\..\..\out\%1\%2
		)
	)
cd ..\..\..\..

EXIT /B 0

:PostBuild
@REM Copy the out folder to the project plugins folder
if exist ..\BreathLibraryUnity3D\Assets\Plugins\BreathLibrary (
	xcopy /s /Y out ..\BreathLibraryUnity3D\Assets\Plugins\BreathLibrary

	@REM Remove all *.lib files from the project plugins folder
	for /r ..\BreathLibraryUnity3D\Assets\Plugins\BreathLibrary\*.lib %%a in (%%a) do (
		del "%%a"
	)
)