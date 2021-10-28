@echo off
TITLE LaGriT automatic installer

set INSTALLPATH=

if exist "%programfiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" (
  for /F "tokens=* USEBACKQ" %%F in (`"%programfiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -version 15.0 -property installationPath`) do set INSTALLPATH=%%F
)

FOR /F %%I IN ('DIR "%programfiles(x86)%\IntelSWTools\" /B /O:D') DO echo %%I |>nul findstr /B "parallel_studio_xe" && set newest=%%I
set INTELPATH=%programfiles(x86)%\IntelSWTools\%newest%

REM Save current dir for later
pushd %CD%

echo ============================================
echo CONFIGURING DEVELOPMENT ENVIRONMENT
echo ============================================
echo Current working directory: "%CD%"
echo IntelSWTools path: "%INTELPATH%"
echo Visual Studio path: "%INSTALLPATH%"
echo ============================================
echo ""

if NOT "" == "%INSTALLPATH%" (
  call "%INSTALLPATH%\Common7\Tools\VsDevCmd.bat"
) else (
  goto ERROR_NO_VS15
)

if NOT "" == "%INTELPATH%" (
  call "%INTELPATH%\bin\psxevars.bat" intel64
) else (
  goto ERROR_NO_INTEL
)


:WORK
REM Retrieve the working dir and proceed
popd
echo Current directory: %CD%

echo ============================================
echo CONFIGURING LAGRIT FOR BUILDING
echo ============================================
echo ""

: rmdir /S /Q build
cmake -G "NMake Makefiles" -B "build" || goto END

: cmake -G "Visual Studio 16 2019" -A x86 -B "build" || goto END
: cmake -B "build" || goto END
cd build

echo ============================================
echo BUILDING LAGRIT
echo ============================================
echo ""

nmake || goto END

echo ============================================
echo TESTING LAGRIT
echo ============================================
echo ""

: cd ..\test
: python suite.py -f -l=1 -exe=..\build\lagrit.exe -hf=3

goto END

:ERROR_NO_VS15
echo Visual Studio Tools not available!
goto END

:ERROR_NO_INTEL
echo Intel install path could not be found!
goto END

:END
echo ============================================
echo Script has finished running.
echo ============================================
echo ""

pause