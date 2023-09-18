echo Set up the environment for building Qt things
set PATH=C:\Qt\Qt5.12.12\5.12.12\mingw73_64\bin;C:/Qt/Qt5.12.12/Tools/mingw730_64\bin;%PATH%
echo Go to where we want to build
mkdir C:\Users\mm\Programming\Cpp\cock-heroine-release
cd /d C:\Users\mm\Programming\Cpp\cock-heroine-release
echo Build the project
qmake ..\cock-heroine\CockHeroine.pro
make release
cd release
echo remove the bits we don't need
rm *.o *.cpp *.h
echo pull in the libraries needed
windeployqt CockHeroine.exe
pause