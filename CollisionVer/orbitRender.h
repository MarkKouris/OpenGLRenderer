/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: OBJReader.h
Purpose: This file serves as the header for the objreader class. This allows
the class to be accessed in any file.
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS300_2
Author: Mark Kouris, mark.kouris, mark.kouris / pushpak
Creation date: 10/12/21
End Header --------------------------------------------------------*/

#pragma once
#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "shaderClass.h"
using namespace glm;
class orbit 
{
public:
    orbit();
    ~orbit();
    void setupOpenGL();
    void generateOrbit(float radius);
    void drawOrbit();
    void setMatricies(Camera& cam, const unsigned int SCR_WIDTH = 800, const unsigned int SCR_HEIGHT = 600);
    mat4 getMVP();
    vec3 getPoint(int index);
    float currRadius = 0;
private:
    bool orbitEnabled = false;
    mat4 MVP = mat4(1.0);
    vec3 lineColor = vec3(1.0f, 1.0f, 1.0f);
    std::vector<glm::vec3> orbitPoints;
    Shader* orbitShader;
    GLuint VAO;
    GLuint positionBuffer;


};