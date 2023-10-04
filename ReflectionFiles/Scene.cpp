/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Scene.cpp
Purpose: This file serves the purpose of using the obj reader and mesh classes to
display an obj on screen. This handles all of the openGL functions and setup.
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS300_2
Author: Mark Kouris, mark.kouris, mark.kouris / pushpak
Creation date: 9/24/21
End Header --------------------------------------------------------*/

#define _CRT_SECURE_NO_WARNINGS
#include "Scene.h"
#include <algorithm>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/glm.hpp"
#include <iostream>
#include "camera.h"
float angleOfRotation = 0;
float angle = 0;


//in constructor, I should allocate the 9 meshes.
// good 
Scene::Scene() : _windowWidth(100), _windowHeight(100)
{
    InitMembers();
}

// good 
Scene::Scene(int windowWidth, int windowHeight)
{
    _windowHeight = windowHeight;
    _windowWidth = windowWidth;
    InitMembers();
}

// good 
Scene::~Scene()
{
    CleanUp();
}

// good 
void Scene::loadObjData(std::string path)
{
    OBJReader reader = OBJReader();
    mainObj->cleanUp();
    mainObj->initData();
    reader.ReadOBJFile(path, mainObj, OBJReader::BLOCK_IO, false);
    loadMesh();
}

// needs to be updated with current stuff
int Scene::InitMembers()
{
    mainObj = new Mesh;
    programID = 0;
    objToLoad = "../Common/models/bunny.obj";
    return 0;
}

// Public methods
// 
// 
// Init: called once when the scene is initialized
// call matricies scene initialize

glm::mat4 Scene::Translate(const glm::vec4& t)
{
    return glm::mat4{ {1,   0,   0,   0},
                      {0,   1,   0,   0},
                      {0,   0,   1,   0},
                      {t.x, t.y, t.z, 1} };
}

glm::mat4 Scene::AffineInverse(glm::mat4& m)
{
    return glm::mat4(glm::inverse(glm::mat3{ glm::vec3(m[0].x,m[0].y,m[0].z),
    glm::vec3(m[1].x,m[1].y,m[1].z), glm::vec3(m[2].x,m[2].y,m[2].z), })) * Translate(-m[3]);
}

/* returns the modeling transformation that maps a canonical camera to the
specified world–space camera*/
glm::mat4 Scene::model(Camera& cam)
{
    //this is the Mc from the notes
    //E is eye point
    //u is right vec
    //v is up vec
    //r is back vec

    return glm::mat4(cam.right(), cam.up(), cam.back(), cam.eye());
}

/* returns the viewing (world–to–camera) transformation for the specified camera.
This transformation is used to view the world from a camera’s point of view */
glm::mat4 Scene::viewMat(Camera& cam)
{
    //this is the inverse of the model matrix
    glm::mat4 modelMat = model(cam);
    return AffineInverse(modelMat);
}

/* returns the perspective (camera–to–ndc) transformation that maps
the camera space view frustum to the standard cube. OpenGL conventions are to be
used: the near plane of the frustum should map to the plane z = −1, and the far plane
should map to the plane z = 1. */
glm::mat4 Scene::perspective(Camera& cam)
{
    //for this, need the capital pi matrix
    //need 2D/W, 2D/H, (N+F)/(N-F), (2*N*F)/(N-F)
    // W, H and D are from the viewport
    glm::vec4 viewPort = cam.viewport(); //x = w, y = h, z = d
    float value1 = (2 * viewPort.z) / viewPort.x;
    float value2 = (2 * viewPort.z) / viewPort.y;
    float value3 = (cam.nearf() + cam.farf()) / (cam.nearf() - cam.farf());
    float value4 = (2 * cam.nearf() * cam.farf()) / (cam.nearf() - cam.farf());
    glm::mat4 perspectiveMat(0);
    perspectiveMat[0][0] = value1;
    perspectiveMat[1][1] = value2;
    perspectiveMat[2][2] = value3;
    perspectiveMat[2][3] = -1;
    perspectiveMat[3][2] = value4;

    return perspectiveMat;
}


