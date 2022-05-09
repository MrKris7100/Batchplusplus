@echo off
@mode con cols=100 lines=50
if exist tmp.bps del tmp.bps /q
setlocal enabledelayedexpansion
set sizeX=20
set sizeY=20
set palette=0123456789abcdef
set color=f
set modified=no
call :drawUI
:input
set "button="
for /f "tokens=1,2,3,4" %%A in ('bpp input') do (
	set inputButton=%%A
	set inputX=%%B
	set inputY=%%C
	set inputKey=%%D
)
if "%inputButton%"=="1" if %inputX% GEQ 0 if %inputX% LEQ 5 if "%inputY%"=="0" (
	bpp capture tmp.bps
	bpp box 0 0 6 11 noshadow "button 0 0  File " "label 0 1 ------" "button 0 2  New  " "label 0 3 ------" "button 0 4  Save " "label 0 5 ------" "button 0 6  Open " "label 0 7 ------" "button 0 8  Size " "label 0 9 ------" "button 0 10  Exit "
	call return.bat
	if "!button!" == " Save " (
		call :saveDialog
		if "!button!"==" Save " (
			call :drawUI
			bpp capture "!input0!" 0 1 %sizeX% %sizeY%
			set modified=no
		)
	)
	if "!button!" == " New  " if "%modified%"=="yes" call :saveFirst
	if "!button!" == " Open " (
		if "%modified%"=="yes" (
			call :saveFirst
			call :openDialog
		) else (
			call :openDialog
		)
	)
	if "!button!" == " Exit " (
		if "%modified%"=="yes" call :saveFirst
		exit
	)
	call :drawUI
)
if "%inputButton%"=="1" if "%inputY%"=="0" (
	for /l %%A in (0, 1, 15) do (
		set/a pos=68+%%A*2
		if "!inputX!"=="!pos!" set color=!palette:~%%A,1!
	)
)
if "%inputButton%"=="1" if %inputY% GTR 0 if %inputY% LEQ %sizeY% if %inputX% LEQ %sizeX% (
	bpp text " " %inputX% %inputY% %color%0
	if "%modified%"=="no" set modified=yes
)
goto input
:saveFirst
call :savePrompt
call :drawUI
if "!button!" == " Yes " (
	call :saveDialog
	if "!button!"==" Save " (
		call :drawUI
		bpp capture "!input0!" 0 1 %sizeX% %sizeY%
		call :newFile
	)
)
if "!button!" == " No " call :newFile
goto :EOF
:newFile
del tmp.bps /q
call :drawUI
set modified=no
goto :EOF
:drawUI
if exist tmp.bps (
	bpp sprite tmp.bps
) else (
	cls
	bpp rect " " 0 0 100 1 1 f0
	for /l %%A in (0, 1, 15) do (
		set/a pos=68+%%A*2
		set/a pos2=!pos!-1
		bpp text " " !pos! 0 !palette:~%%A,1!0
		bpp text "³" !pos2! 0 f0
	)
	bpp text "³" 99 0 f0
	bpp text " File " 0 0 cf
)
goto :EOF
:savePrompt
bpp box 30 22 32 5 "title Save file?" "label 1 1 Do you wanna save image first?" "button 1 3  Yes " "button 7 3  No " "button 12 3  Cancel "
call return.bat
goto :EOF
:saveDialog
call :drawUI
bpp box 30 22 32 7 "title Where save file?" "label 1 1 Where you wanna save file?" "label 1 3 Path:" "input 7 3 24" "button 5 5  Save " "button 19 5  Cancel "
call return.bat
goto :EOF
:openDialog
echo open file
pause
:goto :EOF