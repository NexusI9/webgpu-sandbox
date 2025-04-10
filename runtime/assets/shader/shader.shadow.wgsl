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
                           @location(3) vFrag : vec4<f32>,
};

struct Mesh { // 80B
  model : mat4x4<f32>, position : vec4<f32>,
}

@group(0) @binding(0) var<uniform> light_view_projection : mat4x4<f32>;
@group(0) @binding(1) var<uniform> uModel : Mesh;
@group(0) @binding(2) var<uniform> id : f32;

@vertex fn vs_main(input : VertexIn) -> VertexOut {
  var out : VertexOut;

  out.vFrag = vec4<f32>(input.aPos, 1.0f) * uModel.model;
  out.vPosition = light_view_projection * out.vFrag;
  return out;
}

@fragment fn fs_main(in : VertexOut) -> @location(0) vec4<f32> {
  let ndc = in.vPosition.xyz;
  let depth = in.vPosition.z / in.vPosition.w;
  let color = ndc;
  // return vec4<f32>(vec3<f32>(depth), 1.0f);
  // return vec4<f32>(in.vFrag.xyz, 1.0f);
  //  return vec4<f32>(vec3<f32>(abs(uModel.model[3][0]),
  //  abs(uModel.model[3][1]),
  //                            abs(uModel.model[3][2])),
  //                 1.0f);

  let new_id = id % 0.0f;
  return vec4<f32>(1.0f, 0.0f, 0.0f, 1.0f);
}
