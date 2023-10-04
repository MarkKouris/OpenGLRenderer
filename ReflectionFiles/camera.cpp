/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: camera.h
Purpose: This file serves as the definition for the advanced camera class, currently
    unused.
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS300_2
Author: Mark Kouris, mark.kouris, mark.kouris
Creation date: 10/12/21
End Header --------------------------------------------------------*/

#include "camera.h"

//helper functions for camera functions
float vecLen(const glm::vec4& vector);
glm::vec4 calcBackVec(const glm::vec4& Lvec); //n Vec
glm::vec4 calcRightVec(const glm::vec4& Lvec, const glm::vec4& Rvec); //u vec
glm::vec4 calcUpVec(glm::vec4& Nvec, glm::vec4& Uvec); //v vec
float calcVPWidth(float FOV, float vpDist);
float calcVPHeight(float width, float aspectRatio);
glm::vec4 cross(const glm::vec4& u, const glm::vec4& v);


/**
 * @brief Construct a new Camera object with default parameters
 *
 */
Camera::Camera(void)
{
    eye_point = glm::vec4(0);
    near_distance = 0.1f;
    far_distance = 10;
    vp_distance = 10.0 / 2;
    vp_width = calcVPWidth(90.0f, vp_distance);
    vp_height = calcVPHeight(vp_width, 1);
    back_vector = glm::vec4(0, 0, 1, 0);
    right_vector = glm::vec4(1, 0, 0, 0);
    up_vector = glm::vec4(0, 1, 0, 0);
}

/**
 * @brief Construct a new object based around given params
 *
 * @param E
 * @param look
 * @param rel
 * @param fov
 * @param aspect
 * @param N
 * @param F
 */
Camera::Camera(const glm::vec4& E, const glm::vec4& look, const glm::vec4& rel,
    float fov, float aspect, float N, float F)
{
    eye_point = E;
    near_distance = N;
    far_distance = F;
    vp_distance = (F + N) / 2;
    vp_width = calcVPWidth(fov, vp_distance);
    vp_height = calcVPHeight(vp_width, aspect);
    back_vector = calcBackVec(look);
    right_vector = calcRightVec(look, rel);
    up_vector = calcUpVec(back_vector, right_vector);

}

glm::vec4 Camera::eye(void) const
{
    return eye_point;
}

glm::vec4 Camera::right(void) const
{
    return right_vector;
}

glm::vec4 Camera::up(void) const
{
    return up_vector;
}

glm::vec4 Camera::back(void) const
{
    return back_vector;
}

/**
 * @brief returns the viewport
 *
 * @return glm::vec4
 */
glm::vec4 Camera::viewport(void) const
{
    return glm::vec4(vp_width, vp_height, vp_distance, 0);
}

float Camera::nearf(void) const
{
    return near_distance;
}

float Camera::farf(void) const
{
    return far_distance;
}

/**
 * @brief zooms in the camera
 * just multiplies the width and height by the given factor
 *
 * @param factor
 * @return Camera&
 */
Camera& Camera::zoom(float factor)
{
    vp_width *= factor;
    vp_height *= factor;
    return *this;

}

/**
 * @brief moves the camera forward a given distance
 * subtracts because back vector is pointing opposite direction of camera
 * @param distance
 * @return Camera&
 */
Camera& Camera::forward(float distance)
{
    back_vector.z -= distance;
    return *this;

}

/**
 * @brief rotates the camera along the y axis
 * multiplies the right and back vector by rot
 * @param angle
 * @return Camera&
 */
Camera& Camera::yaw(float angle)
{
    glm::mat4 yawRot = glm::rotate(angle, glm::vec3(up_vector.x, up_vector.y, up_vector.z));
    right_vector = yawRot * right_vector;
    back_vector = yawRot * back_vector;
    return *this;

}

/**
 * @brief rotates the camera along the x axis
 * multiplies the up and back vector by rot
 * @param angle
 * @return Camera&
 */
Camera& Camera::pitch(float angle)
{
    glm::mat4 pitchRot = glm::rotate(angle, glm::vec3(right_vector.x, right_vector.y, right_vector.z));
    up_vector = pitchRot * up_vector;
    back_vector = pitchRot * back_vector;
    return *this;

}

/**
 * @brief rotates the camera along the Z axis
 * multiplies the up and right vector by rot mat
 * @param angle
 * @return Camera&
 */
Camera& Camera::roll(float angle)
{
    glm::mat4 rollRot = glm::rotate(angle, glm::vec3(back_vector.x, back_vector.y, back_vector.z));
    up_vector = rollRot * up_vector;
    right_vector = rollRot * right_vector;
    return *this;
}

//helpers for calculations in non default ctor:

/**
 * @brief gets the length of a given vector
 *
 * @param vector
 * @return float
 */
float vecLen(const glm::vec4& vector)
{
    return sqrtf(powf(vector.x, 2) + powf(vector.y, 2) + powf(vector.z, 2));
}

/**
 * @brief calculates the back vector,
 *  which is based on the look at vector
 * This vector is the N vector in notes
 *
 * @param Lvec
 * @return glm::vec4
 */
glm::vec4 calcBackVec(const glm::vec4& Lvec)
{
    float lVecLenght = -1 * vecLen(Lvec);
    return Lvec / lVecLenght;
}

/**
 * @brief calculates the Right vector,
 *  which is based on the look at vector and relative up vector
 * This vector is the U vector in notes
 *
 * @param Lvec
 * @param Rvec
 * @return glm::vec4
 */
glm::vec4 calcRightVec(const glm::vec4& Lvec, const glm::vec4& Rvec)
{
    glm::vec4 LxR = cross(Lvec, Rvec);
    return LxR / vecLen(LxR);
}

/**
 * @brief calculates the up vector,
 *  which is based on the right and back vectors
 * This vector is the V vector in notes
 *
 * @param Nvec
 * @param Uvec
 * @return glm::vec4
 */
glm::vec4 calcUpVec(glm::vec4& Nvec, glm::vec4& Uvec)
{
    return  cross(Nvec, Uvec);
}

/**
 * @brief calcs the width of a certain viewport
 *
 * @param FOV
 * @param vpDist
 * @return float
 */
float calcVPWidth(float FOV, float vpDist)
{
    return 2 * vpDist * tan(FOV / 2);
}

/**
 * @brief calcs the height of a viewport
 *
 * @param width
 * @param aspectRatio
 * @return float
 */
float calcVPHeight(float width, float aspectRatio)
{
    return width / aspectRatio;
}

glm::vec4 cross(const glm::vec4& u, const glm::vec4& v)
{
    //since 3d vectors, we can just take x,y and z and convert to vec3
    glm::vec3 newU = glm::vec3(u.x, u.y, u.z);
    glm::vec3 newV = glm::vec3(v.x, v.y, v.z);
    glm::vec3 cross = glm::cross(newU, newV);

    return glm::vec4(cross, 0);
}


