#!/bin/bash

# ======================
#
#      BUILD PATH
#
# =======================

# Main output build script
PATH_BUILD_LIB := build/lib
PATH_BUILD_OBJ := build/obj
	
PATH_WEBSITE_ROOT := build/wasm
PATH_WEBSITE_WGPU := $(PATH_WEBSITE_ROOT)/scripts/wgpu
PATH_WEBSITE_SHADER := $(PATH_WEBSITE_WGPU)/wgpu_shader
PATH_WEBSITE_TEXTURE := $(PATH_WEBSITE_WGPU)/wgpu_texture
PATH_WEBSITE_GLTF := $(PATH_WEBSITE_WGPU)/wgpu_gltf
PATH_WEBSITE_MBIN := $(PATH_WEBSITE_WGPU)/wgpu_mbin

OUTPUT_WEBSITE_WGPU := $(PATH_WEBSITE_WGPU)/wgpu_scene.js
OUTPUT_WEBSITE_SHADER := $(PATH_WEBSITE_SHADER)/wgpu_shader.js
OUTPUT_WEBSITE_TEXTURE := $(PATH_WEBSITE_TEXTURE)/wgpu_texture.js
OUTPUT_WEBSITE_GLTF := $(PATH_WEBSITE_GLTF)/wgpu_gltf.js
OUTPUT_WEBSITE_MBIN := $(PATH_WEBSITE_MBIN)/wgpu_mbin.js


# ======================
#
#     FILES GETTERS
#
# =======================

PATH_LIB := ./include
C_EXCLUDE := ./resources/tool $(PATH_LIB)

# === project files ===
FILES_C := $(shell find . -name "*.c" $(foreach dir,$(C_EXCLUDE), ! -path "$(dir)/*"))
FILES_CXX := $(shell find . -name "*.cpp" $(foreach dir,$(C_EXCLUDE), ! -path "$(dir)/*"))

OBJS_C   := $(patsubst %.c,$(PATH_BUILD_OBJ)/%.o,$(FILES_C))
OBJS_CXX := $(patsubst %.cpp,$(PATH_BUILD_OBJ)/%.o,$(FILES_CXX))

# === library files ===
LIB_C := $(shell find $(PATH_LIB) -name "*.c")
LIB_CXX := $(shell find $(PATH_LIB) -name "*.cpp")

# === Build library objects ===
LIB_OBJS := $(patsubst $(PATH_LIB)/%,build/lib/%,$(LIB_C:.c=.o)) \
            $(patsubst $(PATH_LIB)/%,build/lib/%,$(LIB_CXX:.cpp=.o))

# Preprocess cwgsl shader to wgsl Shader files
PATH_WGSL := ./backend/std_pipeline/
CUSTOM_WGSL_EXT = .wgsl.in
CUSTOM_WGSL_IN := $(shell find $(PATH_WGSL) -type f -name "*$(CUSTOM_WGSL_EXT)")
COMPILE_WGSL := $(CUSTOM_WGSL_IN:$(CUSTOM_WGSL_EXT)=.wgsl)

# Get default wgsl files
FILES_WGSL := $(shell find $(PATH_WGSL) -type f -name "*.wgsl")

# Shader wgsl files
FILES_SHADER := $(addprefix --preload-file , $(COMPILE_WGSL) $(FILES_WGSL))  

PATH_FONT := ./resources/assets/font
PATH_GLTF := ./resources/assets/gltf
PATH_MBIN := ./resources/assets/mbin
PATH_TEXTURE := ./resources/assets/texture

FILES_FONT := $(shell find $(PATH_FONT) -type f -name "*.ttf" | sed "s|^|--preload-file &|")
FILES_GLTF := $(shell find $(PATH_GLTF) -type f -name "*.gltf" | sed "s|^|--preload-file &|")
FILES_MBIN := $(shell find $(PATH_MBIN) -type f -name "*.mbin" | sed "s|^|--preload-file &|")
FILES_TEXTURE := $(shell find $(PATH_TEXTURE) \( -name "*.png" -o -name "*.jpg" \) -type f | sed "s|^|--preload-file &|")


# ======================
#
# MACROS
#
#   - ENGINE_EDITOR : allow editor mode related functionnalities (gizmo...)
#
# SHADOW PASS:
#   - RENDER_SHADOW_AS_COLOR : render shadow maps as color texture in the shader
#
# AO BAKING: 
#   - AO_BAKE_DISPLAY_RAY : display raycast during AO Bake pass
#   - AO_BAKE_HIT_COUNT : print the hit count for each mesh
#
# COORDINATES:
#   - CGLM_FORCE_DEPTH_ZERO_TO_ONE
#
# PRINT:
#   - VERBOSE : enable verbose
#   - VERBOSE_BINDING_PHASE : print shader binding structure
#   - VERBOSE_BUILDING_PHASE : print mesh building phase
#   - VERBOSE_CREATING_PHASE: print entity creation (mesh, shaders)
#   - VERBOSE_SHADER_BIND_GROUP_OFFSET : print each bind group offset lists
#   - DEBUG_MALLOC : print each allocation size
#   - DEBUG_TIME : show marked functions execution time
#
# ======================

