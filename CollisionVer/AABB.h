/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: AABB.h
Purpose: This file contains the declaration for the AABB primitive
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS350_2
Author: Mark Kouris, mark.kouris, mark.kouris
Creation date: 3/11/22
End Header --------------------------------------------------------*/

#pragma once
#include "glm/common.hpp"


class Aabb
{
    public:
    glm::vec3 m_Max; //upper right corner
    glm::vec3 m_Min; //lower left corner
};