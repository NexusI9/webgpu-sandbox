struct VertexIn {
  @location(0) aPos : vec3<f32>,
};

struct VertexOut {
  @builtin(position) vPosition : vec4<f32>,
};

@group(0) @binding(0) var<uniform> light_view_projection : mat4x4<f32>;

@vertex fn vs_main(input : VertexIn) -> VertexOut {
  var out : VertexOut;
  out.vPosition = light_view_projection * vec4<f32>(input.aPos, 1.0f);
  return out;
}