MACROS := \
       -DCGLM_FORCE_DEPTH_ZERO_TO_ONE \
       -DVERBOSE \
       -DENGINE_EDITOR \
       -DDEBUG_TIME \
       -DAO_BAKE_HIT_COUNT \

ASAN := \
	-fsanitize=address \
	-fsanitize=undefined \
	-ferror-limit=0 \
	-g

# ======================
# Dev mode
# Need to allow memory growth since ASan shadow memory
# Allocate 400MB+ on the heap on start creating a
# Heap overflow
# ======================
	
DEV_FLAGS := \
	-sALLOW_MEMORY_GROWTH=1 \
	-sMAXIMUM_MEMORY=1073741824 \
	-sINITIAL_MEMORY=67108864

PROD_FLAGS := \
	   -Os \
	   --closure 1

WASM_FLAGS := \
	   -s NO_EXIT_RUNTIME=1 \
	   -s "EXPORTED_RUNTIME_METHODS=['ccall']" \
	   -s EXPORTED_FUNCTIONS="['_main']" \
	   -s USE_WEBGPU=1 \
	   -s SINGLE_FILE  \

INCLUDE := \
	-Iinclude \
	-I. \



# =======================
#
#  COMPILATION COMMANDS
#
# =======================

all:
	@start=$$(date +%s); \
	echo "=== COMPILING SHADERS ==="; \
	make compile_shader; \
	echo ""; \
	echo "=== COMPILING TO WASM ==="; \
	make wasm; \
	echo ""; \
	echo "=== CLEANING SHADERS ==="; \
	make clean_shader; \
	end=$$(date +%s); \
	elapsed=$$((end - start)); \
	echo ""; \
	echo ">> Build time: $${elapsed}s"


mbin:
	$(MAKE) -C resources/tool/obj2mbin

compile_shader: $(COMPILE_WGSL) 

clean_shader:
	@rm -f $(COMPILE_WGSL)
	@echo "done"


bundle_shader:
	@echo "Bundling Shader Data..."; \
	make compile_shader; \
    	start=$$(date +%s); \
	mkdir -p $(PATH_WEBSITE_SHADER); \
	emcc $(FILES_SHADER) -o $OUTPUT_WEBSITE_SHADER; \
	end=$$(date +%s); \
	elapsed=$$((end - start)); \
	echo ">> Done ($${elapsed}s)"


build_objects: $(OBJS_C) $(OBJS_CXX) $(LIB_OBJS)
	
	
wasm:
	make build_objects
	$(shell mkdir -p $(PATH_WEBSITE_WGPU))
	em++ $(DEV_FLAGS) $(OBJS_C) $(OBJS_CXX) $(LIB_OBJS)\
	     -o $(OUTPUT_WEBSITE_WGPU) \
	     $(WASM_FLAGS) \
	     $(ASAN) \
	     $(FILES_SHADER) \
	     $(FILES_FONT) \
	     $(FILES_GLTF) \
	     $(FILES_MBIN) \
	     $(FILES_TEXTURE)

	@echo "Compilation completed: $(OUTPUT_WEBSITE_WGPU)"

serve:
	cd $(PATH_WEBSITE_ROOT)
	python -m http.server

clean:
	find . -name '*.o' -type f -delete

# ======================
#
#   CONVERSION RECEIPT
#
# =======================
	
%.wgsl: %$(CUSTOM_WGSL_EXT)
	cpp -P $(MACROS) $< > $@

$(PATH_BUILD_OBJ)/%.o: %.c
	mkdir -p $(dir $@)
	emcc -c $< -o $@ $(INCLUDE) $(MACROS) $(ASAN)

$(PATH_BUILD_OBJ)/%.o: %.cpp
	mkdir -p $(dir $@)
	em++ -c $< -o $@ $(INCLUDE) $(MACROS) $(ASAN)

#no ASan for lib files
	
$(PATH_BUILD_LIB)/%.o: $(PATH_LIB)/%.c
	mkdir -p $(dir $@)
	emcc -c $< -o $@ $(INCLUDE) $(MACROS)

$(PATH_BUILD_LIB)/%.o: $(PATH_LIB)/%.cpp
	mkdir -p $(dir $@)
	em++ -c $< -o $@ $(INCLUDE) $(MACROS)

#\
Use "ccall" as method call to access the wasm functions\
EXPORTED_RUNTIME_METHOD =>  Module.methodname("myfunction")\
EXPORTED_FUNCTIONS => Module._myFunction()\
NO_EXIT_RUNTIME => disable program exit after main, allow custom function exports
