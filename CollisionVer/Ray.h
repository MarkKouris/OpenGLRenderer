/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Ray.h
Purpose: This file contains the declaration for the Ray primitive
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS350_2
Author: Mark Kouris, mark.kouris, mark.kouris
Creation date: 3/11/22
End Header --------------------------------------------------------*/

#pragma once
#include "glm/common.hpp"

class Ray
{
public:
    glm::vec3 m_Start;     //starting point of the ray.
    glm::vec3 m_Direction; //direction the ray extends
};