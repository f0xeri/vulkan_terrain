#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;
layout (location = 3) in vec2 vTexCoord;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec2 outUV;

layout(set = 0, binding = 0) uniform CameraBuffer {
    mat4 view;
    mat4 proj;
    mat4 modelview;
    vec4 lightPos;
    vec4 frustumPlanes[6];
    float displacementFactor;
    float tessellationFactor;
    vec2 viewportDim;
    float tessellatedEdgeSize;
} cameraData;

layout(push_constant) uniform constants {
    mat4 model;
} inConstants;

void main()
{
    gl_Position = vec4(vPosition.xyz, 1.0);
    outNormal = vNormal;
    outUV = vTexCoord;
}