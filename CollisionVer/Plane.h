/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Plane.h
Purpose: This file contains the declaration for the plane primitive
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS350_2
Author: Mark Kouris, mark.kouris, mark.kouris
Creation date: 3/11/22
End Header --------------------------------------------------------*/

#pragma once
#include "glm/common.hpp"

class Plane
{
public:
    //(n.x, n.y, n.z, d)
    glm::vec4 m_Normal;

};