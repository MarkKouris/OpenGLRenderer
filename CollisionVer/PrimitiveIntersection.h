/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: PrimitiveIntersection.h
Purpose: This file contains the declarations for all the intersection detections for our primitives
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS350_2
Author: Mark Kouris, mark.kouris, mark.kouris
Creation date: 3/11/22
End Header --------------------------------------------------------*/

#pragma once
#include "AABB.h"
#include "BoundingSphere.h"
#include "Plane.h"
#include "Point3D.h"
#include "Ray.h"
#include "Triangle.h"


bool SphereSphere(const Sphere& a, const Sphere& b);
bool AabbSphere(const Aabb& a, const Sphere& b);
bool SphereAabb(const Sphere& a, const Aabb& b);
bool AabbAabb(const Aabb& a, const Aabb& b);
bool PointSphere(const Point3D& a, const Sphere& b);
bool PointAabb(const Point3D& a, const Aabb& b);
bool PointTriangle(const Point3D& a, const Triangle& b);
bool PointPlane(const Point3D& a, const Plane& b);
bool RayPlane(const Ray& a, const Plane& b);
bool RayAabb(const Ray& a, const Aabb& b);
bool RaySphere(const Ray& a, const Sphere& b);
bool RayTriangle(const Ray& a, const Triangle& b);
bool PlaneAabb(const Plane& a, const Aabb& b);
bool PlaneTriangle(const Plane& a, const Triangle& b);