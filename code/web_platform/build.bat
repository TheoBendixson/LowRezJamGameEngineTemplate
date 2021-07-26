@echo off
echo Building Low Rez Game Jam Engine Template (Web Assembly)

::TODO: (Ted)	Define Compiler Flags
setlocal
set web_platform_layer_path=..\..\code/web_platform
set build_directory=..\..\build\web_assembly

set compiler_flags=-s ALLOW_MEMORY_GROWTH=1 ^
		   -fsanitize=address -s ^
		   INITIAL_MEMORY=281804800 ^
		   -s ASSERTIONS=1 ^
		   -s ERROR_ON_UNDEFINED_SYMBOLS=0 ^
		   -s WASM=1 ^
		   -s USE_SDL=2

mkdir %build_directory%
pushd %build_directory%

echo Debug Build
emcc "-Wno-writable-strings" -g %web_platform_layer_path%\web_main.cpp -std=c++11 %compiler_flags% -DWEBASM=1 ^
     --profiling-funcs --preload-file "..\..\resources/" -o game.js

echo Copying other files to build directory
set html_source=..\..\code\web_platform\game.html
set html_dest=game.html

copy %html_source% %html_dest%

popd

ENDLOCAL
