/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: PrimitiveIntersection.cpp
Purpose: This file contains the definitions for all the intersection detections for our primitives
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS350_2
Author: Mark Kouris, mark.kouris, mark.kouris
Creation date: 3/11/22
End Header --------------------------------------------------------*/

#include "PrimitiveIntersection.h"
#include "glm/glm.hpp"
#include <algorithm>

float SqDistPointAABB(const Point3D& p, const Aabb& b);
float calcSignedArea(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
bool near0(float val);

bool near0(float val)
{
    if (abs(val) < 1e-6)
    {
        return true;
    }

    return false;
}

//helper for determining whether a sphere and aabb collide
float SqDistPointAABB(const Point3D& p, const Aabb& b)
{
    float sqDist = 0.0f;
    for (int i = 0; i < 3; i++) //hardcoded 3 because it is Point3D
    {
        // for each axis count any excess distance outside box extents
        float v = p.m_coordinates[i];
        if (v < b.m_Min[i]) sqDist += (b.m_Min[i] - v) * (b.m_Min[i] - v);
        if (v > b.m_Max[i]) sqDist += (v - b.m_Max[i]) * (v - b.m_Max[i]);
    }
    return sqDist;
}

//area of triangles
float calcSignedArea(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
    float p1p2Len = glm::distance(p1, p2);
    float p2p3Len = glm::distance(p2, p3);;
    float p3p1Len = glm::distance(p3, p1);;
    float perimeter = (p1p2Len + p2p3Len + p3p1Len) / 2;

    return glm::sqrt(perimeter * (perimeter - p1p2Len) * (perimeter - p2p3Len) * (perimeter - p3p1Len));

}

//requires use of specific ctor for Sphere
bool SphereSphere(const Sphere& a, const Sphere& b)
{
    return PointSphere(Point3D(b.m_position), Sphere(a.m_position, a.radius + b.radius));
}

//use similar method to Sphere Sphere, except using the radius 
bool AabbSphere(const Aabb& a, const Sphere& b)
{
    //compute Sqdist from sphere center and AABB
    float sqDist = SqDistPointAABB(b.m_position, a);

    //intersection occurs when dist is less than squared sphere radius
    return sqDist <= b.radius * b.radius;
}

bool SphereAabb(const Sphere& a, const Aabb& b)
{
    return AabbSphere(b, a);
}

bool AabbAabb(const Aabb& a, const Aabb& b)
{
    // for each axis { X, Y, Z }
    for (unsigned int i = 0; i < 3; ++i)
    {
        // if no overlap for the axis, no overlap overall
        if (a.m_Max[i] < b.m_Min[i] || b.m_Max[i] < a.m_Min[i])
        {
            return false;

        }
    }
    return true;
}

//true if point in sphere, otherwise false
bool PointSphere(const Point3D& a, const Sphere& b)
{
    float d = glm::distance(a.m_coordinates, b.m_position);
    float r_sq = (b.radius * b.radius);
    return ((d * d) <= r_sq);
}

bool PointAabb(const Point3D& a, const Aabb& b)
{
    //if all in a are between the min and max, there is a collision
    if ((a.m_coordinates.x >= b.m_Min.x && a.m_coordinates.x <= b.m_Max.x) &&
        (a.m_coordinates.y >= b.m_Min.y && a.m_coordinates.y <= b.m_Max.y) &&
        (a.m_coordinates.z >= b.m_Min.z && a.m_coordinates.z <= b.m_Max.z))
    {
        return true;
    }

    return false;
}

//using barycentric coordinates
bool PointTriangle(const Point3D& a, const Triangle& b)
{
    //calc barycentric values for x,y and z for a.
    float areaABC = calcSignedArea(b.v1, b.v2, b.v3);
    float u_val = calcSignedArea(a.m_coordinates, b.v2, b.v3) / areaABC;
    float v_val = calcSignedArea(a.m_coordinates, b.v3, b.v1) / areaABC;
    float w_val = 1 - u_val - v_val;

    // if any are less than 0 or greater than 1, no intersection
    if ((u_val < 0 || u_val > 1) || (v_val < 0 || v_val > 1) || (w_val < 0 || w_val > 1))
    {
        return false;
    }

    return true;
}

bool PointPlane(const Point3D& a, const Plane& b)
{
    //if dot product between plane normal and said point is 0, they are intersecting
    glm::vec3 norm = glm::vec3(b.m_Normal.x, b.m_Normal.y, b.m_Normal.z);
    return 0 == glm::dot(a.m_coordinates, norm);
}

//point plane with the point in the ray that is perpendicular
bool RayPlane(const Ray& a, const Plane& b)
{
    //w value of normal of plane is dist from origin, so if we normalize the normal, we get p0

    //calc the t for this intersection:
    glm::vec3 norm = glm::vec3(b.m_Normal.x, b.m_Normal.y, b.m_Normal.z);
    float ldotn = glm::dot(norm, a.m_Direction);
    float t;
    if (ldotn > 1e-6)
    {
        glm::vec3 p0 = glm::vec3(glm::normalize(b.m_Normal));
        glm::vec3 helper = p0 - a.m_Start;
        t = glm::dot(helper, norm) / ldotn;
        return (t >= 0);

    }

    return false;
}

bool RayAabb(const Ray& a, const Aabb& b)
{
    //determine x t values
    float tmin = (b.m_Min.x - a.m_Start.x) / a.m_Direction.x;
    float tmax = (b.m_Max.x - a.m_Start.x) / a.m_Direction.x;

    if (tmin > tmax)
    {
        std::swap(tmin, tmax);
    }

    //determine y t values
    float tymin = (b.m_Min.y - a.m_Start.y) / a.m_Direction.y;
    float tymax = (b.m_Max.y - a.m_Start.y) / a.m_Direction.y;

    if (tymin > tymax)
    {
        std::swap(tymin, tymax);
    }

    //out of bounds check
    if ((tmin > tymax) || (tymin > tmax))
    {
        return false;
    }

    //clamps to min vals
    if (tymin > tmin)
    {
        tmin = tymin;
    }

    if (tymax < tmax)
    {
        tmax = tymax;
    }

    //determine z t values
    float tzmin = (b.m_Min.z - a.m_Start.z) / a.m_Direction.z;
    float tzmax = (b.m_Max.z - a.m_Start.z) / a.m_Direction.z;

    if (tzmin > tzmax)
    {
        std::swap(tzmin, tzmax);
    }

    //out of bounds check
    if ((tmin > tzmax) || (tzmin > tmax))
    {
        return false;
    }

    //clamps to min vals
    if (tzmin > tmin)
    {
        tmin = tzmin;
    }

    if (tzmax < tmax)
    {
        tmax = tzmax;
    }

    return true;
}

bool RaySphere(const Ray& a, const Sphere& b)
{
    float t0, t1; //solutions for t if ray intersects

   //check if ray is in direction of the sphere
    glm::vec3 L = a.m_Start - b.m_position;
    float a_Q = glm::dot(a.m_Direction, a.m_Direction);
    float b_Q = 2.0f * glm::dot(a.m_Direction, L);
    float c_Q = glm::dot(L, L) - (b.radius * b.radius);

    //solve quadratic
    float discrim = b_Q * b_Q - 4 * a_Q * c_Q;

    if (discrim < 0)
    {
        return false;
    }

    else if (near0(discrim)) //two intersection points if 0
    {
        t0 = -0.5 * b_Q / a_Q;
        t1 = -0.5 * b_Q / a_Q;
    }

    else
    {
        //checks for if we want + or - with quad formula
        float q = (b_Q > 0) ? -0.5f * (b_Q + sqrtf(discrim)) : -0.5f * (b_Q - sqrtf(discrim));
        t0 = q / a_Q;
        t1 = c_Q / q;
    }

    if (t0 > t1)
    {
        std::swap(t0, t1);
    }

    //now we check for bad t values
    if (t0 < 0)
    {
        t0 = t1; // if t0 is negative, let's use t1 instead 
        if (t0 < 0)
        {
            return false; // both t0 and t1 are negative 
        }
    }


    return true;
}

bool RayTriangle(const Ray& a, const Triangle& b)
{
    //compute normals for triangle
    glm::vec3 v1v2 = b.v2 - b.v1;
    glm::vec3 v1v3 = b.v3 - b.v1;
    glm::vec3 normal = glm::cross(v1v2, v1v3);
    float areaSqrd = glm::length(normal);

    //compute P

    //parallel check
    float NdotRayDirection = glm::dot(normal, a.m_Direction);

    if (fabs(NdotRayDirection) < 1e-6) // almost 0 
    {
        return false;
    }

    // compute direction
    float d = glm::dot(-1.0f * normal, b.v1);

    // compute t (time of intersection)
    float t = -(glm::dot(normal, a.m_Start) + d) / NdotRayDirection;

    // check if the triangle is behind the ray
    if (t < 0)
    {
        return false; // the triangle is behind 
    }

    // compute the intersection point
    glm::vec3 inter = a.m_Start + t * a.m_Direction;

    // Step 2: inside-outside test
    glm::vec3 perp; // vector perpendicular to triangle's plane 

    // edge 0
    glm::vec3 edge0 = b.v2 - b.v1;
    glm::vec3 vp0 = inter - b.v1;
    perp = glm::cross(vp0, edge0);
    if (glm::dot(normal, perp) < 0)
    {
        return false; // P is on the right side 
    }
    // edge 1
    glm::vec3 edge1 = b.v3 - b.v2;
    glm::vec3 vp1 = inter - b.v2;
    perp = glm::cross(vp1, edge1);
    if (glm::dot(normal, perp) < 0)
    {
        return false; // P is on the right side 
    }
    // edge 2
    glm::vec3 edge2 = b.v1 - b.v3;
    glm::vec3 vp2 = inter - b.v3;
    perp = glm::cross(vp2, edge2);
    if (glm::dot(normal, perp) < 0)
    {
        return false; // P is on the right side; 
    }

    return true; // this ray hits the triangle 
}

bool PlaneAabb(const Plane& a, const Aabb& b)
{
    glm::vec3 norm = glm::vec3(glm::normalize(a.m_Normal));
    float resMin = glm::dot(b.m_Min, norm);
    float resMax = glm::dot(b.m_Max, norm);

    //check if 2 extreme points are in front of plane
    if (resMin > 0 && resMax > 0)
    {
        return false;
    }

    //check if 2 extreme points are behind the plane
    else if (resMin < 0 && resMax < 0)
    {
        return false;
    }

    else
    {
        return true;
    }
}

bool PlaneTriangle(const Plane& a, const Triangle& b)
{
    //similar methodology to plane aabb, just check if all 3 are in front or behind the plane
    glm::vec3 norm = glm::vec3(glm::normalize(a.m_Normal));
    float res1 = glm::dot(b.v1, norm);
    float res2 = glm::dot(b.v2, norm);
    float res3 = glm::dot(b.v3, norm);

    //check all 3 points are in front of plane
    if (res1 > 0 && res2 > 0 && res3 > 0)
    {
        return false;
    }

    //check all 3 points are behind the plane
    else if (res1 < 0 && res2 < 0 && res3 < 0)
    {
        return false;
    }

    else //otherwise intersection
    {
        return true;
    }
}
