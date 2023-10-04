//Implementation by Mark Kouris for CS300
//Assignment 3, 12/10/21 F21
#version 450 core
//will need to account for spotlight effect

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


//light struct, mirrored from c++ code
struct light
{
    vec4 position; //for point and spot
    vec4 direction; //for spot and directional
    vec3 diffuse;
    int type; // 0 = point, 1 = direction, 2 = spot
    vec3 ambient;
    int enabled; //1 is yes, 0 is no 
    vec3 specular;
    int angularPosition;
    float innerAngle;
    float outerAngle;
    float fallout;
};

//uniform block for lights
layout (std140, binding = 2) uniform lightData
{
    light l[16];
};

uniform sampler2D sideNegX; //index 0
uniform sampler2D sidePosX; //index 1
uniform sampler2D sideNegY; //index 2
uniform sampler2D sidePosY; //index 3
uniform sampler2D sideNegZ; //index 4
uniform sampler2D sidePosZ; //index 5

vec3 CalcCubeMap(vec3 vEntity)
{
    vec3 absVec = abs(vEntity);
    vec3 uv = vec3(0.0);
    // +-X
    if (absVec.x >= absVec.y && absVec.x >= absVec.z)
    {
        (vEntity.x < 0.0) ? (uv.x = vEntity.z, uv.z = 1) : (uv.x = -vEntity.z, uv.z = 0);
        uv.y = vEntity.y;
    }

    // +-Y
    else if (absVec.y >= absVec.x && absVec.y >= absVec.z)
    {
        (vEntity.y < 0.0) ? (uv.y = vEntity.z, uv.z = 3) : (uv.y = -vEntity.z, uv.z = 2);
        uv.x = vEntity.x;
    }
    // +-Z
    else if (absVec.z >= absVec.x && absVec.z >= absVec.y)
    {
        (vEntity.z < 0.0) ? (uv.x = vEntity.x, uv.z = 5) : (uv.x = -vEntity.x, uv.z = 4);
        uv.y = vEntity.y;
    }
    // and so on … +-Y and +-Z
    // At this point, uv.x and uv.y should be in the range [-1, 1]
    // Convert range from -1 to 1 to 0 to 1
     uv.x = (uv.x + 1) * 0.5f;
    uv.y = (uv.y + 1) * 0.5f;
    return uv;
}

in vec4 world_position; // P
in vec4 world_normal; //m vec
in vec4 incident;
in float ratio;
in float EtaG;
in float EtaR;
in float EtaB;
in vec4 viewing_vec;
in float viewLen;
out vec4 frag_color;

