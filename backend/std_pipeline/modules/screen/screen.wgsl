// attribute/uniform decls
struct VertexIn {
  @location(0) aPos : vec3<f32>,
                      @location(1) aNorm : vec3<f32>,
                                           @location(2) aTan : vec3<f32>,
                                                               @location(3) aCol
      : vec3<f32>,
        @location(4) aUv : vec2<f32>,
};

struct VertexOut {
  @builtin(position) Position : vec4<f32>,
                                @location(0) vCol : vec3<f32>,
                                                    @location(1) vUv : vec2<f32>
};

struct Mesh {
  model : mat4x4<f32>, position : vec4<f32>,
}

const SSBO_CAPACITY : u32 = 32u;
@group(0) @binding(0) var<storage, read> uMesh : array<Mesh>;

@group(1) @binding(0) var texture : texture_2d<f32>;
@group(1) @binding(1) var texture_sampler : sampler;

// vertex shader
@vertex fn vs_main(input : VertexIn) -> VertexOut {

  let mesh = uMesh[0];

  // Final Matrix (Projection * View)
  var output : VertexOut;
  let model_pos = mesh.model * vec4<f32>(input.aPos, 1.0f);

  output.Position = vec4<f32>(model_pos.xz, 0.0f, 1.0f);
  output.vCol = input.aCol;
  output.vUv = input.aUv;
  return output;
}

// fragment shader
@fragment fn fs_main(@location(1) vUv : vec2<f32>,
                     @location(0) vCol : vec3<f32>) -> @location(0) vec4<f32> {

  let t : vec4<f32> = vec4<f32>(0.0f, 0.0f, 0.0f, 1.0f);
  return t + textureSample(texture, texture_sampler, vUv);
}
