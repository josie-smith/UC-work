/*----------------------------------------------------------
* COSC363  Ray Tracer - Josie Smith
*
*  The Cone class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#ifndef H_CONE
#define H_CONE

#include <glm/glm.hpp>
#include "SceneObject.h"

class Cone : public SceneObject
{
private:
	glm::vec3 centre = glm::vec3(0);    //The centre of the base of the cylinder
    float r = 1 ;                              //The radius
    float h = 1 ;                              //The height

public:
    Cone() {};                                 //Default constructor creates a unit cylinder

    Cone(glm::vec3 c, float r, float h) : centre(c), r(r), h(h) {}

    float intersect(glm::vec3 p0, glm::vec3 dir);

    glm::vec3 normal(glm::vec3 p);

};

#endif //!H_CONE
