/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: OBJReader.h
Purpose: This file serves to render the dynamic orbit around the main object for the lights
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS300_2
Author: Mark Kouris, mark.kouris, mark.kouris / pushpak
Creation date: 10/12/21
End Header --------------------------------------------------------*/

#include "orbitRender.h"
#include <algorithm>

//helper for cleaning up memory
template <typename T>
void delete_pointed_to(T* const ptr)
{
  delete ptr;
}

void orbit::generateOrbit(float radius)
{
    currRadius = radius;

    //free old memory
    std::for_each(_orbit.begin(), _orbit.end(), delete_pointed_to<orbitSegment>);

    _orbit.clear();
    float angle1 = 0.00f;
    float angle2 = 1.00f;
    while (angle2 < 361.0f)
    {
        vec3 start = vec3(radius * cos(glm::radians(angle1)), 0.856, radius * sin(glm::radians(angle1)));
        vec3 end = vec3(radius * cos(glm::radians(angle2)), 0.856, radius * sin(glm::radians(angle2)));

        _orbit.push_back(new orbitSegment(start, end));

        angle1 += 1.00f;
        angle2 += 1.00f;

    }
}

void orbit::setMatricies()
{
    camera.position = vec3(2 * cos(glm::radians(radiansCamera)), 1, 2 * sin(glm::radians(radiansCamera)));
    view = glm::lookAt(camera.position, glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    for (unsigned i = 0; i < _orbit.size(); ++i)
    {
        _orbit[i]->setMVP(perspective * view);
    }
}

void orbit::drawOrbit()
{
    for (unsigned i = 0; i < _orbit.size(); ++i)
    {
        _orbit[i]->draw();
    }
}

void orbit::setPerspective(const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT)
{
    perspective = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
}

orbitSegment orbit::getSegment(int index)
{
    int safety = index % _orbit.size();
    orbitSegment copy = orbitSegment(_orbit[safety]->getStart(), _orbit[safety]->getEnd());
    copy.setMVP(_orbit[safety]->getMVP());
    return copy;
}
