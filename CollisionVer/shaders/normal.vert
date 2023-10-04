//Implementation by Mark Kouris for CS350
//Assignment 1, 2/16/22 F21

#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT
{   
    vec3 normal;
} vs_out;

uniform mat4 view;
uniform mat4 model;
       
void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vs_out.normal = normalize(vec3(vec4(normalMatrix * aNormal, 0.0)));
    gl_Position = view * model * vec4(aPos, 1.0);
}