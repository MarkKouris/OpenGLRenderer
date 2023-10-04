/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: skybox.cpp
Purpose: This file implements the skybox for our scene using 6 separate textures
Language: C++
Platform: only Windows
Project: mark.kouris_CS300_3
Author: Mark Kouris, mark.kouris, mark.kouris
Creation date: 12/10/21
End Header --------------------------------------------------------*/

#include "skybox.h"
#include "stb_image_helper.h"
//skyboxSide functions
skyboxSide::skyboxSide(std::string image, glm::vec3 center, GLuint programID, int sideNum)
{
    //glDisable(GL_DEPTH_TEST);
    sideNum_ = sideNum;
    localID = programID;
    float vertices[] = {
        // positions          // colors           // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // load and create a texture 
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char* data = stbi_load(image.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    trans = glm::translate(glm::vec3(0, 1, 0)) * glm::translate(center);
    glUseProgram(localID); // don't forget to activate/use the shader before setting uniforms!
    //now set the uniforms
   // either set it manually like so:
    glUniform1i(glGetUniformLocation(localID, "texture1"), 0);

}

skyboxSide::skyboxSide(std::string image, glm::vec3 center, GLuint programID, int sideNum, GLuint mainID)
{
    //glDisable(GL_DEPTH_TEST);
    sideNum_ = sideNum;
    localID = programID;
    float vertices[] = {
        // positions          // colors           // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // load and create a texture 
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char* data = stbi_load(image.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    trans = glm::translate(glm::vec3(0, 1, 0)) * glm::translate(center);
    glUseProgram(localID); // don't forget to activate/use the shader before setting uniforms!
    //now set the uniforms
   // either set it manually like so:
    glUniform1i(glGetUniformLocation(localID, "texture1"), 0);

    glUseProgram(mainID);
    if (sideNum_ == 1)
    {
        glUniform1i(glGetUniformLocation(mainID, "sideNegX"), 0);
    }
    else if (sideNum_ == 2)
    {
        glUniform1i(glGetUniformLocation(mainID, "sidePosX"), 0);
    }
    else if (sideNum_ == 3)
    {
        glUniform1i(glGetUniformLocation(mainID, "sideNegY"), 0);
    }
    else if (sideNum_ == 4)
    {
        glUniform1i(glGetUniformLocation(mainID, "sidePosY"), 0);
    }
    else if (sideNum_ == 5)
    {
        glUniform1i(glGetUniformLocation(mainID, "sideNegZ"), 0);
    }
    else if (sideNum_ == 6)
    {
        glUniform1i(glGetUniformLocation(mainID, "sidePosZ"), 0);
    }
}

//the function we call every frame
void skyboxSide::displaySide(float radiansCamera, int SCR_WIDTH, int SCR_HEIGHT)
{
    glm::vec3 pos = glm::vec3(1 * cos(glm::radians(radiansCamera)), 1, 1 * sin(glm::radians(radiansCamera)));
    glm::mat4 view = glm::lookAt(pos, glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    glm::mat4 persp = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 rot = glm::mat4(1);
    glm::mat4 scale = glm::scale(glm::vec3(2.5f));
    switch (sideNum_)
    {
    case 1: // left side
    {
        rot = glm::rotate(glm::radians(270.f), glm::vec3(0, 1, 0));
        break;
    }
    case 2: //right side
    {
        rot = glm::rotate(glm::radians(270.f), glm::vec3(0, -1, 0));
        break;
    }
    case 3: //bottom side
    {
        rot = glm::rotate(glm::radians(90.f), glm::vec3(1, 0, 0));
        break;
    }
    case 4: //top side
    {
        rot = glm::rotate(glm::radians(-90.f), glm::vec3(-1, 0, 0));
        break;
    } 
    case 5: //back side
    {
        rot = glm::rotate(glm::radians(180.f), glm::vec3(0, 1, 0));
        break;
    }
    case 6: //front side
    {
        //rot = glm::rotate(-55.f, glm::vec3(0, 1, 0));
        break;
    }
    default: 
        break;
    }

    textureModel = trans * rot * scale;
    glm::mat4 mvp = persp * view * textureModel;
    glm::vec4 center = mvp * glm::vec4(0);
    glUseProgram(localID);
    glUniformMatrix4fv(glGetUniformLocation(localID, "mvp"), 1, GL_FALSE, &mvp[0][0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

skyboxSide::~skyboxSide()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

//skybox functions
//should not use
skybox::skybox()
{
    //create shader
    programID = LoadShaders("../Common/shaders/skybox.vert",
                            "../Common/shaders/skybox.frag");
    mainID = 0;
    //add the 6 sides to the vector
    sides.push_back(new skyboxSide("../Common/skybox/left.jpg", glm::vec3(-2.5f, 0, 0.0f), programID, 1));
    sides.push_back(new skyboxSide("../Common/skybox/right.jpg", glm::vec3(2.5f,0,0.0f), programID, 2));
    sides.push_back(new skyboxSide("../Common/skybox/bottom.jpg", glm::vec3(0,-2.5f, 0.0f), programID, 3));
    sides.push_back(new skyboxSide("../Common/skybox/top.jpg", glm::vec3(0, 2.5f, 0.0f), programID, 4));
    sides.push_back(new skyboxSide("../Common/skybox/back.jpg", glm::vec3(0, 0,-2.5f), programID, 5));
    sides.push_back(new skyboxSide("../Common/skybox/front.jpg", glm::vec3(0.0, 0.0, 2.5f), programID, 6));

}

skybox::skybox(GLuint ID)
{
    //create shader
    programID = LoadShaders("../Common/shaders/skybox.vert",
        "../Common/shaders/skybox.frag");
    mainID = ID;
    //add the 6 sides to the vector
    sides.push_back(new skyboxSide("../Common/skybox/left.jpg", glm::vec3(-2.5f, 0, 0.0f), programID, 1, mainID));
    sides.push_back(new skyboxSide("../Common/skybox/right.jpg", glm::vec3(2.5f, 0, 0.0f), programID, 2, mainID));
    sides.push_back(new skyboxSide("../Common/skybox/bottom.jpg", glm::vec3(0, -2.5f, 0.0f), programID, 3, mainID));
    sides.push_back(new skyboxSide("../Common/skybox/top.jpg", glm::vec3(0, 2.5f, 0.0f), programID, 4, mainID));
    sides.push_back(new skyboxSide("../Common/skybox/back.jpg", glm::vec3(0, 0, -2.5f), programID, 5, mainID));
    sides.push_back(new skyboxSide("../Common/skybox/front.jpg", glm::vec3(0.0, 0.0, 2.5f), programID, 6, mainID));
}

void skybox::displaySides(float radiansCamera, int SCR_WIDTH, int SCR_HEIGHT)
{
    glDisable(GL_DEPTH_TEST);
    //glDisable(GL_WR);
    for (unsigned i = 0; i < sides.size(); ++i)
    {
        sides[i]->displaySide(radiansCamera, SCR_WIDTH, SCR_HEIGHT);
    }

    glEnable(GL_DEPTH_TEST);
}

skybox::~skybox()
{
    sides.clear();
    glDeleteProgram(programID);
}
