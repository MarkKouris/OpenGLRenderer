/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: camera.h
Purpose: This file serves as the declaration for the advanced camera class, currently 
    unused.
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS300_2
Author: Mark Kouris, mark.kouris, mark.kouris
Creation date: 10/12/21
End Header --------------------------------------------------------*/

#pragma once
#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>


    class Camera {
    public:
        Camera(void);
        Camera(const glm::vec4& E, const glm::vec4& look, const glm::vec4& rel,
            float fov, float aspect, float N, float F);
        glm::vec4 eye(void) const;
        glm::vec4 right(void) const;
        glm::vec4 up(void) const;
        glm::vec4 back(void) const;
        glm::vec4 viewport(void) const;
        float nearf(void) const;
        float farf(void) const;
        Camera& zoom(float factor);
        Camera& forward(float distance);
        Camera& yaw(float angle);
        Camera& pitch(float angle);
        Camera& roll(float angle);
    private:
        glm::vec4 eye_point;
        glm::vec4 right_vector, up_vector, back_vector;
        float vp_width, vp_height, vp_distance,
            near_distance, far_distance;
    };


    

