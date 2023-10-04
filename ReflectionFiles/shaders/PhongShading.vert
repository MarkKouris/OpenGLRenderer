//Implementation by Mark Kouris for CS300
//Assignment 3, 12/10/21 F21
#version 450 core
layout(std140, binding = 1) uniform matricies
{
    mat4 persp_matrix;
    mat4 view_matrix;
    mat4 model_matrix;
    mat4 normal_matrix;
};

layout (std140, binding = 0) uniform globalData
{
    vec4 eyePosition;
    vec3 attenuationConstants;
    float NS;
    vec3 diffuseCoef;
    float blendVal; // 1 is only reflection, 0 is only lights
    vec3 fogColor;
    float mappingType; // 1 is fresnel, 2 is reflect only, 3 is refract only (floating point issue maybe)
    vec3 ambientCoef;
    float refractionVal; //correlates to refractive index+
    vec3 globalAmbient;
    float chromaticAberration; // intensity, offsets the r g and b values so you can clearly see them.
    vec3 specularCoef;
    float fresnelPower; //0.1 is almost none, 10.0 is completely
    vec3 emissive;
    float pad6;
    vec2 fogDistances;
};


in vec4 position;
in vec4 normal;
out vec4 world_position;
out vec4 world_normal;
out vec4 incident;
out float ratio;
out float EtaG;
out float EtaR;
out float EtaB;
out vec4 viewing_vec;
out float viewLen;


void main()
{
    //setup variables:
    world_normal = normal_matrix  * vec4(normalize(normal.xyz), 0);    // convert to world space
    world_position = model_matrix * position;
    vec4 norm_world_normal = normalize(world_normal);
    EtaG = refractionVal;
    EtaR = EtaG - chromaticAberration;
    EtaB = EtaG + chromaticAberration;
    float F = ((1.0 - EtaG) * (1.0 - EtaG)) / ((1.0 + EtaG) * (1.0 + EtaG));
    viewing_vec = eyePosition - world_position;
    viewLen = length(viewing_vec);
    //viewing_vec = viewing_vec / viewLen;
    incident = -normalize(viewing_vec);
    ratio = F + (1.0 - F) * pow((1.0 - dot(-incident, world_normal)), fresnelPower);
    gl_Position = persp_matrix * view_matrix * world_position;
}
