/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: OBJReader.h
Purpose: This file serves to render the dynamic orbit around the main object for the lights
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS300_2
Author: Mark Kouris, mark.kouris, mark.kouris / pushpak
Creation date: 10/12/21
End Header --------------------------------------------------------*/

#include "orbitRender.h"
#include <algorithm>
#define PI 3.1415926
#define NUM_VERT 181

//helper for cleaning up memory
template <typename T>
void delete_pointed_to(T* const ptr)
{
  delete ptr;
}

orbit::orbit()
{
    orbitShader = new Shader("../Common/shaders/Orbit.vert",
                             "../Common/shaders/Orbit.frag");
}

orbit::~orbit()
{
    delete orbitShader;
}

void orbit::setupOpenGL()
{
    //generate the buffer for the positions
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &positionBuffer);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * orbitPoints.size(), orbitPoints.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

//have radius be attached to model matrix as a scale instead of recalculating the points
//TODO: add another matrix to the MVP that is a scale to modify the radius
//make original radius 1 to make scaling easier
void orbit::generateOrbit(float radius)
{
    //in case of regeneration, clear old points
    orbitPoints.clear();

    GLuint numVerticies = NUM_VERT - 1; //this means there will be 180 sides to the circle

    GLfloat doublePi = 2.0f * PI;

    for (int i = 0; i < NUM_VERT; i++)
    {

        orbitPoints.push_back(vec3(0 + (radius * cos(i * doublePi / (numVerticies))),
                                   0,
                                   0 + (radius * sin(i * doublePi / (numVerticies)))));
    }

}

//
void orbit::setMatricies(Camera& cam, const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT)
{
    glm::mat4 view = cam.GetViewMatrix();
    glm::mat4 perspective = glm::perspective(glm::radians(cam.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    MVP = perspective * view;
}

mat4 orbit::getMVP()
{
    return MVP;
}

vec3 orbit::getPoint(int index)
{
    int newIndex = index % (NUM_VERT - 1); //to make sure we don't go out of bounds
    return orbitPoints[newIndex];
}

void orbit::drawOrbit()
{
    orbitShader->use();
    //set uniforms
    glUniformMatrix4fv(glGetUniformLocation(orbitShader->ID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
    glUniform3fv(glGetUniformLocation(orbitShader->ID, "color"), 1, &lineColor[0]);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_STRIP,0 , orbitPoints.size());
    glBindVertexArray(0);
}

