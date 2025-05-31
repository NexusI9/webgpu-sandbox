#!/bin/bash

# C files
C_EXCLUDE := ./resources/tool
PRUNE_ARGS := $(foreach dir,$(C_EXCLUDE),-path $(dir) -prune -o)
C_FILES := $(shell find . $(PRUNE_ARGS) -name "*.c" -print)

# Macros:
#
# SHADOW PASS:
#   - RENDER_SHADOW_AS_COLOR : render shadow maps as color texture in the shader
#
# AO BAKING: 
#   - AO_BAKE_DISPLAY_RAY : display raycast during AO Bake pass
#   - AO_GLOBAL_SELF : enable comparison with source mesh itself during global phase
#
# COORDINATES:
#   - CGLM_FORCE_DEPTH_ZERO_TO_ONE
#
# PRINT:
#   - VERBOSE : enable verbose
#   - VERBOSE_BINDING_PHASE : print shader binding structure
#   - VERBOSE_BUILDING_PHASE : print mesh building phase
#
MACROS := -DCGLM_FORCE_DEPTH_ZERO_TO_ONE -DVERBOSE

# Preprocess cwgsl shader to wgsl Shader files
SHADER_DIR := ./runtime/assets/shader
CUSTOM_WGSL_EXT = .wgsl.in
CUSTOM_WGSL_IN := $(shell find $(SHADER_DIR) -type f -name "*$(CUSTOM_WGSL_EXT)")
COMPILE_WGSL := $(CUSTOM_WGSL_IN:$(CUSTOM_WGSL_EXT)=.wgsl)

# Get default wgsl files
WGSL_FILES := $(shell find $(SHADER_DIR) -type f -name "*.wgsl")

# Shader wgsl files
SHADER_FILES := $(addprefix --preload-file , $(COMPILE_WGSL) $(WGSL_FILES))  

# GLTF files
GLTF_FILES := $(shell find ./resources/assets/gltf -type f -name "*.gltf" | sed 's/^/--preload-file /')

#MBIN files
MBIN_FILES := $(shell find ./resources/assets/mbin -type f -name "*.mbin" | sed 's/^/--preload-file /')

#Textures files
TEXTURE_FILES := $(shell find ./resources/assets/texture -type f -name "*.png" | sed 's/^/--preload-file /')


# Main output build script
OUTPUT := build/scripts/wgpu/wgpu_scene.js

all:
	@echo "=== COMPILING SHADERS ==="
	@make compile_shader
	@echo
	@echo "=== COMPILING TO WASM ==="
	@make wasm
	@echo	
	@echo "=== CLEANING SHADERS ==="
	@make clean_shader


mbin:
	$(MAKE) -C resources/tool/obj2mbin

compile_shader: $(COMPILE_WGSL) 

clean_shader:
	@rm -f $(COMPILE_WGSL)
	@echo "done"

wasm:
	@emcc $(MACROS) $(C_FILES) -o $(OUTPUT) \
		-I include \
		-s NO_EXIT_RUNTIME=1 \
		-s "EXPORTED_RUNTIME_METHODS=['ccall']" \
		-s EXPORTED_FUNCTIONS="['_main']" \
		-s USE_WEBGPU=1 \
		-s SINGLE_FILE \
		$(SHADER_FILES) \
		$(GLTF_FILES) \
		$(MBIN_FILES) \
		$(TEXTURE_FILES)

	@echo "Compilation completed: $(OUTPUT)"

serve:
	cd ./build
	python -m http.server


%.wgsl: %$(CUSTOM_WGSL_EXT)
	cpp -P $(MACROS) $< > $@


#\
Use "ccall" as method call to access the wasm functions\
EXPORTED_RUNTIME_METHOD =>  Module.methodname("myfunction")\
EXPORTED_FUNCTIONS => Module._myFunction()\
NO_EXIT_RUNTIME => disable program exit after main, allow custom function exports
