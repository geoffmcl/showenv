@setlocal
@if EXIST CMakeCache.txt goto NOWAY
@if EXIST CMakeFiles\nul goto NOWAY
@if EXIST bldlog-1.txt goto NOWAY
@if EXIST Release\nul goto NOWAY
@if EXIST Debug\nul goto NOWAY

@set TMPVER=05
@set TMPZIP=%DOWNLOADS%\showenv-%TMPVER%.zip
@set TMPOPTS=-a
@if EXIST %TMPZIP% (
@call dirmin %TMPZIP%
@echo This is an update
@set TMPOPTS=-u
)

@set TMPOPTS=%TMPOPTS% -r -p

@cd ..
@echo In folder %CD%
@echo Will do: 'call zip8 %TMPOPTS% %TMPZIP% *'
@echo *** CONTINUE? *** Only ctrl+c aborts...
@pause

call zip8 %TMPOPTS% %TMPZIP% *

@goto END

:NOWAY
@echo ERROR: There appears to be build products in this folder - run cmake-clean.bat first...
@goto END


:END
@Endlocal

@REM eof
