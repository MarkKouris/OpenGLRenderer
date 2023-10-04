#version 430 core

in VS_OUT
{
    vec3 fragDiffuse;
    vec4 fragNormal;

} fs_in;


out vec4 fragColor;

void main()
{
	fragColor = vec4(fs_in.fragDiffuse.xyz, 1.0f);
}