//Implementation by Mark Kouris for CS350
//Assignment 1, 2/16/22 F21

#version 450 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.2;

uniform mat4 projection;
uniform bool genVert;

void GenerateLine(int index)
{
    gl_Position = projection * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = projection * (gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void GenerateFaceNormal()
{
    vec4 pos = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.f; //avg of positions
    vec3 norm = normalize(gs_in[0].normal + gs_in[1].normal + gs_in[2].normal); //get avg of normals
    
    gl_Position = projection * pos;
    EmitVertex();
    gl_Position = projection * (pos + vec4(norm, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    if(genVert)
    {
        GenerateLine(0); // first vertex normal
        GenerateLine(1); // second vertex normal
        GenerateLine(2); // third vertex normal
    }
    else
    {
        GenerateFaceNormal();
    }


}