/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: OBJReader.h
Purpose: This file serves as the header for the objreader class. This allows
the class to be accessed in any file.
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS300_1
Author: Mark Kouris, mark.kouris, mark.kouris / pushpak
Creation date: 9/24/21
End Header --------------------------------------------------------*/

#pragma once
#ifndef SIMPLE_SCENE_OBJREADER_H
#define SIMPLE_SCENE_OBJREADER_H
#include <string>
#include <fstream>
#include <vector>

// for OpenGL datatypes
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Mesh.h"

class OBJReader
{

public:
    OBJReader();
    virtual ~OBJReader();

    // initialize the data
    void initData();


    // Read data from a file
    enum ReadMethod { LINE_BY_LINE, BLOCK_IO };
    double ReadOBJFile(std::string filepath,
                       Mesh *pMesh,
                       ReadMethod r = ReadMethod::LINE_BY_LINE,
                       GLboolean bFlipNormals = false);

private:

    // Read OBJ file line by line
    int ReadOBJFile_LineByLine( std::string filepath );

    // Read the OBJ file in blocks -- works for files smaller than 1GB
    int ReadOBJFile_BlockIO( std::string filepath );

    // Parse individual OBJ record (one line delimited by '\n')
    void ParseOBJRecord( char *buffer, glm::vec4 &min, glm::vec4 &max );

    // data members
    Mesh *      _currentMesh;
};


#endif //SIMPLE_SCENE_OBJREADER_H
