#include <glm/glm.hpp> //for vec3

class simpleCamera 
{
public:
    glm::vec3 position;
    simpleCamera() 
    {
        position = glm::vec3(0, 0, 0);
    }

};