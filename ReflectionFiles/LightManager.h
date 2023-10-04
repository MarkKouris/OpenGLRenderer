/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: LightManager.h
Purpose: This file contains the class declaration for the manager for the lights, handles setting
data and passing to shader, along with changing lighting scenarios
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS300_2
Author: Mark Kouris, mark.kouris, mark.kouris
Creation date: 10/12/21
End Header --------------------------------------------------------*/

#pragma once
#include "Light.h"
#include <map>
#include <vector>

//all the data for the lights
struct alignas(16U) globalLightData
{
    lightData l[16];

};


class LightManager
{
public:
    LightManager();
    void drawLights();
    void pickScenario(int scenario);
    void updateLights(orbit* orbit_, int iterator);
    ~LightManager();

    //settors so the structs are matched up with the vector
    void setType(int index, int val);
    void setEnabled(int index, int val);
    void setAngularPosition(int index, int val);
    void setInnerAngle(int index, float val);
    void setOuterAngle(int index, float val);
    void setFallout(int index, float val);
    void setDiffuse(int index, vec3 val);
    void setAmbient(int index, vec3 val);
    void setSpecular(int index, vec3 val);
    void setPosition(int index, vec4 val);
    void setDirection(int index, vec4 val);

    //actually do need the light vector, but it will not change.
    std::vector<Light*> lights;

    //uniform block data
    globalLightData dataGlobal;
    GLuint glBuffer;
    unsigned int binding = 2U;

    //uniform block functions:
    void setupBlock();
    void updateEachLight(lightData* light);
};