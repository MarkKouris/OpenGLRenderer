//Implementation by Mark Kouris for CS350
//Assignment 1, 2/16/22 F21
#version 450 core

in VS_OUT
{
    vec4 worldPos;
    vec3 fragDiffuse;
    vec4 fragNormal;

} fs_in;


out vec4 fragColor;

void main()
{
	fragColor = vec4(fs_in.fragDiffuse.xyz, 1.0f);
}