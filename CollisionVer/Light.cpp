/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Light.cpp
Purpose: This file contains the class definition for each light and the associated functions
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS300_2
Author: Mark Kouris, mark.kouris, mark.kouris
Creation date: 10/12/21
End Header --------------------------------------------------------*/

#include "Light.h"
#include <sstream>

//makes default light
Light::Light(Shader* shad)
{
    data.type = 0;
    data.enabled = 1;
    _mesh = loadObjData("../Common/models/sphere.obj"); //sets the first mesh
    loadMesh(_mesh, shad);

    //find the uniforms for light shader here
    shad->use();
    uModel = glGetUniformLocation(shad->ID, "model");
    uColor = glGetUniformLocation(shad->ID, "color");
}

//moves light one point forward on orbit
void Light::updatePosition(vec3 Pos, mat4 MVP)
{
    if (data.enabled == 1)
    {
        glm::mat4 trans = glm::translate(_mesh->getCentroidVector(glm::vec3(0)));
        //glm::mat4 rot = glm::rotate(, glm::vec3(0, 1, 0)); might need
        glm::mat4 scale = glm::scale(0.03f / _mesh->getModelScale(true));
        glm::mat4 orbitTrans = glm::translate(Pos);
        model = orbitTrans * scale * trans;
    }

}

//this function ONLY updates the per light shader data
void Light::update(Shader* shad)
{
    if (data.enabled == 1)
    {
        shad->use();
        // uniforms for drawing the lights
        glUniformMatrix4fv(uModel, 1, GL_FALSE, &model[0][0]);
        glUniform3fv(uColor, 1, &data.diffuse[0]);
    }
}

//where the opengl draw functions are called
void Light::draw(Shader* shad)
{
    if (data.enabled == 1)
    {
        shad->use();
        glBindVertexArray(_mesh->vao);
        glDrawElements(GL_TRIANGLES, sizeof(GLuint) * _mesh->getIndexBufferSize(), GL_UNSIGNED_INT, 0);
    }

}

//loads the sphere
void Light::loadMesh(Mesh* m, Shader* shad)
{
    //bind vertex buffer
    glGenBuffers(1, &(m->vertex_buffer));
    glBindBuffer(GL_ARRAY_BUFFER, m->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * m->getVertexBufferSize(), m->getVertexBuffer(), GL_STATIC_DRAW);

    //bind normal buffer
    glGenBuffers(1, &m->normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m->normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * m->getVertexNormalCount(), m->getVertexNormals(), GL_STATIC_DRAW);


    //bind face buffer
    glGenBuffers(1, &m->face_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m->face_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint) * m->getIndexBufferSize(), m->getIndexBuffer(), GL_STATIC_DRAW);

    //From here and below is setting up the "in" data

    //now set up the vao
    GLint vPosition = glGetAttribLocation(shad->ID, "modelPosition"),
        vertexNormal = glGetAttribLocation(shad->ID, "vertexNormal");


    glGenVertexArrays(1, &m->vao);
    glBindVertexArray(m->vao);

    glBindBuffer(GL_ARRAY_BUFFER, m->vertex_buffer);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(vPosition);

    glBindBuffer(GL_ARRAY_BUFFER, m->normal_buffer);
    glVertexAttribPointer(vertexNormal, 4, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(vertexNormal);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->face_buffer);

}

//copy of function from scene, just loads a mesh from a path
Mesh* Light::loadObjData(std::string path)
{
    OBJReader reader = OBJReader();
    Mesh* mesh = new Mesh;
    mesh->initData();
    reader.ReadOBJFile(path, mesh, OBJReader::BLOCK_IO, true);
    return mesh;
}

void Light::initLightData()
{
    data.type = 0; // 0 = point, 1 = direction, 2 = spot
    data.enabled = 0; //1 is yes, 0 is no 
    data.innerAngle = 30.0f;
    data.outerAngle = 45.0f;
    data.fallout = 1.0f;
    data.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    data.ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    data.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    data.position = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); //for point and spot
    data.direction = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);; //for spot and directional
}


//only actually happens at end of the program, otherwise they just get disabled.
Light::~Light()
{
    glDeleteBuffers(1, &(_mesh->vertex_buffer));
    glDeleteBuffers(1, &(_mesh->normal_buffer));
    glDeleteBuffers(1, &(_mesh->face_buffer));
    glDeleteVertexArrays(1, &(_mesh->vao));
}
