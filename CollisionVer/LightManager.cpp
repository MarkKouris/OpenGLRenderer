/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: LightManager.cpp
Purpose: This file contains the class definition for the manager for the lights, handles setting
data and passing to shader, along with changing lighting scenarios
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS300_2
Author: Mark Kouris, mark.kouris, mark.kouris
Creation date: 10/12/21
End Header --------------------------------------------------------*/

#include "LightManager.h"
#include <algorithm>
#include <iostream>
#define PI 3.1415926
#define NUM_VERT 181

LightManager::LightManager()
{
    //get up the light shader
    lightShader = new Shader("../Common/shaders/Light.vert",
        "../Common/shaders/Light.frag");

    //fill all the array containers
    for (int i = 0; i < 16; ++i)
    {
        if (i == 0)
        {
            lights.push_back(new Light(lightShader));
            lights[i]->data.enabled = 1;
            dataGlobal.l[i] = lights[i]->data;
        }
        else
        {
            lights.push_back(new Light(lightShader));
            lights[i]->data.enabled = 0;
            dataGlobal.l[i] = lights[i]->data;
        }

    }

    setupBlock();
}


//since the gui sets data directly, update position
//after that, update shader struct

//TODO: recalculate the orbit of the lights separately and not based on the orbit 
void LightManager::updateLights(orbit* orb, int iterator)
{
    for (int i = 0; i < 16; ++i) //16 is the max num lights
    {
        //calc new Pos
        //float angle = iterator + lights[i]->data.angularPos
        //position = radius * vec3(cos(angle), 0, sin(angle))
        glm::vec3 position = orb->getPoint(iterator + lights[i]->data.angularPos);
        lights[i]->updatePosition(position, orb->getMVP());
        lights[i]->update(lightShader);
        setPosition(i, glm::vec4(position, 1.0f));
        setDirection(i, glm::vec4(glm::normalize(position), 0.0f));

    }

    for (int i = 0; i < 16; ++i) //16 is the max num lights
    {
        if (dataGlobal.l[i].enabled == 1)
        {
            updateEachLight(&dataGlobal.l[i]);
        }
    }

}

//the draw call
void LightManager::drawLights()
{
    for (int i = 0; i < 16; ++i) //16 is the max num lights
    {
        lights[i]->draw(lightShader);
    }
}


//helper for cleaning up memory
template <typename T>
void delete_pointed_to(T* const ptr)
{
    delete ptr;
}

//need to overhaul this
void LightManager::pickScenario(int scenario)
{
    //initialize all light data
    for (int i = 0; i < 16; ++i)
    {
        lights[i]->initLightData();
        dataGlobal.l[i] = lights[i]->data;
        lights[i]->data.enabled = 0;
    }

    std::vector<glm::vec3> colors;
    colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f)); //red
    colors.push_back(glm::vec3(1.0f, 0.5f, 0.0f)); //orange
    colors.push_back(glm::vec3(0.5f, 1.0f, 0.0f)); //yellow
    colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); //gree
    colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f)); //blue
    colors.push_back(glm::vec3(0.0f, 1.0f, 1.0f)); //cyan
    colors.push_back(glm::vec3(0.5f, 0.0f, 1.0f)); //pink
    colors.push_back(glm::vec3(1.0f, 0.0f, 1.0f)); //purple


    //clean up the current lights
    if (scenario == 0) //resent to one basic light
    {
        setEnabled(0, 1);
    }

    else if (scenario == 1) //scenario 1 button is pressed
    {
        //all have same color and type
        int angPos = 0;
        for (int i = 0; i < 8; ++i)
        {
            setEnabled(i, 1);
            setAngularPos(i, angPos);
            angPos += 45;

        }
    }
    else if (scenario == 2) //scenario 2 button is pressed
    {
        //all have different color and same type
        int angPos = 0;
        for (int i = 0; i < 8; ++i)
        {
            setEnabled(i, 1);
            setAngularPos(i, angPos);
            angPos += 45;
            setDiffuse(i, colors[i]);
            setSpecular(i, colors[i]);
            setAmbient(i, colors[i]);
        }

    }
    else if (scenario == 3) //scenario 3 button is pressed
    {
        //all have different color and different type type
        int angPos = 0;
        for (int i = 0; i < 8; ++i)
        {
            setEnabled(i, 1);
            setAngularPos(i, angPos);
            angPos += 45;
            setDiffuse(i, colors[i]);
            setSpecular(i, colors[i]);
            setAmbient(i, colors[i]);
            setType(i, i % 2);
        }
    }
}


LightManager::~LightManager()
{
    //free memory for lights
    std::for_each(lights.begin(), lights.end(), delete_pointed_to<Light>);

    lights.clear();
    glDeleteBuffers(1, &glBuffer);
}

//settors so we can modify local and shader variations of the data
void LightManager::setType(int index, int val)
{
    dataGlobal.l[index].type = val;
    lights[index]->data.type = val;
}

void LightManager::setEnabled(int index, int val)
{
    dataGlobal.l[index].enabled = val;
    lights[index]->data.enabled = val;
}

void LightManager::setInnerAngle(int index, float val)
{
    dataGlobal.l[index].innerAngle = glm::radians(val);
    lights[index]->data.innerAngle = glm::radians(val);
}

void LightManager::setOuterAngle(int index, float val)
{
    dataGlobal.l[index].outerAngle = glm::radians(val);
    lights[index]->data.outerAngle = glm::radians(val);
}

void LightManager::setAngularPos(int index, int val)
{
    dataGlobal.l[index].angularPos = val;
    lights[index]->data.angularPos = val;
}

void LightManager::setFallout(int index, float val)
{
    dataGlobal.l[index].fallout = val;
    lights[index]->data.fallout = val;
}

void LightManager::setDiffuse(int index, vec3 val)
{
    dataGlobal.l[index].diffuse = val;
    lights[index]->data.diffuse = val;
}

void LightManager::setAmbient(int index, vec3 val)
{
    dataGlobal.l[index].ambient = val;
    lights[index]->data.ambient = val;
}

void LightManager::setSpecular(int index, vec3 val)
{
    dataGlobal.l[index].specular = val;
    lights[index]->data.specular = val;
}

void LightManager::setPosition(int index, vec4 val)
{
    dataGlobal.l[index].position = val;
    lights[index]->data.position = val;
}

void LightManager::setDirection(int index, vec4 val)
{
    dataGlobal.l[index].direction = val;
    lights[index]->data.direction = val;
}

//sets up the uniform block at sets
void LightManager::setupBlock()
{
    glGenBuffers(1, &glBuffer); //glBuffer is the buffer value, store this value
    glBindBuffer(GL_UNIFORM_BUFFER, glBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(globalLightData), &dataGlobal, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, GLuint(2), glBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

//then call this for each light
void LightManager::updateEachLight(lightData* light)
{
    glBindBuffer(GL_UNIFORM_BUFFER, glBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, reinterpret_cast<char*>(light) - reinterpret_cast<char*>(&dataGlobal), sizeof(lightData), light);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