vec4 calcTotalLighting(vec4 world_position, vec4 world_normal)
{
    //make sure the normals are normalized
    vec4 norm_world_normal = normalize(world_normal);

    //calculates the final light intensity
    vec3 I_local = emissive + (globalAmbient * ambientCoef);

    //start of summation:
    for (int i = 0; i < 16; ++i)
    {
        if (l[i].enabled == 1)
        {
            vec4 newPos =  l[i].position;
            vec4 newDir = l[i].direction;

            if (l[i].type == 0) //point light
            {
                //transform position to correct space   

                // Calc light Vector, L - A
                vec4 light_vec = newPos - world_position;
                float lightDist = length(light_vec);
                light_vec = normalize(light_vec);

                // Calc view vector, eyePos - A

                // Calc Reflection Vector
                float direction_helper = dot(norm_world_normal.xyz, light_vec.xyz);
                vec4 reflection = normalize((2 * direction_helper * world_normal) - light_vec);

                //calc ambient intensity
                vec3 I_amb = l[i].ambient * ambientCoef;

                //calc diffuse intensity
                vec3 I_diff = l[i].diffuse * diffuseCoef * max(direction_helper, 0);

                //calc specular intensity
                float r_dot_v = dot(reflection.xyz, viewing_vec.xyz);
                if (r_dot_v < 0 || direction_helper < 0)
                {
                    r_dot_v = 0; //clamps the value to 0 if neg
                }

                vec3 I_spec = l[i].specular * specularCoef * pow(max(r_dot_v,0), NS);


                //calc attenuation
                float Att = 1.0f / (attenuationConstants.x +
                    attenuationConstants.y * lightDist +
                    attenuationConstants.z * (lightDist * lightDist));
                Att = min(Att, 1.0f);

                //calc the local intensity with the method for point lights:
                I_local += Att * (I_amb + I_diff + I_spec);
            }
            else if(l[i].type == 1) //directional light
            {
                //transform position to correct space   

                // Calc light Vector, L - A
                vec4 light_vec = newDir ;
                float lightDist = length(light_vec);
                light_vec = normalize(light_vec);

                // Calc Reflection Vector
                float direction_helper = dot(norm_world_normal.xyz, light_vec.xyz);
                vec4 reflection = normalize((2 * direction_helper * norm_world_normal) - light_vec);

                //calc ambient intensity
                vec3 I_amb = l[i].ambient * ambientCoef;

                //calc diffuse intensity
                vec3 I_diff = l[i].diffuse * diffuseCoef * max(direction_helper, 0);

                //calc specular intensity
                float r_dot_v = dot(reflection.xyz, viewing_vec.xyz);
                if (r_dot_v < 0 || dot(norm_world_normal, light_vec) < 0)
                {
                    r_dot_v = 0; //clamps the value to 0 if neg
                }

                vec3 I_spec = l[i].specular * specularCoef * pow(max(r_dot_v,0), NS);

                //calc the local intensity with the method for point lights:
                I_local += (I_amb + I_diff + I_spec);
            }
            else if (l[i].type == 2) //spot light, so we need the SpotlightEffect
            {
                // Calc light Vector, L - A
                vec4 light_vec = newPos - world_position;
                float lightDist = length(light_vec);
                light_vec = normalize(light_vec);

                // Calc Reflection Vector
                float direction_helper = dot(norm_world_normal.xyz, light_vec.xyz);
                vec4 reflection = normalize((2 * direction_helper * norm_world_normal) - light_vec);

                //calc ambient intensity
                vec3 I_amb = l[i].ambient * ambientCoef;

                //calc diffuse intensity
                vec3 I_diff = l[i].diffuse * diffuseCoef * max(direction_helper, 0);

                //calc specular intensity
                float r_dot_v = dot(reflection.xyz, viewing_vec.xyz);
                if (r_dot_v < 0 || dot(norm_world_normal, light_vec) < 0)
                {
                    r_dot_v = 0; //clamps the value to 0 if neg
                }

                vec3 I_spec = l[i].specular * specularCoef * pow(r_dot_v, NS);


                //calc attenuation
                float Att = 1.0f / (attenuationConstants.x +
                    attenuationConstants.y * lightDist +
                    attenuationConstants.z * (lightDist * lightDist));
                Att = min(Att, 1.0f);

                //since spotlight, calc spotlight effect
                vec3 D = normalize(newDir.xyz);
                vec3 L = light_vec.xyz;
                float alpha = dot(D, L);
                float phi = cos(radians(l[i].outerAngle));
                float theta = cos(radians(l[i].innerAngle));
                float spotlightEffect;
                if (alpha >= theta)
                    spotlightEffect = 1.f;
                else if (alpha <= phi)
                    spotlightEffect = 0.f;
				else
                    spotlightEffect = pow((alpha - phi) / (theta - phi), l[i].fallout);

                //calc the local intensity with the method for spot lights:
                I_local += Att * (I_amb) + Att * spotlightEffect * (I_diff + I_spec);
            }
        }
    }

    //local intensity calculated, now get final intensity
    float S = (fogDistances.y - viewLen) / (fogDistances.y - fogDistances.x);
    vec3 I_final = S * I_local + (1 - S) * fogColor;
    return vec4(I_final, 1);
}

//I and N should be normalized
vec3 calcRefraction(vec3 I, vec3 N, float eta)
{
    vec3 R = vec3(0.0f);
    float k = 1.0 - eta * eta * (1.0 - dot(N, I) * dot(N, I));

    if (k < 0.0)
    {
        R = vec3(0.0f);
    }
    else
        R = eta * I - (eta * dot(N, I) + sqrt(k)) * N;

    return normalize(R);
}

//I and N should be normalized
vec3 calcReflection(vec3 I, vec3 N)
{
    return normalize(-1*I - 2.0 * dot(N, I) * N);
}

