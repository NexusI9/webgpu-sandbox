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
                                                @location(4) vDepth : f32,
};

@group(0) @binding(0) var<uniform> light_view_projection : mat4x4<f32>;

@vertex fn vs_main(input : VertexIn) -> VertexOut {
  var out : VertexOut;
  let pos = light_view_projection * vec4<f32>(input.aPos, 1.0f);
  out.vPosition = pos;
  out.vDepth = pos.z / pos.w; // Normalized depth (NDC)
  return out;
}

// fragment shader
@fragment fn fs_main(@location(0) vNormal : vec3<f32>,
                     @location(1) vCol : vec3<f32>,
                     @location(2) vUv : vec2<f32>,
                     @location(3) vFrag : vec3<f32>, @location(4) vDepth : f32)
    -> @location(0) vec4<f32> {

  // calculate depth here as a Red color
  let depth_mapped = (vDepth + 1.0 * 0.5); // converts [-1;1] to [0;1]

  return vec4<f32>(1.0f, 0.0f, 1.0f, 1.0f);
  // return vec4<f32>(vec3<f32>(depth_mapped), 1.0f);
}
