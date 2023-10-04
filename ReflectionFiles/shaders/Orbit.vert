//Implementation By Mark Kouris, CS300 Assignment 2
#version 430 

layout (location = 0) in vec3 aPos;
uniform mat4 MVP;
       
void main()
{
    gl_Position = MVP * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}