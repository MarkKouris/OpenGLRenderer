//Implementation by Mark Kouris for CS350
//Assignment 1, 2/16/22 F21

#version 450 core

out vec4 FragColor;
uniform vec3 color;

void main()
{
    FragColor = vec4(color, 1.0f);
}