int Scene::Init()
{
    //sceneCam.zoom(-3.0f);

    // Create and compile our GLSL program from the shaders
    LoadAllShaders();

    //load initial data for the vector of Meshes
    loadObjData(objToLoad); //sets the first mesh

    //load in the data
    //bind normal buffer
    glGenBuffers(1, &normBuff);
    glBindBuffer(GL_ARRAY_BUFFER, normBuff);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mainObj->getVertexNormalCount(), mainObj->getVertexNormals(), GL_STATIC_DRAW);

    // set up uniform buffers
    setupMTX();
    setupGBL();

    return 0;
}

// LoadAllShaders: This is the placeholder for loading the shader files
//once ubo is properly set up, never use this
void Scene::LoadAllShaders()
{
    //this gives us the program glint, still need the other buffers
    programID = LoadShaders("../Common/shaders/PhongShading.vert",
                            "../Common/shaders/PhongShading.frag");
    return;
}

// preRender : called to setup stuff prior to rendering the frame
int Scene::preRender()
{
    return 0;
}

// Render : per frame rendering of the scene
// need to update to set the matrix data
int Scene::Render()
{
    glUseProgram(programID);

    //change camera position here
    glm::vec3 camPos = /*sceneCam.eye();*/glm::vec3(2 * cos(glm::radians(cameraAngle)), 0.856f, 2 * sin(glm::radians(cameraAngle)));
    glm::mat4 view = /*viewMat(sceneCam);*/glm::lookAt(camPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 persp = /*perspective(sceneCam);*/glm::perspective(glm::radians(45.0f), (float)_windowWidth / (float)_windowHeight, 0.1f, 100.0f);
    glm::vec4 eyePosition = glm::vec4(camPos, 1.0f);
    //set uniform block data
    mtxData.view_matrix = view;
    mtxData.persp_matrix = persp;
    gblData.eyePosition = eyePosition;

    /*== Render main obj =======================================================================*/

    // T * R * S * Vertex
    glm::mat4 modelMat = glm::mat4(1);
    glm::mat4 trans = glm::translate(mainObj->getCentroidVector(glm::vec3(0)));
    glm::mat4 rot = glm::rotate(angleOfRotation, glm::vec3(0, 1, 0));
    //glm::mat4 scale = glm::scale(0.4f / mainObj->getModelScale(true));
    auto scl = mainObj->getModelScale(true);
    float max = std::max({ scl.x, scl.y, scl.z });
    glm::mat4 scale = glm::scale(glm::vec3(0.4f / max));
    modelMat = scale * rot * trans;
    mtxData.model_matrix = modelMat;

    /* calculate the normal matrix here */
    glm::mat4 normMatrixModel = calcNormalMat(modelMat);
    mtxData.normal_matrix = normMatrixModel;

    //update the uniform blocks
    updateMTX();
    updateGBL();

    glBindVertexArray(mainObj->vao);
    glDrawElements(GL_TRIANGLES, mainObj->getIndexBufferSize(), GL_UNSIGNED_INT, 0);
    /*=================================================================================*/


    return 0;
}

// postRender : Any updates to calculate after current frame
int Scene::postRender()
{
    return 0;
}

// CleanUp : clean up resources before destruction
void Scene::CleanUp()
{
    // Cleanup VBO
    mainObj->cleanUp();
    glDeleteBuffers(1, &glBufferMTX);
    glDeleteBuffers(1, &glBufferGBL);
    glDeleteProgram(programID);
    return;
}

//loads the mesh
void Scene::loadMesh()
{
    //bind vertex buffer
    glGenBuffers(1, &mainObj->vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, mainObj->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * mainObj->getVertexBufferSize(), mainObj->getVertexBuffer(), GL_STATIC_DRAW);

    //bind normal buffer
    glGenBuffers(1, &mainObj->normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, mainObj->normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * mainObj->getVertexNormalCount(), mainObj->getVertexNormals(), GL_STATIC_DRAW);


    //bind face buffer
    glGenBuffers(1, &mainObj->face_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, mainObj->face_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint) * mainObj->getIndexBufferSize(), mainObj->getIndexBuffer(), GL_STATIC_DRAW);

    //now set up the vao
    GLint vPosition = glGetAttribLocation(programID, "position"),
        vertexNormal = glGetAttribLocation(programID, "normal");


    glGenVertexArrays(1, &mainObj->vao);
    glBindVertexArray(mainObj->vao);

    glBindBuffer(GL_ARRAY_BUFFER, mainObj->vertex_buffer);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(vPosition);

    glBindBuffer(GL_ARRAY_BUFFER, mainObj->normal_buffer);
    glVertexAttribPointer(vertexNormal, 4, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(vertexNormal);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mainObj->face_buffer);
}


