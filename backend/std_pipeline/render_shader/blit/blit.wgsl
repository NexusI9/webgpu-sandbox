@group(0) @binding(0) var myTexture : texture_2d<f32>;
@group(0) @binding(1) var mySampler : sampler;


// https://stackoverflow.com/questions/2588875/whats-the-best-way-to-draw-a-fullscreen-quad-in-opengl-3-2
struct VertexOut {
  @builtin(position) Position: vec4<f32>,
  @location(0) vUv: vec2<f32>
};

  @vertex
fn vs_main(@builtin(vertex_index) vertex_index: u32) -> VertexOut {
    var vertices = array<vec2<f32>, 3>(
        vec2<f32>(-1.0, -1.0), vec2<f32>(3.0, -1.0), vec2<f32>(-1.0, 3.0)
    );

    var out: VertexOut;
    out.Position = vec4<f32>(vertices[vertex_index], 0.0, 1.0);
    out.vUv = vec2<f32>(0.5, -0.5) * out.Position.xy + vec2<f32>(0.5, 0.5);
    return out;
}

    @fragment
fn fs_main(@location(0) fragUV: vec2<f32>) -> @location(0) vec4<f32> {
    return textureSample(myTexture, mySampler, fragUV);
}
