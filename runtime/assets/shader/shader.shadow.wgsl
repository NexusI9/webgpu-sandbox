// attribute/uniform decls
struct VertexIn {
  @location(0) aPos : vec3<f32>,
                      @location(1) aNorm : vec3<f32>,
                                           @location(2) aCol : vec3<f32>,
                                                               @location(3) aUv
      : vec2<f32>,
};

struct VertexOut {
  @builtin(position) vPosition : vec4<f32>,
                                 @location(0) vNormal : vec3<f32>,
                                                        @location(1) vCol
      : vec3<f32>,
        @location(2) vUv : vec2<f32>,
                           @location(3) vFrag : vec3<f32>,
};

struct Mesh { // 80B
  model : mat4x4<f32>, position : vec4<f32>,
}

@group(0) @binding(0) var<uniform> light_view_projection : mat4x4<f32>;
@group(0) @binding(1) var<uniform> uModel : Mesh;

@vertex fn vs_main(input : VertexIn) -> VertexOut {
  var out : VertexOut;
  let pos = light_view_projection * uModel.model * vec4<f32>(input.aPos, 1.0f);
  out.vPosition = pos;
  return out;
}
