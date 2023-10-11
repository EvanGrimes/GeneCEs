@echo off

del "bin\Test2.8xp"
echo Old exe deleted


rem make gfx
make
echo New exe created

cd emu

CEmu.exe -r ti-84ce.rom -m "..\bin\Test2.8xp" --launch "TEST2"

cd ..\


