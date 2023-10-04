/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: orbitSegment.h
Purpose: This file contains the declarationsfor the orbitSegment class. The start point 
is used to change light position
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS300_2
Author: Mark Kouris, mark.kouris, mark.kouris
Creation date: 10/12/21
End Header --------------------------------------------------------*/

#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include<glm/glm.hpp>
#include <vector>
#include "shader.hpp"


using namespace glm;

class orbitSegment
{
public:
    orbitSegment(vec3 start, vec3 end);
    void setMVP(mat4 MVP);
    void setColor(vec3 color);
    void draw();
    mat4 getView();
    mat4 getMVP();
    vec3 getStart();
    vec3 getEnd();
    ~orbitSegment();

    vec3 startPoint;
private:
    int shaderProgram;
    unsigned int VBO, VAO;
    std::vector<float> vertices;
    vec3 endPoint;
    mat4 view = mat4(1.0);
    mat4 MVP = mat4(1.0);
    vec3 lineColor;

};
