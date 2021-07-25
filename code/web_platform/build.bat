::TODO: (Ted)	Define web platform layer path
@echo off
echo Building Low Rez Game Jam Engine Template (Web Assembly)

::TODO: (Ted)	Define Compiler Flags
setlocal
set web_platform_layer_path=..\..\code/web_platform
set build_directory=..\..\build\web_assembly

mkdir %build_directory%
pushd %build_directory%

echo Debug Build

set source=..\..\code\web_platform\game.html
set dest=game.html

echo %source% %dest%
copy %source% %dest%

popd

ENDLOCAL
