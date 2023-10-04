//Implementation by Mark Kouris for CS350
//Assignment 1, 2/16/22 F21
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
    int renderTarget;
    vec3 fogColor;
    float pad2;
    vec3 ambientCoef;
    float pad3;
    vec3 globalAmbient;
    float pad4;
    vec3 specularCoef;
    float pad5;
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
    float innerAngle;
    float outerAngle;
    float fallout;
};

//uniform block for lights
layout (std140, binding = 2) uniform lightData
{
    light l[16];
};


vec4 calcTotalLighting(vec4 world_position, vec4 world_normal)
{
    //make sure the normals are normalized
    vec4 norm_world_normal = normalize(world_normal);

    //calculates the final light intensity
    vec4 viewing_vec = eyePosition - world_position;
    float viewLen = length(viewing_vec);
    vec3 I_local = emissive + (globalAmbient * ambientCoef);

    //need to use albeido as the diffuse coef and specilar c

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
                vec4 light_vec = newPos - world_position ;
                float lightDist = length(light_vec);
                light_vec = normalize(light_vec);

                // Calc view vector, eyePos - A
                viewing_vec = normalize(viewing_vec);

                // Calc Reflection Vector
                float direction_helper = dot(world_normal.xyz, light_vec.xyz);
                vec4 reflection = (2 * direction_helper * world_normal) - light_vec;

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
                vec4 light_vec = newPos ;
                float lightDist = length(light_vec);
                light_vec = normalize(light_vec);

                // Calc view vector, eyePos - A
                viewing_vec = normalize(viewing_vec);

                // Calc Reflection Vector
                float direction_helper = dot(norm_world_normal.xyz, light_vec.xyz);
                vec4 reflection = (2 * direction_helper * norm_world_normal) - light_vec;

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

                // Calc view vector, eyePos - A

                viewing_vec = normalize(viewing_vec);

                // Calc Reflection Vector
                float direction_helper = dot(norm_world_normal.xyz, light_vec.xyz);
                vec4 reflection = (2 * direction_helper * norm_world_normal) - light_vec;

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
                vec3 L = normalize(world_position.xyz - light_vec.xyz);
                float alpha = acos(dot(D, L));
                float phi = cos(l[i].outerAngle);
                float theta = cos(l[i].innerAngle);
                float spotlightEffect = pow((alpha - phi) / (theta - phi), l[i].fallout);

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

// IN values
in VS_OUT
{
    vec4 vertexPosition; // interpolated vPosition
    vec2 vertexUV; // interpolated vUV
} fs_in;
in vec2 TexCoords;


// Uniforms
layout (binding=0) uniform sampler2D viewPosBuffer;
layout (binding=1) uniform sampler2D normalBuffer;
layout (binding=2) uniform sampler2D uvBuffer;
layout (binding=3) uniform sampler2D depthBuffer;

//out value
out vec4 FragColor;

void main()
{
    vec4 normal = texture( normalBuffer, fs_in.vertexUV );
    if (normal.x == 0 && normal.y == 0 && normal.z == 0)
        discard;
    vec4 position = texture( viewPosBuffer, fs_in.vertexUV );
    vec4 albeido = texture( uvBuffer, fs_in.vertexUV);


    switch(renderTarget)
    {
        case 0:
            FragColor = position;
            break;
        case 1:
            FragColor = normal;
            break;
        case 2:
            FragColor = albeido;
            break;
        case 3:
            FragColor = calcTotalLighting(position, normal);
            break;
        default:
            break;
     }
}