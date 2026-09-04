
#version 450

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec4 inColor;
layout(location = 2) in float inRotation;
layout(location = 3) in float inVariation;

layout(location = 0) out vec4 fragColor;

const float PI = 3.14159265359;

void main() {
    float baseAngle = atan(inUV.y, inUV.x);
    float radius = length(inUV);
    float angle = baseAngle + inRotation;

    float segment = PI / 3.0;
    float localAngle = abs(mod(angle, segment) - segment * 0.5);
    vec2 localUV = radius * vec2(cos(localAngle), sin(localAngle));

    // --- Dynamic Crystal Morphing ---
    float varSeedA = fract(inVariation * 1.414);
    float varSeedB = fract(inVariation * 3.141);

    float dynamicCoreSize = 0.12 + (varSeedA * 0.20);
    float branch1Position = 0.15 + (varSeedB * 0.15);
    float branch2Position = 0.40 + (varSeedA * 0.20);
    float branchSlope1 = 0.4 + (varSeedA * 0.3);
    float branchSlope2 = 0.5 + (varSeedB * 0.4);
    float spineThickness = 0.03 + (varSeedB * 0.02);

    // --- Generate Shape Components ---
    float mainSpine = smoothstep(spineThickness, 0.0, abs(localUV.y)) * smoothstep(0.95, 0.90, localUV.x);

    float branchOffset1 = smoothstep(0.018, 0.0, abs(localUV.y - (localUV.x - branch1Position) * branchSlope1))
                          * step(branch1Position, localUV.x)
                          * step(localUV.x, branch1Position + 0.25);

    float branchOffset2 = smoothstep(0.015, 0.0, abs(localUV.y - (localUV.x - branch2Position) * branchSlope2))
                          * step(branch2Position, localUV.x)
                          * step(localUV.x, branch2Position + 0.30);

    float hexCore = smoothstep(dynamicCoreSize, dynamicCoreSize - 0.02, localUV.x + localUV.y * 0.57735);

    float flakeMask = max(mainSpine, max(branchOffset1, max(branchOffset2, hexCore)));

    if (flakeMask < 0.1) {
        discard;
    }

    fragColor = vec4(inColor.rgb, inColor.a * flakeMask);
}
