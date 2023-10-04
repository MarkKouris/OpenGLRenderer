/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Mesh.cpp
Purpose: This file serves as the Mesh implementation for assignment 1.
This will process all of the read in data to be used by openGL.
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS300_2
Author: Mark Kouris, mark.kouris, mark.kouris / pushpak
Creation date: 9/24/21
End Header --------------------------------------------------------*/

//#include <gl/GL.h>
#include <GL/glew.h>
#include <iostream>
#include <set>
#include <unordered_set>
#include "Mesh.h"


#include <glm/gtc/epsilon.hpp>

glm::vec4 crossVec4(glm::vec4 left, glm::vec4 right)
{
    glm::vec3 left3 = glm::vec3(left.x, left.y, left.z);
    glm::vec3 right3 = glm::vec3(right.x, right.y, right.z);

    return glm::vec4(glm::cross(left3, right3), 1.0f);
}

// hash function for use with unordered set
struct MyHash
    {
        std::size_t operator()(const glm::vec4& p) const noexcept
        {
            return std::hash<int>()(p.x) ^ std::hash<int>()(p.y) ^ std::hash<int>()(p.z);
        }
    };

// Initialize the data members in the mesh
void Mesh::initData()
{
    vertexBuffer.clear();
    vertexIndices.clear();
    vertexUVs.clear();
    vertexNormals.clear();
    vertexNormalDisplay.clear();

    normalLength = 0.00f;

    return;
}


//Mesh::~Mesh()
//{
//    glDeleteBuffers(1, &vertex_buffer);
//    glDeleteBuffers(1, &normal_buffer);
//    glDeleteBuffers(1, &face_buffer);
//    glDeleteVertexArrays(1, &vao);
//}

/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
GLfloat *Mesh::getVertexBuffer()
{
    return reinterpret_cast<GLfloat *>(vertexBuffer.data());
}

GLfloat *Mesh::getVertexNormals()
{
    return reinterpret_cast<GLfloat *>(vertexNormals.data());
}

GLfloat *Mesh::getVertexUVs()
{
    return reinterpret_cast<GLfloat *>(vertexUVs.data());
}

GLfloat *Mesh::getVertexNormalsForDisplay()
{
    return reinterpret_cast<GLfloat *>(vertexNormalDisplay.data());
}

GLuint *Mesh::getIndexBuffer()
{
    return vertexIndices.data();
}

////////////////////////////////////
////////////////////////////////////
////////////////////////////////////
unsigned int Mesh::getVertexBufferSize()
{
    return (unsigned int) vertexBuffer.size();
}

unsigned int Mesh::getIndexBufferSize()
{
    return (unsigned int) vertexIndices.size();
}

unsigned int Mesh::getTriangleCount()
{
    return getIndexBufferSize() / 3;
}

unsigned int Mesh::getVertexCount()
{
    return getVertexBufferSize();
}

unsigned int Mesh::getVertexNormalCount()
{
    return vertexNormals.size();
}

glm::vec4  Mesh::getModelScale()
{
    glm::vec4 scale = boundingBox[1] - boundingBox[0];

    if (scale.x == 0.0)
        scale.x = 1.0;

    if (scale.y == 0.0)
        scale.y = 1.0;

    if (scale.z == 0.0)
        scale.z = 1.0;

    if (scale.w == 0.0)
        scale.w = 1.0;

    return scale;
}

