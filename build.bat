@echo off
echo Компиляция BookProcessor...
cd /d "d:\CookPrograms\AllProg\BookProcessor\build"
cmake --build . --config Release
echo Готово!
pause