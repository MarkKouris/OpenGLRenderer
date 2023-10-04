/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Mesh.h
Purpose: This file serves as the Mesh header for assignment 1.
This will allow access to processed mesh data.
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS300_2
Author: Mark Kouris, mark.kouris, mark.kouris / pushpak
Creation date: 9/24/21
End Header --------------------------------------------------------*/
#pragma once
#ifndef SIMPLE_OBJ_SCENE_MESH_H
#define SIMPLE_OBJ_SCENE_MESH_H

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


class Mesh
{
public:
    friend class OBJReader;

    // Get attribute values buffer
    GLfloat *getVertexBuffer();             // attribute 0
    GLfloat *getVertexNormals();            // attribute 1
    GLfloat *getVertexUVs();                // attribute 2

    GLfloat *getVertexNormalsForDisplay();  // attribute 0

    unsigned int getVertexBufferSize();
    unsigned int getVertexCount();
    unsigned int getVertexNormalCount();

    unsigned int getVertexUVsCount();

    // Get vertex index buffer
    GLuint *getIndexBuffer();
    unsigned int getIndexBufferSize();
    unsigned int getTriangleCount();

    //vec3 overloads
    glm::vec3   getModelScale(bool e);
    glm::vec3   getModelCentroid(bool e);
    glm::vec3   getCentroidVector(glm::vec3 vVertex);

    //vec4 overloads
    glm::vec4   getModelScale();
    glm::vec4   getModelCentroid();
    glm::vec4   getCentroidVector( glm::vec4 vVertex );


    GLfloat  &  getNormalLength();
    void setNormalLength( GLfloat nLength );

    // initialize the data members
    void initData();

    // calculate vertex normals
    int calcVertexNormals(GLboolean bFlipNormals = false);

    // calculate the "display" normals
    void calcVertexNormalsForDisplay(GLboolean bFlipNormals = false);

    // calculate texture coordinates
    enum UVType { PLANAR_UV = 0,
                  CYLINDRICAL_UV,
                  SPHERICAL_UV,
                  CUBE_MAPPED_UV};

    int         calcUVs( Mesh::UVType uvType = Mesh::PLANAR_UV );
    glm::vec2   calcCubeMap( glm::vec4 vEntity );

    //openGL variables
    GLuint  vao = 0;             // vao 
    GLuint  vertex_buffer = 0;   // vertex_buffer
    GLuint  normal_buffer = 0;   // normal_buffer
    GLuint  face_buffer = 0;     // face_buffer
    GLuint  UV_buffer = 0;     // UV_buffer
    int faceCount = 0;

private:
    std::vector<GLuint>       vertexIndices;
    std::vector<glm::vec2>    vertexUVs;
    std::vector<glm::vec4>    vertexBuffer; //positions
    std::vector<glm::vec4>    vertexNormals, vertexNormalDisplay; //normals

    glm::vec4               boundingBox[2];
    GLfloat                 normalLength = 0;
};


#endif //SIMPLE_OBJ_SCENE_MESH_H
