/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: OBJReader.cpp
Purpose: This file serves as the interface to read in obj data for a mesh
to be displayed on screen. 
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS300_2
Author: Mark Kouris, mark.kouris, mark.kouris / pushpak
Creation date: 9/24/21
End Header --------------------------------------------------------*/

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include <cfloat>
#include <chrono>
#include <set>
#include "OBJReader.h"

OBJReader::OBJReader()
{
    initData();
}

OBJReader::~OBJReader()
{
    initData();
}

/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
void OBJReader::initData()
{
    _currentMesh = nullptr;
}


/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
double OBJReader::ReadOBJFile(std::string filepath, Mesh *pMesh,
        OBJReader::ReadMethod r, GLboolean bFlipNormals)
{
    int rFlag = -1;

    if( pMesh )
        _currentMesh = pMesh;
    else
        return rFlag;

//    clock_t  startTime, endTime;

    auto startTime = std::chrono::high_resolution_clock::now();

    switch( r )
    {
        case OBJReader::LINE_BY_LINE:
            rFlag = ReadOBJFile_LineByLine( filepath );
            break;

        case OBJReader::BLOCK_IO:
            rFlag = ReadOBJFile_BlockIO( filepath );
            break;

        default:
        std::cout << "Unknown value for OBJReader::ReadMethod in function ReadObjFile." << std::endl;
        std::cout << "Quitting ..." << std::endl;
        rFlag = -1;
        break;
    }

    auto endTime = std::chrono::high_resolution_clock::now();

    double timeDuration = std::chrono::duration< double, std::milli >( endTime - startTime ).count();

    std::cout << "OBJ file read in "
              << timeDuration
              << "  milli seconds." << std::endl;


    // Now calculate vertex normals
    _currentMesh->calcVertexNormals(bFlipNormals);
    _currentMesh->calcUVs(Mesh::CYLINDRICAL_UV);

    return timeDuration;
}

/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
int OBJReader::ReadOBJFile_LineByLine(std::string filepath)
{
    int rFlag = -1;
    glm::vec4 min(FLT_MAX, FLT_MAX, FLT_MAX, 1);
    glm::vec4 max(-FLT_MAX, -FLT_MAX, -FLT_MAX, 1);

    std::ifstream  inFile;
    inFile.open( filepath );

    if( inFile.bad() || inFile.eof() || inFile.fail() )
        return rFlag;

    while( !inFile.eof() )
    {
        char buffer[256] = "\0";
        inFile.getline(buffer, 256, '\n');

        // Use only for debugging purpose
//        std::cout << buffer << std::endl;
        ParseOBJRecord( buffer, min, max );
    }

    _currentMesh->boundingBox[0] = min;
    _currentMesh->boundingBox[1] = max;

    return rFlag;
}

/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
// Read the OBJ file in blocks -- works for files smaller than 1GB
int OBJReader::ReadOBJFile_BlockIO( std::string filepath )
{
    int rFlag = -1;
    long int OneGBinBytes = 1024 * 1024 * 1024 * sizeof(char);

    glm::vec4 min(FLT_MAX, FLT_MAX, FLT_MAX,1.0f);
    glm::vec4 max(-FLT_MAX, -FLT_MAX, -FLT_MAX, 1.0f);


    // Check the file size, if > 1 GB, abort
    std::ifstream inFile( filepath, std::ifstream::in | std::ifstream::binary );

    if( inFile.bad() || inFile.eof() )
        return rFlag;

    char *fileContents = NULL;
    long int count=0;

    // get the file size
    inFile.seekg(0, std::ifstream::end);
    count = inFile.tellg();
    inFile.seekg(0, std::ifstream::beg);

    if( count <= 0 || count >= OneGBinBytes )
    {
        std::cout << " Error reading file " << filepath << std::endl;
        std::cout << "File size reported as : " << count << " bytes." << std::endl;
    }
    else if (count > 0)
    {
        const char *delims = "\n\r";
        fileContents = (char *)malloc(sizeof(char) * (count+1));
        inFile.read(fileContents,count);
        fileContents[count] = '\0';

        rFlag = 0;

        // Now parse the obj file
        char *currPtr = fileContents;
        char *token = strpbrk( currPtr, delims );

        while( token != nullptr )
        {
            int numChars = token - currPtr;
            char ObjLine[256];
            strncpy( ObjLine, currPtr, numChars );
            ObjLine[numChars]='\0';

            ParseOBJRecord( ObjLine, min, max );

            currPtr = token + 1;
            token = strpbrk( currPtr, delims );
        }

        free(fileContents);

        _currentMesh->boundingBox[0] = min;
        _currentMesh->boundingBox[1] = max;
    }

    return rFlag;
}

