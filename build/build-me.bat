@setlocal

@set TMPSRC=..
@set TMPLOG=bldlog-1.txt
@set TMPGEN=Visual Studio 16 2019
@set TMPOPTS=-G "%TMPGEN%" -A x64
@set TMPOPTS=%TMPOPTS% -DCMAKE_INSTALL_PREFIX:PATH=C:\MDOS

@echo Being build %DATE% %TIME% >%TMPLOG%

cmake -S %TMPSRC% %TMPOPTS% >>%TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR1

cmake --build . --config Debug >>%TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR2

cmake --build . --config Release >>%TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR3
:DONEREL

@echo Appears successful...
@echo Continue with install, to C:\MDOS?
@ask Only 'y' continues. All else aborts...
@if ERRORLEVEL 2 goto NOASK
@if ERRORLEVEL 1 goto INSTALL
@echo Skipping install at this time...
@goto END

:NOASK
@echo Ask utility NOT found...
:INSTALL
@echo Continue with install? Only Ctrl+C aborts
@pause

cmake --build . --config Release --target INSTALL >>%TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR4

@echo All done...

@goto END

:ERR1
@echo cmake config, gen error
@goto ISERR

:Err2
@echo build debug error
@goto ISERR

:ERR3
@fa4 "mt.exe : general error c101008d:" %TMPLOG% >nul
@if ERRORLEVEL 1 goto ERR33
:ERR34
@echo build release error
@goto ISERR
:ERR33
@echo Try again for this STUPID STUPID STUPID error
@echo Try again for this STUPID STUPID STUPID error >>%TMPLOG%
cmake --build . --config Release >>%TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR34
@goto DONEREL

:ERR4
@echo install error
@goto ISERR

:ISERR
@echo See %TMPLOG% for details...
@endlocal
@exit /b 1

:END
@endlocal
@exit /b 0

@REM eof
