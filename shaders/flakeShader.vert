
#version 450

layout(push_constant) uniform PushConstants {
    vec2 position;
    vec2 scale;
    vec4 color;
    float rotation;
    float variation;
} push;

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec4 outColor;
layout(location = 2) out float outRotation;
layout(location = 3) out float outVariation;

const vec2 quadPositions[4] = vec2[](
    vec2(-1.0, -1.0),
    vec2( 1.0, -1.0),
    vec2(-1.0,  1.0),
    vec2( 1.0,  1.0)
);

void main() {
    vec2 localPos = quadPositions[gl_VertexIndex];

    outUV = localPos;
    outColor = push.color;
    outRotation = push.rotation;
    outVariation = push.variation;

    vec2 worldPos = push.position + localPos * push.scale;
    gl_Position = vec4(worldPos, 0.0, 1.0);
}