vec3 lightAndReflection(vec4 totLightColor)
{
    vec3 reflectColor;
    vec3 refl = CalcCubeMap(calcReflection(incident.xyz, normalize(world_normal).xyz));
        //reflect val:
    if(refl.z == 0) // left
    {
        reflectColor = vec3(texture(sideNegX, refl.xy));
    }
    else if(refl.z == 1) // right
    {
        reflectColor = vec3(texture(sidePosX, refl.xy));
    }
    else if(refl.z == 2) // bottom
    {
        reflectColor = vec3(texture(sideNegY, refl.xy));
    }
    else if(refl.z == 3) // top
    {
        reflectColor = vec3(texture(sidePosY, refl.xy));
    }
    else if(refl.z == 4) // back 
    {
        reflectColor = vec3(texture(sideNegZ, refl.xy));
    }
    else if(refl.z == 5) // Front
    {
        reflectColor = vec3(texture(sidePosZ, refl.xy));
    }
    return reflectColor;
}

vec3 lightAndRefraction(vec4 totLightColor)
{
    vec3 refractColor;
    vec3 RefR = CalcCubeMap(calcRefraction(incident.xyz, normalize(world_normal).xyz, EtaR));
    vec3 RefB = CalcCubeMap(calcRefraction(incident.xyz, normalize(world_normal).xyz, EtaG));
    vec3 RefG = CalcCubeMap(calcRefraction(incident.xyz, normalize(world_normal).xyz, EtaB));
    
    //red val:
    if(RefR.z == 0) // left
    {
        refractColor.r = vec3(texture(sideNegX, RefR.xy)).r;
    }
    else if(RefR.z == 1) // right
    {
        refractColor.r = vec3(texture(sidePosX, RefR.xy)).r;
    }
    else if(RefR.z == 2) // bottom
    {
        refractColor.r = vec3(texture(sideNegY, RefR.xy)).r;
    }
    else if(RefR.z == 3) // top
    {
        refractColor.r = vec3(texture(sidePosY, RefR.xy)).r;
    }
    else if(RefR.z == 4) // back 
    {
        refractColor.r = vec3(texture(sideNegZ, RefR.xy)).r;
    }
    else if(RefR.z == 5) // Front
    {
        refractColor.r = vec3(texture(sidePosZ, RefR.xy)).r;
    }

    //green val:
    if(RefG.z == 0) // left
    {
        refractColor.g = vec3(texture(sideNegX, RefG.xy)).g;
    }
    else if(RefG.z == 1) // right
    {
        refractColor.g = vec3(texture(sidePosX, RefG.xy)).g;
    }
    else if(RefG.z == 2) // bottom
    {
        refractColor.g = vec3(texture(sideNegY, RefG.xy)).g;
    }
    else if(RefG.z == 3) // top
    {
        refractColor.g = vec3(texture(sidePosY, RefG.xy)).g;
    }
    else if(RefG.z == 4) // back 
    {
        refractColor.g = vec3(texture(sideNegZ, RefG.xy)).g;
    }
    else if(RefG.z == 5) // Front
    {
        refractColor.g = vec3(texture(sidePosZ, RefG.xy)).g;
    }

    //blue val:
    if(RefB.z == 0) // left
    {
        refractColor.b = vec3(texture(sideNegX, RefB.xy)).b;
    }
    else if(RefB.z == 1) // right
    {
        refractColor.b = vec3(texture(sidePosX, RefB.xy)).b;
    }
    else if(RefB.z == 2) // bottom
    {
        refractColor.b = vec3(texture(sideNegY, RefB.xy)).b;
    }
    else if(RefB.z == 3) // top
    {
        refractColor.b = vec3(texture(sidePosY, RefB.xy)).b;
    }
    else if(RefB.z == 4) // back 
    {
        refractColor.b = vec3(texture(sideNegZ, RefB.xy)).b;
    }
    else if(RefB.z == 5) // Front
    {
        refractColor.b = vec3(texture(sidePosZ, RefB.xy)).b;
    }

    return refractColor;
}

vec3 lightAndFresnel(vec4 totLightColor)
{
    vec3 finalColor;
    vec3 refractColor = lightAndRefraction(totLightColor);
    vec3 reflectColor = lightAndReflection(totLightColor);
    finalColor = mix(refractColor, reflectColor, ratio);
    return finalColor;
}

void main()
{
    vec4 light = calcTotalLighting(world_position, world_normal);
    vec3 final;
    if(mappingType == 0.0f)
    {
        final = lightAndFresnel(light);
    }
    else if(mappingType == 1.0f)
    {
        final = lightAndReflection(light);
    }
    else if(mappingType == 2.0f)
    {
        final = lightAndRefraction(light);
    }
    frag_color = (1 - blendVal) * light +  (blendVal)* vec4(final,1.0f);
}