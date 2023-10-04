#version 430 core

uniform mat4 vertexTransform;
uniform vec3 color;

in vec4 modelPosition;
in vec4 vertexNormal;

out VS_OUT
{
    vec3 fragDiffuse;
    vec4 fragNormal;

} vs_out;

void main()
{
    vs_out.fragDiffuse = color;
    vs_out.fragNormal = vertexNormal;

	gl_Position = vertexTransform * modelPosition;
}