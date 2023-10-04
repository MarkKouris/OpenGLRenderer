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

in vec4 position;
in vec4 normal;

out vec4 world_position;
out vec4 world_normal;

void main()
 {
   world_position = model_matrix * position;
   world_normal = normal_matrix * vec4(normalize(normal.xyz), 0);    // convert to world space
   gl_Position = persp_matrix * view_matrix * world_position;
}
