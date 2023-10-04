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

#define NUM_RENDER_TARGETS 3

//in constructor, I should allocate the 9 meshes.
// good 
Scene::Scene() : _windowWidth(100), _windowHeight(100)
{
    cam = Camera();
    normShader = new Shader("../Common/shaders/normal.vert",
        "../Common/shaders/normal.frag",
        "../Common/shaders/normal.geom");

    lightingPass = new Shader("../Common/shaders/DeferedShading.vert",
        "../Common/shaders/DeferedShading.frag");

    geoPass = new Shader("../Common/shaders/gBuffer.vert",
        "../Common/shaders/gBuffer.frag");

    m_textures = new GLuint[NUM_RENDER_TARGETS];

    InitMembers();
}

// good 
Scene::Scene(int windowWidth, int windowHeight)
{
    cam = Camera();
    normShader = new Shader("../Common/shaders/normal.vert",
        "../Common/shaders/normal.frag",
        "../Common/shaders/normal.geom");

    lightingPass = new Shader("../Common/shaders/DeferedShading.vert",
        "../Common/shaders/DeferedShading.frag");

    geoPass = new Shader("../Common/shaders/gBuffer.vert",
        "../Common/shaders/gBuffer.frag");

    m_textures = new GLuint[NUM_RENDER_TARGETS];


    _windowHeight = windowHeight;
    _windowWidth = windowWidth;
    InitMembers();
}

// good 
Scene::~Scene()
{
    delete normShader;
    delete lightingPass;
    delete geoPass;
    delete m_textures;
    CleanUp();
}

// good 
Mesh* Scene::loadObjData(std::string path)
{
    OBJReader reader = OBJReader();
    Mesh* mesh = new Mesh;
    mesh->initData();
    reader.ReadOBJFile(path, mesh, OBJReader::BLOCK_IO, false);
    return mesh;
}

// needs to be updated with current stuff
int Scene::InitMembers()
{
    programID = 0;
    objToLoad = "../Common/models/sphere.obj";
    planePath = "../Common/models/cube2.obj";
    return 0;
}

//currently set up where we only have a single, center obj.
//Could eventually expand to multiple objs
//unused I think?
void Scene::updateObj(std::string newObj)
{
    //bind vertex buffer
    Mesh* m = loadObjData(newObj);
    loadMesh(m);
    std::iter_swap(objects.begin(), objects.begin() + objects.size() - 1);
    objects.pop_back();
}

