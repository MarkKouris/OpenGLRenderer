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
#include "shaderClass.h"

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
    float NS = 9.0f;
    vec3 diffuseCoef = glm::vec3(1.0, 1.0, 1.0);
    int renderTarget = 3;
    vec3 fogColor = glm::vec3(0.2, 0.4, 0.55);
    float pad2 = 0;
    vec3 ambientCoef = glm::vec3(0.1, 0.1, 0.1);
    float pad3 = 0;
    vec3 globalAmbient = glm::vec3(0.0f, 0.0f, 0.100f);
    float pad4 = 0;
    vec3 specularCoef = glm::vec3(1.0, 1.0, 1.0);
    float pad5 = 0;
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

    Mesh* loadObjData(std::string path); //good

    virtual int InitMembers();

    void updateObj(std::string newObj);

    // Init: called once when the scene is initialized
    virtual int Init();

    // LoadAllShaders: This is the placeholder for loading the shader files
    virtual void LoadAllShaders();

    void LoadAllShaders(std::string vert, std::string frag);

    // Display : encapsulates per-frame behavior of the scene
    virtual int Display();

    // Render : per frame rendering of the scene
    virtual int RenderMainObj();

    // copys the depth info from the main obj to the vertex rendering
    virtual int copyDepthInfo();

    // render debug objects
    virtual int RenderDebugObj();

    // cleanup before destruction
    virtual void CleanUp();

    void loadMesh(Mesh* m);

    void activateTextures();

    //helper functions to calculate normal matrix
    glm::mat4 calcNormalMat(glm::mat4 model);

    //helper to set up gBuffer and create FBO
    void genGBuffer();

    //set uniform block data function
    void setupMTX();
    void updateMTX();
    void setupGBL();
    void updateGBL();

    float cameraAngle = 0.0f; //used
    GLuint  programID;      // program this will never change, make public to access in light manager

    matricies mtxData;
    globalData gblData;
   
    GLuint glBufferMTX;
    GLuint glBufferGBL;
    unsigned int bindingMTX = 1U;
    unsigned int bindingGBL = 0U;

    //defered shading variables
    bool copyDepth = true; //set in imgui for whether we copy over the depth info
    bool renderVertNormals = false;
    bool renderFaceNormals = false;
    
    unsigned int gPosition, gNormal, gAlbedoSpec;
    unsigned int gBuffer;
    unsigned int rboDepth;
    GLuint* m_textures;
    unsigned int m_depthTexture;

    //camera class for this scene
    Camera cam;

    //defered shading shader classes
    Shader* lightingPass;
    Shader* geoPass;

    //stuff for the normals
    Shader* normShader;


    //we need to draw the quad hehe
    void renderQuad();
    unsigned int quadVAO = 0;
    unsigned int quadVBO;

protected:
    int _windowHeight, _windowWidth;


    std::string objToLoad; //keep
    std::string planePath; //keep
    std::vector<Mesh> objects; //the objects displayed in screen (kept as a vector incase we want multiple objects in the future
    //need a new vector for the lights

    //all openGL uniforms

};


