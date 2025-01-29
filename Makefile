wasm:
	emcc \
utils/file.c \
backend/generator.c \
runtime/scene.c \
runtime/camera.c \
runtime/viewport.c \
runtime/mesh.c \
runtime/shader.c \
main.c \
-o build/wgpu_scene.js \
-s NO_EXIT_RUNTIME=1  \
-s "EXPORTED_RUNTIME_METHODS=['ccall']" \
-s EXPORTED_FUNCTIONS="['_main']" \
-s USE_WEBGPU=1 \
--preload-file ./shader/rotation.wgsl \
--preload-file ./shader/default.wgsl

#\
Use "ccall" as method call to access the wasm functions\
EXPORTED_RUNTIME_METHOD =>  Module.methodname("myfunction")\
EXPORTED_FUNCTIONS => Module._myFunction()\
NO_EXIT_RUNTIME => disable program exit after main, allow custom function exports


compile:
	clang main.c && ./a.out

serve:
	cd ./build
	python -m http.server
