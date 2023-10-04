//Implementation by Mark Kouris for CS350
//Assignment 1, 2/16/22 F21
#version 450 core
layout(std140, binding = 1) uniform matricies
{
    mat4 persp_matrix;
    mat4 view_matrix;
    mat4 model_matrix;
    mat4 normal_matrix;
};

layout (location = 0) in vec4 modelPos;
layout (location = 1) in vec4 modelNorm;
layout (location = 2) in vec2 modelUV;

out vec3 fragPos;
out vec3 fragNorm;
out vec3 fragUV;

void main()
{
    fragPos = vec4(model_matrix * modelPos).xyz;
    fragNorm = vec4(normal_matrix * modelNorm).xyz;
    fragUV = vec3( modelUV.st, 1.0 );
    gl_Position = persp_matrix * view_matrix * model_matrix * modelPos;
}