//helper functions for calc normal mat
glm::mat3 affineInverseNorm(glm::mat4 m)
{
    return glm::inverse(glm::mat3(m));
}

glm::mat4 transposeNorm(glm::mat3 m)
{
    glm::mat3 temp = glm::transpose(m);
    return glm::mat4(temp);
}


glm::mat4 Scene::calcNormalMat(glm::mat4 model)
{
    return glm::mat4(glm::transpose(glm::inverse(glm::mat3(model))));
    glm::mat4 inv = transposeNorm(affineInverseNorm(model));
    inv[3][3] = 1;
    return inv;
}

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


void Scene::setupMTX()
{
    glGenBuffers(1, &glBufferMTX); //glBuffer is the buffer value, store this value
    glBindBuffer(GL_UNIFORM_BUFFER, glBufferMTX);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(mtxData), nullptr, GL_STATIC_DRAW); //&uniform the value ptr of the struct.
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingMTX, glBufferMTX);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Scene::setupGBL()
{
    glGenBuffers(1, &glBufferGBL); //glBuffer is the buffer value, store this value
    glBindBuffer(GL_UNIFORM_BUFFER, glBufferGBL);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(gblData), nullptr, GL_STATIC_DRAW); //&uniform the value ptr of the struct.
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingGBL, glBufferGBL);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Scene::updateMTX()
{
    glBindBuffer(GL_UNIFORM_BUFFER, glBufferMTX);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(matricies), &mtxData);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Scene::updateGBL()
{
    glBindBuffer(GL_UNIFORM_BUFFER, glBufferGBL);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(globalData), &gblData);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

// Display : Per-frame execution of the scene
int Scene::Display()
{
    preRender();

    Render();

    postRender();

    return 0;
}


#if 0

//should have these functions for all structs:

//what to store:
should make this templatized for binding and struct type
glBuffer
the struct itself


//order for setting up uniform buffers:
glGenBuffers(1, &glBuffer); //glBuffer is the buffer value, store this value
glBindBuffer(GL_UNIFORM_BUFFER, glBUffer);
glBindBuffer(GL_UNIFORM_BUFFER, binding, glBUffer);
glBufferData(GL_UNIFORM_BUFFER, sizeof(/*struct given*/), &uniform, GL_DYNAMIC_DRAW) //&uniform the value ptr of the struct.

//update all the buffers:
glBindBuffer(GL_UNIFORM_BUFFER, glbuffer);
glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), &uniform);
glBindBuffer(GL_UNIFORM_BUFFER, 0);

//cleanup is just
glDeleteBuffers(1, &glbuffer);

//update part is for updating a certain value in the array of lights:
glBindBuffer(GL_UNIFORM_BUFFER, glbuffer);
glBufferSubData(GL_UNIFORM_BUFFER, reinterpret_cast<char*>(part) - reinterpret_cast<char*>(&uniform), sizeof(U), part);
glBindBuffer(GL_UNIFORM_BUFFER, 0);


#endif