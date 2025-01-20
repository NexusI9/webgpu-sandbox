
#include <stdio.h>
#include <emscripten/emscripten.h>


// HEADERS
#include "emscripten/html5_webgpu.h"
#include "include/file.h"
#include "include/webgpu.h"

// MACROS
//#define EMSCRIPTEN_KEEPALIVE

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN
#endif

//  RESOURCES
//  https://github.com/seyhajin/webgpu-wasm-c
//  https://developer.chrome.com/docs/web-platform/webgpu/build-app?hl=en
//  https://stackoverflow.com/questions/23997312/how-do-i-read-a-user-specified-file-in-an-emscripten-compiled-library

typedef struct {

  struct {
    const char *name;
    int width;
    int height;
    int dpi;
  } context;

  struct {
    WGPUInstance instance;
    WGPUDevice device;
    WGPUQueue queue;
    WGPUSurface surface;
    WGPURenderPipeline pipeline;
  } wgpu;

  struct {
    WGPUBuffer v_buffer, i_buffer, u_buffer;
    WGPUBindGroup bind_group;
  } store;

} state_t;

static state_t state;

int main() {
  printf("WASM INIT\n");

  // Loading shader
  void *shader;
  const char *shader_path = "./shader/default.wgsl";
  store_file(shader, shader_path);

  // setup state
  state.context.name = "canvas";
  state.wgpu.instance = wgpuCreateInstance(NULL);
  state.wgpu.device = emscripten_webgpu_get_device();

      return 0;
}

// EMSCRIPTEN_KEEPALIVE make function available in web environment (not
// eliminated as DEAD code)
EXTERN EMSCRIPTEN_KEEPALIVE void setContext(int w, int h, int d) {
  state.context.width = w;
  state.context.height = h;
  state.context.dpi = d;

  printf("%d\t%d\t%d\n", w, h, d);
}
