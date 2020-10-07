/*----------------------------------------------------------
* COSC363  Ray Tracer - Josie Smith
*
*  The Cone class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cone.h"
#include <math.h>

/**
* Cone's intersection method.  The input is a ray (p0, dir).
*/
float Cone::intersect(glm::vec3 p0, glm::vec3 d)
{
    glm::vec3 s = p0 - centre;
    float a = d.x*d.x + d.z*d.z - d.y*d.y*r*r/(h*h);
    float b = 2*(s.x*d.x + s.z*d.z + d.y*r*r/h - s.y*d.y*r*r/(h*h));
    float c = s.x*s.x + s.z*s.z - r*r - s.y*s.y*r*r/(h*h) + 2*s.y*r*r/h;
    float delta = b*b - 4*a*c;

    if(fabs(delta) < 0.001) return -1.0;    // if the determinant is negative, or close to 0 (ray is a tangent)
    if(delta < 0.0) return -1.0;

    float t = -1;
    float t1 = (-b - sqrt(delta)) / (2*a);
    float t2 = (-b + sqrt(delta)) / (2*a);
    if(fabs(t1) < 0.001 )
    {
        if (t2 > 0) return t2;
        else t1 = -1.0;
    }
    if(fabs(t2) < 0.001 ) t2 = -1.0;

    t = (t1 < t2)? t1: t2;

    glm::vec3 q = p0 + d*t;
    if (q.y < h + centre.y && q.y > centre.y) {
        return t;
    } else {
        return -1;
    }
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cylinder.
*/
glm::vec3 Cone::normal(glm::vec3 p)
{
    glm::vec3 n = glm::vec3(0);
    glm::vec3 s = p - centre;
    float t = atan(r/h);
    float a = atan(s.x/s.z);
    n = glm::vec3(sin(a)*cos(t),sin(t),cos(a)*cos(t));
    n = glm::normalize(n);
    return n;
}



