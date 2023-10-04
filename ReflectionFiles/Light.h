/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Light.h
Purpose: This file contains the class declaration for each light and the associated functions
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS300_2
Author: Mark Kouris, mark.kouris, mark.kouris
Creation date: 10/12/21
End Header --------------------------------------------------------*/

#pragma once
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtx/transform.hpp>
#include "glm/gtc/type_ptr.hpp"
#include <list>
#include "Mesh.h"
#include "shader.hpp"
#include "OBJReader.h"
#include "orbitRender.h"


//per light data struct
//order as such for alignment
struct alignas(16U) lightData
{
    vec4 position = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); //for point and spot
    vec4 direction = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);; //for spot and directional
    glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    int type = 0; // 0 = point, 1 = direction, 2 = spot
    glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    int enabled = 0; //1 is yes, 0 is no 
    glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
    int angularPosition = 0;
    float innerAngle = 30.0f;
    float outerAngle = 45.0f;
    float fallout = 1.0f;
};

//Each light currently being displayed on screen
class Light 
{
public:

  Light();
  ~Light();

    void updatePosition(orbitSegment lightLoc); //sets position on the orbit
    void update();
    void draw();
    //helper for loading sphere
    void loadMesh(Mesh* m);
    Mesh* loadObjData(std::string path);

    //reset all data for use of reset
   void initLightData();

   lightData data; //we will access and modify a lot, so made public

private: //data we don't need to give out
   
    glm::mat4 modelMat = glm::mat4(1); //for displaying the light
    Mesh* _mesh = NULL;               //to have the circle that orbits
    GLuint  shaderProgramID = 0;      // program this will never change
    GLint uVertexTransform;
    GLint uColor;
    
};

