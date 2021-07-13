echo "Building Low Rez Game Engine (Web Assembly Target)"

WEB_PLATFORM_LAYER_PATH="../../code/web_platform/"

COMPILER_FLAGS="-s ALLOW_MEMORY_GROWTH=1
                -fsanitize=address
                -s INITIAL_MEMORY=281804800
                -s ASSERTIONS=1
                -s ERROR_ON_UNDEFINED_SYMBOLS=0
                -s WASM=1
                -s USE_SDL=2"

mkdir -p ../../build/web_assembly
pushd ../../build/web_assembly