//ALWAYS MAKE e TRUE
glm::vec3  Mesh::getModelScale(bool e)
{
    glm::vec4 scale = boundingBox[1] - boundingBox[0];
    if (e)
    {
        if (scale.x == 0.0)
            scale.x = 1.0;

        if (scale.y == 0.0)
            scale.y = 1.0;

        if (scale.z == 0.0)
            scale.z = 1.0;
    }

   
    return glm::vec3(scale.x, scale.y, scale.z);
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

glm::vec4  Mesh::getModelCentroid()
{
    return glm::vec4( boundingBox[0] + boundingBox[1] ) * 0.5f;
}

//always make e true
glm::vec3  Mesh::getModelCentroid(bool e)
{
    glm::vec3 result = glm::vec3(0);
    if (e)
    {
        glm::vec4 box = glm::vec4(boundingBox[0] + boundingBox[1]) * 0.5f;
        result = glm::vec3(box.x, box.y, box.z);
    }
    return result;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

glm::vec4 Mesh::getCentroidVector(glm::vec4 vVertex)
{
    return vVertex - getModelCentroid();
}

glm::vec3 Mesh::getCentroidVector(glm::vec3 vVertex)
{
    return vVertex - getModelCentroid(true);
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

struct compareVec
{
    bool operator() (const glm::vec4& lhs, const glm::vec4& rhs) const
    {
        glm::vec3 left = glm::vec3(lhs.x, lhs.y, lhs.z);
        glm::vec3 right = glm::vec3(rhs.x, rhs.y, rhs.z);

        float V = glm::dot( left, right );
        bool bRetCode = glm::epsilonNotEqual( V, 1.0f, FLT_EPSILON);

        return bRetCode;
    }
};

/////////////////////////////////////////////////////////
int Mesh::calcVertexNormals(GLboolean bFlipNormals)
{
    int rFlag = -1;

    // vertices and indices must be populated
    if( vertexBuffer.empty() || vertexIndices.empty() )
    {
        std::cout << "Cannot calculate vertex normals for empty mesh." << std::endl;
        return rFlag;
    }

    // Pre-built vertex normals
//    if( !vertexNormals.empty() )
//    {
//        std::cout << "Vertex normals specified by the file. Skipping this step." << std::endl;
//        return rFlag;
//    }

    // Initialize vertex normals
    GLuint  numVertices = getVertexCount();
    vertexNormals.resize( numVertices, glm::vec4(0.0f) );
    vertexNormalDisplay.resize( numVertices * 2, glm::vec4(0.0f) );

    std::vector< std::unordered_set< glm::vec4, MyHash, compareVec > >  vNormalSet(numVertices, std::unordered_set< glm::vec4, MyHash,compareVec >());
    
    // For every face
    glm::uint index = 0;
    for (; index < vertexIndices.size(); )
    {
        GLuint a = vertexIndices.at(index++);
        GLuint b = vertexIndices.at(index++);
        GLuint c = vertexIndices.at(index++);

        glm::vec4  vA = vertexBuffer[a];
        glm::vec4  vB = vertexBuffer[b];
        glm::vec4  vC = vertexBuffer[c];

        // Edge vectors
        glm::vec4  E1 = vB - vA;
        glm::vec4  E2 = vC - vA;

        glm::vec4  N = glm::normalize(crossVec4( E1, E2 ) );

        if( bFlipNormals )
            N = N * -1.0f;

        // For vertex a
        if (vNormalSet.at(a).find(N) == vNormalSet.at(a).end())
        {
           vNormalSet.at( a ).insert( N );

        }
        if (vNormalSet.at(b).find(N) == vNormalSet.at(b).end())
        {
            vNormalSet.at(b).insert(N);

        }
        if (vNormalSet.at(c).find(N) == vNormalSet.at(c).end())
        {
            vNormalSet.at(c).insert(N);

        }
    }

    // Now sum up the values per vertex
    for( int index =0; index < vNormalSet.size(); ++index )
    {
        glm::vec4  vNormal(0.0f);

//        if( vNormalSet[index].size() <= 0 )
//            std::cout << "[Normal Set](" << index << ") Size = " << vNormalSet[index].size() << std::endl;

        setNormalLength(0.05f);

        auto nIt = vNormalSet[index].begin();
        while(nIt != vNormalSet[index].end())
        {
            vNormal += (*nIt);
            ++nIt;
        }

        // save vertex normal
        vertexNormals[index] = glm::normalize( vNormal );

        // save normal to display
        glm::vec4  vA = vertexBuffer[index];

        vertexNormalDisplay[2*index] = vA;
        vertexNormalDisplay[(2*index) + 1] = vA + ( normalLength * vertexNormals[index] );

    }

    // success
    rFlag = 0;

    return rFlag;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

void Mesh::calcVertexNormalsForDisplay(GLboolean bFlipNormals)
{
    GLuint numVertices = getVertexCount();
    vertexNormalDisplay.resize(numVertices * 2, glm::vec4(0.0f));

    for (int iNormal = 0; iNormal < vertexNormals.size(); ++iNormal)
    {
        glm::vec4 normal = vertexNormals[iNormal] * normalLength;

        vertexNormalDisplay[2 * iNormal] = vertexBuffer[iNormal];
        vertexNormalDisplay[(2 * iNormal) + 1] = vertexBuffer[iNormal] + normal;
    }
}
void Mesh::cleanUp()
{
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteBuffers(1, &normal_buffer);
    glDeleteBuffers(1, &face_buffer);
    glDeleteVertexArrays(1, &vao);
}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

GLfloat  & Mesh::getNormalLength()
{
    return normalLength;
}

void Mesh::setNormalLength(GLfloat nLength)
{
    normalLength = nLength;
}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

int Mesh::calcUVs( Mesh::UVType uvType )
{
    int rFlag = -1;

    // clear any existing UV
    vertexUVs.clear();

    glm::vec4 delta = getModelScale();

    for( int nVertex = 0; nVertex < vertexBuffer.size(); ++nVertex )
    {
        glm::vec4 V = vertexBuffer[nVertex];
        glm::vec2 uv(0.0f);

        glm::vec4 normVertex = glm::vec4( (V.x - boundingBox[0].x ) / delta.x,
                                          (V.y - boundingBox[0].y ) / delta.y,
                                          (V.z - boundingBox[0].z ) / delta.z,
                                           1.0f);

//        normVertex = (2.0f * normVertex) - glm::vec3(1.0f);

        glm::vec4 centroidVec = getCentroidVector(V);

        float theta(0.0f);
        float z(0.0f);
        float phi(0.0f);

        switch( uvType )
        {
            case Mesh::PLANAR_UV:
                uv.x = ( normVertex.x - (-1.0) ) / (2.0);
                uv.y = ( normVertex.y - (-1.0) ) / (2.0);
                break;

            case Mesh::CYLINDRICAL_UV:
                theta = glm::degrees( static_cast<float>( atan2(centroidVec.y, centroidVec.x ) ) );
                theta += 180.0f;

                z = (centroidVec.z + 1.0f) * 0.5f;

                uv.x = theta / 360.0f;
                uv.y = z;
                break;

            case Mesh::SPHERICAL_UV:
                theta = glm::degrees( static_cast<float>(glm::atan(centroidVec.y, centroidVec.x )) );
                theta += 180.0f;

                z = centroidVec.z;
                phi = glm::degrees(glm::acos(z / centroidVec.length() ));

                uv.x = theta / 360.0f;
                uv.y = 1.0f - ( phi / 180.0f );
                break;

            case Mesh::CUBE_MAPPED_UV:
                uv = calcCubeMap(centroidVec);
                break;
        }

        vertexUVs.push_back( uv );
    }

    return rFlag;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

glm::vec2 Mesh::calcCubeMap(glm::vec4 vEntity)
{
    float x = vEntity.x;
    float y = vEntity.y;
    float z = vEntity.z;

    float absX = abs(x);
    float absY = abs(y);
    float absZ = abs(z);

    int isXPositive = x > 0 ? 1 : 0;
    int isYPositive = y > 0 ? 1 : 0;
    int isZPositive = z > 0 ? 1 : 0;

    float maxAxis, uc, vc;
    glm::vec2 uv = glm::vec2(0.0);

    // POSITIVE X
    if (bool(isXPositive) && (absX >= absY) && (absX >= absZ))
    {
        // u (0 to 1) goes from +z to -z
        // v (0 to 1) goes from -y to +y
        maxAxis = absX;
        uc = -z;
        vc = y;
    }

        // NEGATIVE X
    else if (!bool(isXPositive) && absX >= absY && absX >= absZ)
    {
        // u (0 to 1) goes from -z to +z
        // v (0 to 1) goes from -y to +y
        maxAxis = absX;
        uc = z;
        vc = y;
    }

        // POSITIVE Y
    else if (bool(isYPositive) && absY >= absX && absY >= absZ)
    {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from +z to -z
        maxAxis = absY;
        uc = x;
        vc = -z;
    }

        // NEGATIVE Y
    else if (!bool(isYPositive) && absY >= absX && absY >= absZ)
    {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from -z to +z
        maxAxis = absY;
        uc = x;
        vc = z;
    }

        // POSITIVE Z
    else if (bool(isZPositive) && absZ >= absX && absZ >= absY)
    {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from -y to +y
        maxAxis = absZ;
        uc = x;
        vc = y;
    }

        // NEGATIVE Z
    else if (!bool(isZPositive) && absZ >= absX && absZ >= absY)
    {
        // u (0 to 1) goes from +x to -x
        // v (0 to 1) goes from -y to +y
        maxAxis = absZ;
        uc = -x;
        vc = y;
    }

    // Convert range from -1 to 1 to 0 to 1
    uv.s = 0.5f * (uc / maxAxis + 1.0f);
    uv.t = 0.5f * (vc / maxAxis + 1.0f);

    return uv;
}