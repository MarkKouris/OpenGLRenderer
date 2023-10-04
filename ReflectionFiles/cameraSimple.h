/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: cameraSimple.h
Purpose: This file serves as a helper class for moving the camera around the orbit.
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS300_2
Author: Mark Kouris, mark.kouris, mark.kouris
Creation date: 10/12/21
End Header --------------------------------------------------------*/

#pragma once
#include <glm/glm.hpp> //for vec3

class simpleCamera
{
public:
    glm::vec3 position;
    simpleCamera()
    {
        position = glm::vec3(0, 0, 0);
    }

};