int Scene::Init()
{
    //sceneCam.zoom(-3.0f);

    // Create and compile our GLSL program from the shaders
    LoadAllShaders();

    //load initial data for the vector of Meshes
    Mesh* obj = loadObjData(objToLoad); //sets the first mesh
    loadMesh(obj);

    //load in the cube again but as the plane
    Mesh* plane = loadObjData(planePath); //sets the first mesh
    loadMesh(plane);

    //initialize the gBuffer here
    genGBuffer();

    //lighting pass setup
    // --------------------
    lightingPass->use();
    lightingPass->setInt("gPosition", 0);
    lightingPass->setInt("gNormal", 1);
    lightingPass->setInt("gAlbedoSpec", 2);


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

// LoadAllShaders: This is the placeholder for loading the shader files
// reloads the shader, 
void Scene::LoadAllShaders(std::string vert, std::string frag)
{
    //this gives us the program glint, still need the other buffers
    programID = LoadShaders(vert.c_str(), frag.c_str());
    return;
}

// This does the 
int Scene::RenderMainObj()
{
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    geoPass->use();
    //no need to set shader uniforms because we are using uniform blocks.
    //change camera position here
    glm::vec3 camPos = cam.Position;
    glm::mat4 view = cam.GetViewMatrix();
    glm::mat4 persp = glm::perspective(glm::radians(cam.Zoom), (float)_windowWidth / (float)_windowHeight, 0.1f, 100.0f);
    glm::vec4 eyePosition = glm::vec4(camPos, 1.0f);
    //set uniform block data
    mtxData.view_matrix = view;
    mtxData.persp_matrix = persp;
    gblData.eyePosition = eyePosition;

    /*== Geometry Pass here: ==========================================================================*/

    //render plane 
    //uses objects[1] here because the plane will always be the second element in the vector
    glm::mat4 planeMat = glm::mat4(1);
    glm::mat4 transPlane = glm::translate(objects[1].getCentroidVector(glm::vec3(0)));
    glm::mat4 rotP = glm::rotate(angleOfRotation, glm::vec3(0, 1, 0));
    auto sclP = objects[1].getModelScale(true);
    float maxP = std::max({ sclP.x, sclP.y, sclP.z });
    glm::mat4 scaleP = glm::scale(glm::vec3(.4f / maxP));
    glm::mat4 makePlane = glm::scale(vec3(4.0, 0.1, 4.0)) * glm::translate(vec3(0, -2.0, 0));

    /* this is the model matrix uniform */
    planeMat = makePlane * scaleP * rotP * transPlane;
    mtxData.model_matrix = planeMat;

    /* calculate the normal matrix here */
    glm::mat4 normMatrixPlane = calcNormalMat(planeMat);
    mtxData.normal_matrix = normMatrixPlane;

    //update the uniform blocks
    updateMTX();
    updateGBL();

    //draw call
    glBindVertexArray(objects[1].vao);
    glDrawElements(GL_TRIANGLES, objects[1].getIndexBufferSize(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    //finished drawing the plane in the 
    //geoPass->use();
    //render main object here
    // T * R * S * Vertex
    glm::mat4 modelMat = glm::mat4(1);
    glm::mat4 trans = glm::translate(objects[0].getCentroidVector(glm::vec3(0)));
    glm::mat4 rot = glm::rotate(angleOfRotation, glm::vec3(0, 1, 0));
    auto scl = objects[0].getModelScale(true);
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

    glBindVertexArray(objects[0].vao);
    glDrawElements(GL_TRIANGLES, objects[0].getIndexBufferSize(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    /*=================================================================================*/

    //here the geometry pass has finished, now we repeat for the lighting pass.
    lightingPass->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

    //RENDER QUAD HERE
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    renderQuad();

    return 0;
}

// preRender : called to setup stuff prior to rendering the frame
int Scene::copyDepthInfo()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, _windowWidth, _windowHeight, 0, 0, _windowWidth, _windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return 0;
}

// postRender : Any updates to calculate after current frame
int Scene::RenderDebugObj()
{
    //now we draw the main object a second time, but with only the normals being showed
    if (renderVertNormals)
    {
        normShader->use();
        normShader->setMat4("projection", mtxData.persp_matrix);
        normShader->setMat4("view", mtxData.view_matrix);
        normShader->setMat4("model", mtxData.model_matrix);
        normShader->setBool("genVert", true);

        glBindVertexArray(objects[0].vao);
        glDrawElements(GL_TRIANGLES, objects[0].getIndexBufferSize(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    else if (renderFaceNormals)
    {
        normShader->use();
        normShader->setMat4("projection", mtxData.persp_matrix);
        normShader->setMat4("view", mtxData.view_matrix);
        normShader->setMat4("model", mtxData.model_matrix);
        normShader->setBool("genVert", false);

        glBindVertexArray(objects[0].vao);
        glDrawElements(GL_TRIANGLES, objects[0].getIndexBufferSize(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

    }

    return 0;
}

// CleanUp : clean up resources before destruction
void Scene::CleanUp()
{
    // Cleanup VBO
    for (unsigned i = 0; i < objects.size(); ++i)
    {
        glDeleteBuffers(1, &objects[i].vertex_buffer);
        glDeleteBuffers(1, &objects[i].normal_buffer);
        glDeleteBuffers(1, &objects[i].face_buffer);
        glDeleteVertexArrays(1, &objects[i].vao);
    }
    glDeleteBuffers(1, &glBufferMTX);
    glDeleteBuffers(1, &glBufferGBL);
    glDeleteProgram(programID);
    return;
}

//loads the mesh
void Scene::loadMesh(Mesh* m)
{
    //bind vertex buffer
    Mesh newMesh = *m;
    glGenBuffers(1, &newMesh.vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, newMesh.vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * m->getVertexBufferSize(), m->getVertexBuffer(), GL_STATIC_DRAW);

    //bind normal buffer
    glGenBuffers(1, &newMesh.normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, newMesh.normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * m->getVertexNormalCount(), m->getVertexNormals(), GL_STATIC_DRAW);

    //bind UV buffer
    glGenBuffers(1, &newMesh.UV_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, newMesh.UV_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * m->getVertexUVsCount(), m->getVertexUVs(), GL_STATIC_DRAW);

    //bind face buffer
    glGenBuffers(1, &newMesh.face_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, newMesh.face_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint) * m->getIndexBufferSize(), m->getIndexBuffer(), GL_STATIC_DRAW);

    //now set up the vao
    //need to change this to be pos, normal and UV and to use the different shader
    GLint vPosition = glGetAttribLocation(geoPass->ID, "modelPos"),
        vertexNormal = glGetAttribLocation(geoPass->ID, "modelNorm"),
        vertexUV = glGetAttribLocation(geoPass->ID, "modelUV");

    glGenVertexArrays(1, &newMesh.vao);
    glBindVertexArray(newMesh.vao);

    glBindBuffer(GL_ARRAY_BUFFER, newMesh.vertex_buffer);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(vPosition);

    glBindBuffer(GL_ARRAY_BUFFER, newMesh.normal_buffer);
    glVertexAttribPointer(vertexNormal, 4, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(vertexNormal);

    glBindBuffer(GL_ARRAY_BUFFER, newMesh.UV_buffer);
    glVertexAttribPointer(vertexUV, 2, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(vertexUV);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newMesh.face_buffer);

    objects.push_back(newMesh);
}

//this should not be in this file, but ya know :)
void Scene::activateTextures()
{
    //// bind appropriate textures, we will have 3

    //glActiveTexture(GL_TEXTURE0); // active proper texture unit before binding   
    //// now set the sampler to the correct texture unit
    //glUniform1i(glGetUniformLocation(geoPass->ID, (name + number).c_str()), i);
    //// and finally bind the texture
    //glBindTexture(GL_TEXTURE_2D, textures[i].id);
    //glActiveTexture(GL_TEXTURE1); // active proper texture unit before binding   
    //glActiveTexture(GL_TEXTURE2); // active proper texture unit before binding   
}

glm::mat4 Scene::calcNormalMat(glm::mat4 model)
{
    return glm::mat4(glm::transpose(glm::inverse(glm::mat3(model))));
}


//this function will create the gbuffer
void Scene::genGBuffer()
{

    // configure g-buffer framebuffer
    // ------------------------------
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    // position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _windowWidth, _windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    // normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _windowWidth, _windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // color + specular color buffer
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _windowWidth, _windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    // create and attach depth buffer (renderbuffer)v
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _windowWidth, _windowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

// Uniform Buffer functions

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

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
void Scene::renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}


// Display : Per-frame execution of the scene
int Scene::Display()
{
    RenderMainObj();

    if (copyDepth == true)
    {
        copyDepthInfo();
    }

    RenderDebugObj();

    return 0;
}