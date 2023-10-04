//Implementation by Mark Kouris for CS350
//Assignment 1, 2/16/22 F21

#version 450 core

in vec3 fragPos;
in vec3 fragNorm;
in vec3 fragUV;
// Writing the output data into our GBuffer
layout (location=0) out vec3 vPos;
layout (location=1) out vec3 normal;
layout (location=2) out vec3 UVs;
layout (location=3) out vec3 depthValue;

void main()
{
	vPos = fragPos;
	normal = normalize(fragNorm);
	UVs = fragUV;
	float distFromCamera = length(fragPos);
	depthValue = vec3(distFromCamera);
}
