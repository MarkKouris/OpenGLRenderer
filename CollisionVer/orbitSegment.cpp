/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: orbitSegment.cpp
Purpose: This file contains the functionality to render each individual orbit segment,
Used also to change the position of the lights.
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS300_2
Author: Mark Kouris, mark.kouris, mark.kouris
Creation date: 10/12/21
End Header --------------------------------------------------------*/

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "orbitSegment.h"


orbitSegment::orbitSegment(vec3 start, vec3 end) 
{

    startPoint = start;
    endPoint = end;
    lineColor = vec3(1, 1, 1);

    //doing everything explicitely here so the shader function does not print to output and slow down program

    const char* vertexShaderSource = "#version 450 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 MVP;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = MVP * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char* fragmentShaderSource = "#version 450 core\n"
        "out vec4 FragColor;\n"
        "uniform vec3 color;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(color, 1.0f);\n"
        "}\n\0";

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors

    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors

    // link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    vertices = {
         start.x, start.y, start.z,
         end.x, end.y, end.z,

    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void orbitSegment::setMVP(mat4 mvp) 
{
    MVP = mvp;
}

void orbitSegment::setColor(vec3 color)
{
    lineColor = color;
}

void orbitSegment::draw() 
{
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &lineColor[0]);

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);
}

mat4 orbitSegment::getView()
{
    return view;
}

mat4 orbitSegment::getMVP()
{
    return MVP;
}

vec3 orbitSegment::getStart()
{
  return startPoint;
}

vec3 orbitSegment::getEnd()
{
    return endPoint;
}

orbitSegment::~orbitSegment()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}