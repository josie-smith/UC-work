/*----------------------------------------------------------
* COSC363  Ray Tracer - Josie Smith
*
*  The Cylinder class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cylinder.h"
#include <math.h>

/**
* Cylinder's intersection method.  The input is a ray (p0, dir).
*/
float Cylinder::intersect(glm::vec3 p0, glm::vec3 dir)
{
    glm::vec3 vdif = p0 - centre;
    float a = dir.x*dir.x + dir.z*dir.z;
    float b = 2*(dir.x*vdif.x + dir.z*vdif.z);
    float c = vdif.x*vdif.x + vdif.z*vdif.z - r*r;
    float delta = b*b - 4*a*c;

    if(fabs(delta) < 0.001) return -1.0;    // if the determinant is negative, or close to 0 (ray is a tangent)
    if(delta < 0.001) return -1.0;

    float t = -1;
    float t_2 = -1;
    float t1 = (-b - sqrt(delta)) / (2*a);
    float t2 = (-b + sqrt(delta)) / (2*a);
    if(fabs(t1) < 0.001 ) t1 = -1.0;
    if(fabs(t2) < 0.001 ) t2 = -1.0;

    if (t1 > 0 && t2 > 0) {
        t = (t1 < t2)? t1: t2;
        t_2 = (t1 < t2)? t2: t1;
    } else {
        t = (t1 < t2)? t2: t1;               // return the closest intersection
        t_2 = (t1 < t2)? t1: t2;
    }

    glm::vec3 q = p0 + dir*t;
    glm::vec3 q_2 = p0 + dir*t_2;
    if (q.y < h + centre.y && q.y > centre.y) {
        if (t_2 < 0 && q_2.y > centre.y + h) {
            t_2 = (h + centre.y - p0.y) / dir.y;
            return t_2;
        } else {
            return t;
        }
    } else if (q.y > h + centre.y && q_2.y < h + centre.y) {
        if (t_2 < 0 && q_2.y < centre.y) {
            t_2 = (h-0.5 + centre.y - p0.y) / dir.y;
            return t_2;
        } else {
            t_2 = (h + centre.y - p0.y) / dir.y;
            return t_2;
        }
    } else if (q.y < centre.y && q_2.y > centre.y) {
        if (t_2 < 0 && q_2.y > centre.y + h) {
            t_2 = (h + centre.y - p0.y) / dir.y;
            return t_2;
        } else {
            t_2 = (centre.y - p0.y) / dir.y;
            return t_2;
        }
    } else {
        return -1;
    }
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cylinder.
*/
glm::vec3 Cylinder::normal(glm::vec3 p)
{
    glm::vec3 n = glm::vec3(0);
    glm::vec3 s = p - centre;
    if (fabs(s.x*s.x + s.z*s.z - r*r) < 0.01) {
        n = glm::vec3(s.x, 0, s.z);
        n = glm::normalize(n);
    } else if ( s.y - h < 0.001 ) {
        n = glm::vec3(0,1,0);
    } else if (s.y - h + 0.5 < 0.001) {
        n = glm::vec3(0,-1,0);
    }
    return n;
}
