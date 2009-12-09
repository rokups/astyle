@echo off
REM build the AStyle Debug Configuration using the AStyle project file
REM copy the output to the AStyleDev folder

set astyle=..\..\AStyle\build\vs2008
set bindir=..\build\vs2008\bin
set errors=no
set called=no

REM the following batch file is needed for VS2005
REM and change the build directory to buildvs2005
REM call "%VS80COMNTOOLS%\vsvars32.bat"

REM if called from a compile, the directory structure is different
if not exist astylebuilddbg-exe.bat (
set called=yes
set astyle=..\..\%astyle%
set bindir=..\..\%bindir%
)

echo BUILDING ASTYLE DEBUG EXE
%windir%\microsoft.net\framework\v3.5\msbuild  "/property:Configuration=Debug"  "%astyle%\AStyle.sln"
if %ERRORLEVEL% NEQ 0  set errors=yes

REM don't copy if compile errors
if not %errors%==yes (
echo -
echo COPYING
if not exist "%bindir%"  mkdir "%bindir%"
xcopy "%astyle%\bin\AStyled.exe"  "%bindir%\"  /y /q
if %ERRORLEVEL% NEQ 0  set errors=yes
)

if %errors%==yes (
echo -
echo * * * *  ERRORS IN PROCESSING SCRIPT  * * * *
)

if %called%==yes  (
if %errors% == yes exit 100
) else (
echo -
pause
)
