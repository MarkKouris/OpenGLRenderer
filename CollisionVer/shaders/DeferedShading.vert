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

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vUV;
// Interpolating vertex attributes over the rasterizer
out VS_OUT
{
vec4 vertexPosition; // interpolated vPosition
vec2 vertexUV; // interpolated vUV
} vs_out;
void main()
{
    vs_out.vertexPosition = vec4( vPosition, 1.0f );
    vs_out.vertexUV = vUV;
    gl_Position = vs_out.vertexPosition;
}
