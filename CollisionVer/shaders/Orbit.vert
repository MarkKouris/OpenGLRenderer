//Implementation by Mark Kouris for CS350
//Assignment 1, 2/16/22 F21

#version 450 

layout (location = 0) in vec3 aPos;
uniform mat4 MVP;
       
void main()
{
    gl_Position = MVP * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}