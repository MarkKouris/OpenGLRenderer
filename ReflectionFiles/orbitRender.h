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
#include "orbitSegment.h"
#include "cameraSimple.h"
#include <glm/gtc/matrix_transform.hpp>

class orbit 
{
public:
    void generateOrbit(float radius);
    void setMatricies();
    void drawOrbit();
    void setPerspective(const unsigned int SCR_WIDTH = 800, const unsigned int SCR_HEIGHT = 600);
    orbitSegment getSegment(int index);
    float currRadius = 0;
    float radiansCamera = 0;
    float midValue = 0;
    float radiansPersp = 0;
    float finalValPersp = 0;
    float cameraCoef = 0;
    glm::vec3 view1 = { 0,0,0 };
    glm::vec3 view2 = { 0,0,0 };
    bool orbitEnabled = false;
    float anglePos = 0;
    float orbitSpeed = 0;
    mat4 perspective = mat4(1.0);
    mat4 view = mat4(1.0);
    simpleCamera camera;
    std::vector<orbitSegment*> _orbit;
private:


};