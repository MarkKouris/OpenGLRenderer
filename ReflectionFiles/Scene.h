/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Scene.h
Purpose: This file serves as the interface for each scene created and displayed on
screen. Only 1 scene will be displayed at once.
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS300_2
Author: Mark Kouris, mark.kouris, mark.kouris / pushpak
Creation date: 9/24/21
End Header --------------------------------------------------------*/
#pragma once
#ifndef SAMPLE3_2_FBO_SCENE_H
#define SAMPLE3_2_FBO_SCENE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/common.hpp>
#include <glm/gtx/transform.hpp>
#include <string>
#include <map>
#include "Mesh.h"
#include "OBJReader.h"
#include "shader.hpp"
#include "camera.h"

#define _GET_GL_ERROR   { GLenum err = glGetError(); std::cout << "[OpenGL Error] " << glewGetErrorString(err) << std::endl; }

using namespace glm;
struct alignas(sizeof(float) *4) matricies
{
    glm::mat4 persp_matrix = glm::mat4(1);
    glm::mat4 view_matrix = glm::mat4(1);
    glm::mat4 model_matrix = glm::mat4(1);
    glm::mat4 normal_matrix = glm::mat4(1);
};

struct alignas(sizeof(float) * 4) globalData
{
    vec4 eyePosition = glm::vec4(0.0, 0.0, 0.0, 1.0f);
    vec3 attenuationConstants = glm::vec3(1.000, 0.220, 0.200);
    float NS = 10.0f;
    vec3 diffuseCoef = glm::vec3(1.0f, 1.0f, 1.0f);
    float blendVal = 1.00;
    vec3 fogColor = glm::vec3(0.2, 0.4, 0.55);
    float mappingType = 0;
    vec3 ambientCoef = glm::vec3(0.0, 0.0, 0.0);
    float refractionVal = 1.0;
    vec3 globalAmbient = glm::vec3(0.0f, 0.0f, 0.100f);
    float chromaticAberration = 0.02;
    vec3 specularCoef = glm::vec3(1.0f, 1.0f, 1.0f);
    float fresnelPower = 2.0;
    vec3 emissive = glm::vec3(0.0, 0.0, 0.0);
    float pad6 = 0;
    vec2 fogDistances = glm::vec2(0.100, 20.000);
};


class Scene
{

public:
    Scene(); //good
    Scene(int windowWidth, int windowHeight); // good 
    virtual ~Scene(); //good

    // Public methods

    void loadObjData(std::string path); //good

    virtual int InitMembers();

    // Init: called once when the scene is initialized
    virtual int Init();

    // LoadAllShaders: This is the placeholder for loading the shader files
    virtual void LoadAllShaders();

    // Display : encapsulates per-frame behavior of the scene
    virtual int Display();

    // preRender : called to setup stuff prior to rendering the frame
    virtual int preRender();

    // Render : per frame rendering of the scene
    virtual int Render();

    // postRender : Any updates to calculate after current frame
    virtual int postRender();

    // cleanup before destruction
    virtual void CleanUp();

    void loadMesh();

    //helper functions to calculate normal matrix
    glm::mat4 calcNormalMat(glm::mat4 model);

    //set uniform block data function
    void setupMTX();
    void updateMTX();
    void setupGBL();
    void updateGBL();

    float cameraAngle = 0.0f; //used
    GLuint  programID;      // program this will never change, make public to access in light manager

    Camera sceneCam;

    matricies mtxData;
    globalData gblData;
   
    GLuint glBufferMTX;
    GLuint glBufferGBL;
    unsigned int bindingMTX = 1U;
    unsigned int bindingGBL = 0U;

    // functions to get the following from a camera
    glm::mat4 Translate(const glm::vec4& t);
    glm::mat4 AffineInverse(glm::mat4& m);
    glm::mat4 model(Camera& cam);
    glm::mat4 viewMat(Camera& cam);
    glm::mat4 perspective(Camera& cam);

protected:
    int _windowHeight, _windowWidth;


    std::string objToLoad; //keep
    Mesh* mainObj; //the objects displayed in screen (kept as a vector incase we want multiple objects in the future
    //need a new vector for the lights

    //all openGL uniforms
    GLuint normBuff;        // buffer for the normals

};


#endif //SAMPLE3_2_FBO_SCENE_H


#if 0

class UBO {
public:
    GLuint handle;
    std::map<std::string, GLint> offsets;
    std::map<std::string, GLuint> sizes;
    GLuint stride;

    void setData(std::string member, void* data, unsigned arrayIdx = 0) {
        glNamedBufferData(handle, offsets[member] + arrayIdx * stride, sizes[member], data);
    }
};
UBO ubo;
//ubo.offsets.resize(5);
GLint offsets[5];
glGetActiveUniformsiv(programID, 5, indices, GL_UNIFORM_OFFSET, offsets);
ubo.offsets["activeLights"] = offsets[0];
ubo.sizes["activeLights"] = sizeof(int);

#endif
