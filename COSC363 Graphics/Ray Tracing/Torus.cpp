/*----------------------------------------------------------
* COSC363  Ray Tracer - Josie Smith
*
*  The Torus class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Torus.h"
#include "poly34.h"
#include <math.h>

/**
* Torus's intersection method.  The input is a ray.
*/
float Torus::intersect(glm::vec3 p0, glm::vec3 dir)
{
    glm::vec3 s = p0 - center;
    // quartic equation for torus
    float d2 = dir.x*dir.x + dir.y*dir.y + dir.z*dir.z;
    float sd = s.x*dir.x + s.y*dir.y + s.z*dir.z;
    float s2 = s.x*s.x + s.y*s.y + s.z*s.z - r*r - R*R;
    float a = d2*d2;
    float b = 4*d2*sd;
    float c = 2*d2*s2 + 4*sd*sd + 4*R*R*dir.y*dir.y;
    float d = 4*s2*sd + 8*R*R*s.y*dir.y;
    float e = s2*s2 - 4*R*R*(r*r - s.y*s.y);

    double t[4] = {0};
    int roots = SolveP4(t, b/a, c/a, d/a, e/a);

    if (roots == 0) {
        return -1;
    } else if (roots == 2) {
        if (fabs(t[0]) < 0.1 )
        {
            if (t[1] > 0) return t[1];
            else t[0] = -1.0;
        }
        if (fabs(t[1]) < 0.1 ) t[1] = -1.0;
        return (t[0] < t[1])? t[0]: t[1];
    } else { // roots = 4
        if (fabs(t[0]) < 0.1 ) t[0] = -1.0;
        if (fabs(t[1]) < 0.1 ) t[1] = -1.0;
        if (fabs(t[2]) < 0.1 ) t[2] = -1.0;
        if (fabs(t[3]) < 0.1 ) t[3] = -1.0;
        float min = t[0];
        if (t[1] > 0 && t[1] < min) {
            min = t[1];
        }
        if (t[2] > 0 && t[2] < min) {
            min = t[2];
        }
        if (t[3] > 0 && t[3] < min) {
            min = t[3];
        }
        return min;
    }
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the torus.
*/
glm::vec3 Torus::normal(glm::vec3 p)
{
    glm::vec3 d = glm::vec3(p.x - center.x, 0, p.z - center.z);
    d = glm::normalize(d);
    glm::vec3 n = p - center - R*d;
    n = glm::normalize(n);
    return n;
}
