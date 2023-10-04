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

uniform mat4 model;

uniform vec3 color;

in vec4 modelPosition;
in vec4 vertexNormal;

out VS_OUT
{
    vec4 worldPos;
    vec3 fragDiffuse;
    vec4 fragNormal;

} vs_out;

void main()
{
    vs_out.worldPos = model * modelPosition;
    vs_out.fragDiffuse = color;
    vs_out.fragNormal = vertexNormal;

	gl_Position = persp_matrix * view_matrix * model * modelPosition;
}