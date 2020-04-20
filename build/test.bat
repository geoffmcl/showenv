@echo Testing showenv ...
@set TEMP1=Release\showenv.exe
@REM set TEMP1=Release\showenv64.exe
@if NOT EXIST %TEMP1% goto ERR1
%TEMP1%
@if ERRORLEVEL 2 goto TEST12
@if ERRORLEVEL 1 goto TEST11
@if ERRORLEVEL 0 goto TEST10
@echo WHAT IS ERRORLEVEL [%ERRORLEVEL%] ...
@goto DONE1

:TEST12
@echo Got errorlevel 2 [%ERRORLEVEL%] ...
@echo This is CORRECT!
@goto DONE1
:TEST11
@echo Got errorlevel 1 [%ERRORLEVEL%] ...
@goto DONE1
:TEST10
@echo Got errorlevel 0 [%ERRORLEVEL%] ...
@goto DONE1

:DONE1

%TEMP1% PATHEXT -t.com
@if ERRORLEVEL 2 goto TEST22
@if ERRORLEVEL 1 goto TEST21
@if ERRORLEVEL 0 goto TEST20
@echo WHAT IS ERRORLEVEL [%ERRORLEVEL%] ...
@goto DONE2

:TEST22
@echo Got errorlevel 2 [%ERRORLEVEL%] ...
@goto DONE2
:TEST21
@echo Got errorlevel 1 [%ERRORLEVEL%] ...
@echo This is CORRECT!
@goto DONE2
:TEST20
@echo Got errorlevel 0 [%ERRORLEVEL%] ...
@goto DONE2

:DONE2
%TEMP1% PATH -t"C:\Program Files\Subversion\bin"
@if ERRORLEVEL 2 goto TEST32
@if ERRORLEVEL 1 goto TEST31
@if ERRORLEVEL 0 goto TEST30
@echo WHAT IS ERRORLEVEL [%ERRORLEVEL%] ...
@goto DONE3

:TEST32
@echo Got errorlevel 2 [%ERRORLEVEL%] ...
@goto DONE3
:TEST31
@echo Got errorlevel 1 [%ERRORLEVEL%] ...
@echo This is CORRECT!
@goto DONE3
:TEST30
@echo Got errorlevel 0 [%ERRORLEVEL%] ...
@goto DONE3

:DONE3

%TEMP1% PATH -t"C:\Program xxx Files\Subversion\bin"
@if ERRORLEVEL 2 goto TEST42
@if ERRORLEVEL 1 goto TEST41
@if ERRORLEVEL 0 goto TEST40
@echo WHAT IS ERRORLEVEL [%ERRORLEVEL%] ...
@goto DONE4

:TEST42
@echo Got errorlevel 2 [%ERRORLEVEL%] ...
@goto DONE4
:TEST41
@echo Got errorlevel 1 [%ERRORLEVEL%] ...
@goto DONE4
:TEST40
@echo Got errorlevel 0 [%ERRORLEVEL%] ...
@echo This is CORRECT!
@goto DONE4

:DONE4
%TEMP1% INCLUDE -t"C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Bin" -v0
@if ERRORLEVEL 2 goto TEST52
@if ERRORLEVEL 1 goto TEST51
@if ERRORLEVEL 0 goto TEST50
@echo WHAT IS ERRORLEVEL [%ERRORLEVEL%] ...
@goto DONE5

:TEST52
@echo Got errorlevel 2 [%ERRORLEVEL%] ...
@goto DONE4
:TEST51
@echo Got errorlevel 1 [%ERRORLEVEL%] ...
@goto DONE4
:TEST50
@echo Got errorlevel 0 [%ERRORLEVEL%] ...
@echo This is CORRECT!
@goto DONE5

:DONE5




@goto END

:Err1
@echo Can NOT locate [%TEMP1%] ... check name and location ...
@goto END


:END
