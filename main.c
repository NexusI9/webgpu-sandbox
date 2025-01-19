#import <stdio.h>
#define EMSCRIPTEN_KEEPALIVE

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN
#endif

//https://github.com/seyhajin/webgpu-wasm-c

typedef struct {
  int width;
  int height;
  int dpi;
} Context;

int main() {
  printf("WASM INIT\n");
  return 0;
}

// EMSCRIPTEN_KEEPALIVE make function available in web environment (not
// eliminated as DEAD code)
EXTERN EMSCRIPTEN_KEEPALIVE void setContext(int w, int h, int d) {
  const Context context = {.width = w, .height = h, .dpi = d};
  printf("%d\t%d\t%d\n", w, h, d);
}


