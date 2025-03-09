// attribute/uniform decls
struct VertexIn {
  @location(0) aPos : vec3<f32>,
                      @location(1) aNorm : vec3<f32>,
                                           @location(2) aCol : vec3<f32>,
                                                               @location(3) aUv
      : vec2<f32>,
};

struct VertexOut {
  @builtin(position) Position : vec4<f32>,
                                @location(0) vCol : vec3<f32>,
                                                    @location(1) vUv : vec2<f32>
};

struct Mesh {
  model : mat4x4<f32>, position : vec4<f32>,
}

struct Camera {
  view : mat4x4<f32>,
         position : vec4<f32>,
                    lookat : vec4<f32>,
                             mode : u32,
                                    _pad : vec3<u32>,
};

struct Viewport {
  projection : mat4x4<f32>,
}

// camera viewport
@group(0) @binding(0) var<uniform> uViewport : Viewport;
@group(0) @binding(1) var<uniform> uCamera : Camera;
@group(0) @binding(2) var<uniform> uMesh : Mesh;

// material pbr
//@group(1) @binding(0) var diffuse_Texture : texture_2d<f32>;
//@group(1) @binding(2) var normal_Texture : texture_2d<f32>;
//@group(1) @binding(3) var occlusion_Texture : texture_2d<f32>;
//@group(1) @binding(4) var emissive_Texture : texture_2d<f32>;

// vertex shader
@vertex fn vs_main(input : VertexIn) -> VertexOut {

  // Final Matrix (Projection * View)
  var cam : mat4x4<f32> = uViewport.projection * uCamera.view;

  var output : VertexOut;
  output.Position = cam * uMesh.model * vec4<f32>(input.aPos, 1.0);
  output.vCol = input.aCol;

  return output;
}

// fragment shader
@fragment fn fs_main(@location(0) vCol : vec3<f32>) -> @location(0) vec4<f32> {

  return vec4<f32>(1.0, 1.0, 1.0, 1.0);
}
