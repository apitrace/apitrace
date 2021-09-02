@echo off
@setlocal

:: The plmdebug/windbg from the Win10 SDK works a bit better
set windbg=%ProgramFiles(x86)%\Windows Kits\10\Debuggers\x64

::
:: You can figure out the value of `appname` by running `plmdebug /query`
::
:: You can figure out the value of `appurl` by following instructions in
:: http://www.itsjustwhatever.com/2012/12/22/metro-app-names-the-easy-way/ but
:: not all apps have URLs
::
:: TODO: Write a tool to automate this based off
:: http://blogs.msdn.com/b/windowsappdev/archive/2012/09/04/automating-the-testing-of-windows-8-apps.aspx
::

rem set appname=Microsoft.WindowsStore_2015.4.24.0_x86__8wekyb3d8bbwe
rem set appurl=ms-windows-store://

rem set appname=Microsoft.Windows.Spartan_0.11.10064.0_neutral_neutral_cw5n1h2txyewy
rem set appurl=http://msn.com/

rem set appname=windows.immersivecontrolpanel_6.2.0.0_neutral_neutral_cw5n1h2txyew
rem set appurl=

rem set appname=microsoft.windowscommunicationsapps_17.4016.42291.0_x64__8wekyb3d8bbwe
rem set appurl=outlookmail://

rem set appname=Microsoft.WindowsCalculator_10.1503.31000.0_x64__8wekyb3d8bbwe
rem set appurl=calculator://

rem set appname=Microsoft.MicrosoftMinesweeper_2.4.1408.2503_x86__8wekyb3d8bbwe
rem set appurl=

set appname=Microsoft.ZuneVideo_10.20112.10111.0_x64__8wekyb3d8bbwe
set appurl=

@rem 
@echo on

"%windbg%\plmdebug.exe" /disableDebug %appname%
"%windbg%\plmdebug.exe" /disableDebug %appname%
"%windbg%\plmdebug.exe" /terminate %appname%

@rem https://docs.microsoft.com/en-us/windows/uwp/debug-test-perf/deploying-and-debugging-uwp-apps
@rem "%windbg%\plmdebug.exe" /enableDebug %appname% "%windbg%\windbg.exe"
@rem "%windbg%\plmdebug.exe" /enableDebug %appname% "z:\projects\drmingw\bin\drmingw.exe -v"
"%windbg%\plmdebug.exe" /enableDebug %appname% "z:\projects\apitrace\build\mingw64\inject.exe -D dxgitrace.dll"
copy /y build\mingw64\injectee.dll C:\Windows\SysWow64
copy /y build\mingw64\wrappers\dxgitrace.dll C:\Windows\SysWOW64

@if not "%appurl%"=="" (
  start %appurl%
) else (
  echo Run manually
)
pause

"%windbg%\plmdebug.exe" /terminate %appname%
"%windbg%\plmdebug.exe" /disableDebug %appname%

@endlocal
