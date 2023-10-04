/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: skybox.h
Purpose: This file holds the declarations for the skybox and skyboxSide class
Language: C++
Platform: only Windows
Project: mark.kouris_CS300_3
Author: Mark Kouris, mark.kouris, mark.kouris
Creation date: 12/10/21
End Header --------------------------------------------------------*/

#include <vector>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <shader.hpp>
#include <glm/common.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
//This class will be for each of the 6 sides of the skybox, 6 will exist in skybox
class skyboxSide
{
    public:

    skyboxSide(std::string image, glm::vec3 center, GLuint programID, int sideNum);
    skyboxSide(std::string image, glm::vec3 center, GLuint programID, int sideNum, GLuint mainID);
    void displaySide(float radiansCamera, int SCR_WIDTH, int SCR_HEIGHT);
    //openGL vairables:
    unsigned int texture;
    int width, height, nrChannels;
    unsigned int VBO, VAO, EBO; //only need 1 for all 6
    GLuint localID;
    glm::mat4 trans;
    glm::mat4 textureModel;
    int sideNum_;
    //position, color, text coords and indicies:
    ~skyboxSide();
};

//This class handles the overall functionality of the skybox.
class skybox 
{
    public:
    skybox();
    skybox(GLuint ID);
    void displaySides(float radiansCamera, int SCR_WIDTH, int SCR_HEIGHT);
    //static sizes for 
    GLuint programID;
    GLuint mainID;
    std::vector<skyboxSide*> sides;
    ~skybox();
};