/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
void OBJReader::ParseOBJRecord( char *buffer, glm::vec4 &min, glm::vec4 &max )
{
    const char *delims = " \r\n\t";
    GLfloat x, y, z;

    GLfloat   temp;
    GLuint    firstIndex, secondIndex, thirdIndex;

    char *token = strtok( buffer, delims );

    // account for empty lines
    if( token == nullptr )
        return;

    switch( token[0] )
    {
        case 'v':
            // vertex coordinates
            if( token[1] == '\0' )
            {
                token = strtok( nullptr, delims );
                temp = static_cast<GLfloat &&>(atof(token));
                if( min.x > temp )
                    min.x = temp;
                if( max.x <= temp )
                    max.x = temp;
                x = temp;

                token = strtok( nullptr, delims );
                temp = static_cast<GLfloat &&>(atof(token));
                if( min.y > temp )
                    min.y = temp;
                if( max.y <= temp )
                    max.y = temp;
                y = temp;

                token = strtok( nullptr, delims );
                temp = static_cast<GLfloat &&>(atof(token));
                if( min.z > temp )
                    min.z = temp;
                if( max.z <= temp )
                    max.z = temp;
                z = temp;

                _currentMesh->vertexBuffer.emplace_back(x, y, z, 1.0f);
            }
                // vertex normals
            else if( token[1] == 'n' )
            {
                glm::vec4 vNormal;

                token = strtok( nullptr, delims );
                if( token == nullptr )
                    break;

                vNormal[0] = static_cast<GLfloat &&>(atof(token));

                token = strtok( nullptr, delims );
                if( token == nullptr )
                    break;

                vNormal[1] = static_cast<GLfloat &&>(atof(token));

                token = strtok( nullptr, delims );
                if( token == nullptr )
                    break;

                vNormal[2] = static_cast<GLfloat &&>(atof(token));

                vNormal.w = 1.0f;

                _currentMesh->vertexNormals.push_back( glm::normalize(vNormal) );
            }

            break;

        case 'f':
            token = strtok( nullptr, delims );
            if( token == nullptr )
                break;
            firstIndex = static_cast<unsigned int &&>(atoi(token) - 1);

            token = strtok(nullptr, delims);
            if( token == nullptr )
                break;
            secondIndex = static_cast<unsigned int &&>(atoi(token) - 1);

            token = strtok(nullptr, delims);
            if( token == nullptr )
                break;
            thirdIndex = static_cast<unsigned int &&>(atoi(token) - 1);

            // push back first triangle
            _currentMesh->vertexIndices.push_back( firstIndex );
            _currentMesh->vertexIndices.push_back( secondIndex );
            _currentMesh->vertexIndices.push_back( thirdIndex );

            token = strtok(nullptr, delims);

            while( token != nullptr )
            {
                secondIndex = thirdIndex;
                thirdIndex = static_cast<unsigned int &&>(atoi(token) - 1);

                _currentMesh->vertexIndices.push_back( firstIndex );
                _currentMesh->vertexIndices.push_back( secondIndex );
                _currentMesh->vertexIndices.push_back( thirdIndex );

                token = strtok(nullptr, delims);
            }

            break;

        case '#':
        default:
            break;
    }

    return;
}
