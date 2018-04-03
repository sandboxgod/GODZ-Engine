echo off

for /f %%f in ('dir /b *.fx') do (call :compile_shader %%f)

goto :end

:compile_shader
Set _File1=%1
Set _File2=%1o
if exist %_File2% ( call :compile_shader_compare %_File1% ) else ( call :fxc %_File1% %_File2% )
goto :eof

:compile_shader_compare
Set _File1=%1
Set _File2=%1o
For /F "Delims=" %%I In ('xcopy /DHYL %_File1% %_File2% ^|Findstr /I "File"') Do set /a _Newer=%%I 2>Nul
If %_Newer%==1 ( call :fxc %_File1% %_File2% )
goto :eof

:fxc
"%DXSDK_DIR%\Utilities\bin\x86\fxc.exe" /Tfx_2_0 /nologo /Fo %2 %1
if ERRORLEVEL 1 goto :error
goto :eof

:error
pause
goto :eof

:end
echo on
