echo "Building Low Rez Game Engine (Web Assembly Target)"

WEB_PLATFORM_LAYER_PATH="../../code/web_platform"

COMPILER_FLAGS="-s ALLOW_MEMORY_GROWTH=1
                -fsanitize=address
                -s INITIAL_MEMORY=281804800
                -s ASSERTIONS=1
                -s ERROR_ON_UNDEFINED_SYMBOLS=0
                -s WASM=1
                -s USE_SDL=2"

BUILD_DIRECTORY="../../build/web_assembly"
mkdir -p $BUILD_DIRECTORY 
pushd $BUILD_DIRECTORY 

echo "Debug Build"
emcc "-Wno-writable-strings" -g ${WEB_PLATFORM_LAYER_PATH}/web_main.cpp -std=c++11 ${COMPILER_FLAGS} -DWEBASM=1 --profiling-funcs \
                                       --preload-file "../../resources/" -o game.js

cp ${WEB_PLATFORM_LAYER_PATH}/game.html "./game.html"
popd

echo "Finished Building Low Rez Game Engine (Web Assembly)"
