#!/bin/bash

C_FILES := $(shell find . -type f -name "*.c")
WGSL_FILES := $(shell find ./runtime/assets/shader -type f -name "*.wgsl" | sed 's/^/--preload-file /')
OUTPUT := build/scripts/wgpu/wgpu_scene.js

wasm:
	emcc $(C_FILES) -o $(OUTPUT) \
	-s NO_EXIT_RUNTIME=1 \
	-s "EXPORTED_RUNTIME_METHODS=['ccall']" \
	-s EXPORTED_FUNCTIONS="['_main']" \
	-s USE_WEBGPU=1 \
	-s SINGLE_FILE \
	$(WGSL_FILES)

	@echo "Compilation completed: $(OUTPUT)"

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
