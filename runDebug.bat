@echo off

del "bin\Test2.8xp"
rem echo Old exe deleted


make clean
make debug
rem echo New exe created

if exist "bin\Test2.8xp" (
    cd emu

    CEmu.exe -r ti-84ce.rom -m "..\bin\Test2.8xp" -m "clibs.8xg" --launch "TEST2"

    cd ..\
) else (
    echo MAKE ERROR
)






